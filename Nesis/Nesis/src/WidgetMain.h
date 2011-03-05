#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

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

#include "AbstractPanel.h"
#include "PacketComm/Serial.h"
#include "PacketComm/Mabu.h"

#include <QBasicTimer>
#include <QDomDocument>

#define ZOOM_LEVEL_COUNT   5

class QPushButton;
class MainInput;
class QTimerEvent;
class ActiveNavigation;
class Settings;
class NesisAerospaceDataManager;
class FastUnitKeys;
class NesisUnitCAN;
class AlarmContainer;

namespace route {
	class Route;
}

// --------------------------------------------------------------------------

class WidgetMain : public QWidget
{
Q_OBJECT
public:
	enum Panel {
		pAttEng,				//!< Attitude + engine panel.
		pHSIMap,				//!< HSI above moving map panel.
		pEngAcq,				//!< Engine + data acquisition panel.
		pCheck,				//!< Check list panel
		pMiscellaneous,	//!< A panel which holds all other panels.
		pEnd
	};
public:
	//! Constructor.
	WidgetMain(const QDomDocument& doc, QSize sz, QWidget* pParent=NULL);
	//! Destructor.
	virtual ~WidgetMain();

	//! Get active panel widget.
	AbstractPanel* GetActivePanel();

	//! Get access to the panel by index.
	AbstractPanel* GetPanel(int iPanel);

	//! Activate given route object.
	void ActivateRoute(const route::Route* pRoute);
	
	//! Is it reboot?
	bool IsReboot() const
	{ return m_bReboot; }
	//! Set reboot flag.
	void SetReboot(bool bEnable=true)
	{ m_bReboot = bEnable; }

private:
	//! Timer event
	void timerEvent(QTimerEvent* pEvent);

	//! Check and adjust the system time.
	void AdjustSystemTime();
	//! Check if route leg needs update.
	void UpdateRouteLeg();
	//! Check alarms and process them if necessary.
	void UpdateAlarms();

	//! Statup dialog.
	void OnStartupDialog();

public slots:
	//! Activate panel number x and hide the others.
	void ActivatePanel(int iPanel);

	//! QNH was triggered. Open QNH dialog ...
	void OnSetQNH();
	//! Heading bug settings
	void OnSetHeading();
	//! Altitude bug settings
	void OnSetAltitude();
	//! Select nearest airfield.
	void OnSelectNearestAirfield();
	//! Next waypoint selection
	void OnSelectWaypoint();
	//! Select route or route leg or direct waypoint.
	void OnSelectRoute();
	//! Set the fuel level value.
	void OnSetFuelLevel();
	//! Initiate the shutdown process.
	void OnShutdown();
	
private slots:
	//! Update senap coordinates - simulation mode.
	void OnSimulate();

	//! Update coordinates request. Used to update maps.
	void OnUpdateMap();

	//! Report used memory.
	void OnReportFreeMemory();

	//! Can message was received.
	void OnCanReceived(uint uiId, uint uirA, uint uirB);
	
	//! On Mabu (SPI) programming
	void OnProgramMabu(bool bStart);
	
private:
	ActiveNavigation* 			m_pAN;
	Settings*						m_pS;
	NesisAerospaceDataManager* m_pND;
	FastUnitKeys* 					m_pFUK;
	NesisUnitCAN* 					m_pNUC;
	AlarmContainer*            m_pAC;

	//! Panels that can be shown as the PanelWidgets on the screen.
	AbstractPanel* m_apPanel[pEnd];
	//! Current panel id.
	Panel m_ePanel;

	//! Bottom panel buttons.
	QVector<QPushButton*> m_conBtn;

	//! Serial connection.
	packetcomm::AbstractPort* m_pSerial;

	//! The timer.
	QBasicTimer m_timer;
	//! Time counter - counts timer events.
	int m_iTimeCounter;
	
	//! Need reboot?
	bool m_bReboot;
};

#endif
