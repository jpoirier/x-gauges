#ifndef DIALOG_ROUTE_LEG_H
#define DIALOG_ROUTE_LEG_H

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

class DialogRouteLeg : public AbstractNesisDialog
{
Q_OBJECT
public:
	enum Action {
		aInvalid,		//!< Invalid action.
		aSelectLeg,		//!< Select new leg from the Route.
		aDirectTo,		//!< Select a waypoint in direct to mode.
		aDeactivate		//!< Deactivate route.
	};
	//! Constructor
	DialogRouteLeg(const route::Route* pR, unsigned int uiActiveLeg, 
						QPointF ptX, float fGS, QWidget *pParent = 0);
	//! Destructor
	~DialogRouteLeg();

	//! Get selected row (leg, waypoint id).
	int GetSelectedRow() const
	{ return m_iRow; }
	//! Get selected action.
	Action GetAction() const
	{ return m_eAction; }

private slots:
	//! One of the nesis buttons was pressed.
	void OnNesisButton(int iBtn);
	//! Ok Button was pressed.
	void OnNesisOk();

private:
	//! Initialize the waypoint table.
	void InitializeTable();
	
private:
	//! Table - list the filter results.
	NesisTableWidget* m_pwtWpt;

	//! The route
	const route::Route* m_pRoute;
	//! Current active leg.
	unsigned int m_uiActiveLeg;
	//! Current point.
	QPointF m_ptX;
	//! Current ground speed.
	float m_fGS;	

	//! Current row = current waypoint, leg.
	int m_iRow;

	//! What kind of action we have?
	Action m_eAction;
};

#endif
