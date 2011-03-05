#ifndef DIALOG_WAYPOINT_H
#define DIALOG_WAYPOINT_H

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
#include "AbstractNesisInput.h"
#include "Route/Waypoint.h"

class NesisCheckBox;
class NesisTableView;
class NesisSpinBox;
class NesisEditBox2;
class QLabel;
class ModelWaypointTable;

class DialogWaypoint : public AbstractNesisDialog
{
Q_OBJECT
private:
	typedef QVector<const route::Waypoint*> Container;
public:
	//! Constructor
	DialogWaypoint(const QPointF& ptX, bool bDeactivate=false, QWidget *pParent = 0);
	//! Destructor
	~DialogWaypoint();

	//! Return required waypoint.
	const route::Waypoint* GetWaypoint() const;


private slots:
	//! Update the main selection widget according to the settings.
	void OnUpdateList();

	//! Activate list of hits in the edit mode.
	void SetFocusToHits();
	
	//! On button control
	void OnNesisButton(int iBtn);
	//! On button control2
	void OnNesisButton2(int iBtn);
	
	//! Focus to the next/previous type.
	void OnFocusNextType(bool bNext);
	
	//! Name (filter) Ok was pressed on name editor.
	void OnNameOk();

private:
	//! Buttons.
	QStringList m_slButtons;

	//! Current coordinate.
	QPointF m_ptX;

	//! Filter
	NesisEditBox2* m_pedtFilter;
	
	//! Airfield checkbox.
	NesisCheckBox* m_pcbAirfield;
	//! NavAid checkbox.
	NesisCheckBox* m_pcbNavAid;
	//! VFR checkbox.
	NesisCheckBox* m_pcbVFRPoint;
	//! User checkbox.
	NesisCheckBox* m_pcbUserPoint;
	
	//! Spin box - radius filter
	NesisSpinBox* m_pspnRadius;

	//! Table - list the filter results.
	NesisTableView* m_ptblHits;

	//! Comment - label
	QLabel* m_plblComment;

	//! Model which stores the container of hits and works with the QTableView.
	ModelWaypointTable* m_pMod;
	
	//! Is the deactivate button active.
	bool m_bDeactivate;
	//! Was the deactivate button pressed.
	bool m_bWasDeactivated;
};

#endif
