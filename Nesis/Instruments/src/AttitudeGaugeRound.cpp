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

#include <QDebug>
#include <QPainterPath>
#include <QLinearGradient>

#include "MathEx.h"
#include "CommonDefsQt.h"
#include "AttitudeGaugeRound.h"

#define ATT_LONG		32
#define ATT_SHORT		14
#define CHAMFER 		21
#define DT				11
#define VC				260/100

#define HORIZON_FONT_SIZE_PX  15
#define HORIZON_PIXMAP_SIZE	740

#define BALL_SIZE					16

namespace instrument {

AttitudeGaugeRound::AttitudeGaugeRound(QWidget *pParent)
 : AbstractGauge(pParent),
   m_scaleRoll(180,-180, -90,90)
{
/*	m_fRoll  = 45*M_PI/180;
	m_fPitch = -10*M_PI/180;*/
	m_fRoll  = 0.0f;
	m_fPitch = 0.0f;
	m_fSlip  = 0.0f;
	
	m_fHeadRate = 0.0f;

	// Prepare the horizon image
	m_imHorizonSource = QImage(1,HORIZON_PIXMAP_SIZE, QImage::Format_RGB32);
	m_imHorizonSource.fill(IB_YELLOW);

	QLinearGradient grBlue(0,0, 0,HORIZON_PIXMAP_SIZE/2);
	grBlue.setColorAt(0, HORIZON_DARK_BLUE);
	grBlue.setColorAt(1, HORIZON_LIGHT_BLUE);

	QLinearGradient grBrown(0,HORIZON_PIXMAP_SIZE/2, 0,HORIZON_PIXMAP_SIZE);
	grBrown.setColorAt(0, HORIZON_LIGHT_BROWN);
	grBrown.setColorAt(1, HORIZON_DARK_BROWN);

	QPainter P(&m_imHorizonSource);
	P.setPen(Qt::NoPen);
	P.setBrush(grBlue);
	P.drawRect(0,0, 1,HORIZON_PIXMAP_SIZE/2);
	P.setBrush(grBrown);
	P.drawRect(0,HORIZON_PIXMAP_SIZE/2, 1,HORIZON_PIXMAP_SIZE/2);
}


AttitudeGaugeRound::~AttitudeGaugeRound()
{
}

void AttitudeGaugeRound::SetAttitude(float fRoll, float fPitch)
{
/*	m_fPitch = fPitch;
	m_fRoll  = fRoll;*/
	m_fPitch = 0.70*m_fPitch + 0.30*fPitch;
	m_fRoll  = 0.70*m_fRoll  + 0.30*fRoll;
}

// -------------------------------------------------------------------------

void AttitudeGaugeRound::SetBallSlip(float fSlip)
{
	m_fSlip = 0.1*fSlip + 0.9*m_fSlip;
}

// -------------------------------------------------------------------------

void AttitudeGaugeRound::paintEvent(QPaintEvent* /*pEvent*/)
{
	QPainter P(this);
	DrawForeground(P);
	DrawBackground(P);
}

// -------------------------------------------------------------------------

void AttitudeGaugeRound::DrawForeground(QPainter& P)
{
	P.save();
	// Built an image where the horizon is translated according to the pitch
	// and rotated according to the roll.
 	P.setRenderHint(QPainter::Antialiasing,true);
	const int iW2 = width()/2;
	const int iH2 = height()/2;
	P.translate(iW2, iH2);
	CreateHorizonImage(m_imHorizonDest);
	P.drawImage(-iW2+DT,-iH2+DT, m_imHorizonDest);

	// Draw the lines
	P.save();
	QFont font(GR_FONT);
	font.setPixelSize(HORIZON_FONT_SIZE_PX);
	P.setFont(font);
	P.setPen(QPen(Qt::white, 2));
	P.rotate(-m_fRoll*180/M_PI);
	P.translate(0,m_fPitch*180/M_PI*VC);


	P.drawLine(-130*(iW2-2*DT)/100,1,130*(iW2-2*DT)/100,1);

	// negative pitch
	P.drawLine(-ATT_SHORT, 5*VC, ATT_SHORT, 5*VC);
	P.drawLine(-ATT_LONG, 10*VC, ATT_LONG, 10*VC);
	P.drawLine(-ATT_SHORT,15*VC, ATT_SHORT,15*VC);
	P.drawLine(-ATT_LONG, 20*VC, ATT_LONG, 20*VC);
	P.drawLine(-ATT_SHORT,25*VC, ATT_SHORT,25*VC);

	P.drawText(-ATT_LONG-26, 10*VC+5, "10");
	P.drawText( ATT_LONG+8,  10*VC+5, "10");
	P.drawText(-ATT_LONG-26, 20*VC+5, "20");
	P.drawText( ATT_LONG+8,  20*VC+5, "20");

	// positive pitch
	P.drawLine(-ATT_SHORT+5, -5*VC, ATT_SHORT-5, -5*VC);
	if(m_fPitch > -10*M_PI/180) {
		P.drawLine(-ATT_LONG+5,    -10*VC, ATT_LONG-5, -10*VC);
		P.drawText(-ATT_LONG+5-26, -10*VC+5, "10");
		P.drawText( ATT_LONG-5+8,  -10*VC+5, "10");
	}
	if(m_fPitch > -5*M_PI/180)
		P.drawLine(-ATT_SHORT+7,-15*VC, ATT_SHORT-7,-15*VC);
	if(m_fPitch > -0*M_PI/180) {
		P.drawLine(-ATT_LONG+10,    -20*VC, ATT_LONG-10, -20*VC);
		P.drawText(-ATT_LONG+10-26, -20*VC+5, "20");
		P.drawText( ATT_LONG-10+8,  -20*VC+5, "20");
	}
	if(m_fPitch > 5*M_PI/180)
		P.drawLine(-ATT_SHORT,-25*VC, ATT_SHORT,-25*VC);

	P.restore();


	// Draw the ball

	float fBall = qBound(-iW2*.7f, (float)m_fSlip*iW2/0.6f, iW2*.7f);
	QRect rcBall(fBall-BALL_SIZE/2, iH2-DT-BALL_SIZE-1, BALL_SIZE, BALL_SIZE);
	P.setBrush(Qt::white);
	P.drawEllipse(rcBall);

	int iR = qRound(m_fR*height());
	P.setPen(Qt::NoPen);
	P.setBrush(Qt::yellow);
	m_scaleRoll.SetCenter(QPoint(0,0));
	m_scaleRoll.DrawTriangleScale(P, -m_fRoll*180/M_PI, iR-7, -8);
	
	// Debug only - draw the heading rate
// 	P.setPen(Qt::black);
// 	P.drawText(
// 		-iW2+CHAMFER+DT, iH2-DT-5, 
// 		QString::number(common::Deg(m_fHeadRate), 'f',2)
// 	);

	P.restore();
	
}

// -------------------------------------------------------------------------

QPixmap AttitudeGaugeRound::CreateBackground(const QSize& szHint)
{
	// Create the destination image object for the horizon.
	m_imHorizonDest = QImage(width()-2*DT, height()-2*DT, QImage::Format_RGB32);
	// Create the chamfered rect
	QPixmap px(szHint);
	px.fill(Qt::transparent);
	QPainter P(&px);
 	P.setRenderHints(
 		QPainter::Antialiasing |
 		QPainter::SmoothPixmapTransform |
 		QPainter::TextAntialiasing,
 		true
 	);

	const int iW = px.width();
	const int iH = px.height();
	const int iW2 = iW/2;
	const int iH2 = iH/2;

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
	P.setPen(QPen(QColor(GR_GRAY),1.3));
	P.setBrush(QColor(GR_BLACK));
	P.drawPath(path);

	// Draw the W

	P.translate(iW2, iH2);
	P.setPen(QPen(Qt::yellow, 3));

	QPolygon pg;
	pg << QPoint(-65,  0);
	pg << QPoint(-34,  0);
	pg << QPoint(-17, 17);
	pg << QPoint(  0,  0);
	pg << QPoint( 17, 17);
	pg << QPoint( 34,  0);
	pg << QPoint( 65,  0);
	P.drawPolyline(pg);

	// Draw two vertical lines
	P.setPen(QPen(Qt::white, 2));
	P.drawLine(-(BALL_SIZE/2+2), iH2-(BALL_SIZE+DT+1), -10, iH2-(DT+1));
	P.drawLine( (BALL_SIZE/2+2), iH2-(BALL_SIZE+DT+1),  10, iH2-(DT+1));

	// Draw the turn arc on the top

	const int iR = 38*iH/100;
	m_fR = ((float)iR)/height();

	// Draw the white arc and labels
	P.translate(-iW2, -iH2);
	P.setPen(QPen(Qt::white, 1.5));
	P.setBrush(Qt::white);

	m_scaleRoll.SetCenter(QPoint(iW2, iH2));
	m_scaleRoll.DrawTriangleScale(P,   0, iR+10, 7);
	m_scaleRoll.DrawTickScale(P, -60, iR, 8);
	m_scaleRoll.DrawTickScale(P, -45, iR, 5);
	m_scaleRoll.DrawTickScale(P, -30, iR, 8);
	m_scaleRoll.DrawTickScale(P, -15, iR, 5);
	m_scaleRoll.DrawTickScale(P,  15, iR, 5);
	m_scaleRoll.DrawTickScale(P,  30, iR, 8);
	m_scaleRoll.DrawTickScale(P,  45, iR, 5);
	m_scaleRoll.DrawTickScale(P,  60, iR, 8);

	QFont font(GR_FONT);
	font.setPixelSize(HORIZON_FONT_SIZE_PX);
	P.setFont(font);
	m_scaleRoll.DrawLabelScale(P,  60, iR+16, "60", false);
	m_scaleRoll.DrawLabelScale(P,  30, iR+16, "30", false);
	m_scaleRoll.DrawLabelScale(P, -30, iR+16, "30", false);
	m_scaleRoll.DrawLabelScale(P, -60, iR+16, "60", false);

	m_scaleRoll.DrawArcScale(P, -60,60, iR);

	return px;
}

// -------------------------------------------------------------------------

void AttitudeGaugeRound::CreateHorizonImage(QImage &imgDst)
{
	QPoint ptxOrg(imgDst.width()/2, imgDst.height()/2);

	QPoint ptxO(
		m_imHorizonSource.width()/2,
		(int)(m_imHorizonSource.height()/2 - m_fPitch*180/M_PI*VC)
	);

	const int iSideSize = m_imHorizonSource.bytesPerLine()/4;

	// Origin of the destination image
	const int xii0 = ptxOrg.x();
	const int yii0 = ptxOrg.y();

	// Origin of the source image
//	const int x0 = ptxO.x();
	const int y0 = ptxO.y();

	// integer values of the cosinus and sinus.
	const int C = (int)(cos(-m_fRoll)*256);
	const int S = (int)(sin(-m_fRoll)*256);

	// loop limits
	const int xiiMax = imgDst.width();
	const int yiiMax = imgDst.height();

	// offset constants
//	const int A0 = 256*x0 - xii0*C - yii0*S;
	const int B0 = 256*y0 + xii0*S - yii0*C;

	// memory addresses
	QRgb* pDst = (QRgb*)imgDst.bits();
	const QRgb* const pSrc = (QRgb*)m_imHorizonSource.bits();

	// This is faster, single pixel width version.
	for(int yii=0; yii<yiiMax; yii++) {
		int xii=0;
//		int u = xii*C + yii*S + A0; // y*256
		int w = xii*S + yii*C + B0; // x*256

		for(; xii<xiiMax; xii++) {
			//             y * line size       +  x
			*pDst = pSrc[(w>>8)*iSideSize];	// Here we use the first pix column only
//			u += C;
			w -= S;
			pDst++;
		}
	}
	// This is the complete width version.
/*	for(int yii=0; yii<yiiMax; yii++) {
		int xii=0;
		int u = xii*C + yii*S + A0; // y*256
		int w = xii*S + yii*C + B0; // x*256
		for(; xii<xiiMax; xii++) {
			//             y * line size       +  x
 			*pDst = pSrc[(w>>8)*iSideSize+(u>>8)]; // Here we use complete column width
			u += C;
			w -= S;
			pDst++;
		}
	}*/
}

// -------------------------------------------------------------------------

} // namespace
