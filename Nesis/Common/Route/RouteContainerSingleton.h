#ifndef __ROUTE_ROUTE_CONTAINER_SINGLETON_H
#define __ROUTE_ROUTE_CONTAINER_SINGLETON_H

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
#include <QRect>
#include "RouteContainer.h"

namespace route {

class RouteContainerSingleton : public RouteContainer
{
private:
	//! Constructor
	RouteContainerSingleton();
public:
	//! Destructor
	~RouteContainerSingleton();

	//! Access to the singleton object.	
	static RouteContainerSingleton* GetInstance()
	{
		if(s_pInstance == NULL)
			s_pInstance = new RouteContainerSingleton();
		return s_pInstance;
	}
private:
	//! The only object of this class.
	static RouteContainerSingleton* s_pInstance;
};

// --------------------------------------------------------------------

}

#endif
