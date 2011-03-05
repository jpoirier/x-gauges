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
#include <QTimer>

#include "PanelMiscellaneous.h"
#include "NesisTableView.h"
#include "GraphicsSingleton.h"
#include "Unit/Value.h"
#include "Unit/Manager.h"
#include "FastUnitKeys.h"
#include "Settings.h"
#include "DialogYesNo.h"

#include "DialogLogbook.h"

#define MAX_LOGBOOK_ITEMS	50
#define TIME_FORMAT			"hh:mm"

using namespace logbook;

// -----------------------------------------------------------------------

DialogLogbook::DialogLogbook(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	setGeometry(0,0, 600,400);
	m_bMoveToTop = true;

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	setLayout(pLayout);
	QLabel* pLbl = new QLabel(tr("Logbook entries"));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);

	// External labels for the table widget.
	QStringList slExt;
	slExt << QString("Clear");
	slExt << QString();
	slExt << QString();
	slExt << QString();
	slExt << tr("Close");

	// Table
	m_ptwEntry = new NesisTableWidget();
	m_ptwEntry->setFont(fontSm);
	m_ptwEntry->SetExternalButtonLabels(slExt);
	pLayout->addWidget(m_ptwEntry);
	InitializeTable();

	// Selection details. - details are put into a grid
	pLbl = new QLabel(tr("Details"));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);
	QGridLayout* pGrid = new QGridLayout;
	pLayout->addLayout(pGrid);

	m_pleEngineOnOff 		= new QLineEdit;
	m_pleEngineCycleTime = new QLineEdit;
	m_pleDistance 			= new QLineEdit;
	m_pleSpeed 				= new QLineEdit;
	m_pleFuel 				= new QLineEdit;
	m_pleAltitude 			= new QLineEdit;

	QVector<QLineEdit*> conLE;
	conLE << m_pleEngineOnOff;
	conLE << m_pleEngineCycleTime;
	conLE << m_pleDistance;
	conLE << m_pleSpeed;
	conLE << m_pleFuel;
	conLE << m_pleAltitude;

	foreach(QLineEdit* pLE, conLE) {
		pLE->setAlignment(Qt::AlignCenter);
		pLE->setReadOnly(true);
		pLE->setFocusPolicy(Qt::NoFocus);
		pLE->setFont(fontSm);
	}

	pLbl = new QLabel(tr("Engine on/off:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl, 						0,0, Qt::AlignRight);
	pGrid->addWidget(m_pleEngineOnOff,  	0,1);

	pLbl = new QLabel(tr("Cycles/Time:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl,						0,2, Qt::AlignRight);
	pGrid->addWidget(m_pleEngineCycleTime, 0,3);

	pLbl = new QLabel(tr("Distance:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl,						1,0, Qt::AlignRight);
	pGrid->addWidget(m_pleDistance,        1,1);

	pLbl = new QLabel(tr("Speed avg/max:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl,						1,2, Qt::AlignRight);
	pGrid->addWidget(m_pleSpeed,				1,3);

	pLbl = new QLabel(tr("Fuel tot/spec:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl,						2,0, Qt::AlignRight);
	pGrid->addWidget(m_pleFuel,            2,1);

	pLbl = new QLabel(tr("Alt max:"));
	pLbl->setFont(fontSm);
	pGrid->addWidget(pLbl,						2,2, Qt::AlignRight);
	pGrid->addWidget(m_pleAltitude,			2,3);

	connect(m_ptwEntry, SIGNAL(currentCellChanged(int,int,int,int)),
			  this,       SLOT(OnTableRowChanged(int,int,int,int)));

	connect(m_ptwEntry, SIGNAL(NesisButton(int)), this, SLOT(OnNesisButton(int)));

	Activate();

	QTimer* ptimRefresh = new QTimer(this);
	connect(ptimRefresh, SIGNAL(timeout()), this, SLOT(OnUpdateTable()));
	ptimRefresh->start(2000);
}

DialogLogbook::~DialogLogbook()
{
}
// -----------------------------------------------------------------------

void DialogLogbook::Activate()
{
	// This will also show buttons.
	OnUpdateTable();
	m_ptwEntry->setFocus();
	m_ptwEntry->setCurrentCell(0,0);
	m_ptwEntry->SetEditMode();
}

// -----------------------------------------------------------------------

void DialogLogbook::OnClose()
{
	setEnabled(false);
	accept();
}

// -----------------------------------------------------------------------

void DialogLogbook::InitializeTable()
{
	m_ptwEntry->setAlternatingRowColors(true);
	m_ptwEntry->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ptwEntry->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_ptwEntry->setColumnCount(5);

	m_ptwEntry->setColumnWidth(0,  95);
	m_ptwEntry->setColumnWidth(1,  82);
	m_ptwEntry->setColumnWidth(2,  82);
	m_ptwEntry->setColumnWidth(3,  82);

	QStringList slHeader;
	slHeader << tr("Date");
	slHeader << tr("Takeoff");
	slHeader << tr("Land.");
	slHeader << tr("Time");
	slHeader << tr("Pilot");
	m_ptwEntry->setHorizontalHeaderLabels(slHeader);

	m_ptwEntry->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	m_ptwEntry->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_ptwEntry->verticalHeader()->setMaximumWidth(35);
//	m_ptwEntry->verticalHeader()->setStretchLastSection(true);
	m_ptwEntry->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignTop);
	m_ptwEntry->horizontalHeader()->setMaximumHeight(35);
	m_ptwEntry->horizontalHeader()->setStretchLastSection(true);
}

// -----------------------------------------------------------------------

void DialogLogbook::OnUpdateTable()
{
	// We need a copy of the current logbook container.
	Logbook* pBook = Logbook::GetInstance();
	m_conBook = pBook->GetReverseContainer(MAX_LOGBOOK_ITEMS, true);

	// remember current cell
	int iRow = m_ptwEntry->currentRow();

	m_ptwEntry->clearContents();

	const int n = qMin(MAX_LOGBOOK_ITEMS, m_conBook.count());
	m_ptwEntry->setRowCount(n);
	QTableWidgetItem* pItem;
	QString qs;

	for(int i=0; i<n; i++) {
		const Entry& e = m_conBook[i];

		qs = (e.GetDate().isValid() ? e.GetDate() : QDate::currentDate()).toString("dd.MM.yy");
		pItem = new QTableWidgetItem(qs);
		m_ptwEntry->setItem(i,0, pItem);

		qs = e.GetFirstTakeoff().isValid() ?
			e.GetFirstTakeoff().time().toString(TIME_FORMAT) :
			QString("N/A");
		pItem = new QTableWidgetItem(qs);
		m_ptwEntry->setItem(i,1, pItem);

		qs = e.GetLastLanding().isValid() ?
			e.GetLastLanding().time().toString(TIME_FORMAT) :
			QString("N/A");
		pItem = new QTableWidgetItem(qs);
		m_ptwEntry->setItem(i,2, pItem);

		const int iSecs = e.GetFlightTimeSecs()+30;
		qs.sprintf("%02i:%02i", iSecs/3600, (iSecs%3600)/60);
		m_ptwEntry->setItem(i,3, new QTableWidgetItem(qs));

		m_ptwEntry->setItem(i,4, new QTableWidgetItem(e.GetPilot()));

		for(int j=0; j<5; j++)
			m_ptwEntry->item(i,j)->setTextAlignment(Qt::AlignCenter);
	}

	if (iRow <= m_ptwEntry->rowCount())
		m_ptwEntry->setCurrentCell(iRow,0);

}
// -----------------------------------------------------------------------

void DialogLogbook::OnTableRowChanged(int icr, int /*icc*/, int ipr, int /*ipc*/)
{
// 	qDebug() << "Previous row = " << ipr << "Current row = " << icr;
	if(ipr == icr || icr < 0)
		return;
	ASSERT(m_conBook.count() > icr);

	const Entry& e = m_conBook[icr];

	// Engine on/off
	QString qs;
	QDateTime on  = e.GetFirstEngineOn();
	QDateTime off = e.GetLastEngineOff();

	m_pleEngineOnOff->setText(
		QString("%1, %2").arg(
			on.isValid() ? on.time().toString(TIME_FORMAT) : tr("N/A"),
			off.isValid() ? off.time().toString(TIME_FORMAT) : tr("N/A")
		)
	);

	// Engine cycles and time
	int iSecs  = e.GetEngineTimeSecs()+30;
	qs.sprintf("%i, %02i:%02i", e.GetEngineCycles(), iSecs/3600, (iSecs%3600)/60);
	m_pleEngineCycleTime->setText(qs);

	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	Settings*     pS   = Settings::GetInstance();

	// Distance
	unit::Value uv(pFUK->iKey_m, e.GetDistance());
	uv.ConvertTo(pS->GetDistanceUnit());
	m_pleDistance->setText(uv.Format(0));

	// Speed
	uv.Set(pFUK->iKey_m_s, e.GetAverageSpeed());
	uv.ConvertTo(pS->GetAirspeedUnit());
	qs = QString::number(uv.GetValue(), 'f', 0);
	uv.Set(pFUK->iKey_m_s, e.GetMaximalSpeed());
	uv.ConvertTo(pS->GetAirspeedUnit());
	qs += ", " + uv.Format(0);
	m_pleSpeed->setText(qs);

	// Fuel
	uv.Set(pFUK->iKey_l, e.GetFuelConsumption());
	uv.ConvertTo(pS->GetFuelVolumeUnit());
	qs = uv.Format(0);
	uv.Set(pFUK->iKey_l_h, e.GetSpecificFuelConsumption());
	uv.ConvertTo(pS->GetFuelFlowUnit());
	qs += ", " + uv.Format(1);
	m_pleFuel->setText(qs);

	// Altitude
	uv.Set(pFUK->iKey_m, e.GetMaximalAltitude());
	uv.ConvertTo(pS->GetAltitudeUnit());
	qs = uv.Format(0);
	m_pleAltitude->setText(qs);
}

// -----------------------------------------------------------------------

void DialogLogbook::OnNesisButton(int iBtn)
{
	if(iBtn==AbstractNesisInput::pbBtn5) // 4 == pbBtn5
		OnClose();
	else if(iBtn==AbstractNesisInput::pbBtn1)
		OnClear();
}

// -----------------------------------------------------------------------

void DialogLogbook::OnClear()
{
	DialogYesNo dlg(
		tr("Are you sure to delete the complete logbook?")
	);
	if(dlg.exec()==QDialog::Accepted && dlg.IsYes()) {
		if(Logbook::GetInstance()->DeleteAll())
			qDebug() << "Clearing the logbook";
		OnClose();
	}
}

// -----------------------------------------------------------------------
