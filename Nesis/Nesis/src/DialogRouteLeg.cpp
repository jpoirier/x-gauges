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
#include "Settings.h"
#include "FastUnitKeys.h"
#include "Unit/Value.h"

#include "NesisTableView.h"

#include "DialogRouteLeg.h"

// -----------------------------------------------------------------------

DialogRouteLeg::DialogRouteLeg(
	const route::Route* pR, 
	unsigned int uiActiveLeg, 
	QPointF ptX, 
	float fGS,
	QWidget* pParent
)
 : AbstractNesisDialog(pParent)
{
	m_pRoute 		= pR;
	m_ptX 			= ptX;
	m_fGS 			= fGS;
	m_uiActiveLeg 	= uiActiveLeg;

	m_eAction = aInvalid;
	m_iRow = -1; // invalid
	Q_ASSERT(pR);

	setMinimumWidth(500);
	setMinimumHeight(350);

	// Used to set fonts
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	QFont fontSm = pGS->GetFont(GraphicsSingleton::fDialogSmall);

	// Main vertical layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);

	// Route name
	QLabel* pLbl;
	pLbl = new QLabel(tr("Route: <b>%1</b>").arg(pR->GetName()));
	pLbl->setFont(fontSm);
	pMainLayout->addWidget(pLbl);

	// Table of waypoints
	QStringList sl;
	sl << "";
	sl << "";
	sl << tr("Select Leg");
	sl << tr("Direct To");
	sl << tr("Deactivate");

	m_pwtWpt = new NesisTableWidget();
	m_pwtWpt->SetExternalButtonLabels(sl);
	InitializeTable();
	pMainLayout->addWidget(m_pwtWpt);

	// Connections
	connect(m_pwtWpt, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));
	connect(m_pwtWpt, SIGNAL(NesisOk()), this, SLOT(OnNesisOk()));
	connect(m_pwtWpt, SIGNAL(NesisEditCancel()), this, SLOT(reject()));
	
	m_pwtWpt->selectRow(m_uiActiveLeg);
	m_pwtWpt->SetEditMode();
}

DialogRouteLeg::~DialogRouteLeg()
{
}

// -----------------------------------------------------------------------

void DialogRouteLeg::InitializeTable()
{
	Settings*       pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
//	unit::Manager*  pM = unit::Manager::GetInstance();

	m_pwtWpt->setFont(
		GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall)
	);

	m_pwtWpt->setColumnCount(5);
	m_pwtWpt->setColumnWidth(0,90);
	m_pwtWpt->setColumnWidth(1,135);
	m_pwtWpt->setColumnWidth(2,70);
	m_pwtWpt->setColumnWidth(3,70);
	
	QStringList slHeader;
	slHeader << tr("Key");
	slHeader << tr("Desc.");
	slHeader << tr("ETA");
	slHeader << tr("T");
	slHeader << tr("Dist.");
	m_pwtWpt->setHorizontalHeaderLabels(slHeader);

	m_pwtWpt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pwtWpt->verticalHeader()->setMaximumWidth(35);
	m_pwtWpt->horizontalHeader()->setStretchLastSection(true);
	m_pwtWpt->setWordWrap(false);
/*	m_pwtWpt->setDragDropMode(QAbstractItemView::NoDragDrop);
	m_pwtWpt->setEditTriggers(QAbstractItemView::NoEditTriggers);*/
	m_pwtWpt->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	m_pwtWpt->setTextElideMode(Qt::ElideNone);

	const int n = m_pRoute->GetWaypoints().GetCount();
	m_pwtWpt->setRowCount(n);
	QVector<std::pair<float, float> > 
	vTD = m_pRoute->GetTimeDistance(m_uiActiveLeg, m_ptX, m_fGS);
	Q_ASSERT(vTD.count() == n);

	QTableWidgetItem* pItem;
	for(int i=0; i<n; i++) {
		// Key
		const route::Waypoint* pW = m_pRoute->GetWaypoint(i);
		pItem = new QTableWidgetItem(QString(pW->GetKey()));
		m_pwtWpt->setItem(i,0, pItem);

		// Description - name
		pItem = new QTableWidgetItem(QString(pW->GetDescription()));
		m_pwtWpt->setItem(i,1, pItem);

		float fSec  = vTD[i].first;
		float fDist = vTD[i].second;

		// Assume empty
		QString qsT("-");
		QString qsETA("-");
		
		if(fSec > 0.0f) {
			QTime time(0,0);
			time  = time.addSecs(fSec);
			qsT   = time.toString("H:mm");
			time  = QTime::currentTime();
			time  = time.addSecs(fSec);
			qsETA = time.toString("H:mm");
		}
		pItem = new QTableWidgetItem(qsETA);
		pItem->setTextAlignment(Qt::AlignCenter);
		m_pwtWpt->setItem(i,2, pItem);
		
		pItem = new QTableWidgetItem(qsT);
		pItem->setTextAlignment(Qt::AlignCenter);
		m_pwtWpt->setItem(i,3, pItem);

		QString qsDist("-");
		if(fDist > 0.0f) {
			unit::Value uv(pFUK->iKey_m, fDist);
			uv.ConvertTo(pS->GetDistanceUnit());
			qsDist = uv.Format(0);
		} 
		pItem = new QTableWidgetItem(qsDist);
		pItem->setTextAlignment(Qt::AlignCenter);
		m_pwtWpt->setItem(i,4, pItem);
	}
}

// -----------------------------------------------------------------------

void DialogRouteLeg::OnNesisButton(int iBtn)
{
	// Remember the row
	m_iRow = m_pwtWpt->selectionModel()->currentIndex().row();
	switch(iBtn) {
		// Select leg
		case AbstractNesisInput::pbBtn3 :
			m_eAction = aSelectLeg;
			accept();
			break;

		// Direct to
		case AbstractNesisInput::pbBtn4 :
			m_eAction = aDirectTo;
			accept();
			break;
		// Deactivate route
		case AbstractNesisInput::pbBtn5 :
			m_iRow = -1;
			m_eAction = aDeactivate;
			accept();
			break;

		// Ignore anything else.
		default:
			break;
	}
}

// -----------------------------------------------------------------------

void DialogRouteLeg::OnNesisOk()
{
	m_iRow = m_pwtWpt->selectionModel()->currentIndex().row();
	m_eAction = aSelectLeg;
	accept();
}

// -----------------------------------------------------------------------
