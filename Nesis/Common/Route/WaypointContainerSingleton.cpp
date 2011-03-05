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

#include <QVector>
#include "WaypointContainerSingleton.h"

namespace route {

std::auto_ptr<WaypointContainerSingleton> WaypointContainerSingleton::s_apInstance;

// -------------------------------------------------------------------------

WaypointContainerSingleton::WaypointContainerSingleton()
{
}

WaypointContainerSingleton::~WaypointContainerSingleton()
{
}

// -------------------------------------------------------------------------

WaypointContainerSingleton* WaypointContainerSingleton::GetInstance()
{
	if(!s_apInstance.get())
		s_apInstance = std::auto_ptr<WaypointContainerSingleton>(new WaypointContainerSingleton);
	return s_apInstance.get();
}

// -------------------------------------------------------------------------

}
