#ifndef __ROUTE_WAYPOINT_CONTAINER_SINGLETON_H
#define __ROUTE_WAYPOINT_CONTAINER_SINGLETON_H

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
#include "WaypointContainer.h"

namespace route {

// --------------------------------------------------------------------------
	
class WaypointContainerSingleton : public WaypointContainer
{
protected:
	//! Constructor
	WaypointContainerSingleton();
public:
	//! Destructor
	~WaypointContainerSingleton();
	
	//! Access to the only object.
	static WaypointContainerSingleton* GetInstance();
private:
	static std::auto_ptr<WaypointContainerSingleton> s_apInstance;
};

// --------------------------------------------------------------------------

} // namespace

#endif
