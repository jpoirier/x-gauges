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
#include "Route/WaypointContainerSingleton.h"
#include "Route/Airfield.h"
#include "MathEx.h"
#include "ModelWaypointTable.h"
#include "GraphicsSingleton.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "Unit/Manager.h"

#include "NesisCheckBox.h"
#include "NesisEditBox2.h"
#include "NesisSpinBox.h"
#include "NesisTableView.h"
#include "NesisAerospaceDataManager.h"

#include "DialogNearestAirfield.h"

#define DEFAULT_RADIUS 300

// -----------------------------------------------------------------------

DialogNearestAirfield::DialogNearestAirfield(const QPointF& ptX, QWidget* pParent)
 : AbstractNesisDialog(pParent)
{
/*	Settings*       pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	unit::Manager*  pM = unit::Manager::GetInstance();*/
	
	m_bICAO = true;

	m_ptX = ptX;
	
	// Size of the dialog
	setMinimumWidth(500);

	// Used to set fonts
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	QFont fontSm = pGS->GetFont(GraphicsSingleton::fDialogSmall);

	// The model - results of the search operation.
	m_pMod = new ModelWaypointTable(this);
	m_pMod->SetCoordinates(m_ptX);
	m_pMod->SetHeading(NesisAerospaceDataManager::GetInstance()->GetYaw());

	// Main vertical layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);


	// Comment - number of items found in the container
	QLabel* pLbl = new QLabel(tr("List of nearest airfields"));
	pLbl->setFont(fontSm);
	pMainLayout->addWidget(pLbl);

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
	QStringList sl;
	sl << tr("ICAO");
	sl << tr("All");
	m_ptblHits->SetExternalButtonLabels(sl);

	// Connect table Ok button with the acception.
	connect(m_ptblHits, SIGNAL(NesisOk()), this, SLOT(accept()));
	// Connect table Cancel button with rejection
	connect(m_ptblHits, SIGNAL(NesisEditCancel()), this, SLOT(reject()));
	// Connect selection (filter) buttons
	connect(m_ptblHits, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));

	// Update the list
	OnUpdateList();

	// Set focus
	m_ptblHits->SetEditMode();
	m_ptblHits->setFocus();
}

DialogNearestAirfield::~DialogNearestAirfield()
{
}

// -----------------------------------------------------------------------

void DialogNearestAirfield::OnUpdateList()
{
/*	Settings*       pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	unit::Manager*  pM = unit::Manager::GetInstance();*/

	using namespace route;
	WaypointContainerSingleton* pTPC;
	pTPC = WaypointContainerSingleton::GetInstance();

	int iTypes = Waypoint::tAirfield;

	// Search the container for airfields.
	QVector<const Waypoint*> con = pTPC->Search(
		m_ptX, 					// Current point
		iTypes, 					// Type filter
		DEFAULT_RADIUS,		// km, Radius filter
		QString()				// Text filter
	);
	
	// Now, filter the results once again.
	if(m_bICAO) {
		QVector<const Waypoint*> con2;
		QVector<const Waypoint*>::const_iterator ci;
		for(ci=con.begin(); ci!=con.end(); ci++) {
			const route::Airfield* pA = (const route::Airfield*)(*ci);
			if(pA->GetICAO().isEmpty()==false)
				con2 << pA;
		}
		con = con2;
	}

	// Display the results
	m_pMod->SetWaypoints(con);
	for(int i=0; i<con.count(); i++)
		m_ptblHits->setRowHeight(i,23);

	// Select first row
	if(!con.isEmpty())
		m_ptblHits->selectRow(0);
}

// -----------------------------------------------------------------------

const route::Airfield* DialogNearestAirfield::GetAirfield() const
{
	QModelIndex mi = m_ptblHits->currentIndex();
	if(mi.isValid()) {
		return (route::Airfield*)m_pMod->GetWaypoint(mi.row());
	}
	return NULL;
}

// -----------------------------------------------------------------------

void DialogNearestAirfield::OnNesisButton(int i)
{
	qDebug() << "Button" << i;
	if(i==AbstractNesisInput::pbBtn1) {
		m_bICAO = true;
		OnUpdateList();
	}
	else if(i==AbstractNesisInput::pbBtn2) {
		m_bICAO = false;
		OnUpdateList();
	}
}

// -----------------------------------------------------------------------
