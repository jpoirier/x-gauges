#ifndef __ROUTE_ROUTE_H
#define __ROUTE_ROUTE_H

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

#include "WaypointContainer.h"

namespace route {

// -------------------------------------------------------------------------
/** \brief Route is a short container of waypoints.
	 Route object owns its waypoints.
 */	
class Route
{
	
public:
	//! Default constructor.
	Route(const QString& qsName = QString());
	//! Copy constructor.
	Route(const Route& C);
	//! Destructor
	virtual ~Route();

	//! Clear the route - delete all waypoints and reset the name.
	void Clear();

	//! Create a clone (deep copy) of this object.
	virtual Route* Clone() const
	{ return new Route(*this); }

	//! Insert given waypoint object into the container. We own the object.
	void Insert(int iPos, Waypoint* pWpt);
	
	//! Erase waypoint at given index.
	void Erase(int iPos);
	
	//! Set name of the route.
	void SetName(const QString& qsName)
	{ m_qsName = qsName; }
	//! Return route name.
	QString GetName(bool bTrueName=false) const;
	
	//! Return the name obtained from waypoints (first-last) point
	QString GetNameFromWaypoints() const;

	//! Write the route into given stream.
	void Write(QDataStream& out) const;
	//! Read the route from given stream.
	void Read(QDataStream& in);

	//! Allow const access to the waypoint container.
	const WaypointContainer& GetWaypoints() const
	{ return m_conWpt; }

	//! Return pointer to the i-th waypoint in the route.
	const Waypoint* GetWaypoint(int i) const;

	//! Return bearing-distance pair for i-th Leg. (1 is the first valid leg)
	std::pair<float, float> GetBearingDistance(int iLeg) const;

	//! Return time to reach and distance to each waypoint according to given active leg.
	QVector<std::pair<float,float> > GetTimeDistance(
		unsigned int uiActiveLeg, const QPointF& ptX, float fGS) const;

	//! Return true if the content needs to be saved.
	bool IsDirty() const
	{ return m_conWpt.IsDirty(); }

	//! Return route distance in m.
	float GetTotalDistance() const;

	//! Get route waypoint coordinates in vector form.
	QVector<QPointF> GetCoordinates() const;
	
	//! Project current position onto active leg (nearest -perpendicular point).
	QPointF GetNearestPoint(
		const QPointF& pt0, int iLeg,
		float* pfRelPos=NULL, float* pfDist=NULL) const;
		
	//! Check if the route is valid. 
	bool IsValid() const;
	
	//! Reverse the route. We actually reverse the waypoints.
	void Reverse();
private:
	//! The name of the route.
	QString m_qsName;
	//! The container of waypoints - this is a route.
	WaypointContainer m_conWpt;
};

// -------------------------------------------------------------------------

} // namespace

#endif
