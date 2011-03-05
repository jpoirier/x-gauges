/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "ActiveNavigation.h"
#include "Settings.h"
#include "Geometry/Line2D.h"
#include "SphereNavigation.h"

ActiveNavigation* ActiveNavigation::s_pInstance = NULL;
// --------------------------------------------------------------------------

ActiveNavigation::ActiveNavigation()
{
	m_uvAltitudeBug.Set(
		Settings::GetInstance()->GetAirspaceAltitudeUnit(),
		0
	);
	m_iHeadingBugDeg = 343;
	m_uiActiveLeg = 0;
}


ActiveNavigation::~ActiveNavigation()
{
}

// --------------------------------------------------------------------------

const route::Waypoint* ActiveNavigation::GetWaypoint() const
{
	// Direct waypoint takes advantage.
	if(m_apDirectWaypoint.get())
		return m_apDirectWaypoint.get();
	// Find current route waypoint
	else if(m_apRoute.get()) {
		return m_apRoute->GetWaypoint(m_uiActiveLeg);
	}
	return NULL;
}

// --------------------------------------------------------------------------

void ActiveNavigation::SetDirectWaypoint(const route::Waypoint* pWpt)
{
	m_apDirectWaypoint = std::auto_ptr<route::Waypoint>(pWpt ? pWpt->Clone() : NULL);
}

// --------------------------------------------------------------------------

void ActiveNavigation::SetRoute(const route::Route* pRt)
{
	m_apRoute = std::auto_ptr<route::Route>(pRt ? pRt->Clone() : NULL); 
	m_uiActiveLeg = 0;
}

// --------------------------------------------------------------------------

void ActiveNavigation::SetActiveLeg(unsigned int uiLeg)
{
	if(m_apRoute.get()) {
		if((int)uiLeg >= m_apRoute->GetWaypoints().GetCount())
			uiLeg = m_apRoute->GetWaypoints().GetCount()-1;
		m_uiActiveLeg = uiLeg;
	}
}

// --------------------------------------------------------------------------

QVector<std::pair<float,float> > ActiveNavigation::GetRouteTimeDistance(
	const QPointF& ptX, //!< Current point
	float fGS			  //!< Current ground speed.
)
{
	return m_apRoute->GetTimeDistance(m_uiActiveLeg, ptX, fGS);
}

// --------------------------------------------------------------------------

bool ActiveNavigation::UpdateRouteLeg(const QPointF& ptX)
{
	// A route must be active
	if(m_apRoute.get()==NULL) 
		return false;
	
	// We need to check if we can switch to next leg.
	// Case 1: Is it time to activate the first leg
	if(m_uiActiveLeg == 0) {
		float fRelPos;
		QPointF ptN;
		ptN = m_apRoute->GetNearestPoint(
			ptX,
			1,
			&fRelPos
		);
		// We activate the first leg when the nearest point (ptN) comes
		// inside the first leg. We detect this calculating relative position
		// to te the leg 1.
		if(fRelPos >= 0.0) {
			m_uiActiveLeg = 1;
			return true;
		}
	}
	// Is this the last leg.
	else if((int)m_uiActiveLeg == m_apRoute->GetWaypoints().GetCount()-1) {
		// keep it this way.
	}
	// intermediate legs
	else {
		using namespace common;
		// Get distance to the end of the current leg.
		QPointF ptE = m_apRoute->GetWaypoint(m_uiActiveLeg)->GetCoordinate();
		float fD = GetDistanceFlat(ptX.x(), ptX.y(), ptE.x(), ptE.y()) * EARTH_RADIUS_M;
		
		// Is it close enough (inside R1)
		if(fD < Settings::GetInstance()->GetSwitchToNextLegRadius()) {
			m_uiActiveLeg++;
			return true;
		}
		
		// Get nearest point to the active leg.
		float fRelPos1;
		float fDistX1;
		QPointF ptN1;
		ptN1 = m_apRoute->GetNearestPoint(
			ptX,
			m_uiActiveLeg,
			&fRelPos1,
			&fDistX1
		);
		
		// Is it past the end.
		if(fRelPos1 >= 1.0) {
			m_uiActiveLeg++;
			return true;
		}
	}
	return false;
}

// --------------------------------------------------------------------------
