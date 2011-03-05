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

#include <math.h>
#include "SphereNavigation.h"
#include "FlatEarth.h"

namespace map {

// ----------------------------------------------------------------------------

FlatEarth::FlatEarth(const QPointF& ptO)
{
	SetOrigin(ptO);
}

FlatEarth::~FlatEarth()
{
}

// ----------------------------------------------------------------------------

void FlatEarth::SetOrigin(const QPointF& ptO)
{
	m_ptO = ptO;
	m_f   = cos(m_ptO.y());
}
// ----------------------------------------------------------------------------

QPointF FlatEarth::GetCoordinate(float fH_rad, float fD_m) const
{
	float fdN = cos(fH_rad)*fD_m/EARTH_RADIUS_M;     // north [m]
	float fdE = sin(fH_rad)*fD_m/EARTH_RADIUS_M/m_f; // east [m]
	return QPointF(m_ptO.x()+fdE, m_ptO.y()+fdN);
}

// ----------------------------------------------------------------------------

/*QPointF FlatEarth::GetManhattanDistance(const QPointF& pt) const
{
	std::pair<float,float> bd = common::GetBearingDistanceFlat(
		m_ptO.x(),
		m_ptO.y(),
		pt.x(),
		pt.y()
	);
	bd.second *= EARTH_RADIUS_M;
	
	return QPointF(cos(bd.first)*bd.second, sin(bd.first)*bd.second);
}*/
// ----------------------------------------------------------------------------

std::pair<float,float> FlatEarth::GetBearingDistance(const QPointF& pt) const
{
	std::pair<float,float> bd = common::GetBearingDistanceFlat(
		m_ptO.x(), m_ptO.y(),
		pt.x(), pt.y()
	);
	bd.second *= EARTH_RADIUS_M;
	return bd;
}

// ----------------------------------------------------------------------------
} // namespace
