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

#include <QtGui>
#include "MathEx.h"
#include "GraphicsSingleton.h"
#include "Unit/Value.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "CommonDefsQt.h"
#include "NesisAerospaceDataManager.h"
#include "ActiveNavigation.h"

#include "WidgetHSIMiniMap.h"

// See also AttitudeGaugeRound for the CHAMFER and DT constants
#define CHAMFER 		21
#define DT				11

#define HX				22
#define HY				19

// -----------------------------------------------------------------------

WidgetHSIMiniMap::WidgetHSIMiniMap(
	int iArcRadius,
	const QPointF& ptAirplaneRel,
	AirspaceVertical* pAS,
	QWidget *pParent
)
 : WidgetHSIMap(iArcRadius, ptAirplaneRel, pAS, pParent)
{
	m_bDrawFrame = true;
	m_iFrameThickness = DT;

	// Heading background - redefine
	m_pgHeading.clear();

	m_pgHeading << QPoint(-HX,0);
	m_pgHeading << QPoint(HX,0);
	m_pgHeading << QPoint(HX,HY);
	m_pgHeading << QPoint(5,HY);
	m_pgHeading << QPoint(0,HY+5);
	m_pgHeading << QPoint(-5,HY);
	m_pgHeading << QPoint(-HX,HY);
}

WidgetHSIMiniMap::~WidgetHSIMiniMap()
{
}

// -----------------------------------------------------------------------

void WidgetHSIMiniMap::resizeEvent(QResizeEvent* pEvent)
{
	WidgetHSIMap::resizeEvent(pEvent);

	m_pxFrame = QPixmap(size());
	m_pxFrame.fill(Qt::transparent);
	QPainter P(&m_pxFrame);


	const int iW = width();
	const int iH = height();

	// Corners must be drawn using background
	P.setPen(Qt::NoPen);
	QColor colWin = palette().brush(QPalette::Window).color();
	colWin.setAlpha(255);
	P.setBrush(colWin);
//	P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBackground));
//	P.setBrush(Qt::white);
	QPolygon pg;

	pg << QPoint(0,0);
	pg << QPoint(CHAMFER+1,0);
	pg << QPoint(0, CHAMFER+1);
	P.drawConvexPolygon(pg);

	pg[0] = QPoint(iW,0);
	pg[1] = QPoint(iW, CHAMFER+1);
	pg[2] = QPoint(iW-CHAMFER-1,0);
	P.drawConvexPolygon(pg);

	pg[0] = QPoint(iW,iH);
	pg[1] = QPoint(iW,iH-CHAMFER-1);
	pg[2] = QPoint(iW-CHAMFER-1,iH);
	P.drawConvexPolygon(pg);

	pg[0] = QPoint(0,iH);
	pg[1] = QPoint(0,iH-CHAMFER-1);
	pg[2] = QPoint(CHAMFER+1,iH);
	P.drawConvexPolygon(pg);

	// The outer frame
	QPainterPath path;
	QRect rc(0,0, iW+1, iH+1);
	path.addPolygon(common::GetChamferPolygon(rc, CHAMFER));
	path.closeSubpath();

	// Inner frame
	rc.adjust(DT,DT, -DT,-DT);
	path.addPolygon(common::GetChamferPolygon(rc, CHAMFER - 586*DT/1000));
	path.closeSubpath();

	// Draw the thick chamfered frame
//	P.setPen(Qt::NoPen);
	P.setPen(QPen(QColor(qRgb(128,128,128)),1.3));
// 	P.setBrush(QColor(qRgb(126,126,126)));
	P.setBrush(QColor(qRgb(26,26,26)));
	P.drawPath(path);
}

// -----------------------------------------------------------------------

void WidgetHSIMiniMap::paintEvent(QPaintEvent* pPE)
{
	WidgetHSIMap::paintEvent(pPE);
	// Pointers to singleton objects
	QPainter P(this);

	if(m_bDrawFrame) {
		P.drawPixmap(0,0, m_pxFrame);
	}
}

// -----------------------------------------------------------------------

int WidgetHSIMiniMap::DrawTopBar(QPainter& P)
{
	using namespace common;
	using namespace unit;

	const int iW = width();
	const int iH = 32;

	P.resetMatrix();
	P.setFont(m_pGS->GetFont(GraphicsSingleton::fSmall));

	QRect rcW(0,0, iW,iH);
	P.setPen(Qt::white);
	P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBannerBack));
	P.setRenderHint(QPainter::Antialiasing, false);
	P.drawRect(rcW);

	// Bearing, Distance, GS
	QString qsBear("-");
	QString qsDist("-");
	QString qsGS("-");

	const route::Waypoint* pW = m_pAN->GetWaypoint();
	if(pW) {
		PairFF bd = pW->GetBearingDistance(m_pND->GetPosition());
		Value uv(m_pFUK->iKey_m, bd.second);
		uv.ConvertTo(m_pS->GetDistanceUnit());
		qsDist = uv.Format(0);
		qsBear = QString::number(Deg(bd.first), 'f', 0);
	}
	float fGS = m_pND->GetGroundSpeed();
	if(fGS > 0.0f) {
/*			float fSec = bd.second / fGS;
			QTime time = QTime::currentTime().addSecs(fSec);
			qsETA = time.toString("H:mm");*/
		Value uv(m_pFUK->iKey_m_s, fGS);
		uv.ConvertTo(m_pS->GetAirspeedUnit());
		qsGS = uv.Format(0);
	}

	P.drawText(rcW, Qt::AlignBottom | Qt::AlignHCenter,
		trUtf8("B %1° D %2 GS %3").arg(qsBear, qsDist, qsGS)
	);
	return iH+4;
}

// -----------------------------------------------------------------------

int WidgetHSIMiniMap::DrawBottomBar(QPainter& P)
{
	using namespace common;
	using namespace unit;

	const int iW = width();
	const int iH = 31;

	P.resetMatrix();
	P.setFont(m_pGS->GetFont(GraphicsSingleton::fSmall));

	QRect rcW(0,height()-iH, iW,iH);
	P.setPen(Qt::white);
	P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBannerBack));
	P.setRenderHint(QPainter::Antialiasing, false);
	P.drawRect(rcW);

	// Wind, Waypoint
	Value uv(m_pFUK->iKey_m_s, m_pND->GetWindSpeed());
	uv.ConvertTo(m_pS->GetWindspeedUnit());
	QString qsWpt("-");

	const route::Waypoint* pW = m_pAN->GetWaypoint();
	if(pW)
		qsWpt = pW->GetDescription();

	rcW.moveTop(rcW.top()+2);
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, trUtf8("W %1° %2 WPT %3").arg(
		QString::number(qRound(Deg(m_pND->GetWindDirection()))),
		uv.Format(0),
		qsWpt
	));

	return height()-iH;
}

// -----------------------------------------------------------------------
