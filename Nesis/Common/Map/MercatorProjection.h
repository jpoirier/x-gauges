#ifndef __MAP_MERCATOR_PROJECTION_H
#define __MAP_MERCATOR_PROJECTION_H
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

#include "AbstractProjection.h"
#include <vector>

namespace map {

// -----------------------------------------------------------------------
/** \brief MercatorProjection projection transformations.
 */
class MercatorProjection : public AbstractProjection
{
typedef std::vector<float> V;
public:
	//! Constructor.
	MercatorProjection(qreal fLam0);
	//! Destructor.
	virtual ~MercatorProjection();
	
	//! Convert wgs coordinates on the sphere (lat, lon) into (x, y). 
	virtual QPointF ConvertToMap(const QPointF& ptWGS) const;
	//! Convert flat earth map (x,y) coordinates into wgs sphere (lat, lon). 
	virtual QPointF ConvertToWGS(const QPointF& ptMap) const;
	//! Returns true - this is rectangular projection.
	virtual bool IsRectangular() const
	{ return true; }

private:
	//! Set projection parameters.
	void Set(qreal fLam0);
	
	//! Fast Y from Latitude.
	float GetYFromLatitude(float fLat) const;
	//! Fast Latitude from Y.
	float GetLatitudeFromY(float y) const;

private:
	//! Zero (central) longitude.
	qreal m_fLam0;	
	
	//! Speedup data
	V m_vC0, m_vC1, m_vI0, m_vI1;
	//! Step size
	float m_fs;
	
};

// -----------------------------------------------------------------------
} // map namespace

#endif	// __MAP_MERCATOR_PROJECTION_H
