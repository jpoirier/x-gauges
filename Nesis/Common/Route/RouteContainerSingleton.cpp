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

#include "RouteContainerSingleton.h"

namespace route {

RouteContainerSingleton* RouteContainerSingleton::s_pInstance = NULL;

// -------------------------------------------------------------------------
	
RouteContainerSingleton::RouteContainerSingleton()
{
}


RouteContainerSingleton::~RouteContainerSingleton()
{
}

// -------------------------------------------------------------------------

}
