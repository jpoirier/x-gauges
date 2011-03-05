#ifndef __MODEL_ROUTE_WPT_TABLE_H
#define __MODEL_ROUTE_WPT_TABLE_H

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
#include <QStringList>
#include "Route/Route.h"

// -------------------------------------------------------------------------
/** \brief Model is used to display Route Waypoints in table view.
 */
class ModelRouteWptTable : public QAbstractTableModel
{
Q_OBJECT
public:
	//! Constructor
	ModelRouteWptTable(QObject *pParent = 0);
	//! Destructor
	~ModelRouteWptTable();

	//! Set current route.
	void SetRoute(route::Route* pRoute);

	//! Set new speed in m/s.
	void SetSpeed(float fSpeed);

	//! Return true if the route is valid.
	bool IsValid() const
	{ return m_pRoute != NULL; }

	//! Get waypoint coordinates. Take current coordinates if container is empty.
	QPointF GetCoordinates(int iPos) const;
	
	//! Insert new waypoint. We make a copy of the waypoint.
	QModelIndex Insert(int iPos, const route::Waypoint* pW);
	//! Erase (delete) selected waypoint.
	QModelIndex Erase(int iPos);

	//! Number of rows = number of items in container.
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	//! Number of columns.
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	//! Return content of cell specified by index.
	virtual QVariant data(
		const QModelIndex & index, 
		int role = Qt::DisplayRole) const;

	//! Used to define header of the table
	virtual QVariant headerData (
		int section, 
		Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

private:
	//! We have a pointer to the current route.
	route::Route* m_pRoute;
	//! Names used in header.
	QStringList m_qlHeader;
	//! Speed in m/s
	float m_fSpeed;
};
// -------------------------------------------------------------------------

#endif
