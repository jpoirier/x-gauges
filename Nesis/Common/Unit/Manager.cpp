/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QDebug>
#include "Manager.h"

#define TAG_GROUP_LIST			"group_list"
#define TAG_GROUP					"group"
#define TAG_NAME					"name"
#define TAG_UNIT					"unit"
#define TAG_ID						"id"
#define TAG_DESC					"desc"
#define TAG_C1						"C1"
#define TAG_C0						"C0"


namespace unit {

// --------------------------------------------------------------------------

std::auto_ptr<Manager> Manager::s_apInstance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Manager* Manager::GetInstance()
{
	if(s_apInstance.get() == NULL) {
		s_apInstance = std::auto_ptr<Manager>(new Manager);
	}
	return s_apInstance.get();
}

// --------------------------------------------------------------------------

Manager::~Manager()
{
	foreach(Unit* pU, m_conUnit)
		delete pU;
		
	foreach(Group* pG, m_conGroup)
		delete pG;
}
// --------------------------------------------------------------------------

Manager::Manager()
{
	m_iCurrentGroup = -1;
}

// --------------------------------------------------------------------------
/** Search container of groups for \a qsGroupName argument. 
	\retval index of group in the container,
	\retval -1 of the group was not found.
 */
 
int Manager::FindGroupIndex(const QString& qsGroupName) const
{
	int i;
	for(i = 0; i < (int)m_conGroup.size(); i++)
		if(m_conGroup[i]->GetName()==qsGroupName) 
			return i;
	return -1;
}

// --------------------------------------------------------------------------
/*! Add new \a qsGroupName to the container of groups. If such group
	 doesn't exist, a new will be created. Otherwise, index of the 
	 existing group will be returned.
    
    \retval Returns the index of newly created group.
*/

int Manager::AddGroup(
	const QString& qsGroupName //!< A new group to be added.
)
{
	int index = FindGroupIndex(qsGroupName);
	if(index==-1) {
		Group *pGroup = new Group(qsGroupName);
		m_conGroup.push_back(pGroup);
		index = m_conGroup.size()-1;
	}
	return index;
}
// --------------------------------------------------------------------------
/**
	\retval Key of the unit (sequential number of the unit in the array.)
	\retval -1 indicates an error.
 */
int Manager::AddUnit(
	const QString& qsSig,	//!< Short signature 
	const QString& qsDesc,  //!< Long description
	const float fC0, 			//!< Conversion coefficient - constant
	const float fC1, 			//!< Conversion coefficient - linear factor
	const int iGroupId		//!< Group to which the unit belongs.
)
{
	// this is used internally and asserts are acceptable
	Q_ASSERT(iGroupId < m_conGroup.size());
	if(iGroupId < 0 || iGroupId >= (int)m_conGroup.size()) {
		return -1; // invalid key
	}
	
	Group* pG = m_conGroup[iGroupId];
	// Find the key in the group.
	int iKey = GetKey(qsSig);
	// If not found, create one.
	if(iKey<0) {
		m_conUnit.push_back(new Unit(qsSig, qsDesc, fC0, fC1, pG));
		iKey = m_conUnit.size()-1;
		pG->AddUnit(iKey);
	}
	return iKey;
}

// --------------------------------------------------------------------------
/** This member function is slow, since it does a linear search over 
    the unit container.

    \retval Address of the unit hidden behind a key,
    \retval NULL if unit is not found.
 */

const Unit* Manager::GetUnit(
	const QString& qsUnitName //!< The unit name that we are searching for.
) const
{
	foreach(Unit* pU, m_conUnit) {
		if(pU->GetSignature() == qsUnitName)
			return pU;
	}
	return NULL;
}

// --------------------------------------------------------------------------
/** The key must be known, of course. This call is fast, since the key 
    is nothing but index in the unit container.
 */
const Unit* Manager::GetUnit(int iKey) const
{ 
	if(iKey < (int)m_conUnit.size())
		return m_conUnit[iKey]; 
	else
		return NULL;
}
// --------------------------------------------------------------------------
/** The function requires linear search and is therefore slow.

	\retval index (key) of the unit.
	\retval -1 when unit was not found.
 */
 
int Manager::GetKey(
	const QString& qsUnitName //!< Unit name that we are searching for.
) const
{
	for (int i=0; i< m_conUnit.size(); i++) {
		if(m_conUnit[i]->GetSignature() == qsUnitName)
			return i;
	}
	return -1;
}

// --------------------------------------------------------------------------

float Manager::Convert(float f, const Unit* pFrom, const Unit* pTo) const
{
	Q_ASSERT(pFrom && pTo);
	Q_ASSERT(pFrom->GetGroup() == pTo->GetGroup());
	f = pFrom->ConvertToBase(f);
	return pTo->ConvertFromBase(f);
}

// --------------------------------------------------------------------------

float Manager::Convert(float f, int iKeyFrom, int iKeyTo) const
{
	return Convert(f, GetUnit(iKeyFrom), GetUnit(iKeyTo));
}
// --------------------------------------------------------------------------

bool Manager::IsConvertable(int iKey1, int iKey2) const
{
	return IsConvertable(GetUnit(iKey1), GetUnit(iKey2));
}

// --------------------------------------------------------------------------

bool Manager::IsConvertable(const Unit* pU1, const Unit* pU2) const
{
	// Both must be non-NULL.
	if(!pU1 || !pU2)
		return false; 

	return (pU1->GetGroup() == pU2->GetGroup());
}

// --------------------------------------------------------------------------

int Manager::LoadXML(const QString& bsFileName)
{
	// Load data from the file.
	QFile file(bsFileName);
	QXmlInputSource inputSource(&file);
	
	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	reader.setErrorHandler(this);
	
	// Set reader to the unknown state before parsing.
	reader.parse(inputSource);
	
	// Return number of units.
	return m_conUnit.count();
}

// --------------------------------------------------------------------------

bool Manager::startElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName,
	const QXmlAttributes& qaAttr
)
{
	if(qsName == TAG_GROUP) {
		// We must have a name attribute
		if(qaAttr.index(TAG_NAME) != -1) {
			QString qsGroupName = qaAttr.value(TAG_NAME);
			m_iCurrentGroup = AddGroup(qsGroupName);
		}
	}
	else if(qsName == TAG_UNIT) {
		// All information about a unit is from attributes
		QString qsSig  = qaAttr.value(TAG_ID);
		QString qsDesc = qaAttr.value(TAG_DESC);
		QString qsC1   = qaAttr.value(TAG_C1);
		QString qsC0   = qaAttr.value(TAG_C0);
		if(qsSig.isEmpty() || qsC1.isEmpty()) {
			qWarning() << "Unit.xml error: One of unit attributes is wrong or missing.";
			return false;
		}
		AddUnit(qsSig, qsDesc, qsC0.toFloat(), qsC1.toFloat(), m_iCurrentGroup);
	}
	return true;
}

// -----------------------------------------------------------------------

bool Manager::fatalError(const QXmlParseException& ex)
{
	qDebug() << "Unit.xml error: Line=" << ex.lineNumber()
			   << " Col=" << ex.columnNumber() 
			   << " Msg=" << ex.message();
	return false;
}

// -----------------------------------------------------------------------

QString Manager::GetSignature(int iKey) const
{
	const Unit* pU = GetUnit(iKey);
	if(pU)
		return pU->GetSignature();
	return QString();
}

// -----------------------------------------------------------------------
} // namespace
