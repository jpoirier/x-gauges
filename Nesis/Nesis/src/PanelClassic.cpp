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
#include <QPainter>

#include "XMLGaugeRound.h"
#include "XMLGaugeRoundAltitude.h"
#include "AttitudeGaugeRound.h"
#include "EngineMiniGaugeRect.h"
#include "FuelCompGaugeRect.h"
#include "ParseHelper.h"
#include "XMLTags.h"

#include "GraphicsSingleton.h"
#include "NesisAerospaceDataManager.h"
#include "WidgetMain.h"
#include "Settings.h"
#include "FastUnitKeys.h"

#include "PanelClassic.h"


// -----------------------------------------------------------------------

PanelClassic::PanelClassic(
	const QDomDocument& doc, //!< The xml description document
	QSize sz, 					 //!< Size of the widget
	QWidget *pParent			 //!< Parent.
)
 : AbstractPanel(sz, pParent)
{
	Settings* pS              = Settings::GetInstance();
	GraphicsSingleton*    pG  = GraphicsSingleton::GetInstance();

	SetBackgroundColor(pG->GetBrush(GraphicsSingleton::bBackground));
	setFixedSize(sz);

	const int iW = width();
	const int iH = height();
	const int iSize = int(0.30*iW);

	using namespace instrument;
	
	// Get the classic layout
	QDomElement elLayout = doc.documentElement().firstChildElement(TAG_LAYOUT);
	if(elLayout.isNull())
		qFatal("%s tag not found", TAG_LAYOUT);
	QDomElement el = elLayout.firstChildElement(TAG_CLASSIC);
	if(el.isNull())
		qFatal("%s tag not found", TAG_CLASSIC);
	
	m_pwTopLeft			= NULL;
	m_pwTopRight		= NULL;
	m_pwBottomLeft		= NULL;
	m_pwBottomRight	= NULL;
	
	QDomElement elg = el.firstChildElement(TAG_GAUGE);
 	for (; !elg.isNull(); elg = elg.nextSiblingElement(TAG_GAUGE)) {
 		XMLGaugeRound* pGauge = CreateXMLGauge(elg, this);
 		if(pGauge == NULL) {
 			qFatal("Unable to create a valid XMLGaugeRound object from %s element",
 				elg.tagName().toAscii().constData()
 			);
 			exit(3);
 		}
		
		// Parse position
		QString qsP = ParseAttribute(elg, TAG_POS);
		XMLGaugeRound*& pXMLGauge = GetWidget(qsP);
		
		if(pXMLGauge == NULL) {
			pXMLGauge = pGauge;
			pXMLGauge->SetBackgroundFile(pS->GetGaugeRoundBackgroundFile());
		}
		else {
			qFatal("%s widget already exists.", qsP.toAscii().constData());
		}
	}
	
	if(m_pwTopLeft == NULL)
		qFatal("Top-left widget is not defined.");
	if(m_pwTopRight == NULL)
		qFatal("Top-right widget is not defined.");
	if(m_pwBottomLeft == NULL)
		qFatal("Bottom-left widget is not defined.");
	if(m_pwBottomRight == NULL)
		qFatal("Bottom-right widget is not defined.");

	m_pwTopLeft->setGeometry(0,0, iSize, iSize);
	m_pwTopRight->setGeometry(iW-iSize,0, iSize, iSize);
	m_pwBottomLeft->setGeometry(0, iSize, iSize, iSize);
	m_pwBottomRight->setGeometry(iW-iSize, iSize, iSize, iSize);

	// Attitude
 	m_pwAtt = new AttitudeGaugeRound(this);
	m_pwAtt->SetBackgroundFile(pS->GetGaugeRoundBackgroundFile());
 	m_pwAtt->setGeometry(iSize+2, 1, iW-2*iSize-4, iSize-3);

	// HSI (in a form of a map)
	m_pwMap = new WidgetHSIMiniMap(160, QPointF(0.5, 0.81), NULL, this);
//	m_pwMap->EnableFrame();
	m_pwMap->setGeometry(iSize+2,iSize+1, iW-2*iSize-4, iH-iSize-4);

	// Engine panel
	elg = el.firstChildElement(TAG_ENGINE_MINI);
	m_pwEngine = new instrument::EngineMiniGaugeRect(
		elg,
		this
	);
	m_pwEngine->setGeometry(0,2*iSize, iSize, iH-2*iSize-1);

	// Fuel computer panel
	m_pwFuel = new instrument::FuelCompGaugeRect(this);
	m_pwFuel->setGeometry(iW-iSize,2*iSize, iSize, iH-2*iSize-1);
}

// -----------------------------------------------------------------------

PanelClassic::~PanelClassic()
{
}

// -----------------------------------------------------------------------

void PanelClassic::SetZoom(int iZoomFactor)
{
	((WidgetMap*)m_pwMap)->SetZoom(iZoomFactor);
}

// -----------------------------------------------------------------------

bool PanelClassic::ChangeZoom(bool bIn)
{
	return ((WidgetMap*)m_pwMap)->ChangeZoom(bIn);
}

// -----------------------------------------------------------------------

void PanelClassic::Draw(bool bMajor)
{
	NesisAerospaceDataManager* pN = NesisAerospaceDataManager::GetInstance();

	// Always draw the attitude widget first
	m_pwAtt->SetAttitude(pN->GetRoll(), pN->GetPitch());
	m_pwAtt->SetBallSlip(pN->GetBallSlip());
	m_pwAtt->SetHeadingRate(pN->GetYawRate());
 	m_pwAtt->update();

	// Should we update the map?
	if(bMajor) {
		m_pwMap->update();
	}
	else {
		static int iCount = 0;
		iCount++;
		// frequent update
		{
			m_pwTopLeft->update();		// velocity
			m_pwBottomLeft->update();	// RPM + manifold
			m_pwBottomRight->update();	// vario
 		}
		// not so frequent update
 		if(iCount%4==1) {
			m_pwTopRight->update();	// altimeter
			m_pwEngine->update();	// engine parameters
			m_pwFuel->update();		// fuel computers
		}
	}
}

// -----------------------------------------------------------------------

QStringList PanelClassic::GetMenuLabels() const
{
	QStringList sl;
	sl << tr("QNH");
	sl << QString(); //tr("Altitude");
	sl << tr("Airfield");
	sl << tr("Waypoint");
	sl << tr("Route");
	return sl;
}

// -----------------------------------------------------------------------

void PanelClassic::OnMenuButtonPressed(PanelButton ePB)
{
	switch(ePB) {
		case pbBtn1:
			GetWidgetMain()->OnSetQNH();
			break;
		case pbBtn2:
			//GetWidgetMain()->OnSetAltitude();
			break;
		case pbBtn3:
			//GetWidgetMain()->OnSetHeading();
			GetWidgetMain()->OnSelectNearestAirfield();
			break;
		case pbBtn4:
			GetWidgetMain()->OnSelectWaypoint();
			break;
		case pbBtn5:
			GetWidgetMain()->OnSelectRoute();
			break;
		default:
			break;
	}
	SetDirectMode();
}

// -----------------------------------------------------------------------

instrument::XMLGaugeRound*& PanelClassic::GetWidget(QString& qsP)
{
	if(qsP == QLatin1String(TAG_TOP_LEFT))
		return m_pwTopLeft;
	else if(qsP == QLatin1String(TAG_TOP_RIGHT))
		return m_pwTopRight;
	else if(qsP == QLatin1String(TAG_BOTTOM_LEFT))
		return m_pwBottomLeft;
	else if(qsP == QLatin1String(TAG_BOTTOM_RIGHT))
		return m_pwBottomRight;
	else {
		qCritical() << "Invalid position for classic panel:" << qsP;
		return m_pwTopLeft;
	}
}

// -----------------------------------------------------------------------
