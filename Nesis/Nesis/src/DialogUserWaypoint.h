#ifndef DIALOG_USER_WAYPOINT_H
#define DIALOG_USER_WAYPOINT_H

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

class NesisComboBox;
class NesisDoubleSpinBox;
class NesisEditBox2;
class NesisDoubleSpinBox;
class WidgetQuickMap;
class QGridLayout;
class QFont;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogUserWaypoint : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogUserWaypoint(const QPointF& ptHome, QWidget* pParent = 0);

	//! Destructor
	virtual ~DialogUserWaypoint();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize list of languages.
	void InitializeInputs();
	//! Initalize list of map types.
	void InitializeMap();

	//! Initialize controls for entering longitude and latitude.
	void InitializeSpinBox(QFrame *pF);

	//! Set zoom factor.
	void SetZoom(bool bIn);

	//! Set Spin boxes to our current position.
	void SetPosition(QPointF ptPos);
	//! Get position.
	QPointF GetPosition();

	//! Get waypoint name.
	QString GetWaypointName();

	//! Save the point, close the dialog.
	void OnSave();
	//! Close the dialog.
	void OnClose();
	//! Move to the home position.
	void OnHome();
	
private slots:
	//! External command handler.
	void OnExternalCmd(int);
	//! Position has changed();
	void OnPositionChanged();
	
private:
	//! Map widget.
	WidgetQuickMap* m_pMap;

	//! Spin box for degrees of longitude (E-W).
	NesisDoubleSpinBox* m_psbLonDeg;
	//! Spin box for degrees of latitude (N-S).
	NesisDoubleSpinBox* m_psbLatDeg;
	
	//! Spin box for minutes of longitude.
	NesisDoubleSpinBox* m_psbLonMin;
	//! Spin box for minutes of latitude.
	NesisDoubleSpinBox* m_psbLatMin;

	//! Spin box for seconds of longitude.
	NesisDoubleSpinBox* m_psbLonSec;
	//! Spin box for seconds of latitude.
	NesisDoubleSpinBox* m_psbLatSec;

	//! Waypoint Name.
	NesisEditBox2* m_pleName;
	
	//! The "home" coordinate.
	QPointF m_ptHome;
};

// --------------------------------------------------------------------------

#endif // DIALOG_SETTINGS_H
