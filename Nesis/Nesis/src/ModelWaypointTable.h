#ifndef __MODEL_WAYPOINT_TABLE_H
#define __MODEL_WAYPOINT_TABLE_H

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

#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include "Route/Waypoint.h"

class ModelWaypointTable : public QAbstractTableModel
{
Q_OBJECT
private:
	struct Data {
		const route::Waypoint* m_pTP;
		float m_fBearing;	
		float m_fDistance;
		
		//! Used to sort the data
		bool operator()(const Data& a, const Data& b)
		{ return a.m_fDistance < b.m_fDistance; }
	};
	typedef QVector<Data> Container; 
public:
	//! Constructor
	ModelWaypointTable(QObject *pParent = 0);
	//! Destructor
	~ModelWaypointTable();
	
	//! Cet current coordinate
	void SetCoordinates(const QPointF& ptX)
	{ m_ptX = ptX; }
	//! Set container of waypoints ...
	void SetWaypoints(const QVector<const route::Waypoint*>& conWayPts);
	
	//! Set current heading.
	void SetHeading(float fHeading)
	{ m_fHeading = fHeading; }
	
	//! Return waypoint of given index.
	const route::Waypoint* GetWaypoint(int index) const;

	//! Number of rows = number of items in container.
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	//! Number of columns.
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

	//! Return content of cell specified by index.
	virtual QVariant data(
		const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! Used to define header of the table
	virtual QVariant headerData (
		int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

protected:
	//! Internal container of waypoints.
	Container m_conWayPts;
	//! Names used in header.
	QStringList m_qlHeader;
	//! Current coordinate
	QPointF m_ptX;
	//! Current heading [rad]
	float m_fHeading;
};

#endif
