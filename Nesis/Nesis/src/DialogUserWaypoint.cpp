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
#include "Settings.h"
#include "FastUnitKeys.h"
#include "NesisComboBox.h"
#include "NesisDoubleSpinBox.h"
#include "NesisEditBox2.h"
#include "WidgetMessage.h"
#include "NesisUnitCAN.h"
#include "GraphicsSingleton.h"
#include "Settings.h"
#include "Route/WaypointContainerSingleton.h"
#include "CommonDefsQt.h"
#include "WidgetQuickMap.h"

#include "DialogUserWaypoint.h"


// -----------------------------------------------------------------------

DialogUserWaypoint::DialogUserWaypoint(
	const QPointF& ptHome,
	QWidget *pParent
)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	m_ptHome = ptHome*(180/M_PI);

	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QHBoxLayout* pMainLayout = new QHBoxLayout;
	setLayout(pMainLayout);

	// Left layout
	QBoxLayout* pLeftLayout = new QVBoxLayout();
	pMainLayout->addLayout(pLeftLayout);

	// Coordinates entry.
	QFrame* pFrameCoord = new QFrame();
	pFrameCoord->setFrameShape(QFrame::StyledPanel);
	pLeftLayout->addWidget(pFrameCoord);
	InitializeSpinBox(pFrameCoord);

	// Waypoint name.
	QFrame* pFrameName = new QFrame();
	pFrameName->setFrameShape(QFrame::StyledPanel);
	pLeftLayout->addWidget(pFrameName);
	
	// Name editor layout
	QBoxLayout* pNameLayout = new QVBoxLayout();
	pFrameName->setLayout(pNameLayout);

	QLabel* pLbl = new QLabel(tr("Name"));
 	pLbl->setFont(fontSm);
	pNameLayout->addWidget(pLbl);

	// name edit box
	QBoxLayout* pHelpLayout = new QHBoxLayout();
	pNameLayout->addLayout(pHelpLayout);
	
	m_pleName = new NesisEditBox2(QString(""));
	pHelpLayout->addWidget(m_pleName);
	pLbl = new QLabel();
	pHelpLayout->addWidget(pLbl);
	m_pleName->SetBuddy(pLbl);
	m_pleName->setFont(fontSm);
	pLbl->setFont(fontSm);
	pLbl->setFixedWidth(20);
	
	// map image widget
	m_pMap = new WidgetQuickMap();
	pMainLayout->addWidget(m_pMap);

	SetPosition(m_ptHome);

	// List of external commands
	QStringList slExternalCmd;
	slExternalCmd << QString(tr("Zoom +"));
	slExternalCmd << QString(tr("Zoom -"));
	slExternalCmd << QString(tr("Home"));
	slExternalCmd << QString(tr("Save"));
	slExternalCmd << tr("Close");

	m_pleName->SetExternalButtonLabels(slExternalCmd);
	connect(m_pleName, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	connect(m_pleName, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pleName, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_pleName, SIGNAL(NesisEditCancel()), this, SLOT(FocusPrevChild()));
	
	QTimer* pTimer = new QTimer();
	pTimer->setInterval(400);
	pTimer->start();
	connect(pTimer, SIGNAL(timeout()), m_pMap, SLOT(update()));

	Activate();
}

DialogUserWaypoint::~DialogUserWaypoint()
{
}
// -----------------------------------------------------------------------

void DialogUserWaypoint::Activate()
{
	// This will also show buttons.
	m_psbLatDeg->setFocus();
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::OnClose()
{
	accept();
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::OnSave()
{
	// Get the waypoint parameters.
	QPointF pt    = GetPosition();
	QString qsKey = m_pleName->text();
	
	// Create the new waypoint
	using namespace route;
	Waypoint* pW = new Waypoint();
	pW->SetCoordinate(common::Rad(GetPosition()));
	pW->SetKey(m_pleName->text());
	
	if(pW->IsValid()) {
		// Append new point to the end.
		WaypointContainerSingleton::GetInstance()->Insert(-1, pW);
		
		// Load the user waypoint list
		WaypointContainer WC;
		QString qsUserFile = Settings::GetInstance()->GetWaypointsPath()+"User.wpt";
		WC.Load(qsUserFile);
		WC.Insert(-1,pW->Clone());
		WC.Save(qsUserFile);
		
		accept();
	}
	else {
		delete pW;
		m_pleName->setFocus();
	}
}
// -----------------------------------------------------------------------

void DialogUserWaypoint::InitializeInputs()
{
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::InitializeMap()
{
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::OnExternalCmd(int iCmd)
{
	switch (iCmd) {
		case AbstractNesisInput::pbBtn5:
			OnClose();
			break;
		case AbstractNesisInput::pbBtn4:
			OnSave();
			break;
		case AbstractNesisInput::pbBtn3:
			OnHome();
			break;
		case AbstractNesisInput::pbBtn2:
			SetZoom(false);
			break;
		case AbstractNesisInput::pbBtn1:
			SetZoom(true);
			break;
	}
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::InitializeSpinBox(QFrame * pF)
{
	QVector<NesisDoubleSpinBox*> conSpin;
	const QFont& fontSm = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	//create spinboxes
	m_psbLatDeg = new NesisDoubleSpinBox(0);
	m_psbLatDeg->setSuffix(QString(" %1").arg(QChar(0x00b0)));
	m_psbLatDeg->setRange(-90.0f, 90.0f);

	m_psbLatMin = new NesisDoubleSpinBox(0);
	m_psbLatMin->setSuffix(QString(" \'"));
	m_psbLatMin->setRange(-1.0f, 60.0f);
	
	m_psbLatSec = new NesisDoubleSpinBox(0);
	m_psbLatSec->setSuffix(QString(" \""));
	m_psbLatSec->setRange(-1.0f, 60.0f);
	
	m_psbLonDeg = new NesisDoubleSpinBox(0);
	m_psbLonDeg->setSuffix(QString(" %1").arg(QChar(0x00b0)));
	m_psbLonDeg->setRange(-90.0f, 90.0f);
	
	m_psbLonMin = new NesisDoubleSpinBox(0);
	m_psbLonMin->setSuffix(QString(" \'"));
	m_psbLonMin->setRange(-1.0f, 60.0f);

	m_psbLonSec = new NesisDoubleSpinBox(0);
	m_psbLonSec->setSuffix(QString(" \""));
	m_psbLonSec->setRange(-1.0f, 60.0f);

	// add them in container
	conSpin << m_psbLatDeg << m_psbLatMin << m_psbLatSec;
	conSpin << m_psbLonDeg << m_psbLonMin << m_psbLonSec;

	QBoxLayout* pVerticalLayout = new QVBoxLayout();
	pF->setLayout(pVerticalLayout);
	
	QBoxLayout* pHelpLayout = new QHBoxLayout();
	QLabel* pLbl = new QLabel(tr("Latitude"));
	pLbl->setFont(fontSm);
	pVerticalLayout->addWidget(pLbl);
	pVerticalLayout->addLayout(pHelpLayout);
	
	pHelpLayout->addWidget(m_psbLatDeg);
	pHelpLayout->addWidget(m_psbLatMin);
	pHelpLayout->addWidget(m_psbLatSec);
	
	pLbl = new QLabel(tr("Longitude"));
	pLbl->setFont(fontSm);
	pVerticalLayout->addWidget(pLbl);
	pHelpLayout = new QHBoxLayout();
	pVerticalLayout->addLayout(pHelpLayout);
	pHelpLayout->addWidget(m_psbLonDeg);
	pHelpLayout->addWidget(m_psbLonMin);
	pHelpLayout->addWidget(m_psbLonSec);
	
	QStringList slExternalCmd;
	slExternalCmd << QString(tr("Zoom +"));
	slExternalCmd << QString(tr("Zoom -"));
	slExternalCmd << QString(tr("Home"));
	slExternalCmd << QString(tr("Save"));
	slExternalCmd << tr("Close");
	
	foreach(NesisDoubleSpinBox* pSB, conSpin) {
		pSB->setMaximumWidth(75);
		pSB->setFont(fontSm);
 		pSB->SetExternalButtonLabels(slExternalCmd);
  		connect(pSB, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
 		connect(pSB, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
 		connect(pSB, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
//  		connect(pSB, SIGNAL(NesisEditCancel()), this, SLOT(FocusPreviousChild()));
 		connect(pSB, SIGNAL(valueChanged(double)), this, SLOT(OnPositionChanged()));
	}
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::SetZoom(bool bIn)
{
	static int iZoom=2000000;
	if (bIn)
		iZoom /= 1.5;
	else
		iZoom *= 1.5;

	m_pMap->SetZoom(iZoom);
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::SetPosition(QPointF ptPos)
{
	// convert and set
	float f;
	f = ptPos.x();
	m_psbLonDeg->setValue(trunc(f));
	f -= trunc(f);
	f = (f*60);
	m_psbLonMin->setValue(trunc(f));
	f -= trunc(f);
	f = (f*60);
	m_psbLonSec->setValue(round(f));

	f = ptPos.y();
	m_psbLatDeg->setValue(trunc(f));
	f -= trunc(f);
	f = (f*60);
	m_psbLatMin->setValue(trunc(f));
	f -= trunc(f);
	f = (f*60);
	m_psbLatSec->setValue(round(f));
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::OnPositionChanged()
{
	float f;
	// Lat, sec
	if(m_psbLatSec->value()==60.0) {
		m_psbLatSec->setValue(0.0);
		f = m_psbLatMin->value()+1.0;
		m_psbLatMin->setValue(f);
	}
	// Lat, sec
	if(m_psbLatMin->value()==60.0) {
		m_psbLatMin->setValue(0.0);
		f = m_psbLatDeg->value()+1.0;
		m_psbLatDeg->setValue(f);
	}
	
	// Lat, sec
	if(m_psbLatSec->value()==-1.0) {
		m_psbLatSec->setValue(59.0);
		f = m_psbLatMin->value()-1.0;
		m_psbLatMin->setValue(f);
	}
	// Lat, min
	if(m_psbLatMin->value()==-1.0) {
		m_psbLatMin->setValue(59.0);
		f = m_psbLatDeg->value()-1.0;
		m_psbLatDeg->setValue(f);
	}
	
	// Lon, sec
	if(m_psbLonSec->value()==60.0) {
		m_psbLonSec->setValue(0.0);
		f = m_psbLonMin->value()+1.0;
		m_psbLonMin->setValue(f);
	}
	// Lon, sec
	if(m_psbLonMin->value()==60.0) {
		m_psbLonMin->setValue(0.0);
		f = m_psbLonDeg->value()+1.0;
		m_psbLonDeg->setValue(f);
	}
	
	// Lon, sec
	if(m_psbLonSec->value()==-1.0) {
		m_psbLonSec->setValue(59.0);
		f = m_psbLonMin->value()-1.0;
		m_psbLonMin->setValue(f);
	}
	// Lon, min
	if(m_psbLonMin->value()==-1.0) {
		m_psbLonMin->setValue(59.0);
		f = m_psbLonDeg->value()-1.0;
		m_psbLonDeg->setValue(f);
	}

	m_pMap->SetPosition(GetPosition());
}

// -----------------------------------------------------------------------

QPointF DialogUserWaypoint::GetPosition()
{
	QPointF ptP;
	float f;
	
	f  = m_psbLonDeg->value();
	f += m_psbLonMin->value()/60.0;
	f += m_psbLonSec->value()/3600.0;
	ptP.setX(f);

	f  = m_psbLatDeg->value();
	f += m_psbLatMin->value()/60.0;
	f += m_psbLatSec->value()/3600.0;
	ptP.setY(f);

	return ptP;

}

// -----------------------------------------------------------------------

QString DialogUserWaypoint::GetWaypointName()
{
	return m_pleName->text();
}

// -----------------------------------------------------------------------

void DialogUserWaypoint::OnHome()
{
	SetPosition(m_ptHome);
	m_pMap->update();
}

// -----------------------------------------------------------------------

