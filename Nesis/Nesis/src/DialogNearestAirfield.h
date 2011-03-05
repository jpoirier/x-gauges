#ifndef DIALOG_NEAREST_AIRFIELD_H
#define DIALOG_NEAREST_AIRFIELD_H

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
#include "AbstractNesisInput.h"
#include "Route/Airfield.h"

class ModelWaypointTable;
class NesisTableView;

class DialogNearestAirfield : public AbstractNesisDialog
{
Q_OBJECT
private:
	typedef QVector<const route::Waypoint*> Container;
public:
	//! Constructor
	DialogNearestAirfield(const QPointF& ptX, QWidget *pParent = 0);
	//! Destructor
	~DialogNearestAirfield();

	//! Return required airfield.
	const route::Airfield* GetAirfield() const;

private slots:
	//! Update the main selection widget according to the settings.
	void OnUpdateList();
	
	//! Nesis button was pressed.
	void OnNesisButton(int i);

private:
	//! Current coordinate.
	QPointF m_ptX;
	//! Table - list the filter results.
	NesisTableView* m_ptblHits;
	//! Model which stores the container of hits and works with the QTableView.
	ModelWaypointTable* m_pMod;
	//! ICAO flag
	bool m_bICAO;
};

#endif
