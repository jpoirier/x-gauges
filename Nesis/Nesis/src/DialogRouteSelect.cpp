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
#include "GraphicsSingleton.h"
#include "Settings.h"
#include "Route/RouteContainerSingleton.h"
#include "FastUnitKeys.h"
#include "Unit/Value.h"

#include "NesisTableView.h"

#include "DialogRouteSelect.h"

// -----------------------------------------------------------------------

DialogRouteSelect::DialogRouteSelect(
	QWidget* pParent
)
 : AbstractNesisDialog(pParent)
{
	m_pRoute = NULL;
	setMinimumWidth(320);

	// Used to set fonts
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	QFont fontSm = pGS->GetFont(GraphicsSingleton::fDialogSmall);

	// Main vertical layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);

	// Route name
	QLabel* pLbl;
	pLbl = new QLabel(tr("Select Route:"));
	pLbl->setFont(fontSm);
	pMainLayout->addWidget(pLbl);

	// Table of waypoints
	QStringList sl;
	sl << "";
	sl << "";
	sl << "";
	sl << tr("Activate");

	m_pwtRts = new NesisTableWidget();
	m_pwtRts->SetExternalButtonLabels(sl);
	InitializeTable();
	pMainLayout->addWidget(m_pwtRts);

	// Connections
	connect(m_pwtRts, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));
	connect(m_pwtRts, SIGNAL(NesisOk()), this, SLOT(OnNesisOk()));
	connect(m_pwtRts, SIGNAL(NesisEditCancel()), this, SLOT(reject()));
	
	m_pwtRts->selectRow(0);
	m_pwtRts->SetEditMode();
}

DialogRouteSelect::~DialogRouteSelect()
{
}

// -----------------------------------------------------------------------

void DialogRouteSelect::InitializeTable()
{
	using namespace route;
	Settings*       pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
//	unit::Manager*  pM = unit::Manager::GetInstance();
	RouteContainerSingleton* pRC = RouteContainerSingleton::GetInstance();

	m_pwtRts->setFont(
		GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall)
	);

	m_pwtRts->setColumnCount(3);
	m_pwtRts->setColumnWidth(0,120);
	m_pwtRts->setColumnWidth(1,90);
	m_pwtRts->setColumnWidth(2,80);
	
	QStringList slHeader;
	slHeader << tr("Name");
	slHeader << tr("Distance");
	slHeader << tr("# pts");
	m_pwtRts->setHorizontalHeaderLabels(slHeader);

	m_pwtRts->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pwtRts->verticalHeader()->setMaximumWidth(35);
	m_pwtRts->horizontalHeader()->setStretchLastSection(true);
	m_pwtRts->setWordWrap(false);
//	m_pwtRts->setDragDropMode(QAbstractItemView::NoDragDrop);
	m_pwtRts->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pwtRts->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	m_pwtRts->setTextElideMode(Qt::ElideNone);

	const int n = pRC->GetCount();
	m_pwtRts->setRowCount(n);

	QTableWidgetItem* pItem;
	for(int i=0; i<n; i++) {
		const route::Route* pR = pRC->GetRoute(i);
		QString qsName = pR->GetName();
		pItem = new QTableWidgetItem(qsName);
		m_pwtRts->setItem(i,0, pItem);

		unit::Value uv(pFUK->iKey_m, pR->GetTotalDistance());
		uv.ConvertTo(pS->GetDistanceUnit());

		pItem = new QTableWidgetItem(uv.Format(0));
		m_pwtRts->setItem(i,1, pItem);

		pItem = new QTableWidgetItem(QString::number(pR->GetWaypoints().GetCount()));
		m_pwtRts->setItem(i,2, pItem);
	}
}

// -----------------------------------------------------------------------

void DialogRouteSelect::OnNesisButton(int iBtn)
{
	switch(iBtn) {
		// Activate
		case AbstractNesisInput::pbBtn4 :
			OnNesisOk();
			break;

		// Ignore anything else.
		default:
			break;
	}
}

// -----------------------------------------------------------------------

void DialogRouteSelect::OnNesisOk()
{
	int iRow = m_pwtRts->selectionModel()->currentIndex().row();
	m_pRoute = route::RouteContainerSingleton::GetInstance()->GetRoute(iRow);
	accept();
}

// -----------------------------------------------------------------------
