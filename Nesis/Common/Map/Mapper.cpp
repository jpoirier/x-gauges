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

#include "MathEx.h"
#include "Mapper.h"
#include "SphereNavigation.h"

// The ellipsoidal quadratic mean radius provides the best
// approximation of Earth's average transverse meridional radius 
// and radius of curvature. It is this radius that would be used to 
// approximate the ellipsoid's average great ellipse 
// (i.e., this is the equivalent spherical "great-circle" radius 
// of the ellipsoid)
//#define EARTH_RADIUS 6372795.477598 // meters


namespace map {
// ----------------------------------------------------------------------------

Mapper::Mapper(int iPixPerMeter, int iPaperDim)
{
	m_pProjection = NULL;
	m_iPixPerMeter = iPixPerMeter;
	m_iL           = iPaperDim;
}

Mapper::~Mapper()
{
}
// ----------------------------------------------------------------------------

void Mapper::SetScale(
	float fScale, 				//! The scale (1/100000, 1/300000, ...)
	const QPointF& ptWGSc	//! The central point.
)
{
	ASSERT(m_pProjection!=NULL);
	m_fScale = fScale;
	// Get central point in FE coordinates (flat earth)	
	QPointF ptFE = m_pProjection->ConvertToMap(ptWGSc);
	
	// Next two values must be in double precision even on ARM platform.
	double fppm = EARTH_RADIUS_M * m_fScale * m_iPixPerMeter;
	
	// FE into paper (pix)
	m_fAx = fppm;
	m_fBx = (m_iL-2*ptFE.x()*fppm+1)/2;
	
	m_fAy = -fppm;
	m_fBy = (m_iL+2*ptFE.y()*fppm+1)/2;
	 
	// paper (pix) into FE
	m_fIAx = 1.0/m_fAx;
	m_fIBx = -m_fBx/m_fAx;
	
	m_fIAy = 1.0/m_fAy;
	m_fIBy = -m_fBy/m_fAy;
}

// ----------------------------------------------------------------------------
	
int Mapper::GetPixPerDegree() const
{
	return (int)(M_PI/180. * EARTH_RADIUS_M * m_fScale * m_iPixPerMeter);
}

// ----------------------------------------------------------------------------

float Mapper::GetLongitudeOnePixelArcDegree(QPoint ptPix)
{
	QPointF pt1 = ConvertToWGS(ptPix);
	ptPix.rx()++;
	QPointF pt2 = ConvertToWGS(ptPix);
	return common::Deg(pt2.x()-pt1.x());
}

// ----------------------------------------------------------------------------
} // namespace
