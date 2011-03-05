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

#include <QPainter>
#include <QPainterPath>
#include "MathEx.h"
#include "ScaleRound.h"
#include "TurnBankGaugeRound.h"

namespace instrument {

TurnBankGaugeRound::TurnBankGaugeRound(
	const QString& qsPathToAirplane,
	QWidget *pParent
)
 : AbstractGauge(pParent)
{
//	Q_ASSERT(m_pPa
	m_qsTurnImage = qsPathToAirplane;
	m_fRate = -2.43*M_PI/180;
	m_fBallAngle = -0.5/9.81;
}


TurnBankGaugeRound::~TurnBankGaugeRound()
{
}

// -------------------------------------------------------------------------

void TurnBankGaugeRound::DrawForeground(QPainter& P)
{
	AbstractGauge::DrawForeground(P);
	P.setRenderHint(QPainter::SmoothPixmapTransform);
	
	float a;
		
	// Draw ball (slip-skid)
	a = m_fBallAngle;
	int iH = P.device()->height();
	
	float fR  = m_fR*iH;
	float fCy = (m_fC-0.5)*iH;
	float fT  = m_fT*iH;
	
	QPoint pt(a*fR, fCy+fR*(1-a*a/2));
	QRect rc(0,0, 0.7*fT,0.9*fT);
	rc.moveCenter(pt);
	
	P.setPen(Qt::NoPen);
	P.setBrush(QColor(qRgb(38,38,38)));
	P.drawEllipse(rc);
		
	// We need two vertical black strips.
	P.setPen(QPen(Qt::black, 1.5));

	a = 0.6*fT/fR;
	float fSa, fCa;
	float fR1 = fR+fT/2;
	float fR2 = fR-fT/2;
	
// XXX:
//	sincosf(a, &fSa, &fCa);

	P.drawLine(
		-fSa*fR2, fCy+fCa*fR2,
		-fSa*fR1, fCy+fCa*fR1
	);
	P.drawLine(
		+fSa*fR2, fCy+fCa*fR2,
		+fSa*fR1, fCy+fCa*fR1
	);

	// Draw turn coordinator
	// Calculate rotations
	// GetValue = radians per second
	a = m_fRate * 600/M_PI; // degress
	if(a > 50)
		a=50;
	if(a < -50)
		a=-50;
	
	P.rotate(a);
	P.drawPixmap(
		-m_pxTurn.width()/2,
		-m_pxTurn.height()/2, 
		m_pxTurn
	);
	
}

// -------------------------------------------------------------------------

QPixmap TurnBankGaugeRound::CreateBackground(const QSize& /*szHint*/)
{
	// Load base image
	QPixmap px(m_qsBackground);
	QPainter P(&px);
	P.setRenderHints(
		QPainter::Antialiasing | 
		QPainter::SmoothPixmapTransform | 
		QPainter::TextAntialiasing, 
		true
	);

	float fAlpha = 6.5;						// tube half angle
	const int iR1 = 500;						// tube outer radius
	const int iTubeThickness = 28;		// thickness of the tube
	const int iTubeBottomSpacing = 28;	// bottom offset of the outer radius
	const int iR2 = iR1-iTubeThickness;	// tube inner radius
	m_fR = (iR1+iR2)/2.0;					// tube mid point radius

	// Center of the tube
	const QPointF ptC(125,250-iR1-iTubeBottomSpacing);

	// sinus and cosinus of the half angle
	float fSa, fCa;

// XXX:
//	sincosf(fAlpha*M_PI/180, &fSa, &fCa);

	// Construct the painter path of four arcs.
	QPainterPath PP;
	// Starting point
	QPointF pt1 = ptC + QPoint(-fSa*iR1, fCa*iR1);
	PP.moveTo(pt1);

	// bottom arc
	QRectF rc1(ptC.x()-iR1, ptC.y()-iR1, 2*iR1, 2*iR1);
	PP.arcTo(rc1, 270-fAlpha, 2*fAlpha);

	// center point for the right end of the tube
	pt1.rx() = ptC.x() + fSa*m_fR;
	pt1.ry() = ptC.y() + fCa*m_fR;
	// rect for the right arc
 	rc1.setSize(QSize(iTubeThickness, iTubeThickness));
 	rc1.moveCenter(pt1);
 	PP.arcTo(rc1, 270+fAlpha, 180);

	// top arc
	QRectF rc2(ptC.x()-iR2, ptC.y()-iR2, 2*iR2, 2*iR2);
	PP.arcTo(rc2, 270+fAlpha,-2*fAlpha);

	// left arc
	pt1.setX(ptC.x() - fSa*m_fR);
 	rc1.moveCenter(pt1);
 	PP.arcTo(rc1, 90-fAlpha, 180);

	// Gradient used for brush (center in the tube center
	// Focal point is also the tube center.
	QRadialGradient grad(ptC, iR1, ptC);
	// white at inner radius (top arc)
	grad.setColorAt(((float)iR2)/iR1, Qt::white);
	// gray at mid radius
	grad.setColorAt(m_fR/iR1, Qt::lightGray);
	// white at outer radius
	grad.setColorAt(1.0, Qt::white);

	// Draw the path using white pen and gradient brush.
	P.setBrush(grad);
	P.setPen(Qt::NoPen);
	P.drawPath(PP);

	// Make the mid point radius dimensionless according to the pixmap size.
	m_fR /= px.height();
	// Remember the center point
	m_fC = ((float)ptC.y())/px.height();
	// Tube thickenss
	m_fT = ((float)iTubeThickness)/px.height(); 


	// Draw 2 min spots for turn indicator
	const int iW = P.device()->width();
	const int iH = P.device()->height();
	int iR = 0.42*iW;
	
	ScaleRound scale(180,-180,-90,90);
	scale.SetCenter(QPoint(iW/2, iH/2));

	P.setPen(QPen(Qt::white, 7));
	scale.DrawTick(P, 0.0,          iR, -iR/12);
	scale.DrawTick(P, M_PI,         iR, -iR/12);
	scale.DrawTick(P, -20*M_PI/180, iR, -iR/12);
	scale.DrawTick(P, 200*M_PI/180, iR, -iR/12);
	
	// Draw some text
	QFont font(GR_FONT, 11);
	
	// L, R labels
	scale.DrawLabel(P, 215*M_PI/180, iR, "L");
	scale.DrawLabel(P, -35*M_PI/180, iR, "R");
	
	// Main title
	P.translate(iW/2, iH/2);
//	DrawTitle(P, QRect(-75,-64, 150,20), tr("TURN COORDINATOR"));
	
	// units 
//	DrawUnit(P, QRect(-30, 34, 60,20), "1 min");
	
	// Return the pixmap
	return px;
}

// -------------------------------------------------------------------------

void TurnBankGaugeRound::ResizeBackground()
{
	AbstractGauge::ResizeBackground();
	QPixmap px(m_qsTurnImage);
	m_pxTurn = px.scaledToWidth(65*width()/100, Qt::SmoothTransformation);
}

// -------------------------------------------------------------------------
} // namespace
