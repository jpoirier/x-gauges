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
#include <QTime>
#include "CommonDefsQt.h"
#include "ParameterContainer.h"
#include "XMLTags.h"
#include "Unit/Value.h"

#include "FuelCompGaugeRect.h"

#define OFFSET 1

namespace instrument {

// --------------------------------------------------------------------------

FuelCompGaugeRect::FuelCompGaugeRect(QWidget *pParent)
 : AbstractGauge(pParent),
 	m_barFuel(Bar::tBottomTop, 65.0f, -5.0f, 10.0f, 30.0f, false)
{
	ParameterContainer* pPC = ParameterContainer::GetInstance();
	
	m_pParRange		 = pPC->GetParameter(TAG_RANGE);
	m_pParLevel		 = pPC->GetParameter(TAG_FUEL_LEVEL);
	m_pParFuelFlow	 = pPC->GetParameter(TAG_FUEL_FLOW);
	m_pParAvgFlow	 = pPC->GetParameter(TAG_AVG_FUEL_FLOW);
	m_pParEndurance = pPC->GetParameter(TAG_ENDURANCE);
		
	Q_ASSERT(   m_pParRange 
				&& m_pParLevel 
				&& m_pParFuelFlow 
				&& m_pParAvgFlow 
				&& m_pParEndurance);
				
	m_barFuel.SetParameter(m_pParLevel);
}

FuelCompGaugeRect::~FuelCompGaugeRect()
{
}

// -------------------------------------------------------------------------

#define X_OFF	10
#define Y_OFF_1 18
#define Y_OFF_2 12
#define Y_LINE  19

#define FF_LOW_LIMIT			3.0

void FuelCompGaugeRect::DrawForeground(QPainter& P)
{
	using namespace common;
 	const int iW = width();
	const int iH = height();
//	qreal fV;

	QFont font(GR_FONT, GR_LABEL_BOX_TEXT_SIZE, QFont::Bold);
	font.setPixelSize(13);
	P.setPen(Qt::white);

	QString qs;
	
	// Current fuel flow
	qs = "-";
	m_pParFuelFlow->UpdateValueCallback();
	if(m_pParFuelFlow->GetValueSystem() > FF_LOW_LIMIT)
		qs = GetString(m_pParFuelFlow,1);
	P.drawText(m_ix1+4, Y_OFF_1+Y_LINE, qs);

	// Range
	qs = "-";
	m_pParRange->UpdateValueCallback();
	if (m_pParRange->GetValueSystem() >= 0.0)
		qs = GetString(m_pParRange,0);
	P.drawText(m_ix2+4, Y_OFF_1+Y_LINE, qs);

	// Average fuel flow
	qs = "-";
	m_pParAvgFlow->UpdateValueCallback();
	if(m_pParAvgFlow->GetValueSystem() > FF_LOW_LIMIT)
		qs = GetString(m_pParAvgFlow,1);
	P.drawText(m_ix1+4, iH/2+Y_OFF_2+Y_LINE, qs);
	
	// Endurance
	qs = "-:-";
	m_pParEndurance->UpdateValueCallback();
	float fE = m_pParEndurance->GetValueSystem();
	if(fE >= 0.0) {
		QTime time;
		time = time.addSecs(fE);
		qs = time.toString("H:mm");
	}
	P.drawText(m_ix2+4, iH/2+Y_OFF_2+Y_LINE, qs);

	// Fuel bar
	font.setPixelSize(GR_LABEL_BOX_TEXT_SIZE);
	P.setFont(font);
	P.setPen(QPen(Qt::darkGray, 1.3));

	P.translate(iW/2, iH/2);
	P.setFont(QFont(GR_FONT,10));
	m_barFuel.Draw(P, iW);
}

// -------------------------------------------------------------------------

QPixmap FuelCompGaugeRect::CreateBackground(const QSize& szHint)
{
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

	// Draw background
	QRect rc(2,2, iW-4, iH-4);

	P.setPen(GR_GRAY);
	P.setBrush(QColor(GR_BLACK));
	P.drawRect(rc);

	const int iH_2 = iH/2;
	P.setPen(Qt::white);
	QFont font(GR_FONT, 10, QFont::Bold);
	font.setPixelSize(13);
	P.setFont(font);

	m_ix1 = X_OFF;
	P.drawText(m_ix1, Y_OFF_1, tr("Current"));
	P.drawText(m_ix1, Y_OFF_2+iH_2, tr("Average"));

	m_ix2 = 35*iW/100 + X_OFF;
	P.drawText(m_ix2, Y_OFF_1, tr("Range"));
	P.drawText(m_ix2, Y_OFF_2+iH_2, tr("Endur."));

	m_ix3 = 72*iW/100;
	P.drawText(m_ix3+5, Y_OFF_1, tr("Fuel"));
	m_ix4 = iW-X_OFF;

	return px;
}

// -------------------------------------------------------------------------

QString FuelCompGaugeRect::GetString(const Parameter* pP, int iDec) const
{
	return unit::Value(pP->GetUnitKeyUser(), pP->GetValueUser()).Format(iDec);
}

// -------------------------------------------------------------------------

} // namespace
