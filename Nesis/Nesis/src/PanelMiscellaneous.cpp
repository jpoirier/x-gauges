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

#include "Settings.h"
#include "NesisListView.h"
#include "WidgetMain.h"
#include "GraphicsSingleton.h"

#include "DialogFlightPlan.h"
#include "DialogLogbook.h"
#include "DialogPilots.h"
#include "DialogCompassCalibration.h"
#include "DialogSoftwareUpdate.h"
#include "DialogSoftwareManagement.h"
#include "DialogSettings.h"
#include "DialogBrowser.h"
#include "DialogSetTime.h"
#include "DialogMount.h"
#include "DialogEngine.h"
#include "DialogAlarms.h"
#include "DialogReboot.h"

#include "NesisAerospaceDataManager.h"
#include "NesisUnitCAN.h"

#include "PanelMiscellaneous.h"

#define T_FLIGHT_PLANNING				QListWidgetItem::UserType + 0
#define T_LOGBOOK							QListWidgetItem::UserType + 1
#define T_PILOTS							QListWidgetItem::UserType + 2
#define T_SETTINGS						QListWidgetItem::UserType + 3
#define T_LEVEL							QListWidgetItem::UserType + 4
#define T_COMPASS							QListWidgetItem::UserType + 5
#define T_MANUAL							QListWidgetItem::UserType + 6
#define T_ABOUT							QListWidgetItem::UserType + 7
#define T_UPDATE							QListWidgetItem::UserType + 8
#define T_TIME								QListWidgetItem::UserType + 9
#define T_ENGINE							QListWidgetItem::UserType + 10
#define T_ALARMS							QListWidgetItem::UserType + 11

// -----------------------------------------------------------------------

PanelMiscellaneous::PanelMiscellaneous(QSize sz, QWidget *pParent)
 : AbstractPanel(sz, pParent)
{
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(
		GraphicsSingleton::fDialogSmall
	);
	Settings* pS = Settings::GetInstance();
	setFocusPolicy(Qt::NoFocus);

	setFixedSize(sz);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	// Icon widget on the left
	m_pwIcon = new NesisListWidget(true);
	m_pwIcon->setFont(fontSm);
	m_pwIcon->setViewMode(QListView::IconMode);
	m_pwIcon->setIconSize(QSize(64,64));
// 	m_pwIcon->setGridSize(QSize(120,150));
	m_pwIcon->setSpacing(42);
// 	m_pwIcon->setUniformItemSizes(true);
	m_pwIcon->setMovement(QListView::Static);
	m_pwIcon->setAlternatingRowColors(false);
	m_pwIcon->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pwIcon->setWrapping(true);
	m_pwIcon->setWordWrap(true);
	m_pwIcon->setTextElideMode(Qt::ElideNone);
	m_pwIcon->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pwIcon->viewport()->setBackgroundRole(QPalette::Base);

	pLayout->addWidget(m_pwIcon);

	// Flight planning
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"FlightPlan_64.png"),
		tr("Route\nPlanning"),
		m_pwIcon,
		T_FLIGHT_PLANNING
	);

	// Logbook
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Logbook_64.png"),
		tr("Logbook"),
		m_pwIcon,
		T_LOGBOOK
	);

	// Pilots and the active 	setEnabled(false);
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Pilots_64.png"),
		tr("Pilots"),
		m_pwIcon,
		T_PILOTS
	);

	// Customize
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Settings_64.png"),
		tr("Settings"),
		m_pwIcon,
		T_SETTINGS
	);

	// Mount calibration.
	if(pS->IsPrimary()) {
		new QListWidgetItem(
			QIcon(pS->GetImagePath()+"Mount_64.png"),
			tr("Level\nAdjust"),
			m_pwIcon,
			T_LEVEL
		);
	}

	// Calibration panel - magnetic calibration.
	if(pS->IsPrimary()) {
		new QListWidgetItem(
			QIcon(pS->GetImagePath()+"Compass_64.png"),
			tr("Compass\nCalibrat."),
			m_pwIcon,
			T_COMPASS
		);
	}

	// Time and Date
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Time_64.png"),
		tr("Time & Date"),
		m_pwIcon,
		T_TIME
	);

	// Manual
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Manual_64.png"),
		tr("Airplane\nManual"),
		m_pwIcon,
		T_MANUAL
	);

	// About
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"About_64.png"),
		tr("About"),
		m_pwIcon,
		T_ABOUT
	);

	// Update
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Update_64.png"),
		tr("Software\nUpdate"),
		m_pwIcon,
		T_UPDATE
	);

	// Engine
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Engine_64.png"),
		tr("Engine\nAirplane"),
		m_pwIcon,
		T_ENGINE
	);

	// Alarms
	new QListWidgetItem(
		QIcon(pS->GetImagePath()+"Alarms_64.png"),
		tr("Alarms"),
		m_pwIcon,
		T_ALARMS
	);

	// Connect main widget with direct buttons
	connect(m_pwIcon, SIGNAL(NesisButton(int)),
			  GetWidgetMain(), SLOT(ActivatePanel(int)));

	// Connect ok with activation
	connect(m_pwIcon, SIGNAL(NesisOk()), this, SLOT(ActivateSubpanel()));

	// Put selection to the first widget (logbook) by default.
	m_pwIcon->setCurrentRow(0);
}
// -----------------------------------------------------------------------

PanelMiscellaneous::~PanelMiscellaneous()
{
}

// -----------------------------------------------------------------------

void PanelMiscellaneous::ActivateSubpanel()
{
	Settings* pS = Settings::GetInstance();

	int iR = m_pwIcon->currentRow();
	if(iR < 0 || iR >= m_pwIcon->count())
		return;

	// Get the icon type
	iR = m_pwIcon->item(iR)->type();

	// Open a widget according to selected icon
	if(iR==T_FLIGHT_PLANNING) {
		DialogFlightPlan dlg(this);
		dlg.exec();
	}
	else if(iR==T_LOGBOOK) {
		DialogLogbook dlg(this);
		dlg.exec();
	}
	else if(iR==T_PILOTS) {
		DialogPilots dlg(this);
		dlg.exec();
	}
	else if(iR==T_COMPASS) {
		can::UnitInfoMagu* pMagu = NesisUnitCAN::GetInstance()->GetUnitInfoContainer().GetMagu();
		if(pMagu) {
			DialogCompassCalibration dlg(this);
			dlg.exec();
		}
	}
	else if(iR==T_UPDATE) {
		DialogSoftwareManagement dlg(this);
		dlg.exec();
	}
	else if(iR==T_SETTINGS) {
		DialogSettings dlg(this);
		if(dlg.exec()==QDialog::Accepted) {
			DialogReboot rbt(tr("Settings were changed."));
			rbt.exec();
			GetWidgetMain()->SetReboot();
			GetWidgetMain()->close();
		}
	}
	else if(iR==T_ABOUT) {
		QUrl url = QUrl::fromLocalFile(pS->GetHelpPath() + "about.html");
		DialogBrowser dlg(url, this);
		dlg.exec();
	}
	else if(iR==T_TIME) {
		DialogSetTime dlg(this);
		if(dlg.exec() == QDialog::Accepted) {
			DialogReboot rbt(tr("System time was changed."));
			rbt.exec();
			GetWidgetMain()->SetReboot();
			GetWidgetMain()->close();
		} 
	}
	else if(iR==T_LEVEL) {
//		DialogMount dlg(this);
		if(NesisUnitCAN::GetInstance()->GetUnitInfoContainer().GetAiru()) {
			DialogMountCorrection dlg(this);
			dlg.exec();
		}
	}
	else if(iR==T_ENGINE) {
		DialogEngine dlg(this);
		if(dlg.exec() == QDialog::Accepted) {
			DialogReboot rbt(tr("Engine parameters were changed."));
			rbt.exec();
			GetWidgetMain()->SetReboot();
			GetWidgetMain()->close();
		} 
	}
	else if(iR==T_MANUAL) {
		QUrl url = QUrl::fromLocalFile(pS->GetHelpPath() + "manual.html");
		DialogBrowser dlg(url, this);
		dlg.exec();
	}
	else if(iR==T_ALARMS) {
		DialogAlarms dlg(this);
		dlg.exec();
	}
	else {
		qDebug() << "Option not implemented.";
	}

	Activate();
}

// -----------------------------------------------------------------------

void PanelMiscellaneous::Activate()
{
	qDebug() << "Misc Panel";
	m_pwIcon->SetEditMode();
	m_pwIcon->setFocus(Qt::OtherFocusReason);
}

// -----------------------------------------------------------------------
