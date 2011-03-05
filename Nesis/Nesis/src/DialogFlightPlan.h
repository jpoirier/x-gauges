#ifndef DIALOG_FLIGHT_PLAN_H
#define DIALOG_FLIGHT_PLAN_H

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

#include "AbstractNesisDialog.h"

#include <QModelIndex>

class NesisDoubleSpinBox;
class NesisListView;
class NesisTableView;
class QLabel;
class QItemSelectionModel;
class ModelRouteList;
class ModelRouteWptTable;
class QLineEdit;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogFlightPlan : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogFlightPlan(QWidget* pParent);

	//! Destructor
	virtual ~DialogFlightPlan();

private slots:
	//! Respond on the Route based commands.
	void OnRouteCommand(int iCmd);
	//! Respond on the Waypoint based commands.
	void OnWaypointCommand(int iCmd);
	//! Respond on the Speed based commands.
	void OnSpeedCommand(int iCmd);

	//! Create new Route and start editing.
	void OnNewRoute();
	//! Edit selected route.
	void OnEditRoute();
	//! Delete route.
	void OnDeleteRoute();
	//! Reverse route.
	void OnReverseRoute();
	//! Save route and close.
	void OnClose();
	//! Rename the route
	void OnRenameRoute();
	
	//! Insert a new waypoint infront of selection.
	void OnInsertWpt();
	//! Append a new waypoint at the back.
	void OnAppendWpt();
	//! Delete selected waypoint.
	void OnDeleteWpt();
	
	//! A change in speed.
	void OnSpeedChanged(int iSpeed);
	//! A route selection has changed.
	void OnCurrentRowChanged(const QModelIndex& miNew, const QModelIndex& miOld);
	
private:
	//! Insert, append waypoint. Ask for the waypoint first.
	void InsertWaypoint(int iPos);
	//! Set row sizes.
	void SetWaypointTableRowSizes();
private:

	//! Speed spin box.
	NesisDoubleSpinBox* m_psbSpeed;
	//! Estimated time of departure
	QLineEdit* m_pleDeparture;

	//! Route list
	NesisListView* m_plstRoute;
	//! Waypoint list (table)
	NesisTableView* m_ptblWaypoint;
	//! Label above the waypoint list.
	QLabel* m_plblWaypoint;

	//! Model for route names list.
	ModelRouteList* m_pmodRoute;
	//! Model for waypoints in the route.
	ModelRouteWptTable* m_pmodWpt;
};

// --------------------------------------------------------------------------

#endif // DIALOG_FLIGHT_PLAN_H
