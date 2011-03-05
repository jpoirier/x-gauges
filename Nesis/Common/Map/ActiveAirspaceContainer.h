#ifndef MAP_ACTIVE_AIRSPACE_CONTAINER_H
#define MAP_ACTIVE_AIRSPACE_CONTAINER_H

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
#include "Airspace.h"

namespace map {
	
// ------------------------------------------------------------------------ 
/*! \brief ActiveAirspaceContainer - a container of airspaces in the vicinity of airplane.
	
	The container does not own Airspace objects.
 */

	class ActiveAirspaceContainer 
{
private:
	//! Internal container declaration.
	typedef QVector<const Airspace*> Container;
	
	
public:
	//! ActiveAirspaceContaner contructor.
	ActiveAirspaceContainer();
			
	//! Destructor.
	~ActiveAirspaceContainer();
	
	//! Update active airspaces - get airspaces which are near current airplane position.
	void Update(const QPointF& ptX, float fRadiusKm=100);
	
	//! Get airspaces which contain given point.
	QVector<const Airspace*> GetAirspaces(const QPointF& ptX) const;
	
private:
	//! Container of active airspaces.
	Container m_conAS;
};


} // map namespace

#endif	// __MAP_ACTIVE_AIRSPACE_CONTAINER_H
