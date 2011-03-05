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

#include <QtDebug>
#include <QtGui>

#include "GraphicsSingleton.h"
#include "NesisDoubleSpinBox.h"
#include "NesisAerospaceDataManager.h"
#include "NesisUnitCAN.h"
#include "MathEx.h"
#include "CommonDefs.h"

#include "DialogMount.h"


// -----------------------------------------------------------------------

DialogMount::DialogMount(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	
//	setGeometry(0,0, 600,390);

	const QFont& fontS = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QGridLayout* pLayout = new QGridLayout;
	setLayout(pLayout);
	QLabel* pLbl;
	pLbl = new QLabel(tr("Mount correction angles:"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, 0,0, 1,2);

	// Yaw
	pLbl = new QLabel(tr("Yaw:"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, 1,0);
	m_psbYaw = new NesisDoubleSpinBox(1);
	m_psbYaw->setMinimum(-90.0);
	m_psbYaw->setMaximum( 90.0);
	m_psbYaw->setSingleStep(0.2);
	m_psbYaw->setFont(fontS);
	pLayout->addWidget(m_psbYaw, 1,1);
	
	// Pitch
	pLbl = new QLabel(tr("Pitch:"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, 2,0);
	
	m_psbPitch = new NesisDoubleSpinBox(1);
	m_psbPitch->setMinimum(-90.0);
	m_psbPitch->setMaximum( 90.0);
	m_psbPitch->setSingleStep(0.2);
	m_psbPitch->setFont(fontS);
	pLayout->addWidget(m_psbPitch, 2,1);
	
	m_plblCurrentPitch = new QLabel;
	m_plblCurrentPitch->setFont(fontS);
	pLayout->addWidget(m_plblCurrentPitch, 2,2);
	
	// Roll
	pLbl = new QLabel(tr("Roll:"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, 3,0);
	m_psbRoll = new NesisDoubleSpinBox(1);
	m_psbRoll->setMinimum(-90.0);
	m_psbRoll->setMaximum( 90.0);
	m_psbRoll->setSingleStep(0.2);
	m_psbRoll->setFont(fontS);
	pLayout->addWidget(m_psbRoll, 3,1);
	
	m_plblCurrentRoll = new QLabel;
	m_plblCurrentRoll->setFont(fontS);
	pLayout->addWidget(m_plblCurrentRoll, 3,2);
	
	// List of external commands
	
	QStringList slExternalCmd;
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << tr("Save");
	slExternalCmd << tr("Close");
	
	m_psbYaw->SetExternalButtonLabels(slExternalCmd);
	m_psbPitch->SetExternalButtonLabels(slExternalCmd);
	m_psbRoll->SetExternalButtonLabels(slExternalCmd);
	
	// Connections
	connect(m_psbYaw,   SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_psbPitch, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_psbRoll,  SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));

	connect(m_psbYaw, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	connect(m_psbPitch, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	connect(m_psbRoll, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	
	// Timer
	QTimer* pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnUpdateAttitude()));
	pTimer->start(500);
	
	Activate();
}

DialogMount::~DialogMount()
{
}
// -----------------------------------------------------------------------

void DialogMount::Activate()
{
	// This will also show buttons.
	m_psbYaw->setFocus();
}

// -----------------------------------------------------------------------

void DialogMount::OnClose()
{
	accept();
}

// -----------------------------------------------------------------------

void DialogMount::OnSave()
{
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	can::UnitInfoAiru* pA = uibc.GetAiru();

	if(pA) {
		pA->SetMountCorrectionCoefficients(
			common::Rad(m_psbRoll->value()),
			common::Rad(m_psbPitch->value()),
			common::Rad(m_psbYaw->value())
		);
// 		qDebug() << "Download";
		if(pA->DownloadMountCorrection(pADM)) {
		
			// Burn to flash
// 			qDebug() << "Reset";
			pADM->ResetProgramFlash();
// 			qDebug() << "Program";
			pADM->ProgramFlash(pA->GetNodeId());
		
			// WaitProgramFlashToFinish -> QtAerospaceDataManager
			uibc.Wait(600);
	
			// Start using new values 
// 			qDebug() << "Update";
			pADM->ConfigureModule(
				pA->GetNodeId(), 
				MSC_AIRU_CALIBRATION_UPDATE, 
				can::dtULong, 
				MCS_UPDATE_FROM_FLASH
			);
			// TODO Success
		}
	}
	// TODO Warning
	else {
	}

	accept();
}

// -----------------------------------------------------------------------

void DialogMount::OnExternalCmd(int iCmd)
{
	if(iCmd==4)
		OnClose();
	else if(iCmd==3)
		OnSave();
}

// -----------------------------------------------------------------------

void DialogMount::OnUpdateAttitude()
{
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	
	m_plblCurrentRoll->setText(QString::number(common::Deg(pND->GetRoll()),'f',1));
	m_plblCurrentPitch->setText(QString::number(common::Deg(pND->GetPitch()),'f',1));
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

#define AXIS_COL			1
#define OLD_COL			3
#define CUR_COL			5
//#define ATT_COL			7

#define DESC_ROW			1
#define YAW_ROW			3
#define ROLL_ROW			5
#define PITCH_ROW			7
#define COMMENT_ROW		9

#define IROLL				0
#define IPITCH				1
#define IYAW				2

#define MEASURE_COUNT	10

// -----------------------------------------------------------------------

DialogMountCorrection::DialogMountCorrection(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
//	setGeometry(0,0, 600,390);
	
	// Airu object must exist.
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	m_pA = uibc.GetAiru();
	Q_ASSERT(m_pA);


	const QFont& fontS = GraphicsSingleton::GetInstance()->GetFont(
		GraphicsSingleton::fDialogSmall
	);

	// Main layout
	QGridLayout* pLayout = new QGridLayout;
	setLayout(pLayout);
	QLabel* pLbl;
	
	// Comment top line
	pLbl = new QLabel(tr("Axis"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, DESC_ROW, AXIS_COL, Qt::AlignCenter);
	
	pLbl = new QLabel(tr("Current"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, DESC_ROW, OLD_COL, Qt::AlignCenter);

	pLbl = new QLabel(tr("New"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, DESC_ROW, CUR_COL, Qt::AlignCenter);
	
/*	pLbl = new QLabel(tr("Attitude"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, DESC_ROW, ATT_COL, Qt::AlignCenter);*/
	
	// Yaw
	pLbl = new QLabel(tr("Yaw"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, YAW_ROW, AXIS_COL, Qt::AlignCenter);
	
	m_plblCurrentYaw = new QLabel;
	m_plblCurrentYaw->setFont(fontS);
	pLayout->addWidget(m_plblCurrentYaw, YAW_ROW, OLD_COL, Qt::AlignCenter);
	
	m_psbYaw = new NesisDoubleSpinBox(1);
	m_psbYaw->setMinimum(-10.0);
	m_psbYaw->setMaximum( 10.0);
	m_psbYaw->setSingleStep(0.5);
	m_psbYaw->setFont(fontS);
	pLayout->addWidget(m_psbYaw, YAW_ROW, CUR_COL);
	
	// Roll
	pLbl = new QLabel(tr("Roll"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, ROLL_ROW, AXIS_COL, Qt::AlignCenter);
	
	m_plblCurrentRoll = new QLabel;
	m_plblCurrentRoll->setFont(fontS);
	pLayout->addWidget(m_plblCurrentRoll, ROLL_ROW, OLD_COL, Qt::AlignCenter);
	
	m_plblNewRoll = new QLabel;
	m_plblNewRoll->setFont(fontS);
	pLayout->addWidget(m_plblNewRoll, ROLL_ROW, CUR_COL, Qt::AlignCenter);

	// Pitch
	pLbl = new QLabel(tr("Pitch"));
	pLbl->setFont(fontS);
	pLayout->addWidget(pLbl, PITCH_ROW, AXIS_COL, Qt::AlignCenter);
	
	m_plblCurrentPitch = new QLabel;
	m_plblCurrentPitch->setFont(fontS);
	pLayout->addWidget(m_plblCurrentPitch, PITCH_ROW, OLD_COL, Qt::AlignCenter);
	
	m_plblNewPitch = new QLabel;
	m_plblNewPitch->setFont(fontS);
	pLayout->addWidget(m_plblNewPitch, PITCH_ROW, CUR_COL, Qt::AlignCenter);
	
	// The comment row
	m_plblComment = new QLabel;
	m_plblComment->setFont(fontS);
	pLayout->addWidget(m_plblComment, COMMENT_ROW, AXIS_COL, 1, CUR_COL-AXIS_COL+1, Qt::AlignCenter);
	
	
	// List of external commands
	
	QStringList slExternalCmd;
	slExternalCmd << tr("Roll+Pitch");
	slExternalCmd << tr("Pitch");
	slExternalCmd << QString();
	slExternalCmd << tr("Save");
	slExternalCmd << tr("Close");
	
	m_psbYaw->SetExternalButtonLabels(slExternalCmd);
	
	// Connections
	connect(m_psbYaw, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	connect(m_psbYaw, SIGNAL(valueChanged(double)), this, SLOT(OnYawChanged(double)));
	
	// Timer
	QTimer* pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnUpdateAttitude()));
	pTimer->start(500);
	
	// Some controls may not be accessible if flying or engine running.
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	if(pND->IsEngineRunning() || pND->IsFlying()) {
		m_psbYaw->setReadOnly(true);
	}
	
	// Final actions
	// Read current mount correction values.
	using namespace common;
	ReadFlash();
	float fYaw = m_pA->GetMountCorrectionCoefficientFlash(IYAW);
	m_psbYaw->setValue(Snap(Deg(fYaw), 0.1));
	// Sent this to airu, but dont't burn.
	SendMountCorrection(0.0f, 0.0f, fYaw);
	
	for(int i=0; i<3; i++)
		m_afbMC[i] = qMakePair(m_pA->GetMountCorrectionCoefficientFlash(i), false);
	
	Activate();
}

DialogMountCorrection::~DialogMountCorrection()
{
}

// -----------------------------------------------------------------------

void DialogMountCorrection::Activate()
{
	// This will also show buttons.
	m_psbYaw->SetEditMode();
	m_psbYaw->setFocus();
}
// -----------------------------------------------------------------------

void DialogMountCorrection::ReadFlash()
{
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	
	if(m_pA) {
		using namespace common;
		m_pA->UploadMountCorrectionFlash(pADM);
		
		m_plblCurrentRoll->setText(
			QString::number(Deg(m_pA->GetMountCorrectionCoefficientFlash(0)),'f',1)
		);
		m_plblCurrentPitch->setText(
			QString::number(Deg(m_pA->GetMountCorrectionCoefficientFlash(1)),'f',1)
		);
		m_plblCurrentYaw->setText(
			QString::number(Deg(m_pA->GetMountCorrectionCoefficientFlash(2)),'f',1)
		);
	}
	else {
		m_plblComment->setText(tr("Error: Airu unit was not detected."));
		return;
	}
}

// -----------------------------------------------------------------------

void DialogMountCorrection::OnExternalCmd(int iCmd)
{
	if(iCmd==4)
		OnClose();
	else if(iCmd==3)
		OnSave();
	else if(iCmd==1)
		OnPitch();
	else if(iCmd==0)
		OnRollAndPitch();
}

// -----------------------------------------------------------------------

void DialogMountCorrection::OnClose()
{
	// Restore values from flash
	// if the save buttons was pressed, these will be the new values. Otherwise
	// we will restore the old ones.
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	m_pA->UploadMountCorrectionFlash(pADM);
	SendMountCorrection(
		m_pA->GetMountCorrectionCoefficientFlash(0),
		m_pA->GetMountCorrectionCoefficientFlash(1),
		m_pA->GetMountCorrectionCoefficientFlash(2)
	);

	accept();
}

// -----------------------------------------------------------------------

void DialogMountCorrection::OnYawChanged(double dYaw)
{
	qDebug() << "Yaw=" << dYaw;
	m_afbMC[IYAW].first = common::Rad(dYaw);
	m_afbMC[IYAW].second = true; // The yaw was changed
	
	SendMountCorrection(0.0f, 0.0f, m_afbMC[IYAW].first);
}

// -----------------------------------------------------------------------

void DialogMountCorrection::OnRollAndPitch()
{
	// Roll correction is not possible if flying or engine running.
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	
	if(pND->IsEngineRunning() || pND->IsFlying()) {
		m_plblComment->setText(tr("Roll correction is not possible!"));
	}
	else {
		// Take few measurements
		float fAvgRoll = 0.0;
		float fAvgPitch = 0.0;
		for(int i=0; i<MEASURE_COUNT; i++) {
			fAvgRoll += pND->GetRoll();
			fAvgPitch += pND->GetPitch();
			// wait some time between the measurements.
			uibc.Wait(52);
		}
		fAvgRoll  /= MEASURE_COUNT;
		fAvgPitch /= MEASURE_COUNT;
		
// 		qDebug() << "AvgRoll =" << common::Deg(fAvgRoll);
		m_plblNewRoll->setText(QString::number(common::Deg(fAvgRoll), 'f', 1));
		m_plblNewPitch->setText(QString::number(common::Deg(fAvgPitch), 'f', 1));
	
		m_afbMC[IROLL].first = fAvgRoll;
		m_afbMC[IROLL].second = true; // The roll was changed
		m_afbMC[IPITCH].first = fAvgPitch;
		m_afbMC[IPITCH].second = true; // The pitch was changed
	
		SendMountCorrection(
			m_afbMC[IROLL].first, 
			m_afbMC[IPITCH].first, 
			m_afbMC[IYAW].first
		);
	}
}

// -----------------------------------------------------------------------
	
void DialogMountCorrection::OnPitch()
{
	// Pitch correction is always possible.
	
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	
	float fAvgPitch = 0.0;
	for(int i=0; i<MEASURE_COUNT; i++) {
		fAvgPitch += pND->GetPitch();
		// wait some time between the measurements.
		uibc.Wait(52);
	}
	fAvgPitch /= MEASURE_COUNT;
		
// 	qDebug() << "AvgPitch =" << common::Deg(fAvgPitch);
	m_plblNewPitch->setText(QString::number(common::Deg(fAvgPitch), 'f', 1));

	m_afbMC[IPITCH].first = fAvgPitch;
	m_afbMC[IPITCH].second = true; // The roll was changed
	
	// If the roll was corrected, take the corrected roll. If it was not
	// corrected, take roll from flash.
	float fRoll = m_afbMC[IROLL].second ? m_afbMC[IROLL].first 
					: m_pA->GetMountCorrectionCoefficient(0);
	
	SendMountCorrection(fRoll, m_afbMC[IPITCH].first, m_afbMC[IYAW].first);
}

// -----------------------------------------------------------------------

void DialogMountCorrection::OnSave()
{
	float fRoll		= m_afbMC[IROLL].second ? m_afbMC[IROLL].first
						: m_pA->GetMountCorrectionCoefficientFlash(IROLL);
	float fPitch 	= m_afbMC[IPITCH].second ? m_afbMC[IPITCH].first
						: m_pA->GetMountCorrectionCoefficientFlash(IPITCH);
	float fYaw 		= m_afbMC[IYAW].second ? m_afbMC[IYAW].first
						: m_pA->GetMountCorrectionCoefficientFlash(IYAW);

	SendMountCorrection(fRoll, fPitch, fYaw, true);
	accept();
}

// -----------------------------------------------------------------------

void DialogMountCorrection::OnUpdateAttitude()
{
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	
	m_plblCurrentRoll->setText(QString::number(common::Deg(pND->GetRoll()),'f',1));
	m_plblCurrentPitch->setText(QString::number(common::Deg(pND->GetPitch()),'f',1));
}

// -----------------------------------------------------------------------

bool DialogMountCorrection::SendMountCorrection(
	float fR, 
	float fP, 
	float fY, 
	bool bBurn
)
{
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	
	qDebug() << "Setting mount correction R,P,Y" 
		<< common::Deg(fR) 
		<< common::Deg(fP)
		<< common::Deg(fY);
	
	m_pA->SetMountCorrectionCoefficients(fR, fP, fY);
	if(m_pA->DownloadMountCorrection(pADM)) {
//		bBurn = true;
 		if(bBurn) {
 			qDebug() << "Burning ...";
			// Burn to flash, when required
			pADM->ResetProgramFlash();
			pADM->ProgramFlash(m_pA->GetNodeId());
		
			// WaitProgramFlashToFinish -> QtAerospaceDataManager
			uibc.Wait(500);
		}
		uibc.Wait(100);
		
		// Make Airu use new values.
		pADM->ConfigureModule(
			m_pA->GetNodeId(), 
			MSC_AIRU_CALIBRATION_UPDATE, 
			can::dtULong, 
			bBurn ? MCS_UPDATE_FROM_FLASH : MCS_UPDATE_FROM_RAM
		);
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------
