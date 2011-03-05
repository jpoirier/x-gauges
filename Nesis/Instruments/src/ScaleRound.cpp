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

#include <QtXml>
#include <QPainter>
#include "Unit/Manager.h"

#include "ParseHelper.h"
#include "ParameterContainer.h"
#include "ParameterAirspeed.h"
#include "XMLTags.h"
#include "MathEx.h"
#include "CommonDefsQt.h"

#include "ScaleRound.h"

namespace instrument {

// -------------------------------------------------------------------------

ScaleRound::ScaleRound(const QDomElement& el)
: Scale(el)
{
	m_bBoundScale = true;
	
	m_iArcStart = qRound(ParseAttribute(el,TAG_ARC_START).toFloat()*16);	
	m_iArcLen   = qRound(ParseAttribute(el,TAG_ARC_LEN).toFloat()*16); 
			
	m_iFontSize = el.attribute(TAG_SIZE).toInt();
	if(m_iFontSize == 0)
		m_iFontSize = 18; // TODO GR_SCALE_FONT_SIZE_PX;
		
	m_iColdNeedle  = el.attribute(TAG_COLD).toInt()-1;
	m_iHotNeedle   = el.attribute(TAG_HOT).toInt()-1;
	m_bHotColdNeedle = (m_iColdNeedle>=0) || (m_iHotNeedle >=0);
}		
// --------------------------------------------------------------------------

ScaleRound::ScaleRound(
	int iStart, 
	int iArcLen, 
	float fS, 
	float fE
)
: Scale(fS,fE)
{
	m_iArcStart = iStart*16;
	m_iArcLen	= iArcLen*16;
		
	m_iFontSize = 18; // TODO GR_SCALE_FONT_SIZE_PX;
	
	m_iR = 0;
	
	m_iColdNeedle = -1;
	m_iHotNeedle  = -1;
	m_bHotColdNeedle = false;
}

// --------------------------------------------------------------------------

ScaleRound::~ScaleRound()
{
}

// --------------------------------------------------------------------------

void ScaleRound::Draw(QPainter& P, int iS)
{
	const int iW = P.device()->width();
	const int iH = P.device()->height();
	
	m_iR = 0.45*iS;
	m_ptC = QPoint(iW/2, iH/2);
	
	DrawScale(P);
	DrawDecoration(P);
}

// --------------------------------------------------------------------------

void ScaleRound::DrawScale(QPainter& P)
{
	// Draw minor ticks
	QPen pen(Qt::white, 1);
	pen.setCapStyle(Qt::RoundCap);
	P.setPen(pen);

	// Minor ticks are drown using real because the m_iMajorTickStep
	// may not be a multiplier of m_iMinorIntervalCount.
	const float df = m_fMajorTickStep/m_iMinorIntervalCount;
	for(float f=m_fS; f<=m_fE+EPS; f+=df) {
		DrawTickScale(P, f, m_iR, -MINOR_TICK_SIZE*m_iR/100);
	}

	// Draw major ticks
	pen.setWidthF(2.5f);
	P.setPen(pen);
	for(float f=m_fS; f<=m_fE+EPS; f+=m_fMajorTickStep) {
		DrawTickScale(P, f, m_iR, -MAJOR_TICK_SIZE*m_iR/100);
	}
	
	// Draw labels
	if(m_iFontSize > 0) {
		QFont font(GR_FONT, m_iFontSize, QFont::Bold);
		font.setPixelSize(m_iFontSize);
		P.setFont(font);
		
		// if scale arc is full circle, omit the last label.
		float fEnd = m_fE + EPS;
		if(fabs(m_iArcLen) >= 360*16)
			fEnd -= m_fLabelStep;
		for(float f=m_fS; f<=fEnd; f+=m_fLabelStep) {
			DrawLabelScale(P, f, 80*m_iR/100, common::Float2String(f, m_iDecimals));
		}
	}
}

// --------------------------------------------------------------------------

void ScaleRound::DrawDecoration(QPainter& P)
{
	// Draw yellow arc
	QPen pen(QColor(IB_YELLOW), 4);
	pen.setCapStyle(Qt::SquareCap);
	P.setPen(pen);
	
	QPair<float,float> R;
	R = m_pPar->GetYellowRange();
	if(m_bYellow && !IsNull(R)) {
		DrawArcScale(P, ConvertToScale(R), m_iR+2);
	}
		
	// Draw green arc
	pen.setColor(IB_GREEN);
	P.setPen(pen);
	
	R = m_pPar->GetGreenRange();
	if(m_bGreen && !IsNull(R)) {
		DrawArcScale(P, ConvertToScale(R), m_iR+2);
	}
		
	pen.setColor(IB_RED);
	P.setPen(pen);
		
	// Draw hi-limit mark
	R = m_pPar->GetLimits();
	if(m_eHighLimit != ltNone) {
		if(m_eHighLimit == ltTick)
			DrawTickScale(P, ConvertToScale(R.second), m_iR, -m_iR/10);
		else {
			R.first = ConvertToScale(R.second);
			R.second = m_fE;
			DrawArcScale(P, R, m_iR+2);
		}
	} 
	// Draw low-limit mark
	if(m_bLowLimit) {
		DrawTickScale(P, ConvertToScale(R.first), m_iR, -m_iR/10);
	}
}

// --------------------------------------------------------------------------

void ScaleRound::DrawTickScale(
	QPainter& P, 
	float fX, 
	int m_iR, 
	int iL
) const
{
	DrawTick(P,GetAngleRad(fX),m_iR,iL);
}

// --------------------------------------------------------------------------

void ScaleRound::DrawTick(
	QPainter& P,
	float fAngleRad,
	int m_iR,
	int iL
) const
{
	float fSA;
	float fCA;
// XXX:
//	sincosf(fAngleRad, &fSA, &fCA);

	// Start coordinate
	QPointF pt1(m_ptC.x() + fCA*m_iR, m_ptC.y() - fSA*m_iR);
	// End coordinate
	m_iR += iL;
	QPointF pt2(m_ptC.x() + fCA*m_iR, m_ptC.y() - fSA*m_iR);

	// Draw the line
	P.drawLine(pt1, pt2);
}

// --------------------------------------------------------------------------

void ScaleRound::DrawLabelScale(
	QPainter& P, 
	float fX,	//! Value in scale units.
	int m_iR, 
	const QString& qs, 
	bool bInside
) const
{
	DrawLabel(P,GetAngleRad(fX), m_iR,qs,bInside);
}

// --------------------------------------------------------------------------
	
void ScaleRound::DrawLabel(
	QPainter& P, 
	float fAngleRad, 
	int m_iR,
	const QString& qs, 
	bool bInside
) const
{
	// Get font data
	QFontMetrics fm = P.fontMetrics();
	QRect rc = fm.tightBoundingRect(qs);
	int iW = rc.width();

	// Possible y direction correction due descending characters.
	int iZ = rc.top() + rc.height()/2;

	// internal radius
	int ir = 7*fm.averageCharWidth()/10;

	// cos and sin of real angle
	float fSA;
	float fCA;

// XXX:
//	sincosf(fAngleRad, &fSA, &fCA);

	// x - correction due to optimal position
	int iX = qRound((iW-2*ir)*fCA)/2;
	// y - correction due to optimal position
	int iY = qRound(rc.height()*fSA)/7;

	// Reference point
	QPoint pt(m_ptC.x() + qRound(fCA*(m_iR)) -(iW>>1) -1,
             m_ptC.y() - qRound(fSA*(m_iR)) - iZ );

	// Apply Correction
	if(bInside) {
		pt.rx() += -iX;
		pt.ry() += iY;
	}
	else {
		pt.rx() +=  iX;
		pt.ry() += -iY;
	}
	P.drawText(pt, qs);
//	P.drawRect(pt.x(), pt.y()-rc.height()+rc.bottom(), rc.width(), rc.height());
}
// --------------------------------------------------------------------------

void ScaleRound::DrawArcScale(
	QPainter& P, 	//!< The QPainter object.
	float fS, 		//!< Natural value (not angle) where arc starts.
	float fE,		//!< Natural value (not angle) where arc ends.
	int m_iR			//!< Radius of the arc.
) const
{
	int iS = GetAngleQt(fS);
	int iSpan = GetAngleQt(fE) - iS;

	P.drawArc(m_ptC.x()-m_iR, m_ptC.y()-m_iR, m_iR*2, m_iR*2, iS, iSpan);
}

// --------------------------------------------------------------------------

void ScaleRound::DrawCircleScale(
	QPainter& P,
	float fX,
	int m_iR,
	int ir
) const
{
	float fA = GetAngleRad(fX);
	DrawCircle(P, fA, m_iR, ir);
}

// -------------------------------------------------------------------------

void ScaleRound::DrawCircle(QPainter& P, float fAngleRad, int m_iR, int ir) const
{
	float fSA;
	float fCA;

// XXX:
//	sincosf(fAngleRad, &fSA, &fCA);

	// Start coordinate
	P.drawEllipse(m_ptC.x() + fCA*m_iR - ir, m_ptC.y() - fSA*m_iR -ir, 2*ir, 2*ir);
}

// -------------------------------------------------------------------------

void ScaleRound::DrawTriangleScale(
	QPainter& P,
	float fX,
	int m_iR,
	int iL
) const
{
	qreal fA = GetAngleRad(fX);
	DrawTriangle(P, fA, m_iR, iL);
}

// -------------------------------------------------------------------------

void ScaleRound::DrawTriangle(
	QPainter& P,
	float fAngleRad,
	int m_iR,
	int iL
) const
{
	P.save();

	float fSA;
	float fCA;

// XXX:
//	sincosf(fAngleRad, &fSA, &fCA);

	// Start coordinate
	P.translate(m_ptC.x() + fCA*m_iR, m_ptC.y() - fSA*m_iR);
	P.rotate(-fAngleRad*180/M_PI);

	QPolygonF pg;
	pg << QPointF(  0, -iL);
	pg << QPointF(-iL, 0  );
	pg << QPointF(  0, iL);
	
	P.drawConvexPolygon(pg);
	P.restore();
}

// -------------------------------------------------------------------------

void ScaleRound::DrawNeedle(QPainter& P)
{
	UpdateValue();
	
	for(int i=0; i<m_pPar->GetCount(); i++) {
		if(m_bHotColdNeedle) {
			if(i==m_iHotNeedle) {
				P.setBrush(QColor(IB_ORANGE));
				m_needle.SetType(Needle::tPlainLabel);
			}
			else if(i==m_iColdNeedle) {
				P.setBrush(QColor(IB_CYAN));
				m_needle.SetType(Needle::tPlainLabel);
			}
			else {
				m_needle.SetType(Needle::tPlain);
			}
		}
		m_needle.Draw(P, GetAngleDeg(m_vDump[i]));
	}
}

// ***************************************************************************

ScaleRoundAirspeed::ScaleRoundAirspeed(const QDomElement& el)
: ScaleRound(el)
{
	Q_ASSERT(m_pPar->GetName()==TAG_AIRSPEED);
	
	m_bBoundScale = false;
	
	// value on the scale at 12 o'clock.
	float f0 = -((90*16-m_iArcStart)*(m_fE-m_fS)/fabs(m_iArcLen)-m_fS);
	Q_ASSERT(f0 != 0.0);
	
	m_fC1 = (m_fS-f0)/m_fS;
	m_fC0 = f0;
	
	// The decision limit for scale
	m_fScaleLimit = unit::Manager::GetInstance()->Convert(
		m_fMultiplier*m_fS,
		m_pPar->GetUnitKeyUser(),
		m_pPar->GetUnitKeySystem()
	);
}

// --------------------------------------------------------------------------

void ScaleRoundAirspeed::DrawScale(QPainter& P)
{
	ScaleRound::DrawScale(P);
	DrawTick(P, M_PI_2, m_iR, -MAJOR_TICK_SIZE*m_iR/100);
	DrawTick(P, common::Rad((m_iArcStart + 90*16)/(2.0f*16)), m_iR, -MAJOR_TICK_SIZE*m_iR/100);
}

// --------------------------------------------------------------------------

void ScaleRoundAirspeed::DrawDecoration(QPainter& P)
{
	const ParameterAirspeed* pA = dynamic_cast<const ParameterAirspeed*>(m_pPar);
	Q_ASSERT(pA);
		
	// Draw Vy tick
	QPen pen(Qt::white, 4);
	if(pA->GetVy() > 0) {
		pen.setColor(QColor(IB_CYAN));
		P.setPen(pen);
		DrawTickScale(P, ConvertToScale(pA->GetVy()), m_iR+2, -MAJOR_TICK_SIZE*m_iR/100);
	}
	
	// Draw VRef tick
	P.setPen(Qt::transparent);
	if(pA->GetVRef()>0) {
		P.setBrush(QColor(IB_YELLOW));
		DrawCircleScale(P, ScaleRound::ConvertToScale(pA->GetVRef()), m_iR-10, 4);
//		m_grp.DrawTriangleScale(P, m_V.GetVRef().GetAs(m_V.GetUnitKey()), m_m_iR, 7);
	}

	// Draw arcs
	P.setPen(QPen(Qt::white,4));
	if(IsNull(pA->GetWhiteRange())==false) {
		DrawArcScale(P, ScaleRound::ConvertToScale(pA->GetWhiteRange()), m_iR-1);
	}

	// Draw Flap pos 1 triangle
	if(pA->GetVFe1() > 0) {
		pen.setColor(Qt::transparent);
		P.setPen(pen);
		P.setBrush(QBrush(Qt::white));
		float fS = ConvertToScale(pA->GetVFe1());
		DrawTriangleScale(P, fS, m_iR+1, MAJOR_TICK_SIZE*m_iR/100);
	
		P.setBrush(QColor(IB_ORANGE));		
		DrawCircleScale(P, fS, m_iR+9, 3);
	}

	// Draw Flap pos 2 triangle
	if(pA->GetVFe2() > 0) {
		P.setBrush(QBrush(Qt::white));
		float fS = ConvertToScale(pA->GetVFe2());
		DrawTriangleScale(P, fS, m_iR+1, MAJOR_TICK_SIZE*m_iR/100);
	
		fS = GetAngleRad(fS);
		P.setBrush(QColor(IB_ORANGE));		
		DrawCircle(P, fS-2*M_PI/180, m_iR+9, 3);
		DrawCircle(P, fS+2*M_PI/180, m_iR+9, 3);
	}
	
	ScaleRound::DrawDecoration(P);
}
// --------------------------------------------------------------------------

float ScaleRoundAirspeed::ConvertToScale(float fS) const
{
	// Here we take care for the case when given fS is below the
	// scale limit.
	fS = ScaleRound::ConvertToScale(fS);
	if(fS < m_fS)
		fS = m_fC0 + m_fC1*fS;
	
	return fS;
/*	
	if(fS >= m_fScaleLimit)
		return ScaleRound::ConvertToScale(fS);
	else {
		fS = unit::Manager::GetInstance()->Convert(
			fS, 
			m_pPar->GetUnitKeySystem(),
			m_pPar->GetUnitKeyUser()
		) / m_fMultiplier;

		if(fS < m_fS)
			fS = m_fC0 + m_fC1*fS;
		return fS;
	}*/
}

// --------------------------------------------------------------------------
} // namespace
