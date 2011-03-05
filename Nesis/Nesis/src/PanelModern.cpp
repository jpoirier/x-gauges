/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009 by Kanardia d.o.o. [see www.kanardia.eu]           *
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

// #if defined(QT_ARCH_ARM)
// 	#include <mmintrin.h>
// #endif

#include "AttitudeGaugeRound.h"
#include "ParseHelper.h"
#include "XMLTags.h"
#include "ParameterAirspeed.h"
#include "ParameterContainer.h"

#include "GraphicsSingleton.h"
#include "NesisAerospaceDataManager.h"
#include "ActiveNavigation.h"
#include "WidgetMain.h"
#include "Settings.h"
#include "FastUnitKeys.h"

#include "ParameterContainer.h"
#include "Unit/Manager.h"
#include "MathEx.h"
#include "CommonDefsQt.h"

#include "PanelModern.h"

// -------------------------------------------------------------------------

namespace modern {

// -----------------------------------------------------------------------

#define PM_ATT_H						360
#define PM_VEL_W						90
#define PM_ALT_W						90

// -----------------------------------------------------------------------

#define ATT_VC				300/100

#define ATT_FONT_SIZE_PX  			18
#define ATT_BALL_SIZE				18
#define ATT_SCALE_MAJOR_TICK		10
#define ATT_SCALE_MINOR_TICK		10
#define ATT_SCALE_LABEL_OFFSET	(ATT_SCALE_MAJOR_TICK+9)

#define ATT_LONG		42
#define ATT_SHORT		24

// W mark
#define ATT_W_W		130
#define ATT_W_H		20

// Scale
#define ATT_S_T		15

// Heading
#define ATT_H_B		100.0f
#define ATT_H_L		(640-PM_VEL_W-PM_ALT_W)
#define ATT_H_C		5.0f
#define ATT_H_H		30
#define ATT_H_MAJOR_TICK		8
#define ATT_H_MINOR_TICK		5
#define ATT_H_FONT_SIZE_PX  	16

// -----------------------------------------------------------------------

Attitude::Attitude(QRect rc)
:	QRect(rc),
	m_scaleRoll(180,-180, -90,90)
{
	m_fRoll  = 0.0f; //45*M_PI/180;
	m_fPitch = 0.0f; //-0*M_PI/180;
	m_fSlip  = 0.0f;
	
	// Set invalid bearing
	m_iLastBearing = -999;

	// size of the horizon tape in vertical direction
	const int iHS = (height()+ 180*ATT_VC)*1.4;

	// Prepare the horizon source image
	m_imHorizonSource = QImage(1, iHS, QImage::Format_RGB32);
	m_imHorizonSource.fill(IB_YELLOW);

	QLinearGradient grBlue(0,0, 0,iHS/2);
	grBlue.setColorAt(0, HORIZON_DARK_BLUE);
	grBlue.setColorAt(1, HORIZON_LIGHT_BLUE);

	QLinearGradient grBrown(0,iHS/2, 0,iHS);
	grBrown.setColorAt(0, HORIZON_LIGHT_BROWN);
	grBrown.setColorAt(1, HORIZON_DARK_BROWN);

	QPainter P(&m_imHorizonSource);
	P.setPen(Qt::NoPen);
	P.setBrush(grBlue);
	P.drawRect(0,0, 1,iHS/2);
	P.setBrush(grBrown);
	P.drawRect(0,iHS/2, 1,iHS/2);

	// Create the destination image.
	m_imHorizonDest = QImage(width(), height(), QImage::Format_RGB32);
	
	// Create the static images
	CreateStaticRefW();
	CreateStaticScale();
	CreateStaticBall();
	CreateStaticHeading();
	CreateStaticHeadLbl();
	
	// The attitude font
	m_fontAttitude = QFont(GR_FONT, 0);
	m_fontAttitude.setPixelSize(ATT_FONT_SIZE_PX);
}
// -------------------------------------------------------------------------

void Attitude::SetAttitude(float fRoll, float fPitch, float fYaw)
{
	m_fPitch = 0.70*m_fPitch + 0.30*fPitch;
	m_fRoll  = 0.70*m_fRoll  + 0.30*fRoll;
	m_fYaw   = fYaw;
	
/*	m_fPitch = fPitch;
	m_fRoll  = fRoll;
	m_fYaw   = fYaw;*/
}

// -------------------------------------------------------------------------

void Attitude::SetSideSlip(float fSlip)
{
	m_fSlip = 0.1*fSlip + 0.9*m_fSlip;
}

// -------------------------------------------------------------------------

void Attitude::CreateHorizonImage(QImage &imDst)
{
	QPoint ptxOrg(imDst.width()/2, imDst.height()/2);

	QPoint ptxO(
		m_imHorizonSource.width()/2,
		(int)(m_imHorizonSource.height()/2 - m_fPitch*180/M_PI*ATT_VC)
	);

	const int iSideSize = m_imHorizonSource.bytesPerLine()/4;

	// Origin of the destination image
	const int xii0 = ptxOrg.x();
	const int yii0 = ptxOrg.y();

	// Origin of the source image
// 	const int x0 = ptxO.x();
	const int y0 = ptxO.y();

	// integer values of the cosinus and sinus.
	const int C = (int)(cos(-m_fRoll)*256);
	const int S = (int)(sin(-m_fRoll)*256);

	// loop limits
	const int xiiMax = imDst.width();
	const int yiiMax = imDst.height();

	// offset constants
//	const int Ao = 256*x0 - xii0*C - yii0*S;
	const int Bo = 256*y0 + xii0*S - yii0*C;

	// memory addresses
	QRgb* pDst = (QRgb*)imDst.bits();
	const QRgb* const pSrc = (QRgb*)m_imHorizonSource.bits();

	// This is faster, single pixel width version.
	for(int yii=0; yii<yiiMax; yii++) {
		int xii=0;
		int w = xii*S + yii*C + Bo; // x*256

// #if defined(QT_ARCH_ARM)
/*		__m64 avector;
		__m64 evector = _mm_set1_pi32(8);
		__m64 sinus = _mm_set1_pi32(2*S);
		__m64 wvector = _mm_set_pi32(w, w-S);
		__m64 svector = _mm_set1_pi32(iSideSize);

		for(; xii<xiiMax; xii+=2) {
			avector = _mm_mullo_pi16( _mm_sra_pi16(wvector, evector), svector );
			*pDst++ = pSrc[_mm_extract_pi32(avector,0)];
			wvector = _mm_subs_pi32(wvector, sinus);
			*pDst++ = pSrc[_mm_extract_pi32(avector,1)];
		}*/
		
// #else
		for(; xii<xiiMax; xii++) {
			//             y * line size       +  x
			*pDst++ = pSrc[(w>>8)*iSideSize];	// Here we use the first pix column only
			w -= S;
		}
// #endif		
	}
}

// -------------------------------------------------------------------------

void Attitude::CreateStaticRefW()
{
	const int iW = width();
	const int iH = height();
	const int iW2 = iW/2;
	const int iH2 = iH/2;

	// The W pixmap.
	m_pxRefW = QPixmap(ATT_W_W, ATT_W_H);
	m_pxRefW.fill(Qt::transparent);
	m_ptRefW = QPoint(iW2 - m_pxRefW.width()/2, iH2 - 2);
	QPainter W(&m_pxRefW);
/*	W.setPen(Qt::black);
	W.drawRect(0,0,m_pxRefW.width()-1,m_pxRefW.height()-1);*/
 	W.setRenderHint(QPainter::Antialiasing,true);
	W.setPen(QPen(Qt::yellow, 3));

	W.translate(m_pxRefW.width()/2, 2);
	QPolygon pg;
	const int iX = ATT_W_H-4;
	const int iL = ATT_W_W/2-2;
	pg << QPoint(-iL,   0);
	pg << QPoint(-2*iX, 0);
	pg << QPoint(-iX,   iX);
	pg << QPoint(  0,   0);
	pg << QPoint( iX,   iX);
	pg << QPoint( 2*iX, 0);
	pg << QPoint( iL,   0);
	W.drawPolyline(pg);
}

// -------------------------------------------------------------------------

void Attitude::CreateStaticScale()
{
	const int iW = width();
	const int iH = height();
	const int iW2 = iW/2;
	const int iH2 = iH/2;

	// The Scale pixmap
	m_iR = 36*iH/100;
	m_pxScale = QPixmap(2.3f*m_iR, 0.7f*m_iR);
	m_pxScale.fill(Qt::transparent);
	m_ptScale = QPoint(iW2-m_pxScale.width()/2, iH2-(m_iR+ATT_S_T));
	QPainter S(&m_pxScale);
/*	S.setPen(Qt::black);
	S.drawRect(0,0,m_pxScale.width()-1,m_pxScale.height()-1);*/
 	S.setRenderHint(QPainter::Antialiasing,true);

	// Draw the turn arc on the top
	S.setPen(QPen(Qt::white, 1.5f));
	S.setBrush(Qt::white);

	m_scaleRoll.SetCenter(QPoint(m_pxScale.width()/2, m_iR+ATT_S_T));
	m_scaleRoll.DrawTriangleScale(S,   0, m_iR+10, 9);
	m_scaleRoll.DrawTickScale(S, -60, m_iR, ATT_SCALE_MAJOR_TICK);
	m_scaleRoll.DrawTickScale(S, -45, m_iR, ATT_SCALE_MINOR_TICK);
	m_scaleRoll.DrawTickScale(S, -30, m_iR, ATT_SCALE_MAJOR_TICK);
	m_scaleRoll.DrawTickScale(S, -15, m_iR, ATT_SCALE_MINOR_TICK);
	m_scaleRoll.DrawTickScale(S,  15, m_iR, ATT_SCALE_MINOR_TICK);
	m_scaleRoll.DrawTickScale(S,  30, m_iR, ATT_SCALE_MAJOR_TICK);
	m_scaleRoll.DrawTickScale(S,  45, m_iR, ATT_SCALE_MINOR_TICK);
	m_scaleRoll.DrawTickScale(S,  60, m_iR, ATT_SCALE_MAJOR_TICK);

	QFont font(GR_FONT);
	font.setPixelSize(ATT_FONT_SIZE_PX);
	S.setFont(font);
	m_scaleRoll.DrawLabelScale(S,  60, m_iR+ATT_SCALE_LABEL_OFFSET, "60", false);
	m_scaleRoll.DrawLabelScale(S,  30, m_iR+ATT_SCALE_LABEL_OFFSET, "30", false);
	m_scaleRoll.DrawLabelScale(S, -30, m_iR+ATT_SCALE_LABEL_OFFSET, "30", false);
	m_scaleRoll.DrawLabelScale(S, -60, m_iR+ATT_SCALE_LABEL_OFFSET, "60", false);
	m_scaleRoll.DrawArcScale(S, -60,60, m_iR);
}

// -------------------------------------------------------------------------

void Attitude::CreateStaticBall()
{
	const int iW = width();
	const int iH = height();
	const int iW2 = iW/2;
//	const int iH2 = iH/2;

	// The ball pixmap
	m_pxBallBack = QPixmap(1.45f*ATT_BALL_SIZE, 1.2*ATT_BALL_SIZE);
	m_ptBallBack = QPoint(iW2-m_pxBallBack.width()/2, iH-m_pxBallBack.height());
	m_pxBallBack.fill(Qt::transparent);

	QPainter B(&m_pxBallBack);
/*	B.setPen(Qt::black);
	B.drawRect(0,0,m_pxBallBack.width()-1,m_pxBallBack.height()-1);*/
 	B.setRenderHint(QPainter::Antialiasing,true);
	B.translate(m_pxBallBack.width()/2, 0);

	// Draw two vertical lines
	B.setPen(QPen(Qt::white, 2));
	B.drawLine(-(ATT_BALL_SIZE/2+2), 0, -(ATT_BALL_SIZE/2+2), m_pxBallBack.height()-1);
	B.drawLine( (ATT_BALL_SIZE/2+2), 0,  (ATT_BALL_SIZE/2+2), m_pxBallBack.height()-1);

	// Now, create the ball
	m_pxBall = QPixmap(ATT_BALL_SIZE+2, ATT_BALL_SIZE+2);
	m_pxBall.fill(Qt::transparent);
	QPainter F(&m_pxBall);
 	F.setRenderHint(QPainter::Antialiasing,true);
	F.setBrush(Qt::white);
	F.setPen(Qt::black);
	F.drawEllipse(1,1, ATT_BALL_SIZE, ATT_BALL_SIZE);
}
// -------------------------------------------------------------------------

void Attitude::CreateStaticHeading()
{
	// Coefficients first.
	m_fH1 = ATT_H_L/ATT_H_B;
	m_fH0 = (ATT_H_B/2 + ATT_H_C)*m_fH1;
	m_fH1Rad = m_fH1*180/M_PI;

	m_ptHeading = QPoint(PM_VEL_W, 0);

	// Length of the pixmap.
	m_pxHeadingBase = QPixmap(m_fH1*(360+ATT_H_B+ATT_H_C*2), ATT_H_H);
	m_pxHeadingBase.fill(Qt::transparent);

	QPainter P(&m_pxHeadingBase);
 	P.setRenderHint(QPainter::Antialiasing,true);

// 	P.setBrush(QBrush(QColor(10,10,10,90)));
// 	P.setPen(Qt::NoPen);
// 	P.drawRect(0,0,m_pxHeadingBase.width(), m_pxHeadingBase.height());

	// Draw major scale.
	P.setPen(QPen(Qt::white,2));
	for(int iH=-ATT_H_B/2; iH<=360+ATT_H_B/2; iH+=10) {
		int iX = GetHeadingX(iH);
		P.drawLine(iX, ATT_H_H-ATT_H_MAJOR_TICK, iX, ATT_H_H);
	}
	// Draw minor scale.
	P.setPen(QPen(Qt::white,1.5f));
	for(int iH=-ATT_H_B/2+5; iH<=360+ATT_H_B/2-5; iH+=10) {
		int iX = GetHeadingX(iH);
		P.drawLine(iX, ATT_H_H-ATT_H_MINOR_TICK, iX, ATT_H_H);
	}

	// Draw heading numbers.
	QFont font(GR_FONT);
	font.setPixelSize(ATT_H_FONT_SIZE_PX);
	P.setFont(font);
	for(int iH=-ATT_H_B/2; iH<=360+ATT_H_B/2; iH+=10) {
		int iX = GetHeadingX(iH);
		int iHH = iH;
		if(iHH < 360) iHH+=360;
		if(iHH >= 360) iHH-=360;
		QRect rc(iX-15,1, 30,20);
		QString qs;
		if(iHH == 0)
			qs = "N";
		else if(iHH == 90)
			qs = "E";
		else if(iHH == 180)
			qs = "S";
		else if(iHH == 270)
			qs = "W";
		else
			qs = QString::number(iHH/*/10*/);
		P.drawText(rc, Qt::AlignCenter, qs);
	}
	
	// Copy the result into final pixmap.
	// The final pixmap is updated in the UpdateActiveNavigation.
	m_pxHeading = m_pxHeadingBase;
	
	// We also need to create the bearing polygon (triangle)
	m_pgBearing.clear();
	m_pgBearing << QPoint(0,                 ATT_H_H-ATT_H_MAJOR_TICK);
	m_pgBearing << QPoint(-ATT_H_MAJOR_TICK, ATT_H_H);
	m_pgBearing << QPoint( ATT_H_MAJOR_TICK, ATT_H_H);
}
// -------------------------------------------------------------------------

void Attitude::CreateStaticHeadLbl()
{
	// Draw heading label
	m_pxHeadLbl = QPixmap(40,21);
	m_pxHeadLbl.fill(Qt::transparent);
	QPainter P(&m_pxHeadLbl);
	QRect rcH(1, 1, m_pxHeadLbl.width()-2, m_pxHeadLbl.height()-2);
	
	P.setPen(QPen(Qt::darkGray, 2));
	P.setBrush(Qt::white);
	P.drawRect(rcH);
	
	m_rcHeadLbl = QRect(
		(width()-m_pxHeadLbl.width())/2, 0, 
		m_pxHeadLbl.width(), m_pxHeadLbl.height()+2
	);
}
// -------------------------------------------------------------------------

void Attitude::UpdateActiveNavigation()
{
	using namespace route;
	using namespace common;
	
	ActiveNavigation* 			pAN = ActiveNavigation::GetInstance();
//	Settings*						pS  = Settings::GetInstance();
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	
	// Current waypoint
	const Waypoint* pW = pAN->GetWaypoint();
	QPointF ptPos = pND->GetPosition();
	if(pW) {
 		PairFF bd = pW->GetBearingDistance(ptPos);
 		int iBear = qRound(Deg(bd.first));
//		int iBear = 350;
		
		// The bearing triangle on the compass (heading) rose.
		if(iBear != m_iLastBearing) {
			// copy the pixmap
			m_pxHeading = m_pxHeadingBase;
			
			// draw the bearing mark
			QPainter P(&m_pxHeading);
			int iX = GetHeadingX(iBear);
			P.setPen(Qt::NoPen);
			P.setBrush(Qt::yellow);
			
			// 0-359 position
			P.translate(iX,0);
			P.drawConvexPolygon(m_pgBearing);
			
			// we need to draw additional mark for angles [-ATT_H_B/2, 0]
			if(iBear-360 > -ATT_H_B/2) {
				P.resetMatrix();
				iX = GetHeadingX(iBear-360);
				P.translate(iX,0);
				P.drawConvexPolygon(m_pgBearing);
			}
			
			// we need to draw additional mark for angles [0, ATT_H_B/2]
			if(iBear < ATT_H_B/2) {
				P.resetMatrix();
				iX = GetHeadingX(iBear+360);
				P.translate(iX,0);
				P.drawConvexPolygon(m_pgBearing);
			}
			
			m_iLastBearing = iBear;
		}
	}
	// No waypoint is active
	else {
		int iBear = -1;
		if(iBear != m_iLastBearing) {
			m_pxHeading = m_pxHeadingBase;
			m_iLastBearing = iBear;
		}
	}
}

// -------------------------------------------------------------------------

void Attitude::DrawBackground(QPainter& P)
{
	CreateHorizonImage(m_imHorizonDest);
	P.drawImage(topLeft(), m_imHorizonDest);
	
	// Draw W
	P.drawPixmap(m_ptRefW, m_pxRefW);
	// Draw Scale
	P.drawPixmap(m_ptScale, m_pxScale);
	// Draw Ball
	P.drawPixmap(m_ptBallBack, m_pxBallBack);

	// Draw Heading
	int iX = GetHeadingXRad(m_fYaw);
	QRect rcSource(iX - ATT_H_L/2, 0, ATT_H_L, ATT_H_H);
	P.drawPixmap(m_ptHeading, m_pxHeading, rcSource);
	
	// Draw Heading label
	P.drawPixmap(m_rcHeadLbl.topLeft(), m_pxHeadLbl);
}

// -------------------------------------------------------------------------

void Attitude::DrawForeground(QPainter& P)
{
	// Built an image where the horizon is translated according to the pitch
	// and rotated according to the roll.
 	P.setRenderHint(QPainter::Antialiasing,true);
	const int iW2 = width()/2;
	const int iH2 = height()/2;
	P.translate(iW2, iH2);
	
	// Draw the ball
	float fBall = qBound(-iW2*0.7f, (float)m_fSlip*iW2/0.6f, iW2*.07f);
	P.drawPixmap(fBall-m_pxBall.width()/2, iH2-m_pxBall.height(), m_pxBall);

	P.setPen(Qt::NoPen);
	P.setBrush(Qt::yellow);
	m_scaleRoll.SetCenter(QPoint(0,0));
	m_scaleRoll.DrawTriangleScale(P, -m_fRoll*180/M_PI, m_iR-9, -10);

	// Draw the lines
	P.setPen(QPen(Qt::white, 1.5));
	P.rotate(-m_fRoll*180/M_PI);
	P.translate(0,m_fPitch*180/M_PI*ATT_VC);

	P.drawLine(-130*(iW2)/100,1,130*(iW2)/100,1);

	// negative pitch
	P.drawLine(-ATT_SHORT, 5*ATT_VC, ATT_SHORT, 5*ATT_VC);
	P.drawLine(-ATT_LONG, 10*ATT_VC, ATT_LONG, 10*ATT_VC);
	P.drawLine(-ATT_SHORT,15*ATT_VC, ATT_SHORT,15*ATT_VC);
	P.drawLine(-ATT_LONG, 20*ATT_VC, ATT_LONG, 20*ATT_VC);
	P.drawLine(-ATT_SHORT,25*ATT_VC, ATT_SHORT,25*ATT_VC);

	P.setFont(m_fontAttitude);
	P.drawText(-ATT_LONG-31, 10*ATT_VC+5, "10");
	P.drawText( ATT_LONG+13,  10*ATT_VC+5, "10");
	P.drawText(-ATT_LONG-31, 20*ATT_VC+5, "20");
	P.drawText( ATT_LONG+13,  20*ATT_VC+5, "20");

	// positive pitch
	P.drawLine(-ATT_SHORT+5, -5*ATT_VC, ATT_SHORT-5, -5*ATT_VC);
	if(m_fPitch > -20*M_PI/180) {
		P.drawLine(-ATT_LONG+5,    -10*ATT_VC, ATT_LONG-5, -10*ATT_VC);
		P.drawText(-ATT_LONG+5-31, -10*ATT_VC+5, "10");
		P.drawText( ATT_LONG-5+13,  -10*ATT_VC+5, "10");
	}
	if(m_fPitch > -15*M_PI/180)
		P.drawLine(-ATT_SHORT+7,-15*ATT_VC, ATT_SHORT-7,-15*ATT_VC);
	if(m_fPitch > -10*M_PI/180) {
		P.drawLine(-ATT_LONG+10,    -20*ATT_VC, ATT_LONG-10, -20*ATT_VC);
		P.drawText(-ATT_LONG+10-31, -20*ATT_VC+5, "20");
		P.drawText( ATT_LONG-10+13,  -20*ATT_VC+5, "20");
	}
	if(m_fPitch > -5*M_PI/180)
		P.drawLine(-ATT_SHORT,-25*ATT_VC, ATT_SHORT,-25*ATT_VC);

	// Yaw text
	P.resetMatrix();
	int iYaw = qRound(common::Deg(m_fYaw));
	if(iYaw == 0) 
		iYaw = 360;
	P.setPen(Qt::black);
	P.drawText(m_rcHeadLbl, Qt::AlignCenter, QString::number(iYaw));

	P.setRenderHint(QPainter::Antialiasing,false);
}
// -------------------------------------------------------------------------


#define AS_TOP_OFFSET					30
#define AS_BOTTOM_OFFSET				30
#define AS_ARROW_OFFSET					-10
#define AS_SCALE_MAJOR_TICK_SIZE		6
#define AS_SCALE_MINOR_TICK_SIZE		4
#define AS_FONT_SIZE_PX					16
#define AS_SCALE_FONT_SIZE_PX			15
#define AS_ARROW_FONT_SIZE_PX			18
#define AS_ARROW_W						60
#define AS_ARROW_H						30

// -------------------------------------------------------------------------

Airspeed::Airspeed(const QDomElement& el,	QRect rc)
: QRect(rc), instrument::Scale(el)
{
	Q_ASSERT(GetParameter());
	Q_ASSERT(*GetParameter()==typeid(ParameterAirspeed));

	// Calculate the coefficients
	int iT = AS_TOP_OFFSET;
	int iB = height() - AS_BOTTOM_OFFSET;
	
	m_fC1 = (iT-iB)/(GetEnd()-GetStart());
	m_fC0 = -m_fC1*GetStart() + iB;

	CreateBackgroundPixmap();
	CreateArrowPixmap(QSize(AS_ARROW_W, AS_ARROW_H));

	// The arrow font
	m_fontArrow = QFont(GR_FONT, 0, QFont::Bold);
	m_fontArrow.setPixelSize(AS_ARROW_FONT_SIZE_PX);

	// True airspeed
	m_pParTAS = instrument::ParameterContainer::GetInstance()->GetParameter(TAG_TAS);
	Q_ASSERT(m_pParTAS);
}

// -------------------------------------------------------------------------

void Airspeed::CreateBackgroundPixmap()
{
	using namespace instrument;
	const ParameterAirspeed* pPar = (const ParameterAirspeed*)GetParameter();

	// Resize the pixmap
	m_pxBack = QPixmap(width(), height());
	m_pxBack.fill(Qt::transparent);
	QPainter P(&m_pxBack);
	
	P.setBrush(QBrush(QColor(10,10,10,90)));
	P.setPen(Qt::NoPen);
	P.drawRect(0,0,width(), height());

	QPen pen(Qt::white, 1);
	P.setPen(pen);
	QFont font(GR_FONT, 10);
	font.setPixelSize(AS_FONT_SIZE_PX);
	P.setFont(font);
	m_fontLabel = font;

	// The top label
	QRect rc(0,0,width(), 20);
	QString qs;
	qs = QString("IAS %1").arg(
		unit::Manager::GetInstance()->GetSignature(pPar->GetUnitKeyUser())
	);
	P.drawText(rc, Qt::AlignCenter, qs);

	// The TAS label
	m_rcTAS = rc;
	m_rcTAS.moveBottom(height()+2);
	P.drawText(m_rcTAS, Qt::AlignLeft | Qt::AlignVCenter, "TAS");
	m_rcTAS.moveLeft(width()/2-1);
	
	// The vertical band.
	m_rcAirspeed = QRect(
		width()-25, AS_TOP_OFFSET, 
		20, height()-(AS_TOP_OFFSET+AS_BOTTOM_OFFSET)
	);

	P.setPen(QPen(Qt::NoPen));
	rc = m_rcAirspeed;
	QPair<float, float> rR;
	rR = pPar->GetGreenRange();

	// Red range below green
	P.setBrush(QBrush(IB_RED));
	rc.setTop(GetSystemY(rR.first));
	P.drawRect(rc);

	// Green range
	P.setBrush(QBrush(IB_GREEN));
	rc.setTop(GetSystemY(rR.second));
	rc.setBottom(GetSystemY(rR.first));
	P.drawRect(rc);

	// Yellow range	
	P.setBrush(QBrush(IB_YELLOW));
	rR = pPar->GetYellowRange();
	rc.setTop(GetSystemY(rR.second));
	rc.setBottom(GetSystemY(rR.first));
	P.drawRect(rc);

	// Red range	
	P.setBrush(QBrush(IB_RED));
	rc.setTop(GetScaleY(m_fE));
	rc.setBottom(GetSystemY(GetParameter()->GetLimits().second));
	P.drawRect(rc);

	// White range
	P.setBrush(Qt::white);
	rc.setLeft(rc.left()+7*m_rcAirspeed.width()/10);
	rR = pPar->GetWhiteRange();
	rc.setTop(GetSystemY(rR.second));
	rc.setBottom(GetSystemY(rR.first));
	P.drawRect(rc);
	
	// Draw the scale - this scale is fixed.
	P.setPen(QPen(Qt::white, 1));
	for(float f = m_fS; f<= m_fE; f+=m_fMajorTickStep) {
		int iY = GetScaleY(f);
		int iX = m_rcAirspeed.left();
		P.drawLine(iX-AS_SCALE_MAJOR_TICK_SIZE, iY, iX-1, iY);
	}

/*	QPainter::CompositionMode mode = P.compositionMode();
	P.setCompositionMode(QPainter::CompositionMode_Darken);
	P.setPen(QPen(QColor(10,10,10,90),0));
	for(float f = m_fS; f<= m_fE; f+=m_fMajorTickStep) {
		int iY = GetScaleY(f);
		int iX = m_rcAirspeed.left();
		P.drawLine(iX, iY, m_rcAirspeed.right()-1, iY);
	}
	P.setCompositionMode(mode);*/
	
	// Draw the labels
	P.setPen(QPen(Qt::white, 1.0));
	font.setPixelSize(AS_SCALE_FONT_SIZE_PX);
	font.setWeight(QFont::Normal);
	P.setFont(font);
	for(float f = m_fS; f<= m_fE; f+=m_fLabelStep) {
		int iY = GetScaleY(f);
		int iX = m_rcAirspeed.left()-AS_SCALE_MAJOR_TICK_SIZE-3;
		P.drawText(10,iY-10,iX-10,20, Qt::AlignVCenter | Qt::AlignRight, 
			QString::number(qRound(f))
		);
	}
}
// -------------------------------------------------------------------------

void Airspeed::CreateArrowPixmap(const QSize& sz)
{
	m_pxArrow = QPixmap(sz);
	m_pxArrow.fill(Qt::transparent);
	QPainter P(&m_pxArrow);
 	P.setRenderHints(
 		QPainter::Antialiasing |
 		QPainter::SmoothPixmapTransform |
 		QPainter::TextAntialiasing,
 		true
 	);

	const int iX = sz.height()*0.5f;

	QPolygon pg;
	pg << QPoint( 1,            sz.height()/2);
	pg << QPoint( iX,           sz.height()-2);
	pg << QPoint( sz.width()-1, sz.height()-2);
	pg << QPoint( sz.width()-1, 1);
	pg << QPoint( iX,           1);

	P.setPen(QPen(Qt::darkGray, 2));
	P.setBrush(Qt::white);
	P.drawConvexPolygon(pg);
}

// -------------------------------------------------------------------------

void Airspeed::DrawBackground(QPainter& P)
{
	P.drawPixmap(0,0, m_pxBack);
}

// -------------------------------------------------------------------------

void Airspeed::DrawForeground(QPainter& P)
{
	GetParameter()->UpdateValueCallback();
	int iY = GetSystemY(GetParameter()->GetValueSystem());
	int iX = width()+AS_ARROW_OFFSET;
	int iH = m_pxArrow.height();
	P.drawPixmap(iX, iY-iH/2, m_pxArrow);
	
	P.setPen(Qt::black);
	P.setFont(m_fontArrow);
	QRect rc(iX+iH/2-2, iY-iH/2, m_pxArrow.width()-iH/2, iH);
	QString qs = QString::number(qRound(GetParameter()->GetValueUser()));
 	P.drawText(rc, Qt::AlignCenter, qs); 

	P.setPen(Qt::white);
	P.setFont(m_fontLabel);
	m_pParTAS->UpdateValueCallback();
	qs = QString::number(qRound(m_pParTAS->GetValueUser()));
 	P.drawText(m_rcTAS, Qt::AlignLeft | Qt::AlignVCenter, qs);
}
// -------------------------------------------------------------------------

int Airspeed::GetSystemY(float fV) const
{
	fV = ConvertToScale(fV);
	return GetScaleY(fV);
}

// -------------------------------------------------------------------------

int Airspeed::GetScaleY(float fScale) const
{
	int iY = qRound(m_fC0 + m_fC1*fScale);
	return qBound(m_rcAirspeed.top(), iY, m_rcAirspeed.bottom());
}

// -------------------------------------------------------------------------

#define ALT_TOP_OFFSET					30
#define ALT_BOTTOM_OFFSET				30
#define ALT_ARROW_OFFSET				10
#define ALT_SCALE_MAJOR_TICK_SIZE	6
#define ALT_SCALE_MINOR_TICK_SIZE	4
#define ALT_FONT_SIZE_PX				16
#define ALT_SCALE_FONT_SIZE_PX		15
#define ALT_ARROW_FONT_SIZE_PX		18
#define ALT_ARROW_W						65
#define ALT_ARROW_H						30
#define ALT_VARIO_RECT_H				20

#define VAR_TOP_OFFSET					60
#define VAR_BOTTOM_OFFSET				60

// -------------------------------------------------------------------------

Altitude::Altitude(
	const QDomElement& ea,	
	const QDomElement& ev,	
	QRect rc)
: QRect(rc), instrument::Scale(ea), m_sVario(ev)
{
	Q_ASSERT(GetParameter() && m_sVario.GetParameter());

	// Calculate the coefficients
	CalcAltitudeCoef();
	CalcVarioCoef();

	// Setup pixmaps
	CreateBackgroundPixmap();
	CreateArrowPixmap(QSize(ALT_ARROW_W, ALT_ARROW_H));

	// The arrow font
	m_fontArrow = QFont(GR_FONT, 0, QFont::Bold);
	m_fontArrow.setPixelSize(ALT_ARROW_FONT_SIZE_PX);

	unit::Manager* pM = unit::Manager::GetInstance();
	m_pParQNH = instrument::ParameterContainer::GetInstance()->GetParameter(TAG_QNH);
	Q_ASSERT(m_pParQNH);
	m_iQNHDecimals = 0;
	if(pM->GetSignature(m_pParQNH->GetUnitKeyUser())=="inHg")
		m_iQNHDecimals = 1;

	// Is vario in m/s scale?
	m_bMetersPerSecond = (pM->GetSignature(m_sVario.GetParameter()->GetUnitKeyUser())=="m/s");
}
// -------------------------------------------------------------------------

void Altitude::CalcAltitudeCoef()
{
	int iT = ALT_TOP_OFFSET;
	int iB = height() - ALT_BOTTOM_OFFSET;
	
	m_fC1 = (iT-iB)/(GetEnd()-GetStart());
	m_fC0 = -m_fC1*GetStart() + iB;
}

// -------------------------------------------------------------------------

void Altitude::CalcVarioCoef()
{
	int iT = VAR_TOP_OFFSET;
	int iB = height() - VAR_BOTTOM_OFFSET;
	
	m_fV1 = (iT-iB)/(m_sVario.GetEnd()-m_sVario.GetStart());
	m_fV0 = -m_fV1*m_sVario.GetStart() + iB;
}

// -------------------------------------------------------------------------

void Altitude::CreateBackgroundPixmap()
{
	using namespace instrument;
	const Parameter* pPar = GetParameter();

	// Resize the pixmap
	m_pxBack = QPixmap(width(), height());
	m_pxBack.fill(Qt::transparent);
	QPainter P(&m_pxBack);
	
	// set slightly dark transparent
	P.setBrush(QBrush(QColor(10,10,10,90)));
	P.setPen(Qt::NoPen);
	P.drawRect(0,0,width(), height());

	QPen pen(Qt::white, 1);
	P.setPen(pen);

	// Altitude part

	// The vertical band.
	m_rcAltitude = QRect(
		4, ALT_TOP_OFFSET, 
		5, height()-(ALT_TOP_OFFSET+ALT_BOTTOM_OFFSET)
	);

	// White range
	P.setBrush(Qt::white);
	P.drawRect(m_rcAltitude.adjusted(0,0,0,-1));
	
	// Draw the scale - this scale is fixed.
	P.setPen(QPen(Qt::white, 1));
	float df = m_fMajorTickStep/m_iMinorIntervalCount;
	for(float f=m_fS; f<=m_fE+EPS; f+=df) {
		int iY = GetScaleY(f);
		int iX = m_rcAltitude.right();
		P.drawLine(iX, iY, iX+ALT_SCALE_MINOR_TICK_SIZE, iY);
	}

	for(float f = m_fS; f<= m_fE+EPS; f+=m_fMajorTickStep) {
		int iY = GetScaleY(f);
		int iX = m_rcAltitude.right();
		P.drawLine(iX, iY, iX+ALT_SCALE_MAJOR_TICK_SIZE, iY);
	}

	// Draw the labels
	P.setPen(QPen(Qt::white, 1.0));
	QFont font(GR_FONT, 10);
	font.setPixelSize(ALT_SCALE_FONT_SIZE_PX);
	font.setWeight(QFont::Normal);
	P.setFont(font);
	for(float f = m_fS; f<= m_fE; f+=m_fLabelStep) {
		int iY = GetScaleY(f);
		int iX = m_rcAltitude.right()+ALT_SCALE_MAJOR_TICK_SIZE+3;
		P.drawText(iX,iY-10,width()-iX,20, Qt::AlignVCenter | Qt::AlignLeft, 
			QString::number(qRound(f))
		);
	}

	// The vario part

	// The vertical band.
	m_rcVario = QRect(
		0.60*width(), VAR_TOP_OFFSET, 
		1, height()-(VAR_TOP_OFFSET+VAR_BOTTOM_OFFSET)
	);

	// White range
	P.setBrush(Qt::white);
	P.drawRect(m_rcVario.adjusted(0,0,0,-1));
	
	// Draw the scale - this scale is fixed.
	P.setPen(QPen(Qt::white, 1));
	df = m_sVario.GetMajorTickStep()/m_sVario.GetMinorIntervalCount();
	for(float f=m_sVario.GetStart(); f<=m_sVario.GetEnd()+EPS; f+=df) {
		int iY = GetScaleVarioY(f);
		int iX = m_rcVario.right();
		P.drawLine(iX, iY, iX+ALT_SCALE_MINOR_TICK_SIZE, iY);
	}

	df = m_sVario.GetMajorTickStep();
	for(float f=m_sVario.GetStart(); f<=m_sVario.GetEnd()+EPS; f+=df) {
		int iY = GetScaleVarioY(f);
		int iX = m_rcVario.right();
		P.drawLine(iX, iY, iX+ALT_SCALE_MAJOR_TICK_SIZE, iY);
	}

	// Draw the labels
	P.setPen(QPen(Qt::white, 1.0));
	font.setPixelSize(ALT_SCALE_FONT_SIZE_PX);
	font.setWeight(QFont::Normal);
	P.setFont(font);

	df = m_sVario.GetLabelStep();
	for(float f=m_sVario.GetStart(); f<=m_sVario.GetEnd()+EPS; f+=df) {
		int iY = GetScaleVarioY(f);
		int iX = m_rcVario.right()+ALT_SCALE_MAJOR_TICK_SIZE+3;
		P.drawText(iX,iY-10,width()-iX,20, Qt::AlignVCenter | Qt::AlignLeft, 
			QString::number(qRound(fabsf(f)))
		);
	}

	// Move the vario rect
	m_rcVario.moveRight(m_rcVario.left()-3);
	m_rcVario.adjust(-8,0,0,0);

	// Labels
	font.setPixelSize(ALT_FONT_SIZE_PX);
	P.setFont(font);
	m_fontLabel = font;

	// The altitude units
	QRect rc(0,0,width(), 20);
	QString qs;
	qs = unit::Manager::GetInstance()->GetSignature(pPar->GetUnitKeyUser());
	P.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, qs);

	// The QNH label
	rc.moveBottom(height()+2);
	m_rcQNH = rc;
	P.drawText(m_rcQNH, Qt::AlignLeft | Qt::AlignVCenter, "QNH");
	m_rcQNH.moveLeft(rc.left() + width()/2);
	
	// vario units
	qs = unit::Manager::GetInstance()->GetSignature(m_sVario.GetParameter()->GetUnitKeyUser());
	rc = QRect(m_rcVario.left()-10, m_rcVario.top()-ALT_TOP_OFFSET, width()-m_rcVario.left()+10, 20);
	P.drawText(rc, Qt::AlignCenter, qs);

	// Draw vario box.
	m_rcVarioNumber = QRect(
		m_rcVario.left()-5, m_rcVario.bottom()+10, 
		width()-(m_rcVario.left()), ALT_VARIO_RECT_H
	);
	P.setPen(QPen(Qt::darkGray, 1.5f));
	P.setBrush(QColor(255,255,255, 255));
//	P.setBrush(QColor(255,255,255, 210));
	P.drawRect(m_rcVarioNumber);
	// slightly adjust the box for the text
	m_rcVarioNumber.moveTop(m_rcVarioNumber.top()+1);
}
// -------------------------------------------------------------------------

void Altitude::CreateArrowPixmap(const QSize& sz)
{
	m_pxArrow = QPixmap(sz);
	m_pxArrow.fill(Qt::transparent);
	QPainter P(&m_pxArrow);
 	P.setRenderHints(
 		QPainter::Antialiasing |
 		QPainter::SmoothPixmapTransform |
 		QPainter::TextAntialiasing,
 		true
 	);

	const int iX = sz.height()*0.5f;
	QPolygon pg;
	pg << QPoint( 1,             1);
	pg << QPoint( 1,             sz.height()-2);
	pg << QPoint( sz.width()-iX, sz.height()-2);
	pg << QPoint( sz.width()-1,  sz.height()/2);
	pg << QPoint( sz.width()-iX, 1);

	P.setPen(QPen(Qt::darkGray, 2));
	P.setBrush(Qt::white);
	P.drawConvexPolygon(pg);
}

// -------------------------------------------------------------------------

void Altitude::DrawBackground(QPainter& P)
{
	P.drawPixmap(0,0, m_pxBack);
}

// -------------------------------------------------------------------------

void Altitude::DrawForeground(QPainter& P)
{
	// Altitude
	GetParameter()->UpdateValueCallback();
	int iY = GetSystemY(GetParameter()->GetValueSystem());
	int iX = -m_pxArrow.width()+ALT_ARROW_OFFSET;
	int iH = m_pxArrow.height();
	P.drawPixmap(iX, iY-iH/2, m_pxArrow);
	
	P.setPen(Qt::black);
	P.setFont(m_fontArrow);
	QRect rc(iX+2, iY-iH/2, m_pxArrow.width()-iH/2+1, iH);
	P.drawText(rc, Qt::AlignCenter, 
		QString::number(qRound(GetParameter()->GetValueUser()/10)*10)
	); 

	// Vario bar
	m_sVario.UpdateValue();
	float fV = m_sVario.GetDumpedSystemValue();
//	m_sVario.GetParameter()->UpdateValueCallback();
// 	float fV = m_sVario.GetParameter()->GetValueSystem();
	iY = GetSystemVarioY(fV);
	rc = m_rcVario;
	if(fV >= 0) {
		rc.setTop(iY);
		rc.setBottom(m_fV0);
		P.setBrush(QColor(IB_GREEN));
	}
	else {
		rc.setTop(m_fV0);
		rc.setBottom(iY);
		P.setBrush(QColor(IB_YELLOW));
	}
	P.setPen(Qt::NoPen);
	P.drawRect(rc);

	// Vario value
	P.setPen(Qt::black);
	P.setFont(m_fontLabel);
//	fV = m_sVario.GetParameter()->GetValueUser();
	fV = unit::Manager::GetInstance()->Convert(
		fV, 
		m_sVario.GetParameter()->GetUnitKeySystem(), 
		m_sVario.GetParameter()->GetUnitKeyUser()
	);
	QString qs;
	if(m_bMetersPerSecond)
		qs = common::Float2String(fV, 1);
	else
		qs = QString::number(qRound(fV/25)*25);
	P.drawText(m_rcVarioNumber, Qt::AlignCenter, qs);

	// QNH
	P.setPen(Qt::white);
	m_pParQNH->UpdateValueCallback();
	qs = common::Float2String(m_pParQNH->GetValueUser(), m_iQNHDecimals);
	P.drawText(m_rcQNH, Qt::AlignLeft | Qt::AlignVCenter, qs);
}
// -------------------------------------------------------------------------

int Altitude::GetSystemY(float fV) const
{
	fV = ConvertToScale(fV);
	return GetScaleY(fV);
}

// -------------------------------------------------------------------------

int Altitude::GetScaleY(float fScale) const
{
	int iY = qRound(m_fC0 + m_fC1*fScale);
	return qBound(m_rcAltitude.top(), iY, m_rcAltitude.bottom());
}

// -------------------------------------------------------------------------

int Altitude::GetSystemVarioY(float fSy) const
{
	fSy = m_sVario.ConvertToScale(fSy);
	return GetScaleVarioY(fSy);
}

// -------------------------------------------------------------------------

int Altitude::GetScaleVarioY(float fSc) const
{
	int iY = qRound(m_fV0 + m_fV1*fSc);
	return qBound(m_rcVario.top(), iY, m_rcVario.bottom());
}

// -------------------------------------------------------------------------

WidgetFlight::WidgetFlight(
	const QDomDocument& doc, 
	QSize sz, 
	QWidget* pParent)
: QWidget(pParent)
{
//	Settings* pS              = Settings::GetInstance();
//	GraphicsSingleton*    pG  = GraphicsSingleton::GetInstance();

	setFixedSize(sz);
	// There is no need to erase the background here.
	setAttribute(Qt::WA_NoSystemBackground, true);
// 	setAttribute(Qt::WA_PaintOnScreen, true);
// 	setAttribute(Qt::WA_OpaquePaintEvent, true);

	const int iW = width();
//	const int iH = height();

	QDomElement elLayout = doc.documentElement().firstChildElement(TAG_LAYOUT);
	if(elLayout.isNull())
		qCritical() << TAG_LAYOUT << "tag not found.";
	QDomElement el = elLayout.firstChildElement(TAG_MODERN);
	if(el.isNull())
		qCritical() << TAG_MODERN << "tag not found.";

	// Set the geometry

	// Attitude
	m_pAtt = new Attitude(QRect(0,0, iW,PM_ATT_H));

	// Airspeed
	// Search for the scale "airspeed"
	m_pVel = new Airspeed(
		FindScaleElement(el, TAG_AIRSPEED), 
		QRect(0,0, PM_VEL_W,m_pAtt->height())
	);

	// Altitude
	// Search for the scale "altitude"
	m_pAlt = new Altitude(
		FindScaleElement(el, TAG_ALTITUDE),
		FindScaleElement(el, TAG_VARIO),
		QRect(iW-PM_ALT_W, 0, PM_ALT_W+1, m_pAtt->height())
	);
}

WidgetFlight::~WidgetFlight()
{
	delete m_pAtt;
	delete m_pVel;
	delete m_pAlt;
}
// -------------------------------------------------------------------------

void WidgetFlight::SetAttitude(
	float fRoll, 
	float fPitch, 
	float fYaw
)
{
	m_pAtt->SetAttitude(fRoll, fPitch, fYaw);
}

// -------------------------------------------------------------------------

void WidgetFlight::SetSideSlip(float fSS)
{
	m_pAtt->SetSideSlip(fSS);
}
// -------------------------------------------------------------------------

void WidgetFlight::UpdateActiveNavigation()
{
	m_pAtt->UpdateActiveNavigation();
}

// -------------------------------------------------------------------------

void WidgetFlight::paintEvent(QPaintEvent* /*pEvent*/)
{
	QPainter P(this);

	// The attitude
	m_pAtt->DrawBackground(P);
	m_pAtt->DrawForeground(P);
	P.resetMatrix();

	// The velocity
	m_pVel->DrawBackground(P);
	m_pVel->DrawForeground(P);

	// The altitude
	P.translate(m_pAlt->topLeft());
	m_pAlt->DrawBackground(P);
	m_pAlt->DrawForeground(P);
}

// -------------------------------------------------------------------------

QDomElement WidgetFlight::FindScaleElement(
	const QDomElement& el, 
	const QString& qsParam
)
{
	QDomElement ef;
	ef = el.firstChildElement(TAG_SCALE);
	bool bValid = false;
	for (; !ef.isNull(); ef = ef.nextSiblingElement(TAG_SCALE)) {
		if(ef.attribute(TAG_PARAM)==qsParam) {
			bValid = true;
			break;
		}
	}
	if(!bValid) {
		qCritical() << "Can't find the" << qsParam << "scale in modern section.";
		return QDomElement();
	}
	return ef;
}

// -----------------------------------------------------------------------

#define WE_RPM_W					100
#define WE_H_MARGIN				15
#define WE_V_MARGIN				4
#define WE_SMALL_SKIP   		11
#define WE_BAR_W					10
#define WE_FONT_SIZE_PX			14
#define WE_LABEL_H				20

// -----------------------------------------------------------------------

WidgetEngine::WidgetEngine(
	const QDomDocument& doc, 
	QSize sz, 
	QWidget* pParent
)
: QWidget(pParent),
  m_sRPM(200,-220, 0,7000)
{
	setFixedSize(sz);
	// There is no need to erase the background here.
	setAttribute(Qt::WA_NoSystemBackground, true);

	// Set the bottom and top coordinate for bars.
	m_iyT = WE_V_MARGIN + 23; 
	m_iyB = height() - m_iyT; 

	QDomElement elLayout = doc.documentElement().firstChildElement(TAG_LAYOUT);
	if(elLayout.isNull())
		qCritical() << TAG_LAYOUT << "tag not found.";
	QDomElement el = elLayout.firstChildElement(TAG_MODERN);
	if(el.isNull())
		qCritical() << TAG_MODERN << "tag not found.";
	el = el.firstChildElement(TAG_ENGINE);
	if(el.isNull())
		qCritical() << TAG_ENGINE << "tag not found in modern element.";

	m_bOilT 		= el.attribute(TAG_OIL_TEMP, TAG_YES) == QLatin1String(TAG_YES);
	m_bWaterT	= el.attribute(TAG_WATER_TEMP, TAG_YES) == QLatin1String(TAG_YES);
	m_bCarbT		= el.attribute(TAG_CARBURETOR, TAG_YES) == QLatin1String(TAG_YES);
	m_bOilP		= el.attribute(TAG_OIL_PRESS, TAG_YES) == QLatin1String(TAG_YES);
	m_bFuelP		= el.attribute(TAG_FUEL_PRESS, TAG_YES) == QLatin1String(TAG_YES);
	m_bVoltage	= el.attribute(TAG_VOLTAGE, TAG_YES) == QLatin1String(TAG_YES);
	m_bCurrent	= el.attribute(TAG_CURRENT, TAG_YES) == QLatin1String(TAG_YES);
	
	instrument::ParameterContainer* pPC = instrument::ParameterContainer::GetInstance();
	m_pParCHT = pPC->GetParameter(TAG_CHT);
	m_pParEGT = pPC->GetParameter(TAG_EGT);
	m_pParOT  = pPC->GetParameter(TAG_OIL_TEMP);
	m_pParWT	 = pPC->GetParameter(TAG_WATER_TEMP);
	m_pParCarbT = pPC->GetParameter(TAG_CARBURETOR);
	
	m_pParOP	= pPC->GetParameter(TAG_OIL_PRESS);
	m_pParFP = pPC->GetParameter(TAG_FUEL_PRESS);
	
	m_pParRPM = pPC->GetParameter(TAG_RPM);
	m_pParMP  = pPC->GetParameter(TAG_MANIFOLD);
	m_pParV   = pPC->GetParameter(TAG_VOLTAGE);
	m_pParA   = pPC->GetParameter(TAG_CURRENT);
	
	Q_ASSERT(m_pParCHT 
			&& m_pParEGT
			&& m_pParOT
			&& m_pParWT
			&& m_pParCarbT
			&& m_pParOP
			&& m_pParFP
			&& m_pParRPM
			&& m_pParMP
			&& m_pParV
			&& m_pParA);

	m_bPressBar = (unit::Manager::GetInstance()->GetSignature(m_pParOP->GetUnitKeyUser()) == "bar");

	CreateBackgroundPixmap();
	CreateNeedlePixmap();
}

WidgetEngine::~WidgetEngine()
{
}

// -----------------------------------------------------------------------

QPair<float, float> WidgetEngine::CalcScaleFactor(
	const instrument::Parameter* pPar
)
{
	QPair<float, float> ff;
	QPair<float, float> rx = pPar->GetRange();

	ff.second = (m_iyT-m_iyB)/(rx.second-rx.first);
	ff.first  = m_iyB - ff.second*rx.first;

	return ff;

}
// -----------------------------------------------------------------------

int WidgetEngine::GetY(const QPair<float,float>& ff, float fS)
{
	return qBound(m_iyT, qRound(ff.first + ff.second*fS), m_iyB);
}
// -----------------------------------------------------------------------

void WidgetEngine::CreateBar(
	QPainter& P, 
	const QString& qsTxt,
	const instrument::Parameter* pPar,
	QRect rcPar, 
	QRect& rcLabel,
	QPair<float, float>& ffScale
) 
{
	P.setPen(Qt::white);
	P.drawText(
		rcPar.center().x()-30, WE_V_MARGIN, 60, 20, 
		Qt::AlignHCenter | Qt::AlignTop, qsTxt
	);

	rcLabel = QRect(QPoint(0,0), QSize(35,WE_LABEL_H));
	rcLabel.moveCenter(rcPar.center());
	rcLabel.moveBottom(rcPar.bottom());

	ffScale = CalcScaleFactor(pPar);
	QRect rs = rcPar;
		
	P.setPen(Qt::NoPen);
	QPair<float, float> ffR = pPar->GetRange();

	for(int i=0; i<pPar->GetCount(); i++) {
		rs.setLeft(rcPar.left()+i*(WE_BAR_W + WE_SMALL_SKIP));
		rs.setRight(rs.left()+WE_BAR_W);

		// Yellow
		P.setBrush(QColor(IB_YELLOW));
		rs.setBottom(GetY(ffScale, ffR.first));
		rs.setTop   (GetY(ffScale, ffR.second));
		P.drawRect(rs);
	
		// Green
		P.setBrush(QColor(IB_GREEN));
		QPair<float, float> ffG = pPar->GetGreenRange();
		rs.setBottom(GetY(ffScale, ffG.first));
		rs.setTop   (GetY(ffScale, ffG.second));
		P.drawRect(rs);
	
		// Top red
		P.setBrush(QColor(IB_RED));
		QPair<float, float> ffL = pPar->GetLimits();
		if(pPar->IsHighLimit()) {
			rs.setBottom(GetY(ffScale, ffL.second));
			rs.setTop   (GetY(ffScale, ffR.second));
			P.drawRect(rs);
		}
		// Bottom red
		if(pPar->IsLowLimit()) {
			rs.setBottom(GetY(ffScale, ffR.first));
			rs.setTop(GetY   (ffScale, ffL.first));
			P.drawRect(rs);
		}
	}
}

// -----------------------------------------------------------------------

void WidgetEngine::CreateBackgroundPixmap()
{
	m_pxBack = QPixmap(width(), height());
	QPainter P(&m_pxBack);
 	P.setRenderHints(
 		QPainter::Antialiasing |
 		QPainter::SmoothPixmapTransform |
 		QPainter::TextAntialiasing,
 		true
 	);

	// The background
	P.setPen(Qt::NoPen);
	P.setBrush(QColor(10,10,10));
	P.drawRect(0,0,width(),height());

	// number of CHTs and EGTs
	const int nCHT = m_pParCHT->GetCount();
	const int nEGT = m_pParEGT->GetCount();

	// height of the rectangles.
	const int iRH	= height()-2*WE_V_MARGIN;

	// Number of groups we have
	int nG = 2; 			// CHT, EGT
	if(m_bOilT) nG++;		// Oil temperature
	if(m_bWaterT) nG++;	// Water temp
	if(m_bCarbT) nG++;	// Carburetor temp
	if(m_bOilP) nG++;		// Oil pressure
	if(m_bFuelP) nG++;	// Fuel pressure
	if(m_bVoltage) nG++;	// Voltage
	if(m_bCurrent) nG++;	// Amperes

	// Total width of groups of glued together
	int iGW = 0;
	// CHTs
	m_rcCHT = QRect(0,WE_V_MARGIN, nCHT*WE_BAR_W + (nCHT-1)*WE_SMALL_SKIP, iRH);
	iGW += m_rcCHT.width();
	// EGTs
	m_rcEGT = QRect(0,WE_V_MARGIN, nEGT*WE_BAR_W + (nEGT-1)*WE_SMALL_SKIP, iRH);
	iGW += m_rcEGT.width();
	// Oil temperature
	if(m_bOilT) {
		m_rcOT = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcOT.width();
	}
	// Water temperature
	if(m_bWaterT) {
		m_rcWT = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcWT.width();
	}
	// Carburetor temperature
	if(m_bCarbT) {
		m_rcCarbT = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcCarbT.width();
	}
	// Oil pressure
	if(m_bOilP) {
		m_rcOP = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcOP.width();
	}
	// Fuel pressure
	if(m_bFuelP) {
		m_rcFP = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcFP.width();
	}
	// Voltage
	if(m_bVoltage) {
		m_rcV = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcV.width();
	}
	// Current
	if(m_bCurrent) {
		m_rcA = QRect(0,WE_V_MARGIN, WE_BAR_W, iRH);
		iGW += m_rcA.width();
	}
	// The RPM
	m_rcRPM = QRect(width()-WE_RPM_W-WE_H_MARGIN, WE_V_MARGIN, WE_RPM_W, iRH);

	// The available width
	int iAW = width() - iGW  - 3*WE_H_MARGIN - WE_RPM_W;
	// group skip
	int iGS = iAW / (nG-1);

	// Position of groups.
	int iX = WE_H_MARGIN;
	// CHT
	m_rcCHT.moveLeft(iX);
	iX += m_rcCHT.width() + iGS;
	// EGT
	m_rcEGT.moveLeft(iX);
	iX += m_rcEGT.width() + iGS;
	// Oil T
	if(m_bOilT) {
		m_rcOT.moveLeft(iX);
		iX += m_rcOT.width() + iGS;
	}
	// Water T
	if(m_bWaterT) {
		m_rcWT.moveLeft(iX);
		iX += m_rcWT.width() + iGS;
	}
	// Carburetor T
	if(m_bCarbT) {
		m_rcCarbT.moveLeft(iX);
		iX += m_rcCarbT.width() + iGS;
	}
	// Oil pressure
	if(m_bOilP) {
		m_rcOP.moveLeft(iX);
		iX += m_rcOP.width() + iGS;
	}
	// Fuel pressure
	if(m_bFuelP) {
		m_rcFP.moveLeft(iX);
		iX += m_rcFP.width() + iGS;
	}
	// Voltage
	if(m_bVoltage) {
		m_rcV.moveLeft(iX);
		iX += m_rcV.width() + iGS;
	}
	// Current
	if(m_bCurrent) {
		m_rcA.moveLeft(iX);
		iX += m_rcA.width() + iGS;
	}

	// Debug rectangles
// 	P.setPen(Qt::white);
// 	P.setBrush(Qt::transparent);
// 	P.drawRect(m_rcCHT);
// 	P.drawRect(m_rcEGT);
// 	if(m_bOilT) P.drawRect(rcPar);
// 	if(m_bWaterT) P.drawRect(m_rcWT);
// 	if(m_bCarbT) P.drawRect(m_rcCarbT);
// 	if(m_bOilP) P.drawRect(m_rcOP);
// 	if(m_bFuelP) P.drawRect(m_rcFP);
// 	if(m_bVoltage) P.drawRect(m_rcV);
// 	if(m_bCurrent) P.drawRect(m_rcA);
// 	P.drawRect(m_rcRPM);

	// Text
	P.setPen(Qt::white);
	QFont font(GR_FONT);
	font.setPixelSize(WE_FONT_SIZE_PX);
	P.setFont(font);

	// Rects at the bottom
	CreateBar(P, "CHT", m_pParCHT, m_rcCHT, m_rcValCHT, m_ffCHT);
	CreateBar(P, "EGT", m_pParEGT, m_rcEGT, m_rcValEGT, m_ffEGT);

	if(m_bOilT)
		CreateBar(P, "Oil T.", m_pParOT, m_rcOT, m_rcValOT, m_ffOT); 
	if(m_bWaterT)
		CreateBar(P, "Water", m_pParWT, m_rcWT, m_rcValWT, m_ffWT); 
	if(m_bOilP)
		CreateBar(P, "Oil P.", m_pParOP, m_rcOP, m_rcValOP, m_ffOP); 
	if(m_bFuelP)
		CreateBar(P, "Fuel P.", m_pParFP, m_rcFP, m_rcValFP, m_ffFP); 
	if(m_bVoltage)
		CreateBar(P, "Volt", m_pParV, m_rcV, m_rcValV, m_ffV); 
	if(m_bCurrent)
		CreateBar(P, "Current", m_pParA, m_rcA, m_rcValA, m_ffA);
	if(m_bCarbT) {
		CreateBar(P, "Carb.", m_pParCarbT, m_rcCarbT, m_rcValCarbT, m_ffCarbT);
		
		P.setPen(Qt::NoPen);
		P.setBrush(QColor(IB_GREEN));
		QRect rc = m_rcCarbT;
		rc.setBottom(GetY(m_ffCarbT, m_pParCarbT->GetRange().first));
		rc.setTop(GetY(m_ffCarbT, m_pParCarbT->GetRange().second));
		rc.setRight(rc.left()+WE_BAR_W);
		P.drawRect(rc);

		P.setBrush(QColor(IB_YELLOW));
		rc.setTop(GetY(m_ffCarbT, m_pParCarbT->GetYellowRange().second));
		rc.setBottom(GetY(m_ffCarbT, m_pParCarbT->GetYellowRange().first));
		P.drawRect(rc);
	}

	// RPM
	P.setPen(Qt::white);
	P.drawText(
		m_rcRPM.center().x()-30, WE_V_MARGIN, 60, 20, 
		Qt::AlignHCenter | Qt::AlignTop, "RPM"
	);
	
	m_sRPM.SetCenter(QPoint(m_rcRPM.center().x(),m_rcRPM.center().y()+10));
	m_sRPM.SetScaleLimits(m_pParRPM->GetRange());
	m_iR_RPM = m_rcRPM.center().x()-m_rcRPM.left() - WE_BAR_W;

	P.setPen(QPen(QColor(IB_GREEN), WE_BAR_W, Qt::SolidLine, Qt::FlatCap));
	m_sRPM.DrawArcScale(P, m_pParRPM->GetRange(), m_iR_RPM);
	P.setPen(QPen(QColor(IB_YELLOW), WE_BAR_W, Qt::SolidLine, Qt::FlatCap));
	m_sRPM.DrawArcScale(P, m_pParRPM->GetYellowRange(), m_iR_RPM);
	P.setPen(QPen(QColor(IB_GREEN), WE_BAR_W, Qt::SolidLine, Qt::FlatCap));
	m_sRPM.DrawArcScale(P, m_pParRPM->GetGreenRange(), m_iR_RPM);
	P.setPen(QPen(QColor(IB_RED), WE_BAR_W, Qt::SolidLine, Qt::FlatCap));
// 	qDebug() << "Red" << m_pParRPM->GetLimits().second << m_pParRPM->GetRange().second;
	m_sRPM.DrawArcScale(P, m_pParRPM->GetLimits().second, m_pParRPM->GetRange().second, m_iR_RPM);
	if(m_pParRPM->IsLowLimit()) {
		m_sRPM.DrawArcScale(P, m_pParRPM->GetRange().first, m_pParRPM->GetLimits().first, m_iR_RPM);
	}
}
// -----------------------------------------------------------------------

void WidgetEngine::CreateNeedlePixmap()
{
	m_pxNeedle = QPixmap(WE_BAR_W + 2*3+1, 4);
	m_pxNeedle.fill(Qt::transparent);

	QPainter P(&m_pxNeedle);
	P.setRenderHint(QPainter::Antialiasing,true);
	
	P.setPen(QPen(Qt::darkGray, 0.8f));
	P.setBrush(Qt::white);
	P.drawRect(0,0,m_pxNeedle.width(), m_pxNeedle.height());
}

// -----------------------------------------------------------------------

void WidgetEngine::paintEvent(QPaintEvent* /*pEvent*/)
{
	QPainter P(this);
	DrawBackground(P);
	DrawForeground(P);
}

// -----------------------------------------------------------------------

void WidgetEngine::DrawBackground(QPainter& P)
{
	P.drawPixmap(0,0, m_pxBack);
}

// -----------------------------------------------------------------------

void WidgetEngine::DrawForeground(QPainter& P)
{
	P.setPen(Qt::white);
	QFont font(GR_FONT);
	font.setPixelSize(WE_FONT_SIZE_PX);
	P.setFont(font);

	int iY, iX;
	const int iH_2 = m_pxNeedle.height()/2;
	float fMax;

	const int nCHT = m_pParCHT->GetCount();
	m_pParCHT->UpdateValueCallback();
	iX = m_rcCHT.left()-3;
	fMax = 0.0f;
	for(int i=0; i<nCHT; i++) {
		iY = GetY(m_ffCHT, m_pParCHT->GetValueSystem(i));
		P.drawPixmap(iX, iY-iH_2, m_pxNeedle);
		iX += (WE_BAR_W + WE_SMALL_SKIP);
		fMax = qMax(fMax, m_pParCHT->GetValueUser(i));
	}
	P.drawText(
		m_rcValCHT, Qt::AlignHCenter | Qt::AlignBottom, 
		common::Float2String(fMax)
	);

	const int nEGT = m_pParEGT->GetCount();
	m_pParEGT->UpdateValueCallback();
	iX = m_rcEGT.left()-3;
	fMax = 0.0f;
	for(int i=0; i<nEGT; i++) {
		iY = GetY(m_ffEGT, m_pParEGT->GetValueSystem(i));
		P.drawPixmap(iX, iY-iH_2, m_pxNeedle);
		iX += (WE_BAR_W + WE_SMALL_SKIP);
		fMax = qMax(fMax, m_pParEGT->GetValueUser(i));
	}
	P.drawText(
		m_rcValEGT, Qt::AlignHCenter | Qt::AlignBottom, 
		common::Float2String(fMax)
	);

	if(m_bOilT)	{
		m_pParOT->UpdateValueCallback();
		P.drawText(
			m_rcValOT, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParOT->GetValueUser())
		);
		iY = GetY(m_ffOT, m_pParOT->GetValueSystem());
		P.drawPixmap(m_rcOT.left()-3, iY-iH_2, m_pxNeedle);
	}
	if(m_bWaterT)	{
		m_pParWT->UpdateValueCallback();
		P.drawText(
			m_rcValWT, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParWT->GetValueUser())
		);
		iY = GetY(m_ffWT, m_pParWT->GetValueSystem());
		P.drawPixmap(m_rcWT.left()-3, iY-iH_2, m_pxNeedle);
	}
	if(m_bCarbT)	{
		m_pParCarbT->UpdateValueCallback();
		P.drawText(
			m_rcCarbT, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParCarbT->GetValueUser())
		);
		iY = GetY(m_ffCarbT, m_pParCarbT->GetValueSystem());
		P.drawPixmap(m_rcCarbT.left()-3, iY-iH_2, m_pxNeedle);
	}
	if(m_bOilP)	{
		m_pParOP->UpdateValueCallback();
		P.drawText(
			m_rcValOP, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParOP->GetValueUser(), (m_bPressBar ? 1 : 0))
		);
		iY = GetY(m_ffOP, m_pParOP->GetValueSystem());
		P.drawPixmap(m_rcOP.left()-3, iY-iH_2, m_pxNeedle);
	}
	if(m_bFuelP)	{
		m_pParFP->UpdateValueCallback();
		P.drawText(
			m_rcValFP, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParFP->GetValueUser(),1)
		);
		iY = GetY(m_ffFP, m_pParFP->GetValueSystem());
		P.drawPixmap(m_rcFP.left()-3, iY-iH_2, m_pxNeedle);
	}
	if(m_bVoltage)	{
		m_pParV->UpdateValueCallback();
		P.drawText(
			m_rcValV, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParV->GetValueUser(),1)
		);
		iY = GetY(m_ffV, m_pParV->GetValueSystem());
		P.drawPixmap(m_rcV.left()-3, iY-iH_2, m_pxNeedle);
	}
	if(m_bCurrent)	{
		m_pParA->UpdateValueCallback();
		P.drawText(
			m_rcValA, Qt::AlignHCenter | Qt::AlignBottom, 
			common::Float2String(m_pParA->GetValueUser(),1)
		);
		iY = GetY(m_ffA, m_pParA->GetValueSystem());
		P.drawPixmap(m_rcA.left()-3, iY-iH_2, m_pxNeedle);
	}

	// RPM
	m_pParRPM->UpdateValueCallback();
 	P.setRenderHints(QPainter::Antialiasing, true);
	P.setPen(QPen(Qt::white, 3));
	m_sRPM.DrawTickScale(P, m_pParRPM->GetValueSystem(), m_iR_RPM, -m_iR_RPM);

	P.drawText(
		m_rcRPM, Qt::AlignHCenter | Qt::AlignBottom, 
		QString::number(qRound(m_pParRPM->GetValueUser()/10)*10)
	);
}

// -----------------------------------------------------------------------

PanelModern::PanelModern(
	const QDomDocument& doc, //!< The xml description document
	QSize sz, 					 //!< Size of the widget
	QWidget *pParent			 //!< Parent.
)
 : AbstractPanel(sz, pParent)
{
//	Settings* pS              = Settings::GetInstance();
//	GraphicsSingleton*    pG  = GraphicsSingleton::GetInstance();

	setAttribute(Qt::WA_NoSystemBackground, true);
//	SetBackgroundColor(pG->GetBrush(GraphicsSingleton::bModernBlackBgd));
	
	setFixedSize(sz);
	const int iW = width();
	const int iH = height();

	// Top widget
	m_pwFlight = new WidgetFlight(doc, QSize(iW,PM_ATT_H), this); 
	// Bottom widget
	m_pwEngine = new WidgetEngine(doc, QSize(iW, iH-PM_ATT_H), this);
	m_pwEngine->setGeometry(0,PM_ATT_H,iW,iH-PM_ATT_H);
}

// -----------------------------------------------------------------------

PanelModern::~PanelModern()
{
}
// -----------------------------------------------------------------------

void PanelModern::Draw(bool /*bMajor*/)
{
	NesisAerospaceDataManager* pN = NesisAerospaceDataManager::GetInstance();
	Settings* pS = Settings::GetInstance();

	const int iMT = pS->GetHSIMapType();
	float fYaw = 
		(iMT == HSI_HEADING_UP) ?	pN->GetYaw() : 
		(iMT == HSI_TRACKING_UP) ? pN->GetTracking() : 0.0;

	// Always draw the attitude widget first
	m_pwFlight->SetAttitude(pN->GetRoll(), pN->GetPitch(), fYaw);
	m_pwFlight->SetSideSlip(pN->GetBallSlip());
	m_pwFlight->update();

	static uint uiCount = 0;
	if(uiCount%3 == 0)
		m_pwEngine->update();
	if(uiCount%10 == 0)
		m_pwFlight->UpdateActiveNavigation();
}

// -----------------------------------------------------------------------

QStringList PanelModern::GetMenuLabels() const
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

void PanelModern::OnMenuButtonPressed(PanelButton ePB)
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

} // namespace
