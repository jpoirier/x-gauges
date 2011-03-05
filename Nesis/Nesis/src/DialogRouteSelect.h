#ifndef DIALOG_ROUTE_SELECT_H
#define DIALOG_ROUTE_SELECT_H

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

#include "AbstractNesisDialog.h"
#include "Route/Route.h"

class NesisTableWidget;

class DialogRouteSelect : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogRouteSelect(QWidget* pParent=NULL);
	//! Destructor
	~DialogRouteSelect();

	//! Get selected route - pointer to the actual object.
	const route::Route* GetSelectedRoute() const
	{ return m_pRoute; }

private slots:
	//! One of the nesis buttons was pressed.
	void OnNesisButton(int iBtn);
	//! Ok Button was pressed.
	void OnNesisOk();

private:
	//! Initialize the waypoint table.
	void InitializeTable();
	
private:
	//! Table - list of routes
	NesisTableWidget* m_pwtRts;
	
	//! The selection.
	const route::Route* m_pRoute;
};

#endif
