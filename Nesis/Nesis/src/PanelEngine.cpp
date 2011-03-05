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
#include "ParseHelper.h"
#include "XMLTags.h"

#include "GraphicsSingleton.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "Unit/Manager.h"
#include "Unit/Value.h"
#include "NesisAerospaceDataManager.h"
#include "PixmapBattery.h"
#include "WidgetMain.h"

#include "PanelEngine.h"
// -----------------------------------------------------------------------

WidgetBottom::WidgetBottom(QWidget *pParent)
: QWidget(pParent)
{
	m_pPixBat   = PixmapBattery::GetInstance();
	m_iMode     = 0;
	m_iTemp     = 0;
	m_iCapacity = 0;
	m_fVoltage  = 0;
	m_iOAT		= 0;
}

WidgetBottom::~WidgetBottom()
{
}
// -----------------------------------------------------------------------

void WidgetBottom::SetBattery(
	int iMode, 
	float fVoltage, 
	int iTemp, 
	int iCapacity
)
{
	if(iMode==0) 
		iMode = 3;
		
	m_iMode     = iMode;
	m_fVoltage  = fVoltage;
	m_iTemp     = iTemp;
	m_iCapacity = iCapacity;
}

// -----------------------------------------------------------------------

void WidgetBottom::SetOAT(int iOAT)
{
	m_iOAT = iOAT;
}

// -----------------------------------------------------------------------
	
void WidgetBottom::paintEvent(QPaintEvent* /*pEvent*/)
{
	QPainter P(this);
	GraphicsSingleton*    pG  = GraphicsSingleton::GetInstance();
	FastUnitKeys*        pFUK = FastUnitKeys::GetInstance();
	Settings*              pS = Settings::GetInstance();
	
	const int iH = height();
	int iW = 90;
	
	m_pPixBat->Draw(
		P, 
		(iW-m_pPixBat->GetWidth())/2,
		5, 
		(PixmapBattery::Mode)m_iMode, 
		m_iCapacity
	);
	
	P.setFont(pG->GetFont(GraphicsSingleton::fSmall));
	P.setPen(Qt::white);
	
	unit::Value uv(pFUK->iKey_C, m_iTemp);
	uv.ConvertTo(pS->GetTemperatureUnit());
	
	P.drawText(
		0, iH/2, 
		iW,iH/2, 
		Qt::AlignCenter,
		trUtf8("%1 V %2").arg(m_fVoltage).arg(uv.Format(0))
	);
	
	iW = 80;
	uv.Set(pFUK->iKey_C, m_iOAT);
	uv.ConvertTo(pS->GetTemperatureUnit());
	
	P.drawText(width()-iW, 0, iW, height(), Qt::AlignCenter,
		trUtf8("OAT %1").arg(uv.Format(0))
	);
	
//	P.drawLine(iW-1,0, iW-1,iH);
}

	
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

PanelEngine::PanelEngine(const QDomDocument& doc, QSize sz, QWidget *pParent)
 : AbstractPanel(sz, pParent)
{
 	Settings* pS              = Settings::GetInstance();
	GraphicsSingleton*    pG  = GraphicsSingleton::GetInstance();
	SetBackgroundColor(pG->GetBrush(GraphicsSingleton::bBackground));

	using namespace instrument;

	setFixedSize(sz);

	const int iW = width();
	const int iH = height();
	const int iSize = iW/3;

	// Get the classic layout
	QDomElement elLayout = doc.documentElement().firstChildElement(TAG_LAYOUT);
	if(elLayout.isNull())
		qCritical() << TAG_LAYOUT << "tag not found.";
	QDomElement el = elLayout.firstChildElement(TAG_ENGINE);
	if(el.isNull())
		qCritical() << TAG_ENGINE << "tag not found.";
	
	m_pwTopLeft			= NULL;
	m_pwTopMiddle		= NULL;
	m_pwTopRight		= NULL;
	m_pwBottomLeft		= NULL;
	m_pwBottomMiddle	= NULL;
	m_pwBottomRight	= NULL;
	
	QDomElement elg = el.firstChildElement(TAG_GAUGE);
 	for (; !elg.isNull(); elg = elg.nextSiblingElement(TAG_GAUGE)) {
 		XMLGaugeRound* pGauge = CreateXMLGauge(elg, this);
 		if(pGauge == NULL) {
 			qCritical() << "Unable to create a valid XMLGaugeRound object from"
 							<< elg.tagName() << "element.";
 		}
		
		// Parse position
		QString qsP = ParseAttribute(elg, TAG_POS);
		XMLGaugeRound*& pXMLGauge = GetWidget(qsP);
		
		if(pXMLGauge == NULL) {
			pXMLGauge = pGauge;
			pXMLGauge->SetBackgroundFile(pS->GetGaugeRoundBackgroundFile());
		}
		else {
			qCritical() << qsP << "widget already exists.";
		}
	}
	
	if(m_pwTopLeft == NULL)
		qCritical() << "Top-left widget is not defined.";
	if(m_pwTopMiddle == NULL)
		qCritical() << "Top-middle widget is not defined.";
	if(m_pwTopRight == NULL)
		qCritical() << "Top-right widget is not defined.";
	if(m_pwBottomLeft == NULL)
		qCritical() << "Bottom-left widget is not defined.";
	if(m_pwBottomMiddle == NULL)
		qCritical() << "Bottom-middle widget is not defined.";
	if(m_pwBottomRight == NULL)
		qCritical() << "Bottom-right widget is not defined.";

	m_pwTopLeft->setGeometry( 0,       0, iSize, iSize);
	m_pwTopMiddle->setGeometry(iSize,  0, iSize, iSize);
	m_pwTopRight->setGeometry(iW-iSize,0, iSize, iSize);
	
	m_pwBottomLeft->setGeometry(0, iSize, iSize, iSize);
	m_pwBottomMiddle->setGeometry(iSize,  iSize, iSize, iSize);
	m_pwBottomRight->setGeometry(iW-iSize,iSize, iSize, iSize);
	
	m_pwBottom = new WidgetBottom(this);
	m_pwBottom->setGeometry(0, 2*iSize, iW, iH-2*iSize);
}

PanelEngine::~PanelEngine()
{
	qDebug() << "PanelDAQ Delete";
}

//-----------------------------------------------------------------------------

void PanelEngine::Draw(bool bMajor)
{
	using namespace unit;
	static unsigned int uiCounter = 0;
	NesisAerospaceDataManager* pN = NesisAerospaceDataManager::GetInstance();

	uiCounter++;
	unsigned int uiMod = uiCounter%3;

	// Oil, water temperature; current and voltage
	if(uiMod==0) {
		m_pwBottomMiddle->update();
		m_pwTopRight->update();
	}
	else if(uiMod==1) {
		m_pwTopLeft->update();
		m_pwTopMiddle->update();
	}
	else {
		m_pwBottomLeft->update();
		m_pwBottomRight->update();
	}
	
	// Draw battery
	if(bMajor) {
		m_pwBottom->SetBattery(
			pN->GetBackupBatteryMode(),
			pN->GetBackupBatteryCellVoltage(),
			pN->GetBackupBatteryCellTemperature(),
			pN->GetBackupBatteryCapacity()
		);
		m_pwBottom->SetOAT(pN->GetOutsideAirTemperature());
		
		m_pwBottom->update();
	}
}

//-----------------------------------------------------------------------------

QStringList PanelEngine::GetMenuLabels() const
{
	QStringList sl;
	sl << tr("QNH");
	sl << tr("Fuel Level");
	sl << "";
	sl << "";
	sl << "";
	return sl;
}

//-----------------------------------------------------------------------------

void PanelEngine::OnMenuButtonPressed(PanelButton ePB)
{
	switch(ePB) {
		case pbBtn1:
			GetWidgetMain()->OnSetQNH();
			break;
		case pbBtn2:
			GetWidgetMain()->OnSetFuelLevel();
			break;
		case pbBtn3:
		case pbBtn4:
		case pbBtn5:
		default:
			break;
	}
	SetDirectMode();
}

//-----------------------------------------------------------------------------

instrument::XMLGaugeRound*& PanelEngine::GetWidget(QString& qsP)
{
	if(qsP == QLatin1String(TAG_TOP_LEFT))
		return m_pwTopLeft;
	else if(qsP == QLatin1String(TAG_TOP_MIDDLE))
		return m_pwTopMiddle;
	else if(qsP == QLatin1String(TAG_TOP_RIGHT))
		return m_pwTopRight;
	else if(qsP == QLatin1String(TAG_BOTTOM_LEFT))
		return m_pwBottomLeft;
	else if(qsP == QLatin1String(TAG_BOTTOM_MIDDLE))
		return m_pwBottomMiddle;
	else if(qsP == QLatin1String(TAG_BOTTOM_RIGHT))
		return m_pwBottomRight;
	else {
		qCritical() << "Invalid position for engine panel:" << qsP;
		return m_pwTopLeft;
	}
}

// -----------------------------------------------------------------------
