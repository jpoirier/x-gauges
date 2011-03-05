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

#include <QtGui>
#include <QtXml>
#include <QThread>

#include "CommonDefs.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "MathEx.h"
#include "Map/World.h"
#include "Map/MapImageZoom.h"

#include "AbstractNesisInput.h"

#include "PanelClassic.h"
#include "PanelHSIMap.h"
#include "PanelEngine.h"
#include "PanelModern.h"
#include "PanelCheckList.h"
#include "PanelMiscellaneous.h"

#include "NesisAerospaceDataManager.h"
#include "GraphicsSingleton.h"
#include "Logbook/Logbook.h"

#include "WidgetMessage.h"
#include "DialogQNH.h"
#include "DialogHeading.h"
#include "DialogAltitude.h"
#include "DialogWaypoint.h"
#include "DialogRouteLeg.h"
#include "DialogRouteSelect.h"
#include "DialogFuelLevel.h"
#include "DialogShutDown.h"
#include "DialogNearestAirfield.h"
#include "DialogStartup.h"

#include "PacketComm/PackageProcessor.h"
#include "PacketComm/CanEIAPackageEmiter.h"

#include "Route/RouteContainerSingleton.h"
#include "NesisUnitCAN.h"
#include "CanUnitInfo/UnitInfoDaqu.h"
#include "ActiveNavigation.h"
#include "AlarmContainer.h"

#include "WidgetMain.h"

#define BOTTOM_BTN_SPACE			17
#define BOTTOM_BTN_COUNT			5

#define EVERY_5MIN					5*600+1
#define EVERY_60S						601
#define EVERY_20S						199
#define EVERY_5S						53
#define EVERY_100MS					1
#define EVERY_MAJOR_DRAW			10
#define EVERY_MINOR_DRAW			1
#define EVERY_1S						10

// --------------------------------------------------------------------------

WidgetMain::WidgetMain(
	const QDomDocument& doc,
	QSize sz,
	QWidget* pParent
):
 QWidget(pParent, Qt::FramelessWindowHint )
 //,  m_pSerial(QString("/dev/mabu"))
{
	m_iTimeCounter = 0;
	m_bReboot = false;

	// Initialize singletons
	m_pAN  = ActiveNavigation::GetInstance();
	m_pS   = Settings::GetInstance();
	m_pND  = NesisAerospaceDataManager::GetInstance();
	m_pFUK = FastUnitKeys::GetInstance();
	m_pAC  = AlarmContainer::GetInstance();

	// Serial port stuff
	using namespace packetcomm;

	#if defined(QT_ARCH_ARM)
		m_pSerial = new Mabu("/dev/mabu");
		m_pSerial->SetProcessor(m_pND, false); // false = don't own the processor.

		m_pSerial->Open();
		if (!m_pSerial->IsOpen()) {
			qWarning("Failed to open connection with MABU.");
		}
	#else
		// Package processor for serial port.
		m_pSerial = new Serial();
		PackageProcessor* pPP = new PackageProcessor;

		CanEIAPackageEmiter* pPackage = new CanEIAPackageEmiter(this);
		pPP->SetPackage(pPackage, false); 	// false = NOT owned by processor.

		((Serial*)m_pSerial)->SetPort("/dev/ttyUSB0");
		((Serial*)m_pSerial)->SetBaudrate(500000);
		m_pSerial->SetProcessor(pPP);				// owned by serial port.
		m_pSerial->Open();

		connect(pPackage, SIGNAL(CanReceived(uint,uint,uint)),
				  this, SLOT(OnCanReceived(uint,uint,uint)));
	#endif

	m_pNUC = NesisUnitCAN::GetInstance();
	m_pNUC->SetPort(m_pSerial);
	m_pNUC->SetAerospaceDataManager(m_pND);

	connect(m_pNUC, SIGNAL(ProgramMabu(bool)),
			  this, SLOT(OnProgramMabu(bool)));

	// Old "Scan" based system for unit identification. It should not be used
	// anymore.
// 	if(m_pSerial->IsOpen())
// 		m_pNUC->GetUnitInfoContainer().Scan();

	// Layout stuff
	setFixedSize(sz);

	// NOTE Do not use QStackedLayout or any other
	// QLayout base class instead current direct solution.
	// Any QLayout based classes were hiding panel buttons
	// and I didn't find a solution to fix this problem.

	// Load the XML shape file, which describes
//	QDomDocument doc = Load

	qDebug() << "Before Classic";

	// Attitude + Engine + HSI widget.
	m_apPanel[pAttEng] = new PanelClassic(doc, sz, this);
	qDebug() << "Classic";
	// Moving Map + HSI widget.
	m_apPanel[pHSIMap] = new PanelHSIMap(sz, this);
	qDebug() << "HSI";
	// Engine + Data Acquisition widget.
//	m_apPanel[pEngAcq] = new PanelChecklist(sz, this);
	m_apPanel[pEngAcq] = new PanelEngine(doc, sz, this);
	qDebug() << "Engine";
	// Checklist panel
/*	m_apPanel[pCheck] = new PanelChecklist(sz, this);
	qDebug() << "Checklist";*/
	// Checklist panel
	m_apPanel[pCheck] = new modern::PanelModern(doc, sz, this);
	qDebug() << "Modern";
	// Miscellaneous panel
//	m_apPanel[pMiscellaneous] = new PanelChecklist(sz, this);
	m_apPanel[pMiscellaneous] = new PanelMiscellaneous(sz, this);
	qDebug() << "Misc.";

	// Bottom buttons - hidden by default.
	int iBtnWidth = (sz.width()-6*BOTTOM_BTN_SPACE)/5;
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	m_conBtn.resize(BOTTOM_BTN_COUNT);
	for(int i=0; i<BOTTOM_BTN_COUNT; i++) {
		m_conBtn[i] = new QPushButton("Test", this);
		// Calculate the size
		m_conBtn[i]->setGeometry(
			BOTTOM_BTN_SPACE + i*(iBtnWidth+BOTTOM_BTN_SPACE),
			sz.height()-44,
			iBtnWidth,
			39
// 			31
		);
		// Set the font
		m_conBtn[i]->setFont(pGS->GetFont(GraphicsSingleton::fButton));
		// Hide buttons by default.
		m_conBtn[i]->setVisible(false);
		m_conBtn[i]->setFocusPolicy(Qt::NoFocus);
	}

	// Set the shared resource for all Input objects.
	AbstractNesisInput::SetButtons(m_conBtn);

	// Message widget
	WidgetMessage::CreateInstance(this);
	WidgetMessage* pwMessage = WidgetMessage::GetInstance();
	pwMessage->setGeometry(30,sz.height()-115,sz.width()-60, 70);
	pwMessage->setVisible(false);

	// Activate the main panel
	ActivatePanel(0);

	// We use only one timer -
	m_timer.start(100, this);

	// Maybe wa already have a time and date? The airu might be already running.
	AdjustSystemTime();
}

// --------------------------------------------------------------------------

WidgetMain::~WidgetMain()
{
	m_timer.stop();
	map::SingletonMapImageZoom::GetInstance()->Terminate();
	qDebug() << "Terminating MapImage threads.";
	m_pSerial->Close();
	qDebug() << "Mabu Closed.";
}

// --------------------------------------------------------------------------

AbstractPanel* WidgetMain::GetActivePanel()
{
	return (AbstractPanel*)m_apPanel[m_ePanel];
}

// --------------------------------------------------------------------------

AbstractPanel* WidgetMain::GetPanel(int iPanel)
{
	return (AbstractPanel*)m_apPanel[iPanel];
}

// --------------------------------------------------------------------------

void WidgetMain::timerEvent(QTimerEvent* pEvent)
{
	// We should get this event 10 times per second.
	if(pEvent->timerId() == m_timer.timerId()) {
		pEvent->accept();
		m_iTimeCounter++;

		// depending on the counter we dispatch the events;

		// every 20 seconds
/*		if(m_iTimeCounter % EVERY_20S == 0) {
 			OnReportFreeMemory();
		}*/
		// every 5 seconds
		if(m_iTimeCounter % EVERY_5S == 0) {
 			OnUpdateMap();
			AdjustSystemTime();
		}
		if(m_iTimeCounter % EVERY_1S == 0) {
			logbook::Logbook::GetInstance()->Update();
// 			m_pND->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_ENABLE);
   		//OnSimulate();
   		UpdateRouteLeg();
   		UpdateAlarms();
		}
		if(m_iTimeCounter % EVERY_MINOR_DRAW == 0) {
			GetActivePanel()->Draw(m_iTimeCounter % EVERY_MAJOR_DRAW == 0);
		}

 		if(m_iTimeCounter==11) {
 			OnStartupDialog();
 		}
	}
}

// --------------------------------------------------------------------------

void WidgetMain::ActivatePanel(int iPanel)
{
	ASSERT(iPanel>=0 && iPanel<pEnd);

	// Set new panel
	m_ePanel = static_cast<Panel>(iPanel);

	// Make the panel visible and fresh
	for(int i=0; i<pEnd; i++)
		m_apPanel[i]->setVisible(iPanel==i);

// 	qDebug() << "Before update" << iPanel;
	GetActivePanel()->update();
// 	qDebug() << "After update" << iPanel;

	// Give new panel the chance to set focus, ...
	GetActivePanel()->Activate(/*TODO ePreviousPanel ???*/);
// 	qDebug() << "After activate";
}

// --------------------------------------------------------------------------

void WidgetMain::OnReportFreeMemory()
{
	QFile fMem("/proc/meminfo");
	if (fMem.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&fMem);
		QString qsLine;
		qsLine = in.readLine();
		qsLine = in.readLine();
		std::cout << " ### " << qsLine.toAscii().constData() << std::endl << std::flush;
		qsLine = in.readLine();
		qsLine = in.readLine();
		std::cout << " ### " << qsLine.toAscii().constData() << std::endl << std::flush;
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnSetQNH()
{
	// QNH is always stored in hPa. We need to convert current
	// value into user units.
	unit::Value uvQNH(m_pFUK->iKey_hPa, m_pND->GetQNH());
	uvQNH.ConvertTo(m_pS->GetQNHUnit());

	// Show the dialog
	DialogQNH dlg(uvQNH);
	if(dlg.exec()==QDialog::Accepted) {
		// Store the results in hPa!
		qreal fNewQNH = dlg.GetQNH().GetAs(m_pFUK->iKey_hPa);

		// Write new QNH into Airu
		const can::UnitInfoAiru* pA = m_pNUC->GetUnitInfoContainer().GetAiru();
		if(pA) {
			m_pND->ConfigureModule(
				pA->GetNodeId(),
				MCS_AIRU_QNH,
				can::dtFloat,
				(float)fNewQNH
			);
			// Show in user units.
			WidgetMessage::GetInstance()->SetText(
				tr("New QNH <font color=green size=+1>%1</font> was set.").arg(dlg.GetQNH().Format(0)),
				3,
				WidgetMessage::lNormal
			);
		}
		else {
			qWarning() << "Error: OnSetQNH() Airu was not detected.";
		}
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnSetHeading()
{
	DialogHeading dlg(m_pAN->GetHeadingBugDeg());
	if(dlg.exec()==QDialog::Accepted) {
		int iH = dlg.GetHeading();
		if(iH >= 0 && iH <= 360) {
			m_pAN->SetHeadingBugDeg(iH);
			WidgetMessage::GetInstance()->SetText(
				tr("Heading <font color=green size=+1>%1</font> was set.").arg(iH, 3, 10, QChar('0')),
				3,
				WidgetMessage::lNormal
			);
		}
		else {
			WidgetMessage::GetInstance()->SetText(
				tr("Invalid heading <font color=red size=+1>%1</font>! The command was ignored.").arg(iH),
				10,
				WidgetMessage::lWarning
			);
		}
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnSetAltitude()
{
	// Altitude is always stored in meters. We need to convert current
	// value into user units.

	// Show dialog
	DialogAltitude dlg(m_pAN->GetAltitudeBug());
	if(dlg.exec()==QDialog::Accepted) {
		// Store the results in meters!
		m_pAN->SetAltitudeBug(dlg.GetAltitude());
		// Show message in user units.
		WidgetMessage::GetInstance()->SetText(
			tr("New altitude <font color=green size=+1>%1</font> was set.").arg(dlg.GetAltitude().Format(0)),
			3,
			WidgetMessage::lNormal
		);
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnUpdateMap()
{
	int iR = 0;
	for(int i=0; i<pEnd; i++) {
		iR = qMax(iR, GetPanel(i)->GetMapRadius());
	}

	if(iR==0) {
		iR = (int)sqrt(width()*width()/4 + height()*height()*4/9)+2;
	}

	// Get coordinates from Senap.
	NesisAerospaceDataManager* m_pND = NesisAerospaceDataManager::GetInstance();
	QPointF ptCoor = m_pND->GetPosition();

	// Update maps.
	map::MapImageZoom* pMIZ = map::SingletonMapImageZoom::GetInstance();
	pMIZ->UpdateImages(ptCoor, iR);
}

// --------------------------------------------------------------------------

void WidgetMain::OnSelectWaypoint()
{
	DialogWaypoint dlg(m_pND->GetPosition(), m_pAN->GetWaypoint()!=NULL);
	if(dlg.exec()==QDialog::Accepted) {
		const route::Waypoint* pW = dlg.GetWaypoint();
		m_pAN->SetDirectWaypoint(pW);
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnSelectNearestAirfield()
{
	DialogNearestAirfield dlg(m_pND->GetPosition());
	if(dlg.exec()==QDialog::Accepted) {
		const route::Airfield* pA = dlg.GetAirfield();
		qDebug() << "Airfield" << pA->GetDescription() << "selected.";
		m_pAN->SetDirectWaypoint(pA);
	}
}

// --------------------------------------------------------------------------

void WidgetMain::ActivateRoute(const route::Route* pRoute)
{
	m_pAN->SetRoute(pRoute);
	if(pRoute) {
		m_pAN->SetDirectWaypoint(NULL);
		// Show message in user units.
		WidgetMessage::GetInstance()->SetText(
			tr("New route <font color=green size=+1>%1</font> was activated.").arg(pRoute->GetName()),
			3,
			WidgetMessage::lNormal
		);
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnSelectRoute()
{
	using namespace route;
	const Route* pRoute = m_pAN->GetRoute();
	if(pRoute) {
		DialogRouteLeg dlg(pRoute, m_pAN->GetActiveLeg(),
			m_pND->GetPosition(), m_pND->GetGroundSpeed()
		);
		if(dlg.exec()==QDialog::Accepted) {
			switch(dlg.GetAction()) {
			// Should we deactivate the route?
			case DialogRouteLeg::aDeactivate :
				m_pAN->SetRoute(NULL);
				m_pAN->SetDirectWaypoint(NULL);
				break;
			// Is it a leg?
			case DialogRouteLeg::aSelectLeg :
				if(dlg.GetSelectedRow()!=0) {
					m_pAN->SetActiveLeg(dlg.GetSelectedRow());
					m_pAN->SetDirectWaypoint(NULL);
					break;
				}
			case DialogRouteLeg::aDirectTo :
				m_pAN->SetDirectWaypoint(pRoute->GetWaypoint(dlg.GetSelectedRow()));
				break;
			default:
				break;
			}
		}
	}
	// If there is no active route, select one
	else {
		// We can select a route only if there is at least one in container.
		RouteContainerSingleton* pRC = RouteContainerSingleton::GetInstance();
		int iRouteCount = pRC->GetCount();
		if(iRouteCount > 0) {
			DialogRouteSelect dlg;
			if(dlg.exec()==QDialog::Accepted) {
				ActivateRoute(dlg.GetSelectedRoute());
			}
		}
		else {
			// TODO Message box displays info that route must be created first.
		}
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnSetFuelLevel()
{
	// Get current fuel level.
	unit::Value uvFL = unit::Value(m_pFUK->iKey_l, m_pND->GetTotalFuelLevel());
	uvFL.ConvertTo(m_pS->GetFuelVolumeUnit());

	// Ask for new level?
	DialogFuelLevel dlg(uvFL);
	if(dlg.exec()==QDialog::Accepted) {
		uvFL = dlg.GetFuel();
		qDebug() << "Fuel level = " << uvFL.GetAs("l") << "liters";

		// Write new level into daqu
		const can::UnitInfoDaqu* pD = m_pNUC->GetUnitInfoContainer().GetDaqu();
		if(pD) {
			m_pND->ConfigureModule(
				pD->GetNodeId(),
				MCS_DAQU_FUEL_LEVEL,
				can::dtFloat,
				(float)uvFL.GetAs(m_pFUK->iKey_l)
			);
		}
		else {
			qWarning() << "Error: OnSetFuelLevel() Daqu was not detected.";
		}
	}
}
// --------------------------------------------------------------------------

void WidgetMain::OnShutdown()
{
	static bool bActive = false;

	if (bActive)
		return;

	bActive = true;
	DialogShutDown dlg;
	if(dlg.exec()==QDialog::Accepted) {
		// We need to close any other open dialogs.
		QWidget* pW = qApp->activeModalWidget();
		if(pW) {
			AbstractNesisDialog* pD = dynamic_cast<AbstractNesisDialog*>(pW);
			if(pD) {
				qDebug() << "An active dialog is detected. Trying to close it down.";
				pD->reject();
				QTime tm;
				tm.start();
				while(tm.elapsed() < 500) {
					qApp->processEvents();
				}
			}
		}
		close();
	}
	bActive = false;
}

// --------------------------------------------------------------------------
/** We can use this auto-correction only if the difference is small enough.
    This prevents changing time into something meaningless. Once the time
    was set, we do not verify it anymore.
*/

void WidgetMain::AdjustSystemTime()
{
	// A flag will be set when the time is adjusted.
	static bool bTimeChecked = false;
	if(bTimeChecked==false) {
		// We can't update time if recording has already started.
		if(logbook::Logbook::GetInstance()->IsRecording())
			return;

		// Get the GPS time. It must be valid.
		QDateTime dtGPS = m_pND->GetDateTimeUTC();
		if(dtGPS.isValid()) {
			// Get the time difference with the system timer.
			// If the difference is small enough (say up to a few minutes),
			// we will adjust system time.
			int iSecDiff = abs(dtGPS.secsTo(QDateTime::currentDateTime()));
			qDebug() << "Time check. The difference is" << iSecDiff << "seconds.";
			if(iSecDiff < 5*60) { // 5 min
				if(iSecDiff > 2) {
					qDebug() << "System time is adjusted with the GPS time.";
					QProcess::execute(
						"date",
						QStringList() << "-u" << "-s" << dtGPS.toString("MMddhhmmyyyy.ss")
					);
					// Set the flag. It will not be checked anymore.
				}
				bTimeChecked = true;
			}
		}
	}
}

// --------------------------------------------------------------------------

void WidgetMain::UpdateRouteLeg()
{
	if(m_pAN->UpdateRouteLeg(m_pND->GetPosition())) {
		QString qs = m_pAN->GetRoute()->GetWaypoint(m_pAN->GetActiveLeg())->GetDescription();
		WidgetMessage::GetInstance()->SetText(
			tr("New route leg to <font color=green size=+1>%1</font>"
			   " was activated.").arg(qs),
			3,
			WidgetMessage::lNormal
		);
	}
}

// --------------------------------------------------------------------------

void WidgetMain::OnStartupDialog()
{
	// Get current fuel level.
	unit::Value uvFL = unit::Value(m_pFUK->iKey_l, m_pND->GetTotalFuelLevel());
	uvFL.ConvertTo(m_pS->GetFuelVolumeUnit());

	DialogStartup dlg(uvFL);
	if(dlg.exec() == QDialog::Accepted) {
		uvFL = dlg.GetFuel();
		qDebug() << "Fuel level = " << uvFL.GetAs("l") << "liters";

		m_pS->SetCurrentPilot(dlg.GetPilot());
		qDebug() << "Current pilot" << m_pS->GetCurrentPilot();

		// Write new level into daqu
		const can::UnitInfoDaqu* pD = m_pNUC->GetUnitInfoContainer().GetDaqu();
		if(pD) {
			m_pND->ConfigureModule(
				pD->GetNodeId(),
				MCS_DAQU_FUEL_LEVEL,
				can::dtFloat,
				(float)uvFL.GetAs(m_pFUK->iKey_l)
			);
		}
		else {
			qWarning() << "Error: OnSetFuelLevel() Daqu was not detected.";
		}
	}
}

// --------------------------------------------------------------------------

void WidgetMain::UpdateAlarms()
{
	if(m_pAC->Update())
	  	m_pAC->Process();
}

// --------------------------------------------------------------------------

void WidgetMain::OnCanReceived(uint uiId, uint uirA, uint uirB)
{
	m_pND->ProcessCanMessage(uiId, uirA, uirB);
}

// --------------------------------------------------------------------------

void WidgetMain::OnSimulate()
{
//	return;
	static uint uiSecTick=0;
	uiSecTick++;

	if(uiSecTick == 20) {
		qDebug() << "Engine Start RPM = 4000" << QTime::currentTime();
		m_pND->m_EFSD.afEngineN1[0] = 4278;
	}
// 	else if(uiSecTick == 20) {
// 		qDebug() << "Take off IAS = 30" << QTime::currentTime();
// 		m_pND->m_FSAD.fIndicatedAirspeed = 30;
// 	}
// 	else if(uiSecTick == 30) {
// 		qDebug() << "Stall test = IAS" << QTime::currentTime();
// 		m_pND->m_FSAD.fIndicatedAirspeed = 21;
// 	}
/*	else if(uiSecTick == 24) {
		qDebug() << "Engine stop = 0" << QTime::currentTime();
		m_pND->m_EFSD.afEngineN1[0] = 0;
	}*/
// 	else if(uiSecTick == 230) {
// 		qDebug() << "In flight engine off RPM = 0" << QTime::currentTime();
// 		m_pND->m_EFSD.afEngineN1[0] = 0;
// 	}
// 	else if(uiSecTick == 280) {
// 		qDebug() << "In flight engine on RPM = 4000" << QTime::currentTime();
// 		m_pND->m_EFSD.afEngineN1[0] = 4000;
// 	}
	else if(uiSecTick == 8) {
		qDebug() << "Landing IAS = 2" << QTime::currentTime();
		m_pND->m_FSAD.fTrueAirspeed = 62.342;
// 		m_pND->m_FSAD.fGroundSpeed = 60.342;
		m_pND->m_FSAD.fWindSpeed = 3.342;
		m_pND->m_FSAD.fBaroCorrectedAltitude = 2755;
		m_pND->m_FSAD.fWindDirection = 0.5;
		m_pND->m_FSAD.fBodyRollAngle = 0.22;
		m_pND->m_FSAD.fBodyPitchAngle = 0.1;
		m_pND->m_FSAD.fHeadingAngle = 0.33;
		m_pND->m_FSAD.fHeadingAngle = 0.33;
		m_pND->m_ND.fGPSGroundSpeed = 59.33;
	}
// 	else if(uiSecTick == 420) {
// 		qDebug() << "Engine of RPM = 0 - should be end of record." << QTime::currentTime();
// 		m_pND->m_EFSD.afEngineN1[0] = 0;
// 	}

}

// --------------------------------------------------------------------------

void WidgetMain::OnProgramMabu(bool bStart)
{
	if(bStart) {
		WidgetMessage::GetInstance()->SetText(
			tr("Mabu programming has started. Please, wait."),
			90,
			WidgetMessage::lCritical
		);
	}
	else {
		WidgetMessage::GetInstance()->SetText(
			tr("Mabu programming has finihsed."),
			90,
			WidgetMessage::lNormal
		);
	}
}

// --------------------------------------------------------------------------
