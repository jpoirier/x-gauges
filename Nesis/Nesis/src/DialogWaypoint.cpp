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
#include "Route/WaypointContainerSingleton.h"
#include "Route/Airfield.h"
#include "MathEx.h"
#include "ModelWaypointTable.h"
#include "GraphicsSingleton.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "Unit/Manager.h"
#include "DialogUserWaypoint.h"

#include "NesisCheckBox.h"
#include "NesisEditBox2.h"
#include "NesisSpinBox.h"
#include "NesisTableView.h"

#include "DialogWaypoint.h"

// -----------------------------------------------------------------------
// TODO The selection in the table should not be set until the table gets focus.

DialogWaypoint::DialogWaypoint(const QPointF& ptX, bool bDeactivate, QWidget* pParent)
 : AbstractNesisDialog(pParent)
{
	Settings*       pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	unit::Manager*  pM = unit::Manager::GetInstance();

	m_ptX = ptX;
	m_bDeactivate = bDeactivate;
	m_bWasDeactivated = false;

	// Used to set fonts
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	QFont fontSm = pGS->GetFont(GraphicsSingleton::fDialogSmall);
	const QFont& fontBuddy = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fKnobBuddy);

	// The model - results of the search operation.
	m_pMod = new ModelWaypointTable(this);
	m_pMod->SetCoordinates(m_ptX);

	// Main vertical layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);

	// First line layout -----------------------------
	QHBoxLayout* pFirstLineLayout = new QHBoxLayout;

	QHBoxLayout* pGroupBoxLayout = new QHBoxLayout;

	// Check boxes
	m_pcbAirfield = new NesisCheckBox(tr("Airfield"));
	m_pcbAirfield->setFont(fontSm);
	m_pcbAirfield->setChecked(true);	// TODO Store/read from settings.
	pGroupBoxLayout->addWidget(m_pcbAirfield);

	m_pcbNavAid = new NesisCheckBox(tr("NavAid"));
	m_pcbNavAid->setFont(fontSm);
	pGroupBoxLayout->addWidget(m_pcbNavAid);

	m_pcbVFRPoint = new NesisCheckBox(tr("VFR"));
	m_pcbVFRPoint->setFont(fontSm);
	pGroupBoxLayout->addWidget(m_pcbVFRPoint);

	m_pcbUserPoint = new NesisCheckBox(tr("User"));
	m_pcbUserPoint->setFont(fontSm);
	pGroupBoxLayout->addWidget(m_pcbUserPoint);

	QGroupBox* pGIP = new QGroupBox(tr("Active types"));
	pGIP->setLayout(pGroupBoxLayout);
	pFirstLineLayout->addWidget(pGIP);

	// Spacer
	pFirstLineLayout->addStretch(100);

	// Radius label
	QLabel* pLbl = new QLabel(tr("Radius:"));
	pLbl->setFont(fontSm);
	pFirstLineLayout->addWidget(pLbl);

	// Radius spin
	int iDistUnit = pS->GetDistanceUnit();
	int iStep = common::Snap(pM->Convert(  50, pFUK->iKey_km, iDistUnit), 25);
	int iMin  = common::Snap(pM->Convert( 100, pFUK->iKey_km, iDistUnit), iStep);
	int iMax  = common::Snap(pM->Convert(2000, pFUK->iKey_km, iDistUnit), iStep);
	int iVal  = common::Snap(pM->Convert(pS->GetWaypointSearchRadius(), pFUK->iKey_km, iDistUnit), iStep);

	m_pspnRadius = new NesisSpinBox(0);
	m_pspnRadius->setFont(fontSm);
	m_pspnRadius->setRange(iMin,iMax);
	m_pspnRadius->setSingleStep(iStep);
	m_pspnRadius->setValue(iVal);
	m_pspnRadius->setSuffix(QString(" ") + pM->GetUnit(iDistUnit)->GetSignature());
	m_pspnRadius->setButtonSymbols(QSpinBox::NoButtons);
	pFirstLineLayout->addWidget(m_pspnRadius);

	// end of first line
	pMainLayout->addLayout(pFirstLineLayout);

	// Second line --------------------------------
	QHBoxLayout* pSecondLineLayout = new QHBoxLayout;

	// Filter label
	pLbl = new QLabel(tr("Name Filter:"));
	pLbl->setFont(fontSm);
	pSecondLineLayout->addWidget(pLbl);

	// Filter
	m_pedtFilter = new NesisEditBox2(QString());
	m_pedtFilter->setFont(fontSm);
	m_pedtFilter->setMaximumWidth(140);
	pSecondLineLayout->addWidget(m_pedtFilter);
	QLabel* plBuddy = new QLabel;
	plBuddy->setFont(fontBuddy);
	plBuddy->hide();
	pSecondLineLayout->addWidget(plBuddy);
	m_pedtFilter->SetBuddy(plBuddy);

	// Spacer
	pSecondLineLayout->addStretch(100);

	// Comment - number of items found in the container
	m_plblComment = new QLabel(tr("0 items found."));
	m_plblComment->setFont(fontSm);
	pSecondLineLayout->addWidget(m_plblComment);

	// end of second line
	pMainLayout->addLayout(pSecondLineLayout);

	// Table ----------------------------------
	m_ptblHits = new NesisTableView();
	m_ptblHits->setModel(m_pMod);
	m_ptblHits->setFont(fontSm);
	m_ptblHits->setColumnWidth(0,90);
	m_ptblHits->setColumnWidth(1,215);
	m_ptblHits->setColumnWidth(2,60);
	m_ptblHits->setColumnWidth(3,68);
	m_ptblHits->verticalHeader()->setMaximumWidth(35);
	m_ptblHits->horizontalHeader()->setStretchLastSection(true);
	m_ptblHits->setWordWrap(false);
//	m_ptblHits->setDragDropMode(QAbstractItemView::NoDragDrop);
	m_ptblHits->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_ptblHits->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	m_ptblHits->setTextElideMode(Qt::ElideNone);

	pMainLayout->addWidget(m_ptblHits);

	// Buttons
	QStringList slCmd;
	slCmd << tr("Name");
	slCmd << tr("Radius");
	slCmd << tr("Type");
	slCmd << tr("New");
 	if(m_bDeactivate) {
 		slCmd << tr("Deselect");
 	}
 	else {
 		slCmd << tr("Close");
 	}
	m_ptblHits->SetExternalButtonLabels(slCmd);

	// List of external commands
	QStringList slCmd2;
	slCmd2 << tr("Accept");
	slCmd2 << QString();
	slCmd2 << QString();
	slCmd2 << QString();
	slCmd2 << QString();
	m_pspnRadius->SetExternalButtonLabels(slCmd2);
	m_pcbAirfield->SetExternalButtonLabels(slCmd2);
	m_pcbVFRPoint->SetExternalButtonLabels(slCmd2);
	m_pcbNavAid->SetExternalButtonLabels(slCmd2);
	m_pcbUserPoint->SetExternalButtonLabels(slCmd2);

	// Connections

	// Control changes
	connect(m_pcbAirfield, 		SIGNAL(stateChanged(int)), this, SLOT(OnUpdateList()));
	connect(m_pcbVFRPoint, 		SIGNAL(stateChanged(int)), this, SLOT(OnUpdateList()));
	connect(m_pcbUserPoint, 	SIGNAL(stateChanged(int)), this, SLOT(OnUpdateList()));
	connect(m_pcbNavAid, 		SIGNAL(stateChanged(int)), this, SLOT(OnUpdateList()));
	connect(m_pspnRadius, 		SIGNAL(valueChanged(int)), this, SLOT(OnUpdateList()));
	connect(m_pedtFilter, 		SIGNAL(textChanged(const QString&)), this, SLOT(OnUpdateList()));

	// Filter Ok signal
	connect(m_pspnRadius, SIGNAL(NesisOk()), this, SLOT(SetFocusToHits()));
	connect(m_pedtFilter, SIGNAL(NesisOk()), this, SLOT(OnNameOk()));
	connect(m_pedtFilter, SIGNAL(NesisEditCancel()), this, SLOT(SetFocusToHits()));

	// Connect table Ok button with the acception.
	connect(m_ptblHits, SIGNAL(NesisOk()), this, SLOT(accept()));

	// Connect table Cancel button with rejection
	connect(m_ptblHits, 		SIGNAL(NesisEditCancel()), this, SLOT(reject()));
	
	// Connect knob buttons on types 
	connect(m_pcbAirfield,  SIGNAL(NesisKnob(bool)), this, SLOT(OnFocusNextType(bool)));
	connect(m_pcbNavAid,    SIGNAL(NesisKnob(bool)), this, SLOT(OnFocusNextType(bool)));
	connect(m_pcbVFRPoint,  SIGNAL(NesisKnob(bool)), this, SLOT(OnFocusNextType(bool)));
	connect(m_pcbUserPoint, SIGNAL(NesisKnob(bool)), this, SLOT(OnFocusNextType(bool)));
	
	// Cancel on type button activates the list
	connect(m_pcbAirfield, 	SIGNAL(NesisCancel()), this, SLOT(SetFocusToHits()));
	connect(m_pcbNavAid, 	SIGNAL(NesisCancel()), this, SLOT(SetFocusToHits()));
	connect(m_pcbVFRPoint, 	SIGNAL(NesisCancel()), this, SLOT(SetFocusToHits()));
	connect(m_pcbUserPoint, SIGNAL(NesisCancel()), this, SLOT(SetFocusToHits()));
	// The same is true with the radius
	connect(m_pspnRadius,   SIGNAL(NesisEditCancel()), this, SLOT(SetFocusToHits()));

	// Connect external buttons with the handler
 	connect(m_ptblHits, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));
 	
	connect(m_pspnRadius, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton2(int)));
	connect(m_pcbAirfield, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton2(int)));
	connect(m_pcbVFRPoint, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton2(int)));
	connect(m_pcbNavAid, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton2(int)));
	connect(m_pcbUserPoint, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton2(int)));

	// Update the list
	OnUpdateList();

	// Set focus
	SetFocusToHits();
}

DialogWaypoint::~DialogWaypoint()
{
}

// -----------------------------------------------------------------------

void DialogWaypoint::OnUpdateList()
{
	Settings*       pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	unit::Manager*  pM = unit::Manager::GetInstance();

// 	qDebug() << "Update called";
	using namespace route;
	WaypointContainerSingleton* pTPC;
	pTPC = WaypointContainerSingleton::GetInstance();

	int iTypes = 0;
	if(m_pcbAirfield->isChecked())
		iTypes |= Waypoint::tAirfield;
	if(m_pcbNavAid->isChecked())
		iTypes |= Waypoint::tNavAid;
	if(m_pcbVFRPoint->isChecked())
		iTypes |= Waypoint::tVFR;
	if(m_pcbUserPoint->isChecked())
		iTypes |= Waypoint::tUser;

	// Get radius in km
	int iRadiusKm = pM->Convert(
		m_pspnRadius->value(),
		pS->GetDistanceUnit(),
		pFUK->iKey_km
	);
	// Store it into settings.
	pS->SetWaypointSearchRadius(iRadiusKm);

	// Search the contaimer ...
	QVector<const Waypoint*> con = pTPC->Search(
		m_ptX, 					// Current point
		iTypes, 					// Type filter
		iRadiusKm,				// Radius filter
		m_pedtFilter->text() // Text filter
	);

	// Set the comment text
	m_plblComment->setText((tr("<b>%1</b> items found.").arg(con.size())));

	// Display the results
	m_pMod->SetWaypoints(con);
	for(int i=0; i<con.count(); i++)
		m_ptblHits->setRowHeight(i,23);// was 21

	// Select first row
	if(!con.isEmpty())
		m_ptblHits->selectRow(0);
}

// -----------------------------------------------------------------------

const route::Waypoint* DialogWaypoint::GetWaypoint() const
{
	if(m_bWasDeactivated)
		return NULL;
	
	QModelIndex mi = m_ptblHits->currentIndex();
	if(mi.isValid()) {
		qDebug() << "Selected -> " << m_pMod->GetWaypoint(mi.row())->GetDescription();
		return m_pMod->GetWaypoint(mi.row());
	}
	return NULL;
}

// -----------------------------------------------------------------------

void DialogWaypoint::SetFocusToHits()
{
	m_ptblHits->SetEditMode();
	m_ptblHits->setFocus();
}

// -----------------------------------------------------------------------

void DialogWaypoint::OnNesisButton(int iBtn)
{
	switch(iBtn) {
		case 0:	// Name filter
			m_pedtFilter->setFocus();
			m_pedtFilter->SetEditMode();
			break; 
		case 1:	// Radius filter
			m_pspnRadius->setFocus();
			m_pspnRadius->SetEditMode();
			break;
		case 2:	// Type filter
			m_pcbAirfield->setFocus();
			break;
		case 3:	// new filter
			{
				DialogUserWaypoint dlg2(m_ptX);
				dlg2.exec();
			}
			break;
		case 4:	// Deactivate or close
			if (m_bDeactivate) {
				m_bWasDeactivated = true;
				accept();
			}
			else {
				reject();
			}
				
			break;
		default:
			break;
	}
}
// -----------------------------------------------------------------------

void DialogWaypoint::OnNesisButton2(int iBtn)
{
	switch(iBtn) {
		case 0:	// Accept
			SetFocusToHits();
			break;
		default:
			break;
	}
}

// -----------------------------------------------------------------------

void DialogWaypoint::OnFocusNextType(bool bNext)
{
	if(m_pcbAirfield->hasFocus()) {
		if(bNext)
			m_pcbNavAid->setFocus(Qt::OtherFocusReason);
		else
			m_pcbUserPoint->setFocus(Qt::OtherFocusReason);
	}
	else if(m_pcbNavAid->hasFocus()) {
		if(bNext)
			m_pcbVFRPoint->setFocus(Qt::OtherFocusReason);
		else
			m_pcbAirfield->setFocus(Qt::OtherFocusReason);
	}
	else if(m_pcbVFRPoint->hasFocus()) {
		if(bNext)
			m_pcbUserPoint->setFocus(Qt::OtherFocusReason);
		else
			m_pcbNavAid->setFocus(Qt::OtherFocusReason);
	}
	else if(m_pcbUserPoint->hasFocus()) {
		if(bNext)
			m_pcbAirfield->setFocus(Qt::OtherFocusReason);
		else
			m_pcbVFRPoint->setFocus(Qt::OtherFocusReason);
	}
}

// -----------------------------------------------------------------------

void DialogWaypoint::OnNameOk()
{
	SetFocusToHits();
	// Check if there is only one selection
	if(m_pMod->rowCount()==1)
		accept();
}

// -----------------------------------------------------------------------

