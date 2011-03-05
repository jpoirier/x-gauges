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
#include <QPainter>
#include "CommonDefsQt.h"
#include "ParameterContainer.h"
#include "Frame.h"
#include "XMLTags.h"

#include "EngineMiniGaugeRect.h"

#define BAR_WIDTH		7
#define MED_SKIP		11
#define SMALL_SKIP	2


namespace instrument {

// -------------------------------------------------------------------------

EngineMiniGaugeRect::EngineMiniGaugeRect(
	const QDomElement& el,
	QWidget *pParent
)
 : AbstractGauge(pParent)
{
	Q_ASSERT(el.tagName()==QLatin1String(TAG_ENGINE_MINI));
	
	m_bOilT 		= el.attribute(TAG_OIL_TEMP, TAG_YES) == QLatin1String(TAG_YES);
	m_bWaterT	= el.attribute(TAG_WATER_TEMP, TAG_YES) == QLatin1String(TAG_YES);
	m_bCarbT		= el.attribute(TAG_CARBURETOR, TAG_YES) == QLatin1String(TAG_YES);
	m_bOilP		= el.attribute(TAG_OIL_PRESS, TAG_YES) == QLatin1String(TAG_YES);
	m_bFuelP		= el.attribute(TAG_FUEL_PRESS, TAG_YES) == QLatin1String(TAG_YES);
	
	ParameterContainer* pPC = ParameterContainer::GetInstance();
	m_pParCHT = pPC->GetParameter(TAG_CHT);
	m_pParEGT = pPC->GetParameter(TAG_EGT);
	m_pParOT  = pPC->GetParameter(TAG_OIL_TEMP);
	m_pParWT	 = pPC->GetParameter(TAG_WATER_TEMP);
	m_pParCarbT = pPC->GetParameter(TAG_CARBURETOR);
	
	m_pParOP	= pPC->GetParameter(TAG_OIL_PRESS);
	m_pParFP = pPC->GetParameter(TAG_FUEL_PRESS);
	
	m_pParRPM = pPC->GetParameter(TAG_RPM);
	
	Q_ASSERT(m_pParCHT 
			&& m_pParEGT
			&& m_pParOT
			&& m_pParWT
			&& m_pParCarbT
			&& m_pParOP
			&& m_pParFP
			&& m_pParRPM);
}

EngineMiniGaugeRect::~EngineMiniGaugeRect()
{
}

// --------------------------------------------------------------------------

void EngineMiniGaugeRect::DrawForeground(QPainter& P)
{
//	using namespace unit;
//	const int iW = width();
	const int iH = height();
	const int iTop = 26;
	
	QRect rc(0,0, BAR_WIDTH, iH-50);
	
	bool bActive = m_pParRPM->GetValueSystem() > 400;
	if(!bActive)
		return;
	
	// CHT
	P.translate(m_ixCht1, iTop);
	m_pParCHT->UpdateValueCallback();
	for(int i=0; i<m_pParCHT->GetCount(); i++) {
 		float fS = m_pParCHT->GetValueSystem(i);
		Bar::DrawBar(P, m_pParCHT, Bar::tBottomTop, rc, fS, 255);
		P.translate(BAR_WIDTH + SMALL_SKIP, 0);
	}
	
	// EGT
	P.resetMatrix();
	P.translate(m_ixEgt1, iTop);
	m_pParEGT->UpdateValueCallback();
	for(int i=0; i<m_pParEGT->GetCount(); i++) {
 		float fS = m_pParEGT->GetValueSystem(i);
		Bar::DrawBar(P, m_pParEGT, Bar::tBottomTop, rc, fS, 255);
		P.translate(BAR_WIDTH + SMALL_SKIP, 0);
	}
	
	// Oil temp
	if(m_bOilT) {
		P.resetMatrix();
		P.translate(m_ixOilT1, iTop);
		m_pParOT->UpdateValueCallback();
		Bar::DrawBar(P, m_pParOT, Bar::tBottomTop, rc, m_pParOT->GetValueSystem(), 255);
	}
	
	// Water temp
	if(m_bWaterT) {
		P.resetMatrix();
		P.translate(m_ixWaterT1, iTop);
		m_pParWT->UpdateValueCallback();
		Bar::DrawBar(P, m_pParWT, Bar::tBottomTop, rc, m_pParWT->GetValueSystem(), 255);
	}	
	
	// Carburetor temp
	if(m_bCarbT) {
		P.resetMatrix();
		P.translate(m_ixCarbT1, iTop);
		m_pParCarbT->UpdateValueCallback();
		// TODO Fix this
		QPair<float,float> ffL = m_pParCarbT->GetLimits();
		float fC = m_pParCarbT->GetValueSystem();
		QBrush oldBrush = P.brush();
		if(fC >=ffL.first || fC<=ffL.second)
			P.setBrush(QBrush(IB_YELLOW));
		else
			P.setBrush(QBrush(IB_GREEN));
		P.drawRect(rc); 
		P.setBrush(oldBrush);
	}
	
	// Oil pressure
	if(m_bOilP) {
		P.resetMatrix();
		P.translate(m_ixOilP1, iTop);
		m_pParOP->UpdateValueCallback();
		Bar::DrawBar(P, m_pParOP, Bar::tBottomTop, rc, m_pParOP->GetValueSystem(), 255);
	}
	
	// Fuel pressure
	if(m_bFuelP) {
		P.resetMatrix();
		P.translate(m_ixFuelP1, iTop);
		m_pParFP->UpdateValueCallback();
		Bar::DrawBar(P, m_pParFP, Bar::tBottomTop, rc, m_pParFP->GetValueSystem(), 255);
	}
}

// -------------------------------------------------------------------------

QPixmap EngineMiniGaugeRect::CreateBackground(const QSize& szHint)
{
	const int iBorder = 2;
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
	
	QRect rc(iBorder,iBorder, iW-2*iBorder, iH-2*iBorder);
	P.setPen(GR_GRAY);
	P.setBrush(QColor(GR_BLACK));
	P.drawRect(rc);
	
	// Calculate number of bars
	int nB = m_pParEGT->GetCount() + m_pParCHT->GetCount();
	if(m_bOilT) nB++;
	if(m_bWaterT) nB++;
	if(m_bCarbT) nB++;
	if(m_bOilP) nB++;
	if(m_bFuelP) nB++;
	
	// Number of small skips
	int nSS = 0;
	if(m_pParCHT->GetCount() > 1)
		nSS += m_pParCHT->GetCount()-1;
	if(m_pParEGT->GetCount() > 1)
		nSS += m_pParEGT->GetCount()-1;
	
	// Number of temperature groups
	int nTG = 2; // EGT + CHT
	if(m_bOilT)   nTG++;
	if(m_bWaterT) nTG++;
	if(m_bCarbT)  nTG++;
	
	// Number of pressure groups
	int nPG = 0;
	if(m_bOilP)  nPG++;
	if(m_bFuelP) nPG++;
	
	// Number of medium skips
	int nMS = nTG-1 + nPG-1;
	
	// Number of big skips
	int nBS = (nTG>0 && nPG>0) ? 1 : 0;
	
	// Calculate free width space, big skip and margins.
	int iFreeW   = iW - 2*iBorder - nB*BAR_WIDTH - nSS*SMALL_SKIP - nMS*MED_SKIP;
	int iBigSkip = nBS > 0 ? iFreeW/2 : 0;
	int iMargin  = (iFreeW-iBigSkip)/2;
	
	// Distribute x coordinates.
	int ix;
	
	// CHT columns
	ix = m_ixCht1 = m_ixCht2 = iMargin + iBorder;
	m_ixCht2 += m_pParCHT->GetCount()*BAR_WIDTH + (m_pParCHT->GetCount()-1)*SMALL_SKIP;
	ix = m_ixCht2 + MED_SKIP;
		
	// EGT columns
	m_ixEgt1 = m_ixEgt2 = ix;
	m_ixEgt2 += m_pParEGT->GetCount()*BAR_WIDTH + (m_pParEGT->GetCount()-1)*SMALL_SKIP;
	ix = m_ixEgt2 + MED_SKIP;
	
	// Oil temperature
	m_ixOilT1 = m_ixOilT2 = ix;
	if(m_bOilT) {
		m_ixOilT2 += BAR_WIDTH;
		ix = m_ixOilT2 + MED_SKIP;
	}
	
	// Water temperature
	m_ixWaterT1 = m_ixWaterT2 = ix;
	if(m_bWaterT) {
		m_ixWaterT2 += BAR_WIDTH;
		ix = m_ixWaterT2 + MED_SKIP;
	}
	
	// Carburetor temperature
	m_ixCarbT1 = m_ixCarbT2 = ix;
	if(m_bCarbT) {
		m_ixCarbT2 += BAR_WIDTH;
		ix = m_ixCarbT2 + MED_SKIP;
	}
	
	// Big skip - reset to pressure position.
	// We have one MED_SKIP too much.
	ix += nBS*iBigSkip - MED_SKIP;
	
	// Oil pressure
	m_ixOilP1 = m_ixOilP2 = ix;
	if(m_bOilP) {
		m_ixOilP2 += BAR_WIDTH;
		ix = m_ixOilP2 + MED_SKIP;
	}
	
	// Fuel pressure
	m_ixFuelP1 = m_ixFuelP2 = ix;
	if(m_bFuelP) {
		m_ixFuelP2 += BAR_WIDTH;
		ix = m_ixFuelP2 + MED_SKIP;
	}
	
	// Draw the text
	QFont font(GR_FONT, 10);
	font.setPixelSize(11);
	P.setFont(font);
	
	P.setPen(Qt::white);
	
	if(nTG > 0)
		DrawLabel(P, (m_ixCht1+m_ixCarbT2)/2, 5,  tr("TEMPERATURE"), 100);
	if(nPG > 0)
		DrawLabel(P, (m_ixOilP1+m_ixFuelP2)/2,5, tr("PRESS."), 60);
	
	int iy = iH-24;
	
	DrawLabel(P, (m_ixCht1+m_ixCht2)/2, iy, "CHT");
	DrawLabel(P, (m_ixEgt1+m_ixEgt2)/2, iy, "EGT");
	if(m_bOilT)
		DrawLabel(P, (m_ixOilT1+m_ixOilT2)/2, iy, "O.");
	if(m_bWaterT)
		DrawLabel(P, (m_ixWaterT1+m_ixWaterT2)/2, iy, "W.");
	if(m_bCarbT)
		DrawLabel(P, (m_ixCarbT1+m_ixCarbT2)/2, iy, "C.");
	if(m_bOilP)
		DrawLabel(P, (m_ixOilP1+m_ixOilP2)/2, iy, "O.");
	if(m_bFuelP)
		DrawLabel(P, (m_ixFuelP1+m_ixFuelP2)/2, iy, "F.");
	
	return px;
}

// -------------------------------------------------------------------------
	
void EngineMiniGaugeRect::DrawLabel(
	QPainter& P, 
	int iCx, 
	int iCy, 
	const QString& qs, 
	int iW, 
	int iH)
{
	P.drawText(QRect(iCx-iW/2, iCy, iW, iH), Qt::AlignCenter, qs);
}

// -------------------------------------------------------------------------
} // namespace
