#ifndef __UNIT_MANAGER_H
#define __UNIT_MANAGER_H

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

#include <memory>
#include <QXmlDefaultHandler>
#include <QVector>
#include "Group.h"

namespace unit {

// --------------------------------------------------------------------------

/** \brief The unit::Manager class stores all Unit anf Group objects.

    Only one instace of Manager may exist - hence it is written in 
    singleton form. The Manager stores Group object and Unit objects. 
    It also takes care that Group object stores correct indices (keys)
    if individual Unit objects.

    Units are typically loaded and parsed from a XML file.
 */
class Manager : public QXmlDefaultHandler
{
	typedef QVector<Unit*>  ConUnit;
	typedef QVector<Group*> ConGroup; 

public:
	//! Returns the pointer to the one and only UnitManager object.
	static Manager* GetInstance();
	
	//! Destructor. It deletes all units and unit group objects.
	~Manager();

private:
	//! Default constructor. Units must be loaded using LoadXML.
	Manager();

	//! Creates new unit group. 
	int AddGroup(const QString& qsGroupName);

	//! Creates new unit and forvards its key to its group. 
	int AddUnit(const QString& qsSig, const QString& qsDesc, 
					const float fC0, const float fC1, 
					const int iGroupId);

	//! Searcheas for the group object, if the group name is known. 
	int FindGroupIndex(const QString& qsGroupName) const;

	//! XML simple parser - starting element
	bool startElement(const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName,
							const QXmlAttributes& qaAttributes);
	//! XML simple parser - the error handler.
	bool fatalError(const QXmlParseException& exception);

public:
	//! Load XML file of units.
	int LoadXML(const QString& bsFileName);

	//! Returns the address of the unit with given key. 
	const Unit* GetUnit(int iKey) const;
	//! Returns the unit address for the known unit name.
	const Unit* GetUnit(const QString& unitName) const;
	//! Returns the unit key for the given unit signature. 
	int GetKey(const QString& unitName) const;

	//! Converts a value f from one unit into second unit.
	float Convert(float f, const Unit* pFrom, const Unit* pTo) const;
	//! Converts a value f from one unit into second unit.
	float Convert(float f, int iKeyFrom, int iKeyTo) const;

	//! Check if keys are convertable (are members of the same group).
	bool IsConvertable(int iKey1, int iKey2) const;
	//! Check if units are convertable (are members of the same group).
	bool IsConvertable(const Unit* pU1, const Unit* pU2) const;

	//! Get signature from key.
	QString GetSignature(int iKey) const;

protected:
	//! Automatic pointer to single instance of the Manager object.
	static std::auto_ptr<Manager> s_apInstance;
	
	//! Container of all groups.
	ConGroup m_conGroup;
	//! Container of all units.
	ConUnit m_conUnit;
	
	//! Currently active group (during XML parsing).
	int m_iCurrentGroup;
};

// --------------------------------------------------------------------------

} // namespace

#endif 
