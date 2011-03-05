#ifndef __ROUTE_WAYPOINT_CONTAINER_H
#define __ROUTE_WAYPOINT_CONTAINER_H

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

#include <QHash>
#include <QVector>
#include <QRect>
#include "Waypoint.h"

namespace route {

class WaypointContainer
{
public:
	typedef QVector<Waypoint*> Container;
	typedef QVector<const Waypoint*> ConstContainer;
protected:
	typedef QHash<quint16, ConstContainer> SectorContainer;
public:
	//! Constructor
	WaypointContainer();
	//! Copy constructor
	WaypointContainer(const WaypointContainer& C);
	//! Destructor
	~WaypointContainer();
	
	//! Copy (deep).
	void Copy(const WaypointContainer& C);

	//! Clear - make the container empty.
	void Clear();
	
	//! Return number of items in the container.
	int GetCount() const
	{ return m_con.size(); }

	//! Insert given waypoint object into the container. We own the object.
	void Insert(int iPos, Waypoint* pWpt);
	
	//! Erase waypoint at given index.
	void Erase(int iPos);
	
	//! Return const access to the waypoint.
	const Waypoint* GetWaypoint(int i) const
	{ return m_con[i]; }
	//! Return const access to the waypoint.
	const Waypoint* operator[](int i) const
	{ return m_con[i]; }
	
	//! Get requested subset of turning points.
	QVector<const Waypoint*> Search(
		const QPointF& ptCoor, int iTypes, 
		int iRadius_km, const QString& qsName) const;

	//! Load container form the original WELT2000 file.
	int LoadW2000(const QString& qsFile);
	//! Load CSV file of VFR points.
	int LoadVFRCSV(const QString& qsFile);
	//! Load CSV file of radio navigation aids.
	int LoadNavAidsCSV(const QString& qsFile);
	//! Save the container into given file using binary format.
	bool Save(const QString& qsFile) const;
	//! Load the container from given file in native format.
	int Load(const QString& qsFile, const QPointF& ptX=QPointF(), int iRadiusKm=0);

	//! Write the container into given stream.
	bool Write(QDataStream& out) const;
	//! Read the container from given stream.
	bool Read(QDataStream& in, const QPointF& ptX=QPointF(), int iRadiusKm=0);
	
	//! Get Airfields that are within the given rect in radians.
	QVector<const Waypoint*> GetPointsInRect(const QRectF& rcRad) const;
	
	//! Return if the container is dirty - has changed.
	bool IsDirty() const
	{ return m_bDirty; }
	
	//! Reverse the waypoints in the container.
	void Reverse();
	
	//! Create container with sector based pointers for fast access objects in sector.
	void CreateSectorContainer();
	//! Get waypoints is given sector.
	ConstContainer GetSectorWaypoints(quint16 uiSector) const;
	
private:
	//! A helper which tells type of the turning point described by baDataLine.
	bool IsAirfieldW2000(const QByteArray& baDataLine) const;
	
private:
	//! Linear container of points.
	Container m_con;	
	//! Waypoints container organised in 1x1 map sectors key.
	SectorContainer m_sec;
	//! Dirty flag - tells if the container has changed.
	mutable bool m_bDirty;
};

// --------------------------------------------------------------------

}

#endif
