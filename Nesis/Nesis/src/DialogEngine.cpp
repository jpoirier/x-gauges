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

#include "PanelMiscellaneous.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "GraphicsSingleton.h"
#include "NesisComboBox.h"
#include "NesisSpinBox.h"
#include "NesisDoubleSpinBox.h"
#include "NesisEditBox2.h"
#include "WidgetMessage.h"
#include "NesisUnitCAN.h"

#include "DialogEngine.h"

// -----------------------------------------------------------------------

DialogEngine::DialogEngine(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
//	setGeometry(0,0, 600,390);

	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	// Fuel flow grid layout inside a frame
	QFrame* pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	QGridLayout* pGrid = new QGridLayout;
	pFrame->setLayout(pGrid);
	pLayout->addWidget(pFrame);

	// Fuel flow label
	QLabel* pLbl = new QLabel(tr("Fuel flow sensor settings:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl, 0,0, 1,2);
	
	// Fuel flow K factor
	pLbl = new QLabel(tr("K factor:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl, 1,0);
	
	m_pedFuelFlowK = new NesisEditBox2("");
	m_pedFuelFlowK->SetCharacters("1234567890");
	m_pedFuelFlowK->setFont(fontSm);
	pGrid->addWidget(m_pedFuelFlowK, 1,1);
	QLabel* pBuddy = new QLabel;
	pBuddy->setFont(fontSm);
	pGrid->addWidget(pBuddy, 1,2);
	m_pedFuelFlowK->SetBuddy(pBuddy);
	
	// Fuel flow correction factor
	pLbl = new QLabel(tr("Correction:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl, 2,0);
	
	m_psbFuelFlowCorrection = new NesisDoubleSpinBox(2);
	m_psbFuelFlowCorrection->setAlignment(Qt::AlignLeft);
	m_psbFuelFlowCorrection->setFont(fontSm);
	m_psbFuelFlowCorrection->setRange(0.7, 2.5);
	m_psbFuelFlowCorrection->setSingleStep(0.02);
	pGrid->addWidget(m_psbFuelFlowCorrection, 2,1);
	
	// RPM Divisor
	pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	pGrid = new QGridLayout;
	pFrame->setLayout(pGrid);
	pLayout->addWidget(pFrame);
	// RPM label
	pLbl = new QLabel(tr("RPM divisor:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl, 0,0);
	// The edit box
	m_psbRpmDivisor = new NesisDoubleSpinBox(0);
	m_psbRpmDivisor->setAlignment(Qt::AlignLeft);
	m_psbRpmDivisor->setFont(fontSm);
	m_psbRpmDivisor->setRange(1.0, 110.0);
	pGrid->addWidget(m_psbRpmDivisor, 0,1);

// 	m_psbRpmDivisor->SetCharacters("1234567890");
// 	m_psbRpmDivisor->setFont(fontSm);
// 	pGrid->addWidget(m_psbRpmDivisor, 0,1);
// 	pBuddy = new QLabel;
// 	pBuddy->setFont(fontSm);
// 	pGrid->addWidget(pBuddy, 0,2);
// 	m_psbRpmDivisor->SetBuddy(pBuddy);

	const can::UnitInfoDaqu* pDaqu = NULL;
	if(Settings::GetInstance()->IsPrimary()) {
		pDaqu = NesisUnitCAN::GetInstance()->GetUnitInfoContainer().GetDaqu();
	}
	
	if(pDaqu) {
		qDebug() << "K=" << pDaqu->GetFuelFlowK() << "FF=" << pDaqu->GetFuelFlowFactor();
		m_pedFuelFlowK->setText(QString::number(pDaqu->GetFuelFlowK()));
		m_psbFuelFlowCorrection->setValue(pDaqu->GetFuelFlowFactor());
		m_psbRpmDivisor->setValue(pDaqu->GetRpmDivisor());
	}
	else {
		m_pedFuelFlowK->setText(QString());
		m_pedFuelFlowK->setEnabled(false);
		m_psbFuelFlowCorrection->setValue(0);
		m_psbFuelFlowCorrection->setEnabled(false);
		m_psbRpmDivisor->setValue(0);
		m_psbRpmDivisor->setEnabled(false);
	}
	
	// Engine-Airplane selection frame
	pFrame = new QFrame;
	pFrame->setFrameStyle(QFrame::StyledPanel);
	pGrid = new QGridLayout;
	pFrame->setLayout(pGrid);
	pLayout->addWidget(pFrame);
	
	// Airplane type list
	pLbl = new QLabel(tr("Airplane:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl, 0,0);
	
	// The list airplane-engine settings
	m_pcbAirplaneEngine = new NesisComboBox;
	m_pcbAirplaneEngine->setFont(fontSm);
	m_pcbAirplaneEngine->setMinimumWidth(170);
	pGrid->addWidget(m_pcbAirplaneEngine,0,1);
	InitializeAirplaneEngine();
	
	// List of external commands
	QStringList slExternalCmd;
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << tr("Save");
	slExternalCmd << tr("Close");
	
	m_psbFuelFlowCorrection->SetExternalButtonLabels(slExternalCmd);
	m_pedFuelFlowK->SetExternalButtonLabels(slExternalCmd);
	m_psbRpmDivisor->SetExternalButtonLabels(slExternalCmd);
	m_pcbAirplaneEngine->SetExternalButtonLabels(slExternalCmd);
	
	// Connections
	connect(m_psbFuelFlowCorrection, SIGNAL(NesisKnob(bool)), 
			  this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pedFuelFlowK, SIGNAL(NesisKnob(bool)), 
			  this, SLOT(FocusNextPrevChild(bool)));
	connect(m_psbRpmDivisor, SIGNAL(NesisKnob(bool)),
			  this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pcbAirplaneEngine, SIGNAL(NesisKnob(bool)), 
			  this, SLOT(FocusNextPrevChild(bool)));
	
	connect(m_psbFuelFlowCorrection, SIGNAL(NesisButton(int)),
			  this, SLOT(OnExternalCmd(int)));
	
	connect(m_pedFuelFlowK, SIGNAL(NesisButton(int)),
			  this, SLOT(OnExternalCmd(int)));
	connect(m_pedFuelFlowK, SIGNAL(NesisOk()),
			  this, SLOT(FocusNextChild()));
	connect(m_pedFuelFlowK, SIGNAL(NesisEditCancel()),
			  this, SLOT(FocusNextChild()));
	
	connect(m_psbRpmDivisor, SIGNAL(NesisButton(int)),
			  this, SLOT(OnExternalCmd(int)));
	connect(m_pcbAirplaneEngine, SIGNAL(NesisButton(int)),
			  this, SLOT(OnExternalCmd(int)));
}

DialogEngine::~DialogEngine()
{
}

// -----------------------------------------------------------------------

void DialogEngine::Activate()
{
	// This will also show buttons.
	m_pedFuelFlowK->setFocus();
}

// -----------------------------------------------------------------------

void DialogEngine::OnClose()
{
	reject();
}

// -----------------------------------------------------------------------
void DialogEngine::OnSave()
{
	NesisUnitInfoContainer& uibc    = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	can::AbstractAerospaceDataManager* pADM = uibc.GetAerospaceDataManager();
	const can::UnitInfoDaqu* pDaqu = uibc.GetDaqu();
	Settings* pS = Settings::GetInstance();
	
	// Daqu settings
	if(pDaqu) {
		bool bBurn = false;
		
		// Fuel flow factor
		if(pDaqu->GetFuelFlowFactor() != m_psbFuelFlowCorrection->value()) {
			// Burn new settings into Daqu.
			qDebug() << "New fuel flow correction" << m_psbFuelFlowCorrection->value() << "for DAQU.";
			
			// Set new value 
			pADM->ConfigureModule(
				pDaqu->GetNodeId(), 
				MCS_DAQU_FUEL_FLOW_FACTOR, 
				can::dtFloat, 
				(float)m_psbFuelFlowCorrection->value()
			);
			bBurn = true;
		}
		
		// Fuel flow K
		unsigned int uiK = m_pedFuelFlowK->text().toUInt();
		if(pDaqu->GetFuelFlowK() != uiK) {
			// Burn new settings into Daqu.
			qDebug() << "New fuel flow K" << uiK << "for DAQU.";
			
			// Set new value 
			pADM->ConfigureModule(
				pDaqu->GetNodeId(), 
				MCS_DAQU_FUEL_FLOW_K, 
				can::dtULong, 
				uiK
			);
			bBurn = true;
		}
		
		// RPM divisor
		unsigned int uiRpm = m_psbRpmDivisor->value();
		if(pDaqu->GetRpmDivisor() != uiRpm) {
			// Burn new settings into Daqu.
			qDebug() << "New RPM divisor" << uiRpm << "for DAQU.";
			
			// Set new value 
			pADM->ConfigureModule(
				pDaqu->GetNodeId(), 
				MCS_DAQU_RPM_DIVISOR, 
				can::dtULong, 
				uiRpm
			);
			bBurn = true;
		}
		
		// Should we burn?
		if(bBurn) {	
			// Burn to flash
			qDebug() << "Burning new values into DAQU."; 
			uibc.Wait(100);
			pADM->ResetProgramFlash();
			pADM->ProgramFlash(pDaqu->GetNodeId());
			// WaitProgramFlashToFinish -> QtAerospaceDataManager
			uibc.Wait(600);
			
			// Ask for the new values
			pADM->RequestModuleInformation(pDaqu->GetNodeId(), MIS_DAQU_FUEL_FLOW_FACTOR);
			pADM->RequestModuleInformation(pDaqu->GetNodeId(), MIS_DAQU_FUEL_FLOW_K);
		}
	}

	// Airplane-engine selection
	if(pS->GetAirplaneEngine() != m_pcbAirplaneEngine->currentText()+".conf") {
		qDebug() << "Setting new airplane type:" << m_pcbAirplaneEngine->currentText();
		pS->SetAirplaneEngine(m_pcbAirplaneEngine->currentText()+".conf");
		
		// This needs a reboot.
		pS->Save();
		accept(); // This tells the caller to the reboot.
		return; // We must not call reject after the accept.
	}
	
	// This will close the dialog without rebooting.
	reject();
}

// -----------------------------------------------------------------------

void DialogEngine::InitializeAirplaneEngine()
{
	Settings* pS = Settings::GetInstance();

	// Read the filenames from the airplane folder
	QDir dir(pS->GetAirplanePath(), "*.conf");
	QStringList sl = dir.entryList(QDir::Files | QDir::Readable, QDir::Name);

	foreach(QString qs, sl) {
		qs = QFileInfo(qs).baseName();
		m_pcbAirplaneEngine->insertItem(0, qs);
	}
	int iF = m_pcbAirplaneEngine->findText(
		QFileInfo(pS->GetAirplaneEngine()).baseName()
	);
	m_pcbAirplaneEngine->setCurrentIndex(iF != -1 ? iF : 0);
}

// -----------------------------------------------------------------------

void DialogEngine::OnExternalCmd(int iCmd)
{
	if(iCmd == AbstractNesisInput::pbBtn4)
		OnSave();
	if(iCmd == AbstractNesisInput::pbBtn5)
		OnClose();
}

// -----------------------------------------------------------------------

