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
#include <QPaintEvent>
#include <QTime>
#include "MathEx.h"
#include "Unit/Value.h"
#include "CommonDefsQt.h"
#include "ScaleRound.h" // instruments

#include "NesisAerospaceDataManager.h"
#include "GraphicsSingleton.h"
#include "ActiveNavigation.h"
#include "Settings.h"
#include "PixmapBattery.h"
#include "FastUnitKeys.h"

#include "WidgetHSIMap.h"

#define AIRSPACE_WIDTH_100			14
#define WX								10

// #define HX								33
// #define HY								31
#define HX				22
#define HY				19

// -----------------------------------------------------------------------

WidgetHSIMap::WidgetHSIMap(int iArcRadius, const QPointF& ptAirplaneRel, AirspaceVertical* pAS, QWidget *parent)
 : WidgetMap(ptAirplaneRel, parent)
{
	m_iArcRadius = iArcRadius;
	m_pAS        = pAS;

	// Pointers to major singleton objects
	m_pS		= Settings::GetInstance();
	m_pFUK 	= FastUnitKeys::GetInstance();
	m_pND		= NesisAerospaceDataManager::GetInstance();
	m_pGS		= GraphicsSingleton::GetInstance();
	m_pAN		= ActiveNavigation::GetInstance();
	m_pPixBat = PixmapBattery::GetInstance();

	// Some pixmaps
	m_pixHeadingTriangle.load(m_pS->GetImagePath()+"HeadingTriangle.png");
// 	m_pixHeadingBug.load(m_pS->GetImagePath()+"HeadingBug.png");
// 	m_pixHeadingBug = m_pixHeadingBug.scaled(25,14, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	// Wind polygon
	m_pgWind << QPoint(   WX,    0);
	m_pgWind << QPoint(    0,   WX);
	m_pgWind << QPoint(    0,   4*WX/10);
	m_pgWind << QPoint(-3*WX/2, 4*WX/10);
	m_pgWind << QPoint(-3*WX/2,-4*WX/10);
	m_pgWind << QPoint(    0,  -4*WX/10);
	m_pgWind << QPoint(    0,  -WX);
	m_pgWind.translate(-WX/4, 0);

	// Frame thickenss
	m_iFrameThickness = 0;

	// Heading background
	m_pgHeading << QPoint(-HX,0);
	m_pgHeading << QPoint(HX,0);
	m_pgHeading << QPoint(HX,HY);
	m_pgHeading << QPoint(5,HY);
	m_pgHeading << QPoint(0,HY+5);
	m_pgHeading << QPoint(-5,HY);
	m_pgHeading << QPoint(-HX,HY);
}

WidgetHSIMap::~WidgetHSIMap()
{
}

// -----------------------------------------------------------------------

void WidgetHSIMap::resizeEvent(QResizeEvent* pEvent)
{
	WidgetMap::resizeEvent(pEvent);
}

// -----------------------------------------------------------------------

void WidgetHSIMap::paintEvent(QPaintEvent* /*pPE*/)
{
	// The Painter
	QPainter P(this);

	// Current heading
	const int iMT = m_pS->GetHSIMapType();
	m_fMapRotation = 
		(iMT == HSI_HEADING_UP) ?	m_pND->GetYaw() : 
		(iMT == HSI_TRACKING_UP) ? m_pND->GetTracking() : 0.0;
//	const float fHeadingDeg = common::Deg(fHeadingRad);
	
	// Get current waypoint (may be null)
	const route::Waypoint* pWaypoint = m_pAN->GetWaypoint();
	// Get current route (may be null)
	const route::Route* pRoute = m_pAN->GetRoute();

	// We need to convert few points during GetImage call of the WidgetMap.
	// These points are given in WGS84 coordinates (longitude, latitude) pairs.
	// During the WidgetMap::Draw() call these coordinates will be converted
	// into pixels.
	QVector<QPointF> conCoorEx;
	int iFirstWaypoint = 0;
	int iFirstRoute = 0;
	float fRelPosOnLeg = 0;
	// Ask zoom level, which time is the best to predict"
	const QVector<QPair<int, const char*> >& vPP = m_pMIZ->GetPredictionPoints(m_iZoomFactor);
	for(int i=0; i<vPP.count(); i++)
		conCoorEx << m_pND->PredictPosition(vPP[i].first);

	if(m_pAN->IsDirectWaypoint()) {
		iFirstWaypoint = conCoorEx.count();
		conCoorEx << pWaypoint->GetCoordinate();
	}
	if(pRoute) {
		iFirstRoute = conCoorEx.count();
		conCoorEx << pRoute->GetCoordinates();
		// if we have an active leg, we need "abeam" point
		conCoorEx << pRoute->GetNearestPoint(
			m_pND->GetPosition(),
			m_pAN->GetActiveLeg(),
			&fRelPosOnLeg
		);
	}
	// Set the coordinates to convert
	m_pMIZ->SetExternalCoordinates(m_iZoomFactor, conCoorEx);

	// Draw the background usigng the base class. Here we get the
	// complete map image with correct rotation and cities.
 	bool bImageSuccess = WidgetMap::Draw(P);

	// Now we can also get external points. These points are integers!
	QVector<QPoint> conPtsExternal;
	if(bImageSuccess)
		conPtsExternal = m_pMIZ->GetExternalPoints(m_iZoomFactor);

	// Get the widget size
	const int iW = width();
	const int iH = height();

	// Turn the antialiasing on.
	P.setRenderHints(
		QPainter::Antialiasing |  QPainter::SmoothPixmapTransform,
		true
	);

	// Set font for the main heading numbers
	QFont font(m_pGS->GetFont(GraphicsSingleton::fHSIHeading));
	P.setFont(font);
	P.setPen(m_pGS->GetPen(GraphicsSingleton::pHSICompass));

	// Draw the HSI scale (arc, ticks and numbers)
	DrawHSI(P, common::Deg(m_fMapRotation));
	DrawWindDirection(P);

	// Draw tracking line - from airplane to the ring
	P.resetMatrix();
 	if(bImageSuccess) {
 		DrawTrackingLine(P, conPtsExternal, vPP);
 		if(m_pAN->IsDirectWaypoint()) {
 			DrawDirectToWaypoint(P, conPtsExternal[iFirstWaypoint]);
 		}
 		if(pRoute) {
 			DrawRoute(P, conPtsExternal, iFirstRoute, fRelPosOnLeg);
 		}
 		DrawScale(P,m_iFrameThickness);
 	}
	// Draw the airplane symbol image
	DrawAirplane(P);

	// Draw the airspace, if present
	if(m_pAS) {
		P.setRenderHint(QPainter::Antialiasing, false);
		P.translate(iW*(100-AIRSPACE_WIDTH_100)/100, 0);
		m_pAS->Draw(P, QRect(0, 0, AIRSPACE_WIDTH_100*iW/100, iH));
	}

	// Draw the top bar information (if any)
	int iOff = DrawTopBar(P);
	// Draw the bottom bar information (if any)
	int iBot = DrawBottomBar(P);
	// Draw Heading number
	DrawHeadingNumbers(P, iOff);
	// Draw Battery
	P.resetMatrix();
	m_pPixBat->Draw(
		P, 
		m_iFrameThickness+4,
		iBot-24, 
		(PixmapBattery::Mode)m_pND->GetBackupBatteryMode(), 
		m_pND->GetBackupBatteryCapacity()
	);
}

// -----------------------------------------------------------------------
// TODO The scale drawing may be optimized - now we draw ticks and
// labels that are not visible (and the Qt does the clipping.)

void WidgetHSIMap::DrawHSI(QPainter& P, float fHeadingDeg)
{
	// Set center of the airplane.
	P.translate(m_ptAirplane);

	// Helper class used to draw the heading scale
	instrument::ScaleRound grp(fHeadingDeg+90,-360, 0,360);

	const int iR         = m_iArcRadius;// scale radius
	const int iTickLong  = 5*iR/100+2;	// long tick length
	const int iTickShort = 3*iR/100;		// short tick length

	P.setPen(QPen(Qt::black, 1.3));
	// Draw the circle scale
	grp.DrawArcScale(P, 0,360, iR);

	// Long 10 degree ticks
	for(int i=0; i<360; i+=10) {
		grp.DrawTickScale(P, i, iR, iTickLong);
	}

	// Short 5 degree ticks	(but only if radius is large enough)
	P.setPen(QPen(Qt::black, 1));
	if(iR > 150) {
		for(int i=5; i<360; i+=10) {
			grp.DrawTickScale(P, i, iR, iTickShort);
		}
	}

	// Labels
	P.setPen(Qt::black);
	QFont font(P.font());
	// TODO Verify (optimize) the HSI font size below
	font.setPixelSize(qBound(14, 25*iTickLong/10, 22));
	P.setFont(font);
	QString qs;
	for(int i=0; i<360; i+=30) {
		switch(i) {
			case   0: qs = "N"; break;
			case  90: qs = "E"; break;
			case 180: qs = "S"; break;
			case 270: qs = "W"; break;
			default: qs.setNum(i/10);
		}
		grp.DrawLabelScale(P, i, iR+iTickLong*2+3, qs, false);
	}

	// Draw the heading triangle
	P.drawPixmap(-8,-iR-4, m_pixHeadingTriangle);

	// Draw heading bug
/*	int iHB = m_pAN->GetHeadingBugDeg();
	if(iHB >= 0) {
		P.save();
		const int iC = qRound(cos(common::Rad(iHB-fHeadingDeg))*256);
		const int iS = qRound(sin(common::Rad(iHB-fHeadingDeg))*256);

		P.translate((iS*iR)>>8, (-iC*iR)>>8);
		P.rotate(iHB-fHeadingDeg);
		P.drawPixmap(
			-m_pixHeadingBug.size().width()/2,
			-m_pixHeadingBug.size().height(),
			m_pixHeadingBug
		);
		P.restore();
	}*/

}
// -----------------------------------------------------------------------

void WidgetHSIMap::DrawTrackingLine(
	QPainter& P,
	const QVector<QPoint>& vEP,
	const QVector<QPair<int, const char*> >& vPP
)
{
	// We only draw tracking line if we have some real ground speed.
	if(m_pND->GetGroundSpeed() < 12)
		return;

	P.setPen(m_pGS->GetPen(GraphicsSingleton::pHSITracking));
	QColor color = m_pGS->GetPen(GraphicsSingleton::pHSITracking).color();
	P.setBrush(color);
// 		P.setBrush(Qt::transparent);

	const float fT = m_pND->GetTracking()-m_fMapRotation;
	const int iC = qRound(cos(fT)*256);
	const int iS = qRound(sin(fT)*256);
	const int iER4 = m_iArcRadius-11;

	QPoint pt1((20*iS)>>8, -(20*iC)>>8);
	QPoint pt2((iER4*iS)>>8, -(iER4*iC)>>8);
	pt1 += m_ptAirplane;
	pt2 += m_ptAirplane;
	P.drawLine(pt1, pt2);

	P.setPen(color);
	P.setFont(m_pGS->GetFont(GraphicsSingleton::fHSIText));
	for(int i=0; i<vPP.count(); i++) {
		P.drawEllipse(QRect(vEP[i]-QPoint(4,4), QSize(7,7)));
		P.drawText(vEP[i]+QPoint(7,5), vPP[i].second);
	}
}

// -----------------------------------------------------------------------

void WidgetHSIMap::DrawRoute(
	QPainter& P,
	const QVector<QPoint>& vEP,
	int iFirst,
	float fRelPosLeg
)
{
	const route::Route* pR = m_pAN->GetRoute();
	P.setPen(m_pGS->GetPen(GraphicsSingleton::pRouteInactiveLeg));

	bool bIsDirect = m_pAN->IsDirectWaypoint();
	const int iActiveLeg = m_pAN->GetActiveLeg();

	// Draw inactive legs first ...
	P.setPen(m_pGS->GetPen(GraphicsSingleton::pRouteInactiveLeg));
	for(int iLeg=1; iLeg<pR->GetWaypoints().GetCount(); iLeg++) {
		if(iActiveLeg==iLeg && bIsDirect==false) continue;
		//             previous point, leg (final) point
		P.drawLine(vEP[iFirst+iLeg-1], vEP[iFirst+iLeg]);
	}

	// Draw the active leg last
	if(iActiveLeg > 0 && bIsDirect==false) {
		// If the "current" point is on the leg, draw part of the
		// leg that was already passed in inactive color and the
		// part of the leg yet to be flown in active color.
		if(fRelPosLeg > 0.0f && fRelPosLeg < 1.0) {
			// Part of the leg is inactive
			P.drawLine(vEP[iFirst+iActiveLeg-1], vEP.back());

			P.setPen(m_pGS->GetPen(GraphicsSingleton::pRouteActiveLeg));
			P.drawLine(vEP.back(), vEP[iFirst+iActiveLeg]);
		}
		else {
			P.setPen(m_pGS->GetPen(GraphicsSingleton::pRouteActiveLeg));
			P.drawLine(vEP[iFirst+iActiveLeg-1], vEP[iFirst+iActiveLeg]);
		}
	}
	
	if(bIsDirect==false) { 
		// Now, draw the time of arrivals on the route
		const int n = pR->GetWaypoints().GetCount();
		QVector<std::pair<float, float> > 
		vTD = pR->GetTimeDistance(iActiveLeg, m_pND->GetPosition(), m_pND->GetGroundSpeed());
		Q_ASSERT(vTD.count() == n);
		
		P.setRenderHint(QPainter::Antialiasing, false);
		P.setFont(m_pGS->GetFont(GraphicsSingleton::fBanner));
		// For each, but the active leg(point)
		for(int i=0; i<n; i++) {
			
			if(vTD[i].first > 0) {
				QRect rc(vEP[iFirst+i]-QPoint(50,20), QSize(45,17));
				QTime time = QTime::currentTime().addSecs(vTD[i].first);
				
				// Active leg has different background
				if(iActiveLeg==i) {
					P.setPen(Qt::black);
					P.setBrush(Qt::white);
				}
				// Other points
				else {
					P.setPen(Qt::white);
					P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBannerBack));
				}
				
				P.drawRect(rc);
				rc.translate(0,2);
				P.drawText(rc, Qt::AlignCenter, time.toString("HH:mm"));
			}
		}
	}
}

// -----------------------------------------------------------------------

void WidgetHSIMap::DrawDirectToWaypoint(QPainter& P, const QPoint& ptD)
{
	// Draw Waypoint path
	// Set center of the airplane.
	P.setPen(m_pGS->GetPen(GraphicsSingleton::pDirectWaypoint));
	P.drawLine(m_ptAirplane, ptD);

	const route::Waypoint* pW = m_pAN->GetWaypoint();
	float fDist = pW->GetBearingDistance(m_pND->GetPosition()).second;
	float fGS = m_pND->GetGroundSpeed();

	// How much sec is to the waypoint
	if(fGS > 1.0f) {
		float fSec = fDist / fGS;
		QTime time = QTime::currentTime().addSecs(fSec);

		P.setPen(Qt::black);
		P.setBrush(Qt::white);
		P.setRenderHint(QPainter::Antialiasing, false);
		P.setFont(m_pGS->GetFont(GraphicsSingleton::fBanner));

		QRect rc(ptD-QPoint(50,20), QSize(45,17));
		P.drawRect(rc);
		rc.translate(0,2);
		P.drawText(rc, Qt::AlignCenter, time.toString("HH:mm"));

		P.setRenderHint(QPainter::Antialiasing, true);
	}
}

// -----------------------------------------------------------------------

int WidgetHSIMap::DrawTopBar(QPainter& P)
{
	using namespace common;
	using namespace unit;

	const int iW = width()*(100-AIRSPACE_WIDTH_100)/100;
	const int iH = 19;
	const int iDX = iW/5;

	P.resetMatrix();
	P.setFont(m_pGS->GetFont(GraphicsSingleton::fBanner));

	QRect rcW(0,0, iW,iH);
	P.setPen(Qt::NoPen);
	P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBannerBack));
	P.setRenderHint(QPainter::Antialiasing, false);
	P.drawRect(rcW);

	rcW.setRight(iDX);
	rcW.setTop(2);
	P.setPen(Qt::white);

	// First segment (Wind)
	Value uv(m_pFUK->iKey_m_s, m_pND->GetWindSpeed());
	uv.ConvertTo(m_pS->GetWindspeedUnit());
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, trUtf8("W %1° %2").
		arg(qRound(Deg(m_pND->GetWindDirection()))).
		arg(uv.Format(0))
	);

	// Second segment (TAS)
	P.translate(iDX,0);
	uv.Set(m_pFUK->iKey_m_s, m_pND->GetTAS());
	uv.ConvertTo(m_pS->GetAirspeedUnit());
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, tr("TAS %1").
		arg(uv.Format(0))
	);

	// Third segment
	P.translate(iDX,0);
	float fGS = m_pND->GetGroundSpeed();
	uv.Set(m_pFUK->iKey_m_s, fGS);
	uv.ConvertTo(m_pS->GetAirspeedUnit());
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, tr("GS %1").
		arg(uv.Format(0))
	);

	// Fourth segment (Satellite count and fix)
	P.translate(iDX,0);
//	uint uiFix = m_pND->GetGPSFix();
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, tr("%1 sat").
		arg(m_pND->GetSatelliteCount())
	);
// 	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, tr("%1 sat/%2").
// 		arg(m_pND->GetSatelliteCount()).
// 		arg(uiFix < 3 ? "No fix" : uiFix == 3? "2D" : "3D")
// 	);

	// Fifth segment (current time)
	P.translate(iDX,0);
	int iLT = m_pS->GetLocalTime2UTCDifferenceMinutes()*60;
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter,
		QString("%1 %2").arg(QTime::currentTime().addSecs(iLT).toString("H:mm"),(iLT==0?"UTC":"LT"))
	);

	return iH+37;
}

// -----------------------------------------------------------------------

int WidgetHSIMap::DrawBottomBar(QPainter& P)
{
	using namespace common;
	using namespace unit;
	using namespace route;

	const int iW = width()*(100-AIRSPACE_WIDTH_100)/100;
	const int iH = 38;
	const int iDX = iW/5;

	P.resetMatrix();
	P.translate(0,height()-iH);
	P.setFont(m_pGS->GetFont(GraphicsSingleton::fMapCity));

	QRect rcW(0,0, iW,iH);
	P.setPen(Qt::NoPen);
	P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBannerBack));
	P.setRenderHint(QPainter::Antialiasing, false);
	P.drawRect(rcW);

	rcW.setRight(iDX);
	rcW.setTop(2);
	P.setPen(Qt::white);

	Value uv;

	// Current waypoint
	const Waypoint* pW = m_pAN->GetWaypoint();
	// Current position coordinate
	QPointF ptPos = m_pND->GetPosition();
	QString qsLat = QString("%1 %2").arg(
		(ptPos.y()>=0 ? "N":"S"),
		QString::number(common::Deg(fabs(ptPos.y())),'f',4)
	);
	QString qsLon = QString("%1 %2").arg(
		(ptPos.x()>=0 ? "E":"W"),
		QString::number(common::Deg(fabs(ptPos.x())),'f',4)
	);


	// Ground speed
	float fGS = m_pND->GetGroundSpeed();
// 	uv.Set(m_pFUK->iKey_m_s, fGS);
// 	uv.ConvertTo(m_pS->GetAirspeedUnit());
// 	QString qsGS(tr("GS %1").arg(uv.Format(0)));

	// Bearing and distance to the waypoint.
	QString qsBear("-");
	QString qsDist("-");
	QString qsETA("-");
	QString qsT("-");

	PairFF bd = std::make_pair(0.0f, 0.0f);
	if(pW) {
		bd = pW->GetBearingDistance(ptPos);
		uv.Set(m_pFUK->iKey_m, bd.second);
		uv.ConvertTo(m_pS->GetDistanceUnit());
		qsBear = trUtf8("Bear %1°").arg(Deg(bd.first), 0, 'f', 0);
		qsDist = tr("Dist %1").arg(uv.Format(0));

		// How much sec is to the waypoint
		if(fGS > 1.0f) {
			float fSec = bd.second / fGS;
			QTime time(0,0);
			time  = time.addSecs(fSec);
			qsT   = QString("T %1").arg(time.toString("H:mm:s"));
			time  = QTime::currentTime();
			time  = time.addSecs(fSec);
			qsETA = QString("ETA %1").arg(time.toString("H:mm"));
		}
	}

	// First segment (bearing to the waypoint, tracking).
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, qsBear);
	P.drawText(rcW, Qt::AlignBottom | Qt::AlignHCenter,
		trUtf8("Track %1°").arg(Deg(m_pND->GetTracking()), 0, 'f', 0)
	);

	// Second segment
	P.translate(iDX,0);
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, qsDist);
	P.drawText(rcW, Qt::AlignBottom | Qt::AlignHCenter, qsT);

	// Third segment
	P.translate(iDX,0);
 	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, qsLat);
 	P.drawText(rcW, Qt::AlignBottom | Qt::AlignHCenter, qsLon);

	// Data required in fourth and fifth segment
	qsDist = "-";
	qsETA  = "-";
	qsT    = "-";
	QString qsCrossTrack("-"); // Cross Track Error XTK
	const route::Route* pR = m_pAN->GetRoute();
	if(pR) {
		std::pair<float, float>
		td = m_pAN->GetRouteTimeDistance(m_pND->GetPosition(), m_pND->GetGroundSpeed()).back();
		if(td.first != 0) {
			QTime time(0,0);
			time  = time.addSecs(td.first);
			qsT   = QString("R.T %1").arg(time.toString("H:mm"));
			time  = QTime::currentTime();
			time  = time.addSecs(td.first);
			qsETA = QString("R.ETA %1").arg(time.toString("H:mm"));
			if(m_pAN->GetActiveLeg() > 0) {
				float fRelPos;
				float fCrossTrackDist;
				pR->GetNearestPoint(
					m_pND->GetPosition(),
					m_pAN->GetActiveLeg(),
					&fRelPos,
					&fCrossTrackDist
				);
				if(fRelPos >= 0.0f && fRelPos <= 1.0f)	{
					uv.Set(m_pFUK->iKey_m, fCrossTrackDist);
					uv.ConvertTo(m_pS->GetDistanceUnit());
					qsCrossTrack = QString("XTK %1").arg(uv.Format(1));
				}
			}
		}
		uv.Set(m_pFUK->iKey_m, td.second);
		uv.ConvertTo(m_pS->GetDistanceUnit());
		qsDist = QString("R.Dist %1").arg(uv.Format(0));
	}


	// Fourth segment
	P.translate(iDX,0);
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, qsETA);
	P.drawText(rcW, Qt::AlignBottom | Qt::AlignHCenter, qsT);

	// Fifth segment
	P.translate(iDX,0);
	P.drawText(rcW, Qt::AlignTop | Qt::AlignHCenter, qsDist);
	P.drawText(rcW, Qt::AlignBottom | Qt::AlignHCenter, qsCrossTrack);

	return height()-iH;
}

// -----------------------------------------------------------------------

void WidgetHSIMap::DrawWindDirection(QPainter& P)
{
	// TODO Change color according to the windspeed limits
	float fSpeed = m_pND->GetWindSpeed();
	// TODO Make the low limit configurable
	if(fSpeed < 1.5)
		return;
	float fDir   = m_pND->GetWindDirection() + M_PI_2 - m_pND->GetYaw();

	P.save();
	P.setBrush(QColor(255,255,255, 160));
	P.resetMatrix();
	P.translate(50,60);
	P.rotate(common::Deg(fDir));
	P.drawPolygon(m_pgWind);
	P.restore();
}

// -----------------------------------------------------------------------

void WidgetHSIMap::DrawHeadingNumbers(QPainter& P, int iTopOffset)
{
	int iHT = m_pS->GetHSIMapType();
	if(iHT == HSI_NORTH_UP)
		return;
		
//	const char acMT[] = "HTN";
	const int iW = width();
	P.resetMatrix();
	P.translate(iW/2,iTopOffset);

	QRect rc(-HX, 1, HX*2, HY+1);

	P.setPen(Qt::NoPen);
// 	P.setFont(m_pGS->GetFont(GraphicsSingleton::fHSIHeading));
	P.setFont(m_pGS->GetFont(GraphicsSingleton::fHSIText));
	P.setBrush(m_pGS->GetBrush(GraphicsSingleton::bBannerBack));

	P.setPen(Qt::white);
	P.drawPolygon(m_pgHeading);

	int iHeadingDeg = qRound(common::Deg(m_fMapRotation));
	if(iHeadingDeg == 0) 
		iHeadingDeg = 360;
		
	P.drawText(
		rc,
		Qt::AlignTop | Qt::AlignHCenter,
// 		QString("%1").arg(iHeadingDeg, 3,10,QChar('0'))
		QString("%1").arg(iHeadingDeg)
//		QString("%1%2").arg(iHeadingDeg).arg(acMT[iHT])
	);
}

// -----------------------------------------------------------------------

void WidgetHSIMap::DrawScale(QPainter& P, int iLeftOffset)
{
	const QImage& imgScale = m_pMIZ->GetScaleImage(m_iZoomFactor);
	P.drawImage(iLeftOffset, m_ptAirplane.y()-imgScale.height(), imgScale);
}

// -----------------------------------------------------------------------
