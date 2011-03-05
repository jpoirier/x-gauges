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

#include <algorithm>
#include "Group.h"


namespace unit {

// ------------------------------------------------------------------

Group::Group(const QString& qsGroupName)
	: m_qsGroupName(qsGroupName)
{
}
// ------------------------------------------------------------------

Group::~Group()
{
}

// --------------------------------------------------------------------------

void Group::AddUnit(int iKey)
{
	// Search for the key in the container.
	// Add only if it is not there.
	KeyVector::iterator f = std::find(m_conUnit.begin(), m_conUnit.end(), iKey);
	if(f==m_conUnit.end())
		m_conUnit.push_back(iKey);
}

// --------------------------------------------------------------------------

} // namespace
