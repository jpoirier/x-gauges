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

#include "Group.h"
#include "Unit.h"

namespace unit {
// ------------------------------------------------------------------

Unit::Unit(
	const QString& qsSig, 	//!< Signature (unique and short string)
	const QString& qsDesc,	//!< Description (longer string)
	const float fC0, 			//!< constant conversion coefficient
	const float fC1,			//!< Linear conversion coefficient
	const Group* pGroup		//!< A group that unit belongs to (owner).
) 
:	
	m_pGroup(pGroup),
	m_qsSig(qsSig), 
	m_qsDesc(qsDesc),
	m_fC0(fC0),
	m_fC1(fC1)
{
}

Unit::~Unit()
{
}

// --------------------------------------------------------------------------

} // namespace unit;
