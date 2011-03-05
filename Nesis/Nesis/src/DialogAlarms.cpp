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
#include "NesisCheckBox.h"
#include "Settings.h"
#include "GraphicsSingleton.h"
#include "AlarmContainer.h"

#include "DialogAlarms.h"


// -----------------------------------------------------------------------

DialogAlarms::DialogAlarms(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	// List of pilots:
	QLabel* pLbl = new QLabel(tr("User configurable alarms:"));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);

	// Alarm texts
	QVector<QString> vqsAlarms;
	vqsAlarms.resize(Alarm::aEnd);
	vqsAlarms[Alarm::aCHT] 					= tr("High CHT");
	vqsAlarms[Alarm::aEGT] 					= tr("High EGT");
	vqsAlarms[Alarm::aFuelLevel] 			= tr("Low fuel level");
	vqsAlarms[Alarm::aFuelPressure] 		= tr("Fuel pressure range");
	vqsAlarms[Alarm::aOilPressure] 		= tr("Oil pressure range");
	vqsAlarms[Alarm::aOilTemperature]		= tr("High oil temperature");
	vqsAlarms[Alarm::aRPM]					= tr("High RPM");
	vqsAlarms[Alarm::aStall]					= tr("Stall warning");
	vqsAlarms[Alarm::aVNE]					= tr("Max airspeed");
	vqsAlarms[Alarm::aWaterTemperature]	= tr("High water temperature");
	vqsAlarms[Alarm::aCurrent]				= tr("High electic current");
	vqsAlarms[Alarm::aVoltage]				= tr("High/low voltage");
	vqsAlarms[Alarm::aCarburetor]			= tr("Carburetor heat");
	
	// External commands
	QStringList slExternalCmd;
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << tr("Close");

	// Get list of user configurable alarms.
	// Alarms are organised in a grid control.
	QGridLayout* pGrid = new QGridLayout;
	pLayout->addLayout(pGrid);
	
	AlarmContainer::ConstCon con = AlarmContainer::GetInstance()->GetAlarms(instrument::Parameter::atUser);
	bool bTwoCols = con.count() > 6;
	for(int i=0; i<con.count(); i++) {
		NesisCheckBox* pCB = new NesisCheckBox(vqsAlarms[con[i]->GetAlarmEnum()]);
		m_vpcbUserAlarms << qMakePair(pCB, (int)con[i]->GetAlarmEnum());
		pCB->setFont(fontSm);
		pCB->SetExternalButtonLabels(slExternalCmd);
		pCB->setChecked(con[i]->IsActive());
		
		pGrid->addWidget(pCB, bTwoCols ? i/2 : i, bTwoCols ? i%2 : 0);

		connect(pCB, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
		connect(pCB, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	}
}

DialogAlarms::~DialogAlarms()
{
}
// -----------------------------------------------------------------------

void DialogAlarms::Activate()
{
	// This will also show buttons.
	setEnabled(true);
}

// -----------------------------------------------------------------------

void DialogAlarms::OnClose()
{
	// Store some values into settings.
	Settings* pS = Settings::GetInstance();
	
	// Write settings for each user alarm.
	for(int i=0; i<m_vpcbUserAlarms.count(); i++) {
		pS->EnableUserAlarm(
			m_vpcbUserAlarms[i].second, 
			m_vpcbUserAlarms[i].first->isChecked()
		);
	}
	AlarmContainer::GetInstance()->UpdateActive();
	accept();
}

// -----------------------------------------------------------------------

void DialogAlarms::OnExternalCmd(int iCmd)
{
	if(iCmd == AbstractNesisInput::pbBtn5) {
		OnClose();
	}
}

// -----------------------------------------------------------------------
