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

#include "CommonDefsQt.h"
#include "PanelMiscellaneous.h"
#include "NesisTableView.h"
#include "GraphicsSingleton.h"
#include "Settings.h"
#include "NesisUnitCAN.h"
#include "CanAerospace/StaticHelper.h"
#include "WidgetMessage.h"
#include "DialogYesNo.h"

#include "DialogSoftwareUpdate.h"

#define STATUS_COL			5
#define STATUS_INVALID		0
#define STATUS_UPGRADE		1
#define STATUS_DOWNGRADE	2
#define STATUS_KEEP			3

// -----------------------------------------------------------------------

DialogSoftwareUpdate::DialogSoftwareUpdate(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	setGeometry(0,0, 590,380);

	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	setLayout(pLayout);
	QLabel* pLbl = new QLabel(tr("List of detected hardware units."));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);

	// External labels for the table widget.
	QStringList slExt;
	slExt << tr("Rescan");
	slExt << tr("Downgr.");
	slExt << tr("Program");
	slExt << QString();
	slExt << tr("Close");

	// Table
	m_pwtUnit = new NesisTableWidget();
	m_pwtUnit->setFont(fontSm);
	m_pwtUnit->SetExternalButtonLabels(slExt);
	pLayout->addWidget(m_pwtUnit);
	InitializeTable();
	
	// TextEdit
	m_pwText = new QTextEdit();
	m_pwText->setFont(fontSm);
	m_pwText->setReadOnly(true);
	pLayout->addWidget(m_pwText);

	// Progress bar
	m_pwProgress = new QProgressBar();
	m_pwProgress->setAlignment(Qt::AlignCenter);
	m_pwProgress->setRange(0,100);
	pLayout->addWidget(m_pwProgress);

	// Stretches
	pLayout->setStretchFactor(m_pwtUnit, 60);
	pLayout->setStretchFactor(m_pwText, 40);

	// Connections
	connect(m_pwtUnit, SIGNAL(currentCellChanged(int,int,int,int)),
			  this,       SLOT(OnTableRowChanged(int,int,int,int)));

	connect(m_pwtUnit, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));

	// Progress reports
	can::UnitProgrammer& cup = NesisUnitCAN::GetInstance()->GetCanUnitProgrammer();
	connect(
		&cup, SIGNAL(APSStartReport(quint8, quint32)),
		this, SLOT(OnAPSStartReport(quint8, quint32))
	);
	connect(
		&cup, SIGNAL(APSPageRequestReport(quint32)),
		this, SLOT(OnAPSPageRequestReport(quint32))
	);
	connect(
		&cup, SIGNAL(APSFinishReport(qint32)),
		this, SLOT(OnAPSFinishReport(qint32))
	);
	
	connect(
		NesisUnitCAN::GetInstance(), SIGNAL(ProgramMabu(bool)),
		this, SLOT(OnProgramMabu(bool))
	);

	Activate();
}

DialogSoftwareUpdate::~DialogSoftwareUpdate()
{
}
// -----------------------------------------------------------------------

void DialogSoftwareUpdate::Activate()
{
	// This will show buttons.
	setEnabled(true);
	m_pwtUnit->setFocus();
	m_pwtUnit->SetEditMode();
	m_pwtUnit->setCurrentCell(0,0);
}

// -----------------------------------------------------------------------

void DialogSoftwareUpdate::OnClose()
{
	accept();
}

// -----------------------------------------------------------------------
/** The table is initialized according to the primary / secondary
    status of the Nesis. The primary Nesis is used to programm all
    devices but the Mabu of the secodary Nesis. The secondary Nesis 
    can program its Mabu only and no other device.
    
    Such solution is needed from two reasons:
    - Only one "programmer" should be active on CAN to prevent
      multiple programming responces.
    - Secondary Mabu, when in the bootloader programming mode 
      represents itself as standard Mabu. The messages received by the 
      primary Mabu and sent by the secondary mabu have the primary Mabu id.
      Therefore they are rejected by primary Mabu and primary Nesis can not 
      see them.
 */

void DialogSoftwareUpdate::InitializeTable()
{
	using namespace can;
	
	Settings* pS = Settings::GetInstance();

	m_pwtUnit->clear();
	m_pwtUnit->setAlternatingRowColors(true);
	m_pwtUnit->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pwtUnit->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pwtUnit->setColumnCount(6);
	m_pwtUnit->setRowCount(0);

	m_pwtUnit->setColumnWidth(0,  100); // Unit
	m_pwtUnit->setColumnWidth(1,  75);  // HW
	m_pwtUnit->setColumnWidth(2,  95);  // SW installed
	m_pwtUnit->setColumnWidth(3,  95);  // SW available
	m_pwtUnit->setColumnWidth(4,  80);  // HW req.

	QStringList slHeader;
	slHeader << tr("Unit");
	slHeader << tr("HW");
	slHeader << tr("Installed");
	slHeader << tr("Available");
	slHeader << tr("HW req.");
	slHeader << tr("Status");
	m_pwtUnit->setHorizontalHeaderLabels(slHeader);

	m_pwtUnit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pwtUnit->verticalHeader()->setMaximumWidth(35);
	m_pwtUnit->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignTop);
	m_pwtUnit->horizontalHeader()->setMaximumHeight(35);
	m_pwtUnit->horizontalHeader()->setStretchLastSection(true);

	NesisUnitInfoContainer& uibc = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
	qDebug() << uibc.GetCount() << "units found.";

	can::UnitProgrammer& cup = NesisUnitCAN::GetInstance()->GetCanUnitProgrammer();
	QTableWidgetItem* pItem;
	QString qs;
	QVariant v;
	
	int r = 0;
	for(int iU=0; iU < uibc.GetCount(); iU++) {
		const UnitInfoBase* pI = uibc[iU];
		// The primary nesis can update all but the secondary mabu
		if(pS->IsPrimary() && pI->GetNodeId() == KHU_MABU_SECONDARY)
			continue;
		// The secondary nesis can update secondary mabu only
		if(pS->IsSecondary() && pI->GetNodeId() != KHU_MABU_SECONDARY)
			continue;
			
		m_pwtUnit->insertRow(r);
		
		// Hardware id
		uint8_t byId = pI->GetHardwareId();
		v = (int)pI->GetNodeId();
		
		// Id, name
		qs = QString("(%1)  %2").arg(pI->GetNodeId()).arg(can::GetHardwareName(byId));
		pItem = new QTableWidgetItem(qs);
		pItem->setTextAlignment(Qt::AlignCenter);
		pItem->setData(Qt::UserRole, v);
		m_pwtUnit->setItem(r, 0, pItem);
		
		// Hardware version
		pItem = new QTableWidgetItem(pI->GetHardwareVersionString());
		pItem->setTextAlignment(Qt::AlignCenter);
		m_pwtUnit->setItem(r, 1, pItem);
		
		// Installed software
		qs = QString("%1 (%2)").
			arg(pI->GetSoftwareVersionString()).
			arg(pI->GetSVN());
		pItem = new QTableWidgetItem(qs);
		pItem->setTextAlignment(Qt::AlignCenter);
		m_pwtUnit->setItem(r, 2, pItem);
		
		// Available software version
		can::UnitProgrammer::BinInfo bi = cup.GetBinInfo(byId);
		if(bi.m_bMD5) {
			qs = QString("%1.%2 (%3)").
				arg(bi.m_uiSV>>4).arg(bi.m_uiSV & 0x0f).
				arg(bi.m_uiSVN);
			pItem = new QTableWidgetItem(qs);
			pItem->setTextAlignment(Qt::AlignCenter);
			m_pwtUnit->setItem(r, 3, pItem);
			
			if(bi.m_uiRHV == 0)
				qs = tr("Any");
			else
				qs = QString("%1.%2").arg(bi.m_uiRHV>>4).arg(bi.m_uiRHV & 0x0f);
			
			pItem = new QTableWidgetItem(qs);
			pItem->setTextAlignment(Qt::AlignCenter);
			m_pwtUnit->setItem(r, 4, pItem);
		}
		// Bin info failed. There is nothing we can do with this bin.
		else {
			pItem = new QTableWidgetItem(tr("N/A"));
			pItem->setTextAlignment(Qt::AlignCenter);
			m_pwtUnit->setItem(r, 3, pItem);
			pItem = new QTableWidgetItem(tr("N/A"));
			pItem->setTextAlignment(Qt::AlignCenter);
			m_pwtUnit->setItem(r, 4, pItem);
		}
		
		// Status - check the bin info.
		// The MD5 must be OK. Hardware version must be OK.
		if(bi.m_bMD5 && bi.m_uiRHV <= pI->GetHardwareVersion()) {
			bool bUpgrade = false;
			// Check the software vesion
			if(bi.m_uiSV > pI->GetSoftwareVersion())
				bUpgrade = true;
			else if(bi.m_uiSV == pI->GetSoftwareVersion()) {
				if(bi.m_uiSVN > pI->GetSVN())
					bUpgrade = true;
			}
			
			if(bUpgrade) {
				qs = "Upgrade";
				v  = STATUS_UPGRADE;
			}
			else {
				qs = "Keep";
				v  = STATUS_KEEP;
			}
		}
		// Update is not possible.
		else {
			qs = tr("N/A");
			v  = STATUS_INVALID;
		}
		pItem = new QTableWidgetItem(qs);
		pItem->setData(Qt::UserRole, v);
		pItem->setTextAlignment(Qt::AlignCenter);
		if(v.toInt()==STATUS_UPGRADE)
			pItem->setForeground(Qt::green);
		m_pwtUnit->setItem(r, STATUS_COL, pItem);
		
		// next row
		r++;
	}
}

// -----------------------------------------------------------------------

void DialogSoftwareUpdate::OnNesisButton(int iBtn)
{
	if(iBtn==AbstractNesisInput::pbBtn5) // 4 == pbBtn5
		OnClose();
	else if(iBtn==AbstractNesisInput::pbBtn1)
		OnRescan();
	else if(iBtn==AbstractNesisInput::pbBtn2)
		OnDowngrade();
	else if(iBtn==AbstractNesisInput::pbBtn3)
		OnProgram();
}

// -----------------------------------------------------------------------

void DialogSoftwareUpdate::OnTableRowChanged(
	int iCurrentRow, 
	int /*iCurrentColumn*/, 
	int /*iPreviousRow*/, 
	int /*iPreviousColumn*/
)
{
	if(iCurrentRow <0 || iCurrentRow >= m_pwtUnit->rowCount())
		return;
		
	int iStatus  = m_pwtUnit->item(iCurrentRow,STATUS_COL)->data(Qt::UserRole).toInt();
	uint8_t byId = m_pwtUnit->item(iCurrentRow,0)->data(Qt::UserRole).toInt();
	
	QString qs;
	if(iStatus == STATUS_UPGRADE) {
		qs = tr("The %1 unit should be upgraded. Press the 'Program' button to "
				  "start the upgrade.");
	}
	else if(iStatus == STATUS_DOWNGRADE) {
		qs = tr("You have forced the downgrade for the %1 unit. "
		        "Press the 'Program' button to start the downgrade.");
	}
	else if(iStatus == STATUS_KEEP) {
		qs = tr("No software change was detected for the %1 unit. It is "
				  "recommended to keep the current software. "
				  "The 'Program' button is not active.");
	}
	else {
		qs = tr("There is no available software for the %1 unit. "
				  "The 'Program' button is not active.");
	}
	
	m_pwText->setText(qs.arg(can::GetHardwareName(byId)));
}

// -----------------------------------------------------------------------

void DialogSoftwareUpdate::OnAPSStartReport(quint8 byHardwareId, quint32 uiPages)
{
	QString qs = tr("Programming %1 - %p%").arg((can::GetHardwareName(byHardwareId)));
	m_pwProgress->setFormat(qs);
	m_pwProgress->setRange(0, uiPages);
	m_pwProgress->setValue(0);
}

// --------------------------------------------------------------------

void DialogSoftwareUpdate::OnAPSPageRequestReport(quint32 uiPage)
{
	m_pwProgress->setValue(uiPage);
}

// --------------------------------------------------------------------

void DialogSoftwareUpdate::OnAPSFinishReport(qint32 /*iState*/)
{
/*	m_pStatusBar->showMessage(
		tr("Programming is complete. State = %1").arg(iState)
	);*/
	m_pwProgress->reset();
}

// -----------------------------------------------------------------------

void DialogSoftwareUpdate::OnRescan()
{
	using namespace can;

	NesisUnitCAN* pNUC = NesisUnitCAN::GetInstance();
	NesisUnitInfoContainer& uibc = pNUC->GetUnitInfoContainer();
	
	uibc.Scan();
	InitializeTable();
	Activate();
	OnTableRowChanged(m_pwtUnit->currentRow(),0,0,0);
}

// -----------------------------------------------------------------------

void DialogSoftwareUpdate::OnProgram()
{
	using namespace can;

	NesisUnitCAN* pNUC 						= NesisUnitCAN::GetInstance();
	NesisUnitInfoContainer& uibc 			= pNUC->GetUnitInfoContainer();
	AbstractAerospaceDataManager* pADM	= uibc.GetAerospaceDataManager();
	can::UnitProgrammer& cup 				= pNUC->GetCanUnitProgrammer();

	// Get the selected line and check if it is programmable.
	int r = m_pwtUnit->currentRow();
	int iStatus  = m_pwtUnit->item(r,STATUS_COL)->data(Qt::UserRole).toInt();
	uint8_t byId = m_pwtUnit->item(r,0)->data(Qt::UserRole).toInt();
	uint8_t byHwId = uibc.Find(byId)->GetHardwareId();
	
	qDebug() << "Program: Row =" << r 
				<< "byId =" << byId << "Status =" << iStatus 
				<< "Hardware id" << byHwId;
	
	// Yes, we can program this device
	if(iStatus == STATUS_UPGRADE || iStatus == STATUS_DOWNGRADE) {
		// Aks for confirmation.
		QString qs = tr(
			"Nesis is about to program the %1 unit.\n"
			"Do you want to program this unit?"
		).arg(GetHardwareName(byId));
		DialogYesNo dlg(qs);
		dlg.exec();
		if(dlg.IsYes()==false) {
			return;
		}
		
		// Is it mabu?
		bool bMabu = (byId == KHU_MABU || byId == KHU_MABU_SECONDARY);
		
		QDir dir(Settings::GetInstance()->GetFirmwarePath());
		qDebug() << "Firmware path =" << Settings::GetInstance()->GetFirmwarePath();
		
		// check the fw and bin files ...
		if(common::CompareFiles(cup.GetFileName(byHwId), cup.GetFileName(byHwId, "bin"))==false) {
			bool bRemove = dir.remove(cup.GetFileName(byHwId));
			bool bRename = dir.rename(
				cup.GetFileName(byHwId, "bin"), 
				cup.GetFileName(byHwId)
			);
			qDebug() << "Renaming bin->fw: " 
						<< cup.GetFileName(byHwId, "bin") << "->" << cup.GetFileName(byHwId)
						<< "remove, rename" << bRemove << bRename;
		}
		
		// Wait for programming service to become available.
		m_pwText->clear();
		while(pADM->IsAPSActive()) {
			uibc.Wait(100);
		}
		cup.Reset();
		
		// Try to programm (all but the mabu).
		if(bMabu==false) {
			qs = tr(
				"The %1 unit will program itself until internal checksum becomes valid. "
				"This may require several progress bars passes."
				"\n\n"
				"After there is no visual activity on the progress bar for a few "
				"seconds, press the 'Rescan' button to check the new status."
			).arg(GetHardwareName(byHwId));
			pADM->StartAPS(byId, byHwId);
			cup.Reset();
			m_pwText->setText(qs);
			uibc.Wait(100);
		}
		// Mabu specific
		else {
			qs = tr(
				"The %1 unit will program itself, after the progress bar has completed. "
				"Please wait ..."
			).arg(GetHardwareName(byHwId));
			pADM->StartAPS(byId, byHwId);
			cup.Reset();
			m_pwText->setText(qs);
			uibc.Wait(2000);
			
			while(pADM->IsAPSActive()) {
				uibc.Wait(1000);
			}
		}
		
	
		// Turn reports back on.
/*		pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_ENABLE);
		uibc.Wait(5);
		pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_ENABLE);*/
	}
}

// --------------------------------------------------------------------

void DialogSoftwareUpdate::OnDowngrade()
{
	using namespace can;

	int r = m_pwtUnit->currentRow();
	int iStatus  = m_pwtUnit->item(r,STATUS_COL)->data(Qt::UserRole).toInt();
	uint8_t byId = m_pwtUnit->item(r,0)->data(Qt::UserRole).toInt();
	
	// Is downgrade possible?
	if(iStatus != STATUS_KEEP) 
		return;
		
	// Ask for downgrade.
	QString qs = tr("Do you really want to downgrade the %1 unit?\n\n"
	                "Do not downgrade unless recommended by Kanardia."
	                ).arg(can::GetHardwareName(byId));
	DialogYesNo dlg(qs);
	dlg.exec();
	if(dlg.IsYes()) {
		QTableWidgetItem* pItem = m_pwtUnit->item(r,STATUS_COL);
		pItem->setText(tr("Downgrade"));
		pItem->setForeground(QBrush(Qt::red));
		pItem->setData(Qt::UserRole, QVariant(STATUS_DOWNGRADE));
		OnTableRowChanged(r, 0,0,0);
	}
}

// --------------------------------------------------------------------

// void DialogSoftwareUpdate::OnTurnNesisOff()
// {
// 	// Currently, we can't turn the nesis off but we display message instead.
// 	m_pwText->append(
// 		"<b>Turn Nesis off by pressing the Off button for 5 seconds.</b>"
// 	);
// }

// --------------------------------------------------------------------
	
void DialogSoftwareUpdate::OnProgramMabu(bool bStart)
{
	if(bStart)
		m_pwText->append(
			tr("Mabu programming has started, please wait for about 60 seconds.")
		);
	else {
		m_pwText->append(
			tr("Finished! <b>Turn Nesis off. Press the Off button for 5 seconds.</b>")
		);
	}
}

// --------------------------------------------------------------------
