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
#include <QtXml>
#include "Unit/Manager.h"
#include "XMLTags.h"

#include "ParameterAirspeed.h"
#include "ParameterContainer.h"

namespace instrument {

// --------------------------------------------------------------------------

std::auto_ptr<ParameterContainer> ParameterContainer::s_apInstance;

// --------------------------------------------------------------------------

ParameterContainer::ParameterContainer()
{
}

ParameterContainer::~ParameterContainer() 
{
	Container::iterator i;
	for(i = m_con.begin(); i!= m_con.end(); i++) {
		delete *i;
	}
}

// --------------------------------------------------------------------------

ParameterContainer* ParameterContainer::GetInstance()
{
	if(s_apInstance.get()==NULL) {
		s_apInstance = std::auto_ptr<ParameterContainer>(new ParameterContainer);
	}
	return s_apInstance.get();
}

// --------------------------------------------------------------------------

bool ParameterContainer::Load(const QDomDocument& doc)
{
	// Ok, go and get the details.
	QDomElement el = doc.documentElement().firstChildElement(TAG_PARAMS);
	if(el.isNull())
		qDebug() << TAG_PARAMS << "tag not found.";

	Parameter* pParam;
	QDomElement ep = el.firstChildElement(TAG_ITEM);
	bool bValid = true;
 	for (; !ep.isNull(); ep = ep.nextSiblingElement(TAG_ITEM)) {
		// Create the object
		pParam = CreateParameter(ep);
		if(pParam->IsValid()==false)
			bValid = false;

		m_con.insert(pParam->GetName(), pParam);
	}

	// List all parameters
// 	Container::iterator i;
// 	for(i=m_con.begin(); i!=m_con.end(); i++)
// 		qDebug() << i.value()->GetName();

	return bValid;
}

// --------------------------------------------------------------------------

Parameter* ParameterContainer::CreateParameter(
	const QDomElement& el
)
{
	// Get parameter attribute
	QString qsA = el.attribute(TAG_PARAM);
	if(qsA.isEmpty()) {
		qCritical() << "The" << TAG_PARAM << "attribute is missing.";
		return NULL;
	}

	if(qsA == QLatin1String(TAG_AIRSPEED))
		return new ParameterAirspeed(el);
	else
		return new Parameter(el);
}
// --------------------------------------------------------------------------

Parameter* ParameterContainer::GetParameter(const QString& qsName)
{
	Container::iterator f = m_con.find(qsName);
	if(f == m_con.end())
		return NULL;
	return *f;
}

// --------------------------------------------------------------------------

QPair<Parameter*, int> ParameterContainer::CheckParameterAndKey(
	const QString& qsParam, 
	const QString& qsUserUnitKey
)
{
	QPair<Parameter*, int> R = qMakePair((Parameter*)NULL, -1);

	unit::Manager* pM = unit::Manager::GetInstance();

	R.first = GetParameter(qsParam);
	if(!R.first) {
		qCritical() << "Parameter" << qsParam << "not found in container.";
	}

	R.second = pM->GetKey(qsUserUnitKey);			

	if(R.second < 0) {
		qCritical() << "Unit" << qsUserUnitKey 
						<<	"is not recognised by the unit manager.";
	}

	// Check group.
	if(pM->IsConvertable(R.second, R.first->GetUnitKeySystem()) == false) {
		qCritical() << "Units" << pM->GetUnit(R.second)->GetSignature()
						<< "and" << pM->GetUnit(R.first->GetUnitKeySystem())->GetSignature()
						<< "are not members of the same group.";
	}

	return R;
}

// --------------------------------------------------------------------------

bool ParameterContainer::SetCallbackUserKey(
	const QString& qsParam, 
	Delegate0<float> D, 
	const QString& qsUserUnitKey
)
{
	QPair<Parameter*, int> R = CheckParameterAndKey(qsParam, qsUserUnitKey);
	if(!R.first)
		return false;

	R.first->SetCallback(D);
	R.first->SetUnitKeyUser(R.second);
	return true;
}

// --------------------------------------------------------------------------

bool ParameterContainer::SetCallbackUserKey(
	const QString& qsParam, 
	Delegate<float, int> D, 
	const QString& qsUserUnitKey
)
{
	QPair<Parameter*, int> R = CheckParameterAndKey(qsParam, qsUserUnitKey);
	if(!R.first)
		return false;

	R.first->SetCallback(D);
	R.first->SetUnitKeyUser(R.second);
	return true;
}

// --------------------------------------------------------------------------
}
