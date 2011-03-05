#ifndef ACTIVE_NAVIGATION_H
#define ACTIVE_NAVIGATION_H

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

#include <memory>
#include "Route/Route.h"
#include "Unit/Value.h"

// --------------------------------------------------------------------------
//! Stores active navigation objects (Waypoint and Route).

class ActiveNavigation
{
protected:
	//! Constructor
	ActiveNavigation();
public:
	//! Destructor.
	~ActiveNavigation();

	//! We can have only one such object.
	static ActiveNavigation* GetInstance()
	{
		if(s_pInstance == NULL)
			s_pInstance = new ActiveNavigation();
		return s_pInstance;
	}

	//! Get the current waypoint object. It may be NULL.
	const route::Waypoint* GetWaypoint() const;

	//! Set the direct waypoint object. We own the object here.
	void SetDirectWaypoint(const route::Waypoint* pWpt);

	//! Return true if the DirectWaypoint is active.
	bool IsDirectWaypoint() const
	{ return m_apDirectWaypoint.get() != NULL; }

	//! Get the route object. It may be NULL.
	const route::Route* GetRoute() const
	{ return m_apRoute.get(); }
	//! Set the route object. We own the object here.
	void SetRoute(const route::Route* pRt);

	//! Get current active leg
	unsigned int GetActiveLeg() const
	{ return m_uiActiveLeg; }
	//! Set active route leg
	void SetActiveLeg(unsigned int uiLeg);

	//! Time, distances to each waypoint in the route.
	QVector<std::pair<float,float> > GetRouteTimeDistance(const QPointF& ptX, float fGS);
	
	//! Update route (leg switching)
	bool UpdateRouteLeg(const QPointF& ptX);

	// Heading bug

	//! Get current heading bug settings [degrees].
	int GetHeadingBugDeg() const
	{ return m_iHeadingBugDeg; }
	//! Set current heading bug settings [degrees].
	void SetHeadingBugDeg(int iHeadingBugDeg)
	{ m_iHeadingBugDeg = iHeadingBugDeg; }

	// Altitude bug

	//! Get current altitude bug settings [meters].
	const unit::Value& GetAltitudeBug() const
	{ return m_uvAltitudeBug; }
	//! Set current altitude bug settings [meters].
	void SetAltitudeBug(unit::Value uvAltitudeBug)
	{ m_uvAltitudeBug = uvAltitudeBug; }

private:
	static ActiveNavigation* s_pInstance;

	//! The active route object.
	std::auto_ptr<route::Route> m_apRoute;
	//! Current active leg on the route.
	unsigned int m_uiActiveLeg;

	//! The active direct waypoint object.
	std::auto_ptr<route::Waypoint> m_apDirectWaypoint;

	//! Heading bug (degrees)
	int m_iHeadingBugDeg;
	//! Altitude bug (meters)
	unit::Value m_uvAltitudeBug;
};

// --------------------------------------------------------------------------
#endif
