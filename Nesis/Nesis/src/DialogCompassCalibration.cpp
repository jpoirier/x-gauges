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

#include "MathEx.h"
#include "PanelMiscellaneous.h"
#include "NesisTableView.h"
#include "GraphicsSingleton.h"
#include "NesisAerospaceDataManager.h"
#include "NesisUnitCAN.h"

#include "Geometry/Angle.h"
#include "Numeric/FitCompass.h"
#include "Numeric/FitSolverLevenbergMarquard.h"

#include "DialogCompassCalibration.h"

#define INVALID_HEADING 		-999.9f
#define MAG_CAL_CRITERION		  10.0f

// -----------------------------------------------------------------------

DialogCompassCalibration::DialogCompassCalibration(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	setGeometry(0,0,520,390);
	
	m_iMeasureCount = 0;
	m_bValidMagCalibration = false;
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	QLabel* pLbl = new QLabel(tr("Compass Calibration"));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);

	// Table
	m_ptwCompass = new NesisTableWidget();
	m_ptwCompass->setFont(fontSm);
	QStringList slHeader;
	slHeader << tr("Reference");
	slHeader << tr("Status");
	slHeader << tr("Calibrated");
	slHeader << tr("Error");
   m_ptwCompass->setColumnCount(4);
   m_ptwCompass->setRowCount(MAGNETIC_POINTS_COUNT);
	for(int i=0; i<4; i++)
		m_ptwCompass->setColumnWidth(i, 115);
	for(int i=0; i<MAGNETIC_POINTS_COUNT; i++)
		m_ptwCompass->setRowHeight(i, 30);

	m_ptwCompass->setHorizontalHeaderLabels(slHeader);
	pLayout->addWidget(m_ptwCompass);
	m_ptwCompass->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_ptwCompass->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_ptwCompass->verticalHeader()->setMaximumWidth(35);
	m_ptwCompass->verticalHeader()->setStretchLastSection(true);
	m_ptwCompass->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignTop);
	m_ptwCompass->horizontalHeader()->setMaximumHeight(35);
	m_ptwCompass->horizontalHeader()->setStretchLastSection(true);
	m_ptwCompass->setMinimumHeight(290);
	
	// Coefficients label
	m_pwLblMagReport = new QLabel();
	pLayout->addWidget(m_pwLblMagReport);

	// Make the bottom empty.
	pLayout->addStretch(1);

	QStringList slExternalCmd;
	slExternalCmd << tr("Measure");
	slExternalCmd << tr("Calculate");
	slExternalCmd << tr("Reset");
	slExternalCmd << tr("Save");
	slExternalCmd << tr("Close");
	m_ptwCompass->SetExternalButtonLabels(slExternalCmd);

	// Connections
	connect(m_ptwCompass, SIGNAL(NesisButton(int)), this, SLOT(OnCompassCommand(int)));

	// Reset the table
	OnReset();
	Activate();
}

DialogCompassCalibration::~DialogCompassCalibration()
{
}
// -----------------------------------------------------------------------

void DialogCompassCalibration::Activate()
{
	// This will also show buttons.
	if(NesisUnitCAN::GetInstance()->GetUnitInfoContainer().GetMagu()!=NULL) {
		setEnabled(true);
		m_ptwCompass->setFocus();
		m_ptwCompass->setCurrentCell(0,0);
	}
	else {
		setEnabled(false);
	}
}

// -----------------------------------------------------------------------

void DialogCompassCalibration::OnCompassCommand(int iCmd)
{
	switch(iCmd) {
		case 0: OnMeasure(); break;
		case 1: OnCalculate(); break;
		case 2: OnReset(); break;
		case 3: OnSave(); break;
		case 4: OnClose(); break;
		default: break;
	}
}

// -----------------------------------------------------------------------

void DialogCompassCalibration::OnMeasure()
{
	using namespace common;
	using namespace can;
	
	int iRow = m_ptwCompass->currentRow();
	qDebug() << "Row = " << iRow;
	QString qsDone = tr("Done");
	
	QTableWidgetItem* pItem = m_ptwCompass->item(iRow, 1);
	if(pItem != NULL) {
		if(pItem->text()==qsDone) {
			// TODO Set correct message
			qDebug() << "Already made...";
			return;
		}
	}
	
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	UnitInfoMagu* pMagu = uibc.GetMagu();
 	if(pMagu) {
 		m_iMeasureCount++;

 		pMagu->ResetCalibrationPointsCount();
		NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
		pADM->ConfigureModule(
			uibc.GetMagu()->GetNodeId(), 
			MCS_MAGU_CAL_ADD,
			can::dtFloat,
			common::Rad(iRow*45.0f)
		);
		// Give the system some time to respond (and handle the messages meanwhile)
		uibc.Wait(300);
		
		if(pMagu->GetCalibrationPointsCount() == m_iMeasureCount) {
			m_ptwCompass->setItem(iRow, 1, new QTableWidgetItem(tr("Done")));
			m_ptwCompass->setCurrentCell((iRow+1)%MAGNETIC_POINTS_COUNT,0);
		}
		else {
			m_ptwCompass->setItem(iRow, 1, new QTableWidgetItem(tr("Try again!")));
			m_iMeasureCount--;
		}
		m_pwLblMagReport->setText(
			tr("%1 data points measured.")
		   .arg(m_iMeasureCount)
		);
	}
	else {
		qWarning() << "DialogCompassCalibration::OnMeasure: Magu not found.";
	}
}

// -----------------------------------------------------------------------

void DialogCompassCalibration::OnCalculate()
{
	using namespace can;
	
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	UnitInfoMagu* pMagu = uibc.GetMagu();
	
 	m_bValidMagCalibration = false;
 	if(pMagu) {
 		m_pwLblMagReport->setText("Calibrating ...");
 		
		pADM->ConfigureModule(
			pMagu->GetNodeId(), 
			MCS_MAGU_CAL_FINISH,
			can::dtLong,
			FINISH_CALCULATE_ONLY
		);
	
		pMagu->ResetFinishResult();
		// Give the system some time to respond (and handle the messages meanwhile)
		uibc.Wait(200);
		// Success?
		if(pMagu->GetFinishResult()==0) {
			// We want to upload the results from magu
			if(pMagu->UploadData(pADM)) {
				// assume valid results
			 	m_bValidMagCalibration = true;
				// Update the table
				QTableWidgetItem* pItem;
				for(int r=0; r<8; r++) {
					float fCal = common::Deg(pMagu->GetCalibrationData(r,2));
					float fRef = common::Deg(pMagu->GetCalibrationData(r,0));
					float fDif = fRef-fCal;
					if(fDif < -180.0f)
						fDif += 360.0f;
					if(fabs(fDif) > MAG_CAL_CRITERION)
						m_bValidMagCalibration = false;
					
					pItem = new QTableWidgetItem(QString::number(fCal, 'f',1));
					m_ptwCompass->setItem(r, 2, pItem);
					pItem = new QTableWidgetItem(QString::number(fDif, 'f',1));
					m_ptwCompass->setItem(r, 3, pItem);
				}
		 		m_pwLblMagReport->setText("Done.");
			}
			else {
				m_pwLblMagReport->setText(
					tr("Data upload failed. Press Calculate again.")
				);
				pADM->ResetUpload();
			}
		}
		else {
			m_pwLblMagReport->setText(
				tr("Mag. calibration failed. Error = %1. Reset and try again.")
			   .arg(pMagu->GetFinishResult())
			);
		}
	}
}

// -----------------------------------------------------------------------

void DialogCompassCalibration::OnReset()
{
	using namespace can;

	static const char* apcRefStrings[] = {	
		"N [360]", "NE [045]", "E [090]", "SE [135]", 
		"S [180]", "SW [225]", "W [270]", "NW [315]" 
	};
	
	m_iMeasureCount = 0;

	m_ptwCompass->clearContents();
	for(int i=0; i<MAGNETIC_POINTS_COUNT; i++) {
		m_afMeasurement[i] = INVALID_HEADING;
		QTableWidgetItem* pItem = new QTableWidgetItem(apcRefStrings[i]);
		pItem->setTextAlignment(Qt::AlignCenter);
		m_ptwCompass->setItem(i,0, pItem);
	}
	
	// Send it to all magu units.
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
 	if(uibc.GetMagu()) {
		NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
		pADM->ConfigureModule(uibc.GetMagu()->GetNodeId(), MCS_MAGU_CAL_RESET, can::dtNoData, 0);
	}
	
	m_pwLblMagReport->setText(QString());
	m_ptwCompass->setCurrentCell(0,0);
}

// -----------------------------------------------------------------------

void DialogCompassCalibration::OnSave()
{
	using namespace can;
	
	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	NesisAerospaceDataManager* pADM = NesisAerospaceDataManager::GetInstance();
	UnitInfoMagu* pMagu = uibc.GetMagu();
	
	if(m_bValidMagCalibration == false) {
		// TODO Report that we can't save.
		return;
	}
		 
	// Download coefficients
	// We want to upload coefficietns
	if(pMagu->UploadCalibration(pADM)==false) {
		m_pwLblMagReport->setText(tr("Coefficient upload failed, please try again."));
		pADM->ResetUpload();
		return;
	}

	pADM->ConfigureModule(
		pMagu->GetNodeId(),
		MCS_MAGU_CAL_FINISH,
		can::dtLong,
		FINISH_CALCULATE_SET
	);
	// We must give Magu some time to process the request.
	uibc.Wait(300);

	// Burn to flash
	pADM->ResetProgramFlash();
	pADM->ProgramFlash(pMagu->GetNodeId());
	// We must give Magu some time to process the request.
	uibc.Wait(600);

	// Download coefficients
	// We want to upload flash coefficietns
	if(pMagu->UploadCalibrationFlash(pADM)==false) {
		m_pwLblMagReport->setText(
			tr("Flash coefficient upload failed, please try again."));
		pADM->ResetUpload();
		return;
	}

	// Compare the coefficients.
	bool bSuccess = true;
	for(int i=0; i<5; i++) {
		if(pMagu->GetCoefficient(i) != pMagu->GetCoefficientFlash(1, i)) {
			bSuccess = false;
			break;
		}
	}
	
	if(bSuccess) {
		m_pwLblMagReport->setText(
			tr("New magnetic calibration was saved successfully!")
		);
	}
	else {
		m_pwLblMagReport->setText(
			tr("Failed to save the calibration!")
		);
	}
}

// -----------------------------------------------------------------------

void DialogCompassCalibration::OnClose()
{
	accept();
}

// -----------------------------------------------------------------------
