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

#include <QtDebug>
#include <QtGui>

#include "PanelMiscellaneous.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "GraphicsSingleton.h"
#include "NesisAerospaceDataManager.h"

#include "NesisComboBox.h"
#include "NesisSpinBox.h"
#include "WidgetMessage.h"
#include "Logbook/Logbook.h"

#include "DialogSetTime.h"

// -----------------------------------------------------------------------

DialogSetTime::DialogSetTime(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	
	// Main layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);
	
	QFrame* pFrame;
	QLabel* pLbl;
	QVBoxLayout* pVLayout;
	QHBoxLayout* pHLayout;
	const QFont& fontS = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);
	
	// Current UTC time box
	
	pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	pMainLayout->addWidget(pFrame);
	pVLayout = new QVBoxLayout;
	pFrame->setLayout(pVLayout);
	
	pLbl = new QLabel(tr("Current computer time:"));
	pLbl->setFont(fontS);
	pVLayout->addWidget(pLbl);
	m_plblCurrentUTC = new QLabel;
	m_plblCurrentUTC->setFont(fontS);
	pVLayout->addWidget(m_plblCurrentUTC);
	m_plblCurrentLT = new QLabel;
	m_plblCurrentLT->setFont(fontS);
	pVLayout->addWidget(m_plblCurrentLT);

	// Controls to change current local time
	
	pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	pMainLayout->addWidget(pFrame);
	pVLayout = new QVBoxLayout;
	pFrame->setLayout(pVLayout);
	
	pLbl = new QLabel(tr("Set local time difference:"));
	pLbl->setFont(fontS);
	pVLayout->addWidget(pLbl);
	
	m_pspLocalTimeH = new NesisSpinBox(0/*,NesisSpinBox::mKnob*/);
	m_pspLocalTimeH->setFont(fontS);
	m_pspLocalTimeH->setRange(-12,12);
	m_pspLocalTimeH->setSuffix(" h");
	m_pspLocalTimeMin = new NesisSpinBox(0/*,NesisSpinBox::mKnob*/);
	m_pspLocalTimeMin->setFont(fontS);
	m_pspLocalTimeMin->setRange(0,45);
	m_pspLocalTimeMin->setSingleStep(15);
	m_pspLocalTimeMin->setSuffix(" min");
	
	pHLayout = new QHBoxLayout;
	pLbl = new QLabel(tr("Local time = UTC +"));
	pLbl->setFont(fontS);
	pHLayout->addWidget(pLbl);
	pHLayout->addWidget(m_pspLocalTimeH);
	pHLayout->addWidget(m_pspLocalTimeMin);
	pHLayout->addStretch(1);
	pVLayout->addLayout(pHLayout);
	pLbl = new QLabel(tr("Note: Any change will result in Nesis reboot."));
	pVLayout->addWidget(pLbl);
	InitializeLocalTime();
	
	// GPS (UTC) Time box
	
	pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	pMainLayout->addWidget(pFrame);
	pVLayout = new QVBoxLayout;
	pFrame->setLayout(pVLayout);
	
	pLbl = new QLabel(tr("UTC received from GPS:"));
	pLbl->setFont(fontS);
	pVLayout->addWidget(pLbl);
	
	m_plblGPSStatus = new QLabel;
	m_plblGPSStatus->setFont(fontS);
	m_plblGPSTime   = new QLabel;
	m_plblGPSTime->setFont(fontS);
	pVLayout->addWidget(m_plblGPSStatus);
	pVLayout->addWidget(m_plblGPSTime);
	pLbl = new QLabel(tr(
		"(1) In order to perform the update the engine must be stopped\n"
		"and airplane must not move!\n"
		"(2) Update from GPS will result in Nesis reboot.")
	);
	pVLayout->addWidget(pLbl);
	
	// Manual settings time, date box
	
/*	pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	pMainLayout->addWidget(pFrame);
	QVBoxLayout* pMTSLayout = new QVBoxLayout;
	pFrame->setLayout(pMTSLayout);
	
	pHLayout = new QHBoxLayout;
	pMTSLayout->addLayout(pHLayout);
	pHLayout->addWidget(new QLabel(tr("New Time: ")));
	
	m_pspManualH = new NesisSpinBox;
	m_pspManualH->setFont(fontS);
	m_pspManualH->setRange(0,23);
	m_pspManualH->setSuffix(" h");
	pHLayout->addWidget(m_pspManualH);
	
	m_pspManualMin = new NesisSpinBox;
	m_pspManualMin->setFont(fontS);
	m_pspManualMin->setRange(0,59);
	m_pspManualMin->setSuffix(" min");
	pHLayout->addWidget(m_pspManualMin);
	
	pHLayout->addStretch(1);
	
	pHLayout = new QHBoxLayout;
	pMTSLayout->addLayout(pHLayout);
	pHLayout->addWidget(new QLabel(tr("New Date: ")));
	
	m_pspManualDay = new NesisSpinBox;
	m_pspManualDay->setFont(fontS);
	m_pspManualDay->setRange(1,31);
	pHLayout->addWidget(m_pspManualDay);
	
	m_pspManualMonth = new NesisSpinBox;
	m_pspManualMonth->setFont(fontS);
	m_pspManualMonth->setRange(1,12);
	pHLayout->addWidget(m_pspManualMonth);
	
	m_pspManualYear = new NesisSpinBox;
	m_pspManualYear->setFont(fontS);
	m_pspManualYear->setRange(2008,2050);
	pHLayout->addWidget(m_pspManualYear);
	
	pHLayout->addWidget(new QLabel(tr("(day, month, year)")));
	
	pHLayout->addStretch(1);*/
	
	
	
	pMainLayout->addStretch(1);
	
	// List of external commands
	QStringList slExternalCmd;
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << tr("Use GPS");
	slExternalCmd << tr("Close");
	
	// External commands
	m_pspLocalTimeH->SetExternalButtonLabels(slExternalCmd);
	m_pspLocalTimeMin->SetExternalButtonLabels(slExternalCmd);
	
	// Connections
	
/*	connect(m_pspLocalTimeH, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_pspLocalTimeMin, SIGNAL(NesisOk()), this, SLOT(FocusPrevChild()));*/
	connect(m_pspLocalTimeH, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	connect(m_pspLocalTimeMin, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	// Knob moves the focus
	connect(m_pspLocalTimeH, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pspLocalTimeMin, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	
	QTimer* pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnTimeUpdate()));
	pTimer->start(1000);
	
	OnTimeUpdate();
}

DialogSetTime::~DialogSetTime()
{
}
// -----------------------------------------------------------------------

void DialogSetTime::Activate()
{
/*	// This will also show buttons.
	m_pcbLanguage->setFocus();*/
}

// -----------------------------------------------------------------------

bool DialogSetTime::OnClose()
{
	Settings* pS = Settings::GetInstance();
	// Did the user changed the local time settings.
	if(pS->GetLocalTime2UTCDifferenceMinutes() != GetLocalTimeDifferenceMin()) {
		pS->SetLocalTime2UTCDifferenceMinutes(GetLocalTimeDifferenceMin());
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------

void DialogSetTime::InitializeLocalTime()
{
	Settings* pS = Settings::GetInstance();

	int iMin = pS->GetLocalTime2UTCDifferenceMinutes();
	int iH = abs(iMin)/60;
	if(iMin < 0) iH = -iH;
	iMin = abs(iMin)%60;
	m_pspLocalTimeH->setValue(iH);
	m_pspLocalTimeMin->setValue(iMin);
}

// -----------------------------------------------------------------------

void DialogSetTime::OnExternalCmd(int iCmd)
{
	qDebug() << "Command" << iCmd;
	bool bReboot = false;
	if(iCmd == AbstractNesisInput::pbBtn5)
		bReboot = OnClose();
	else if(iCmd == AbstractNesisInput::pbBtn4) {
		bool bR1 = OnClose();
		bool bR2 = OnUseGPSTime();
		bReboot = bR1 || bR2;
	}
	if(bReboot) {
		Settings* pS = Settings::GetInstance();
		pS->Save();
		accept();
		return;
	}
	reject();
}

// -----------------------------------------------------------------------

void DialogSetTime::OnTimeUpdate()
{
	QDateTime dt = QDateTime::currentDateTime();
	m_plblCurrentUTC->setText(
		tr("UTC time <b>%1</b>, date <b>%2</b>.").arg(
			dt.toString(tr("hh:mm:ss")), dt.toString(tr("dd.MM.yyyy"))
		)
	);
	
	// Local time
	dt = dt.addSecs(GetLocalTimeDifferenceMin()*60);
	m_plblCurrentLT->setText(
		tr("Local time <b>%1</b>, date <b>%2</b>.").arg(
			dt.toString(tr("hh:mm:ss")), dt.toString(tr("dd.MM.yyyy"))
		)
	);
	
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	dt = pADM->GetDateTimeUTC();
	int iSatCount = pADM->GetSatelliteCount();
	if(dt.isValid() && iSatCount > 2) {
		m_plblGPSTime->setText(
			tr("Current GPS time <b>%1</b>, date <b>%2</b>.").arg(
				dt.toString(tr("hh:mm:ss")), dt.toString(tr("dd.MM.yyyy"))
			)
		);
	}
	else {
		m_plblGPSTime->setText(tr("GPS time is not available. Time update is not possible."));
	}
	m_plblGPSStatus->setText(tr("<b>%1</b> GPS satellites are visible.").arg(iSatCount));
}

// -----------------------------------------------------------------------

int DialogSetTime::GetLocalTimeDifferenceMin() const
{
	int iH = m_pspLocalTimeH->value();
	int iMin = abs(iH)*60 + m_pspLocalTimeMin->value();
	if(iH < 0)
		iMin = -iMin;
		
	return iMin;
}

// -----------------------------------------------------------------------

bool DialogSetTime::OnUseGPSTime()
{
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	int iSatCount = pADM->GetSatelliteCount();

	if(iSatCount < 3) {
		WidgetMessage::GetInstance()->SetText(
			tr("GPS time update failed!\nNot enough satellites are visible."), 5
		);
		return false;
	}

	// We can't update time if recording has already started.
	if(logbook::Logbook::GetInstance()->IsRecording()) {
		WidgetMessage::GetInstance()->SetText(
			tr("GPS time update failed!\nAirplane is moving or engine is running."), 5
		);
		return false;
	}

	QDateTime dtGPS = pADM->GetDateTimeUTC();
	if(dtGPS.isValid()) {
		qDebug() << "System time is adjusted with the GPS time.";
		QProcess::execute(
			"date",
			QStringList() << "-u" << "-s" << dtGPS.toString("MMddhhmmyyyy.ss")
		);
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------
