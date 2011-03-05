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

#include "WidgetMain.h"
#include "ModelRouteList.h"
#include "ModelRouteWptTable.h"
#include "Route/RouteContainerSingleton.h"
#include "GraphicsSingleton.h"
#include "Unit/Value.h"
#include "Unit/Manager.h"
#include "Settings.h"
#include "FastUnitKeys.h"

#include "DialogLineEdit.h"
#include "DialogYesNo.h"
#include "DialogWaypoint.h"

#include "NesisTableView.h"
#include "NesisListView.h"
#include "NesisDoubleSpinBox.h"
#include "PanelMiscellaneous.h"

#include "DialogFlightPlan.h"


// -----------------------------------------------------------------------

DialogFlightPlan::DialogFlightPlan(QWidget* pParent)
 : AbstractNesisDialog(pParent)
{
	Settings* pS 			= Settings::GetInstance();
	FastUnitKeys* pFUK 	= FastUnitKeys::GetInstance();
	unit::Manager* pM 	= unit::Manager::GetInstance();

	using namespace common;
	
	setGeometry(0,0, 600,400);
	m_bMoveToTop = true;
	
	// Create Models
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);
	
	// Routes
	m_pmodRoute = new ModelRouteList(
		route::RouteContainerSingleton::GetInstance(),
		this
	);
	// Waypoints
	m_pmodWpt = new ModelRouteWptTable(this);

	// main layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;

	// horizontal layout for the top
	QHBoxLayout* pHorLayout = new QHBoxLayout;

	// Crusing speed ------------------------------------------
	int iSpeedUnit = pS->GetAirspeedUnit();		// user unit
	QVBoxLayout* pLayoutLeft = new QVBoxLayout;
	QLabel* pLbl = new QLabel(tr("Cruising speed"));
	pLbl->setFont(fontSm);
	pLayoutLeft->addWidget(pLbl);
	m_psbSpeed = new NesisDoubleSpinBox(0);
	m_psbSpeed->setFont(fontSm);
	m_psbSpeed->setRange(
		Snap(pM->Convert( 50.0, pFUK->iKey_km_h, iSpeedUnit), 5.0f),
		Snap(pM->Convert(350.0, pFUK->iKey_km_h, iSpeedUnit), 5.0f)
	);
	m_psbSpeed->setSingleStep(5);
	float fSpeed = pS->GetCruisingSpeed(); // read from setting m/s 
	m_psbSpeed->setValue(Snap(pM->Convert(fSpeed, pFUK->iKey_m_s, iSpeedUnit), 5.0f));
	m_psbSpeed->setSuffix(QString(" " + pM->GetUnit(iSpeedUnit)->GetSignature()));
	pLayoutLeft->addWidget(m_psbSpeed);
	QStringList slSpeedExternalCmd;
	slSpeedExternalCmd << QString();
	slSpeedExternalCmd << QString();
	slSpeedExternalCmd << QString();
	slSpeedExternalCmd << QString();
	slSpeedExternalCmd << tr("Close");
	m_psbSpeed->SetExternalButtonLabels(slSpeedExternalCmd);

	// Estimated time of departure
/*	pLbl = new QLabel(tr("ET Departure"));
	pLbl->setFont(fontSm);
	pLayoutLeft->addWidget(pLbl);*/
/*	m_pleDeparture = new QLineEdit;
	pLayoutLeft->addWidget(m_pleDeparture);*/
	pLayoutLeft->addStretch(100);

	pHorLayout->addLayout(pLayoutLeft);

	// Route list view ----------------------------------------
	QVBoxLayout* pLayoutRoute = new QVBoxLayout;
	pLbl = new QLabel(tr("Route list"));
	pLbl->setFont(fontSm);
	pLayoutRoute->addWidget(pLbl);

	QStringList slRouteExternalCmd;
	slRouteExternalCmd << tr("New");
	slRouteExternalCmd << tr("Edit");
	slRouteExternalCmd << tr("Delete");
	slRouteExternalCmd << tr("Reverse");
	slRouteExternalCmd << tr("Close");
	
	m_plstRoute = new NesisListView();
	m_plstRoute->setFont(fontSm);
	m_plstRoute->SetExternalButtonLabels(slRouteExternalCmd);
	m_plstRoute->setModel(m_pmodRoute);
	pLayoutRoute->addWidget(m_plstRoute);

	pHorLayout->addLayout(pLayoutRoute);
	pHorLayout->setStretchFactor(pLayoutLeft,2);
	pHorLayout->setStretchFactor(pLayoutRoute,3);

	pMainLayout->addLayout(pHorLayout);
	
	// Waypoint table view ------------------------------------
	m_plblWaypoint = new QLabel(tr("Waypoint list"));
	m_plblWaypoint->setFont(fontSm);
	pMainLayout->addWidget(m_plblWaypoint);
	m_ptblWaypoint = new NesisTableView();
	m_ptblWaypoint->setFont(fontSm);
	m_ptblWaypoint->setModel(m_pmodWpt);

	QStringList slWaypointExternalCmd;
	slWaypointExternalCmd << tr("Insert");
	slWaypointExternalCmd << tr("Delete");
	slWaypointExternalCmd << tr("Rename");
	slWaypointExternalCmd << tr("Reverse");
	slWaypointExternalCmd << tr("Close");
	m_ptblWaypoint->SetExternalButtonLabels(slWaypointExternalCmd);

	m_ptblWaypoint->setColumnWidth(0,75);	// key
	m_ptblWaypoint->setColumnWidth(1,185);	// descr
	m_ptblWaypoint->setColumnWidth(2,55);	// bearing
	m_ptblWaypoint->setColumnWidth(3,77);	// distance
	m_ptblWaypoint->setColumnWidth(4,60);	// time
	m_ptblWaypoint->setColumnWidth(5,60);	// ETA

	pMainLayout->addWidget(m_ptblWaypoint);
	
	// Set the main layout.
	pMainLayout->setStretchFactor(pHorLayout, 1);
	pMainLayout->setStretchFactor(m_ptblWaypoint, 4);
	setLayout(pMainLayout);
	
	// Connect
	// Focus connections.
	connect(m_psbSpeed, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextChild()));
	connect(m_plstRoute, SIGNAL(NesisKnob(bool)), this, SLOT(FocusPreviousChild()));
	
	connect(m_plstRoute->selectionModel(),
			  SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
			  this, 
           SLOT(OnCurrentRowChanged(const QModelIndex&, const QModelIndex&))
	);
	connect(m_psbSpeed, SIGNAL(valueChanged(int)), 
			  this, SLOT(OnSpeedChanged(int))
   );

	// Speed connections
	connect(m_psbSpeed, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_psbSpeed, SIGNAL(NesisButton(int)), this, SLOT(OnSpeedCommand(int)));

	// Route connections
	connect(m_plstRoute, SIGNAL(NesisButton(int)), this, SLOT(OnRouteCommand(int)));
	connect(m_plstRoute, SIGNAL(NesisOk()), this, SLOT(OnEditRoute()));
// 	connect(m_plstRoute, SIGNAL(NesisCancel()), this, SLOT(FocusPreviousChild()));

	// Waypoint connections
	connect(m_ptblWaypoint, SIGNAL(NesisButton(int)), this, SLOT(OnWaypointCommand(int)));
	connect(m_ptblWaypoint, SIGNAL(NesisOk()), this, SLOT(OnInsertWpt()));
	connect(m_ptblWaypoint, SIGNAL(NesisCancel()), this, SLOT(FocusPreviousChild()));

	// Select the first route if there are any.
	m_plstRoute->setCurrentIndex(m_pmodRoute->CreateModelIndex(0));
// 	m_plstRoute->setCurrentRow(-1);

	// Speed 
	OnSpeedChanged(m_psbSpeed->value());
	
	// Set focus to the list
	m_plstRoute->setFocus();
}

DialogFlightPlan::~DialogFlightPlan()
{
}

// -----------------------------------------------------------------------

void DialogFlightPlan::OnSpeedCommand(int iCmd)
{
	if(iCmd == AbstractNesisInput::pbBtn5)
		OnClose();
}

// -----------------------------------------------------------------------

void DialogFlightPlan::OnRouteCommand(int iCmd)
{
	switch(iCmd) {
		case 0: OnNewRoute(); break;
		case 1: OnEditRoute(); break;
		case 2: OnDeleteRoute(); break;
		case 3: OnReverseRoute(); break;
		case 4: OnClose(); break;
		default: break;
	}
}

// -----------------------------------------------------------------------

void DialogFlightPlan::OnWaypointCommand(int iCmd)
{
	switch(iCmd) {
		case 0: OnInsertWpt(); break;
		case 1: OnDeleteWpt(); break;
		case 2: OnRenameRoute(); break;
		case 3: OnReverseRoute(); break;
		case 4: OnClose(); break;
		default: break;
	}
}

// -----------------------------------------------------------------------

void DialogFlightPlan::OnCurrentRowChanged(
		const QModelIndex& miNew, 
	const QModelIndex& /*miOld*/
												)
{
	route::Route* pR = m_pmodRoute->GetRoute(miNew);
	qDebug() << "Row changed: row = " << miNew.row() << ", " << pR->GetName();
	m_pmodWpt->SetRoute(pR);
	m_plblWaypoint->setText(
			tr("Waypoint list for \"<font color=blue>%1</font>\" route.").arg(pR->GetName())
								  );
	SetWaypointTableRowSizes();
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnNewRoute()
{
	using namespace route;
	// First we need a name for the route.
	DialogLineEdit dlg(
		tr("Enter new route name:\n"
		   "(keep empty for auto name)")
	);
	if(dlg.exec()==QDialog::Accepted) {
		QString qsName = dlg.GetInputText();
		QModelIndex mi = m_pmodRoute->Append(new Route(qsName));
		m_plstRoute->setCurrentIndex(mi);
		m_ptblWaypoint->setFocus(Qt::OtherFocusReason);
		m_ptblWaypoint->SetEditMode();
	}
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnEditRoute()
{
	// Get current selection.
	QModelIndex mi = m_plstRoute->selectionModel()->currentIndex();
	if(mi.isValid()) {
		m_ptblWaypoint->setFocus(Qt::OtherFocusReason);
		m_ptblWaypoint->selectRow(0);
		m_ptblWaypoint->SetEditMode();
	}
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnDeleteRoute()
{
	// Get current selection.
	QModelIndex mi = m_plstRoute->selectionModel()->currentIndex();
	if(mi.isValid()) {
		// We need conirmation.
		DialogYesNo dlg(
			tr("Are you sure to delete the selected route?")
		);
		if(dlg.exec()==QDialog::Accepted && dlg.IsYes()) {
			mi = m_pmodRoute->Erase(mi);
			if(!mi.isValid())
				m_pmodWpt->SetRoute(NULL);
			m_plstRoute->setCurrentIndex(mi);
		}
	}
}
// --------------------------------------------------------------------

void DialogFlightPlan::OnReverseRoute()
{
	qDebug() << "Reverse route";
	// Get current selection.
	QModelIndex mi = m_plstRoute->selectionModel()->currentIndex();
	if(mi.isValid()) {
		route::Route* pR = m_pmodRoute->GetRoute(mi);
		pR->Reverse();
		m_pmodWpt->SetRoute(pR);
		if(QApplication::focusWidget()==m_ptblWaypoint)
			m_ptblWaypoint->selectRow(0);
		// TODO A way to change the name of the list for all "auto names".
	}
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnClose()
{
	m_pmodRoute->Save();
	m_pmodWpt->SetRoute(NULL);
	accept();
}

// --------------------------------------------------------------------
	
void DialogFlightPlan::SetWaypointTableRowSizes()
{
	for(int i=0; i<m_pmodWpt->rowCount(); i++)
		m_ptblWaypoint->setRowHeight(i,21);
}

// --------------------------------------------------------------------

void DialogFlightPlan::InsertWaypoint(int iPos)
{
	// TODO The control should not receive focus until there is a vaild route.
	if(!m_pmodWpt->IsValid())
		return;

	// Waypoint selection dialog - initialize it with selected waypoint coordinate.
	// If there are no waypoints, take current position from senap.
	DialogWaypoint dlg(m_pmodWpt->GetCoordinates(iPos));
	if(dlg.exec()==QDialog::Accepted) {
		const route::Waypoint* pW = dlg.GetWaypoint();
		if(pW != NULL) {
			QModelIndex mi = m_pmodWpt->Insert(iPos, pW);
//			m_ptblWaypoint->setCurrentIndex(mi);
			m_ptblWaypoint->selectRow(iPos+1);
			SetWaypointTableRowSizes();
		}
	}
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnInsertWpt()
{
	if(!m_pmodWpt->IsValid())
		return;
	// Get current selection.
	QModelIndex mi = m_ptblWaypoint->selectionModel()->currentIndex();
	InsertWaypoint(mi.row());
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnAppendWpt()
{
	if(!m_pmodWpt->IsValid())
		return;
	InsertWaypoint(-1);
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnDeleteWpt()
{
	if(!m_pmodWpt->IsValid())
		return;
	// Get current selection.
	QModelIndex mi = m_ptblWaypoint->selectionModel()->currentIndex();
	if(mi.isValid()) {
		DialogYesNo dlg(
				tr("Are you sure to delete the waypoint?")
		);
		if(dlg.exec()==QDialog::Accepted && dlg.IsYes()) {
			int iPos = mi.row();
			QModelIndex mi = m_pmodWpt->Erase(iPos);
			m_ptblWaypoint->setCurrentIndex(mi);
			SetWaypointTableRowSizes();
		}
	}
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnSpeedChanged(int iSpeed)
{
	Settings* pS = Settings::GetInstance();
	float fSpeed = unit::Manager::GetInstance()->Convert(
		iSpeed, 
		pS->GetAirspeedUnit(),
		FastUnitKeys::GetInstance()->iKey_m_s
	);

	m_pmodWpt->SetSpeed(fSpeed);
	pS->SetCruisingSpeed(fSpeed);
}

// --------------------------------------------------------------------

void DialogFlightPlan::OnRenameRoute()
{
	using namespace route;
	// First we need a name for the route.
	DialogLineEdit dlg(
		tr("Enter new route name:\n"
		   "(keep empty for auto name)")
	);
	if(dlg.exec()==QDialog::Accepted) {
		QString qsName = dlg.GetInputText();
		QModelIndex mi = m_plstRoute->selectionModel()->currentIndex();
		m_pmodRoute->Rename(mi, qsName);
		m_plstRoute->setCurrentIndex(mi);
		m_ptblWaypoint->setFocus(Qt::OtherFocusReason);
		m_ptblWaypoint->SetEditMode();
	}
}
