#ifndef __MAP_FLAT_EARTH_H
#define __MAP_FLAT_EARTH_H

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

#include <QPointF>

namespace map {
	
class FlatEarth 
{	
public:
	//! Constructor needs origin.
	FlatEarth(const QPointF& ptO);
	//! Destructor
	~FlatEarth();
	
	//! Set new origin.
	void SetOrigin(const QPointF& ptO);
	
	//! Get WGS coordinats for given heading (rad) and distance (m)
	QPointF GetCoordinate(float fH_rad, float fD_m) const;
	//! Return Manhattan distance (m) x=North, y=East from given coordinate.
// 	QPointF GetManhattanDistance(const QPointF& pt) const;
	//! Return Bearing (rad) distance (m) from origin to given point.
	std::pair<float,float> GetBearingDistance(const QPointF& pt) const;
		
private:
	//! Origin point - longitude, latitude.
	QPointF m_ptO;
	//! Correction factor for longitudes. f=cos(lat);
	float m_f;
};

// -----------------------------------------------------------------------

} // map namespace

#endif	// __MAP_FLAT_EARTH_H
