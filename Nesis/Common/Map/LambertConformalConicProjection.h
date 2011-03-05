#ifndef __MAP_LAMBERT_CONFORMAL_CONIC_PROJECTION_H
#define __MAP_LAMBERT_CONFORMAL_CONIC_PROJECTION_H
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

namespace map {

// -----------------------------------------------------------------------
/** \brief Lambert's conformal conic projection transformation.
  
	 This projection seems to be exellent projection for aviation use,
    since it only slightly distors the landscape. However, it is 
    numerically intensive.
 */
class LambertConformalConicProjection : public AbstractProjection
{
public:
	//! Constructor.
	LambertConformalConicProjection(qreal fLam0, qreal fPhi0, qreal fPhi1, qreal fPhi2);
	//! Destructor.
	virtual ~LambertConformalConicProjection();
	
	//! Convert wgs coordinates on the sphere (lat, lon) into (x, y). 
	virtual QPointF ConvertToMap(const QPointF& ptWGS) const;
	//! Convert flat earth map (x,y) coordinates into wgs sphere (lat, lon). 
	virtual QPointF ConvertToWGS(const QPointF& ptMap) const;
	
	//! Returns false - this is not a rectangular projection.
	virtual bool IsRectangular() const
	{ return false; }

private:
	//! Set projection parameters.
	void Set(qreal fLam0, qreal fPhi0, qreal fPhi1, qreal fPhi2);
	
private:
	//! Internal projection specific variable 'n'.
	qreal m_fn;	
	//! Internal projection specific variable '1/n'.
	qreal m_1_fn;	
	//! Internal projection specific variable 'F'.
	qreal m_fF;	
	//! Internal projection specific variable 'Rho0'.
	qreal m_fRho0;
	//! Zero (central) longitude.
	qreal m_fLam0;	
};

// -----------------------------------------------------------------------
} // map namespace

#endif	// __MAP_LAMBERT_CONFORMAL_CONIC_PROJECTION_H
