#ifndef __UNIT_GROUP_H
#define __UNIT_GROUP_H

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

#include "Unit.h"
#include <vector>

namespace unit {

class UnitManager;

// --------------------------------------------------------------------------

/** \brief This class defines a group of units of the same class. 

	 The class stores keys of all Unit objects that belong to this group.
	 All units are stored by the Manager object each with a unique key. Unit
	 key is determined at run-time, when units and groups are loaded.

	 The constructor is protected and only Manager class may create
	 Group objects.
 */
class Group
{
	friend class Manager;
	
	//! Units are identified by their keys - defined by the Manager object.
	typedef std::vector<int> KeyVector;

protected:
	//! The constructor takes a group name.
	Group(const QString& qsGroupName);

public:
	//! The destructor - it does not have a serious job.
	~Group();

public:
	//! Returns the group name.
	const QString& GetName() const { return m_qsGroupName; }

	//! Adds new unit to the group.
	void AddUnit(int iUnitKey);

	//! Returns number of units in the group.
	int GetCount() const { return m_conUnit.size(); }

	//! Returns access to the unit keys in the group.
	const std::vector<int>& GetKeys() const
	{ return m_conUnit; }

protected:
	//! Name of the group.
	const QString m_qsGroupName;
	//! Vector of keys that is used to access individal units in the group.
	KeyVector m_conUnit;
};
// --------------------------------------------------------------------------

} // namespace

#endif 
