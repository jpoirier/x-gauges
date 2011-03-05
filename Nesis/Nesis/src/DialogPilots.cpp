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
#include "NesisListView.h"
#include "NesisEditBox2.h"
#include "Logbook/PilotList.h"
#include "Settings.h"
#include "DialogLineEdit.h"
#include "DialogYesNo.h"
#include "GraphicsSingleton.h"

#include "DialogPilots.h"


// -----------------------------------------------------------------------

DialogPilots::DialogPilots(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pPilotLayout = new QVBoxLayout;
	setLayout(pPilotLayout);

	// List of pilots:
	QFrame* pFrame = new QFrame();
	pFrame->setFrameShape(QFrame::StyledPanel);
	pPilotLayout->addWidget(pFrame);

	// LIST layout
	QVBoxLayout* pListLayout = new QVBoxLayout;
	pFrame->setLayout(pListLayout);
	
	QLabel* pLbl = new QLabel(tr("Pilots"));
	pLbl->setFont(fontSm);
	pListLayout->addWidget(pLbl);
	
	QStringList slEditCmd;
	slEditCmd << tr("New");
	slEditCmd << tr("Edit");
	slEditCmd << tr("Delete");
	slEditCmd << QString();
	slEditCmd << tr("Close");
	
	m_plwPilots = new NesisListWidget(true /*edit mode only*/);
	m_plwPilots->SetExternalButtonLabels(slEditCmd);
	m_plwPilots->setFont(fontSm);

	m_plwPilots->setSortingEnabled(true);
	pListLayout->addWidget(m_plwPilots);

	// Currently selected pilot:
	pFrame = new QFrame();
	pFrame->setFrameShape(QFrame::StyledPanel);
	pPilotLayout->addWidget(pFrame);

	// current layout
	QVBoxLayout* pCurrentLayout = new QVBoxLayout;
	pFrame->setLayout(pCurrentLayout);
	
	pLbl = new QLabel(tr("Active pilot"));
	pLbl->setFont(fontSm);
	pCurrentLayout->addWidget(pLbl);
	m_pleSelected = new QLineEdit;
	m_pleSelected->setReadOnly(true);
	m_pleSelected->setFocusPolicy(Qt::NoFocus);
	m_pleSelected->setFont(fontSm);
	pCurrentLayout->addWidget(m_pleSelected);
	
	// External commands
	QStringList slExternalCmd;
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << tr("Close");

	connect(m_plwPilots, SIGNAL(NesisOk()), this, SLOT(OnPilotSelect()));
	connect(m_plwPilots, SIGNAL(NesisButton(int)), this, SLOT(OnNesisEditList(int)));
	connect(m_plwPilots, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));

	InitializeList();
	Activate();
}

DialogPilots::~DialogPilots()
{
}
// -----------------------------------------------------------------------

void DialogPilots::Activate()
{
	// This will also show buttons.
	setEnabled(true);
	m_plwPilots->SetEditMode();
	m_plwPilots->setFocus();
}

// -----------------------------------------------------------------------

void DialogPilots::OnClose()
{
	// Store some values into settings.
	Settings* pS = Settings::GetInstance();
	pS->SetCurrentPilot(m_pleSelected->text());

	if(m_bDirty) {
		logbook::PilotList::GetInstance()->WriteXML();
		m_bDirty = false;
	}
	accept();
}

// -----------------------------------------------------------------------

void DialogPilots::InitializeList()
{
	logbook::PilotList* pPL = logbook::PilotList::GetInstance();
	Settings* pS = Settings::GetInstance();

	// Set list of pilots
	m_plwPilots->addItems(pPL->GetList());
	m_bDirty = false;
	
	// Get current pilot from settings
	QString qs = pS->GetCurrentPilot();
	
	// Select correct pilot from the list
	QList<QListWidgetItem*> qlFound = m_plwPilots->findItems(qs, Qt::MatchFixedString);
	if(qlFound.isEmpty()==false) {
		m_plwPilots->setCurrentItem(qlFound[0]);
		m_pleSelected->setText(qs);
	}
	// If not found, select the first one.
	else {
		m_plwPilots->setCurrentRow(0);
	}
}

// -----------------------------------------------------------------------

void DialogPilots::OnExternalCmd(int iCmd)
{
	if(iCmd == AbstractNesisInput::pbBtn5) {
		OnClose();
	}
}

// -----------------------------------------------------------------------

void DialogPilots::OnPilotSelect()
{
	// Get selection from the list
	QString qs = m_plwPilots->currentItem()->text();
	m_pleSelected->setText(qs);
	// We will write the settigns, when the window is closed.
}

// -----------------------------------------------------------------------

void DialogPilots::OnNesisEditList(int iCmd)
{
	logbook::PilotList* pPL = logbook::PilotList::GetInstance();

	// New 
	if(iCmd == 0) {
		DialogLineEdit dlg(tr("Enter new pilot name."));
		if(dlg.exec()==QDialog::Accepted) {
			QString qsName = dlg.GetInputText();
			if(qsName.size() > 1) {
				m_plwPilots->addItem(qsName);
				// Update the PilotList
				pPL->Add(qsName);
				// Write to file.
				m_bDirty = true;
			}
		}
	}

	// Edit
	else if(iCmd == 1) {
		int iRow = m_plwPilots->currentRow();
		QString qsOld = m_plwPilots->currentItem()->text();

		DialogLineEdit dlg(tr("Modify pilot name."), qsOld);

		if(dlg.exec()==QDialog::Accepted) {
			QString qsNew = dlg.GetInputText();
			if(qsNew.size() > 1 && qsNew != qsOld) {
				// Update the widget list
				delete m_plwPilots->takeItem(iRow);
				m_plwPilots->addItem(qsNew);
				QList<QListWidgetItem*> qlFound = m_plwPilots->findItems(qsNew, Qt::MatchFixedString);
				if(qlFound.isEmpty()==false)
					m_plwPilots->setCurrentItem(qlFound[0]);

				// Update the PilotList
				pPL->Remove(qsOld);
				pPL->Add(qsNew);

				// Update current
				if(m_pleSelected->text() == qsOld) {
					m_pleSelected->setText(qsNew);
				}
				// Write to file.
				m_bDirty = true;
			}
		}
	}
	
	// Delete - remove selected pilot from the list.
	else if(iCmd == 2) {
		int iRow = m_plwPilots->currentRow();
		if(iRow < 0) return;
		
		// Request confirmation
		DialogYesNo dlg(
			tr("Are you sure to delete the selected pilot?")
		);
		if(dlg.exec()==QDialog::Accepted && dlg.IsYes()) {
			// Remove the item from the widget list
			QListWidgetItem* pItem = m_plwPilots->takeItem(iRow);
			QString qs = pItem->text();
			delete pItem;
			// and from the pilot list
			pPL->Remove(qs);

			// if removed pilot was also selected, clear selected string
			if(m_pleSelected->text() == qs) {
				m_pleSelected->setText(QString());
			}
			// Write changes to the file.
			m_bDirty = true;
		}
	}
	
	else {
		qDebug() << "Pilot list edit cmd" << iCmd << "was not handled."; 
	}
}

// -----------------------------------------------------------------------
