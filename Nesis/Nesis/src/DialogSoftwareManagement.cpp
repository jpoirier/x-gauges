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
#include "WidgetMessage.h"
#include "DialogYesNo.h"
#include "PacketManager.h"
#include "DialogSoftwareUpdate.h"

#include "DialogSoftwareManagement.h"

#define STATUS_COL			5
#define STATUS_INVALID		0
#define STATUS_UPGRADE		1
#define STATUS_DOWNGRADE	2
#define STATUS_KEEP			3

// -----------------------------------------------------------------------

DialogSoftwareManagement::DialogSoftwareManagement(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	setGeometry(0,0, 620,380);

	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	setLayout(pLayout);
	QLabel* pLbl = new QLabel(tr("List of detected hardware units."));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);

	// External labels for the table widget.
	QStringList slExt;
	slExt << tr("Refresh");
	slExt << tr("Apply");
	slExt << tr("Show all");
	slExt << tr("Firmware");
	slExt << tr("Close");

	m_pPM = new sup::PacketManager();

	// Table
	m_pwtPackage = new NesisTableWidget();
	m_pwtPackage->setFont(fontSm);
	m_pwtPackage->SetExternalButtonLabels(slExt);
	pLayout->addWidget(m_pwtPackage);
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
	pLayout->setStretchFactor(m_pwtPackage, 70);
	pLayout->setStretchFactor(m_pwText, 30);

	// Connections
	connect(m_pwtPackage, SIGNAL(NesisOk()), this, SLOT(OnPackageChange()));

	connect(m_pwtPackage, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));

	connect(m_pPM, SIGNAL(Progress(int)), m_pwProgress, SLOT(setValue(int)));
	connect(m_pPM, SIGNAL(Message(QString)), m_pwText, SLOT(append(QString)));

	QTimer::singleShot(250, this, SLOT(OnRescan()));

	Activate();
}

DialogSoftwareManagement::~DialogSoftwareManagement()
{
}

// -----------------------------------------------------------------------

void DialogSoftwareManagement::Activate()
{
	// This will show buttons.
	setEnabled(true);
	m_pwtPackage->setFocus();
	m_pwtPackage->SetEditMode();
	if ( m_pwtPackage->currentRow() > m_pwtPackage->rowCount() ||
		  m_pwtPackage->currentRow() < 0)
	 	m_pwtPackage->setCurrentCell(0,0);
}

// -----------------------------------------------------------------------

void DialogSoftwareManagement::OnClose()
{
	accept();
}

// -----------------------------------------------------------------------
/** The table is build acordingly to current package situation
 */

void DialogSoftwareManagement::InitializeTable()
{
	Settings* pS = Settings::GetInstance();

	m_pwtPackage->clear();
	m_pwtPackage->setAlternatingRowColors(true);
	m_pwtPackage->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pwtPackage->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pwtPackage->setColumnCount(3);
	m_pwtPackage->setRowCount(0);

	m_pwtPackage->setColumnWidth(0,  260);  // Unit
	m_pwtPackage->setColumnWidth(1,  160);  // SW installed
	m_pwtPackage->setColumnWidth(2,  100);  // Action

	QStringList slHeader;
	slHeader << tr("Name");
	slHeader << tr("Version");
	slHeader << tr("Action");
	m_pwtPackage->setHorizontalHeaderLabels(slHeader);

	m_pwtPackage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pwtPackage->verticalHeader()->setMaximumWidth(42);
	m_pwtPackage->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignTop);
	m_pwtPackage->horizontalHeader()->setMaximumHeight(35);
	m_pwtPackage->horizontalHeader()->setStretchLastSection(true);

	QTableWidgetItem* pItem;
	sup::PackageInfo* pPI;

	for (int i = 0; i < m_pPM->GetChangedPackageCount(); i++) {
		using namespace sup;

		m_pwtPackage->insertRow(i);

		pPI = m_pPM->GetChangedPackage(i);

		pItem = new QTableWidgetItem(pPI->GetName());
		m_pwtPackage->setItem(i, 0, pItem);

		pItem = new QTableWidgetItem(pPI->GetVersion());
		m_pwtPackage->setItem(i, 1, pItem);

		pItem = new QTableWidgetItem(QString("Upgrade"));
		m_pwtPackage->setItem(i, 2, pItem);
	}

}

// -----------------------------------------------------------------------

void DialogSoftwareManagement::OnNesisButton(int iBtn)
{
	if(iBtn==AbstractNesisInput::pbBtn5)
		OnClose();
	else if(iBtn==AbstractNesisInput::pbBtn1)
		OnRescan();
	else if(iBtn==AbstractNesisInput::pbBtn2)
		OnUpgrade();
	else if(iBtn==AbstractNesisInput::pbBtn3)
		OnShowAll();
	else if(iBtn==AbstractNesisInput::pbBtn4)
		OnFirmware();
}

// -----------------------------------------------------------------------

void DialogSoftwareManagement::OnRescan()
{
	m_pwText->clear();
	m_pPM->Refresh();
	InitializeTable();
	Activate();
}

// --------------------------------------------------------------------

void DialogSoftwareManagement::OnUpgrade()
{
	m_pPM->Apply();
}

// --------------------------------------------------------------------

void DialogSoftwareManagement::OnShowAll()
{
	using namespace sup;

	PackageInfo* pPI;
	QTableWidgetItem* pItem;
	int iRow;

	// clear only the contents
	m_pwtPackage->clearContents();
	// set row count
	m_pwtPackage->setRowCount(
		m_pPM->GetInstalledPackageCount() + 
		m_pPM->GetAvailablePackageCount()
	);

	for (int i = 0; i < m_pPM->GetAvailablePackageCount(); i++) {
		pPI = m_pPM->GetAvailablePackage(i);

		pItem = new QTableWidgetItem(pPI->GetName());
		m_pwtPackage->setItem(i, 0, pItem);

		pItem = new QTableWidgetItem(pPI->GetVersion());
		m_pwtPackage->setItem(i, 1, pItem);

		pItem = new QTableWidgetItem(QString(tr("Available")));
		m_pwtPackage->setItem(i, 2, pItem);
	}

	iRow = m_pPM->GetAvailablePackageCount();
	for (int i = 0; i < m_pPM->GetInstalledPackageCount(); i++) {
		pPI = m_pPM->GetInstalledPackage(i);

		pItem = new QTableWidgetItem(pPI->GetName());
		m_pwtPackage->setItem(i+iRow, 0, pItem);

		pItem = new QTableWidgetItem(pPI->GetVersion());
		m_pwtPackage->setItem(i+iRow, 1, pItem);

		pItem = new QTableWidgetItem(QString(tr("Installed")));
		m_pwtPackage->setItem(i+iRow, 2, pItem);
	}

	Activate();
}

// --------------------------------------------------------------------

void DialogSoftwareManagement::OnFirmware()
{
	DialogSoftwareUpdate dlg(this);
	dlg.exec();
}

// --------------------------------------------------------------------

void DialogSoftwareManagement::OnPackageChange()
{
	int i = m_pwtPackage->currentRow();
	if (m_pwtPackage->item(i,2)->text() == QString("Available")) {
		m_pwText->append(QString("Selecting package %1 %2")
			.arg(m_pwtPackage->item(i,0)->text())
			.arg(m_pwtPackage->item(i,1)->text())
		);
		m_pwtPackage->item(i,2)->setText("Selected");

		m_pPM->SelectPackage(m_pwtPackage->item(i,0)->text(), m_pwtPackage->item(i,1)->text());
	}
	Activate();
}

// --------------------------------------------------------------------

