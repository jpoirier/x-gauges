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

#include  <QtXml>
#include "MathEx.h"
#include "CommonDefsQt.h"
#include "XMLTags.h"

#include "XMLGaugeRound.h"


namespace instrument {

// --------------------------------------------------------------------------

XMLGaugeRound::XMLGaugeRound(const QDomElement& el, QWidget *pParent)
 : AbstractGauge(pParent)
{
	// Parse the element and build internal structure.
	// Start with the scales.
	QDomElement se = el.firstChildElement(TAG_SCALE); 
	for(; !se.isNull(); se = se.nextSiblingElement(TAG_SCALE)) {
		ScaleRound* pS;
		if(se.attribute(TAG_PARAM) == TAG_AIRSPEED)
			pS = new ScaleRoundAirspeed(se);
		else
			pS = new ScaleRound(se);
		m_vpScale << pS;
	}

	// Labels
	se = el.firstChildElement(TAG_LABEL);
	for(; !se.isNull(); se = se.nextSiblingElement(TAG_LABEL)) {
		Label* pL = new Label(se);
		m_vpLabel << pL;
	}

	// Frames
	se = el.firstChildElement(TAG_FRAME);
	for(; !se.isNull(); se = se.nextSiblingElement(TAG_FRAME)) {
		Frame* pFrame = new Frame(se);
		m_vpFrame << pFrame;
	}
	
	// Parse Bars
	se = el.firstChildElement(TAG_BAR);
	for(; !se.isNull(); se = se.nextSiblingElement(TAG_BAR)) {
		Bar* pBar = new Bar(se);
		m_vpBar << pBar;
	}
}

XMLGaugeRound::~XMLGaugeRound()
{
	// Delete all but the first one.
	for(int i=0; i<m_vpScale.count(); i++)
		delete m_vpScale[i];
		
	for(int i=0; i<m_vpFrame.count(); i++)
		delete m_vpFrame[i];
		
	for(int i=0; i<m_vpBar.count(); i++)
		delete m_vpBar[i];
}

// --------------------------------------------------------------------------

void XMLGaugeRound::DrawForeground(QPainter& P)
{
	const int iW = width();
	// Setup standard pen, (no) brush and rendering.
	AbstractGauge::DrawForeground(P);
	
	// Draw frames if any.
	if(!m_vpFrame.isEmpty()) {
		QVector<Frame*>::const_iterator fi;
		for(fi=m_vpFrame.begin(); fi!=m_vpFrame.end(); fi++)
			(*fi)->Draw(P, iW);
	}
	
	// Draw bars if any.
	if(!m_vpBar.isEmpty()) {
		QVector<Bar*>::const_iterator bi;
		for(bi=m_vpBar.begin(); bi!=m_vpBar.end(); bi++)
			(*bi)->Draw(P, iW);
	}
	
	// Scales
	QVector<ScaleRound*>::const_iterator si;
	for(si=m_vpScale.begin(); si!=m_vpScale.end(); si++) {
		(*si)->DrawNeedle(P);
	}
}

// --------------------------------------------------------------------------

QPixmap XMLGaugeRound::CreateBackground(const QSize& /*szHint*/)
{
	// Load base image
	QPixmap px(m_qsBackground);
	const int iW = px.width();
	const int iH = px.height();
	QPainter P(&px);
	P.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
	
	// Scales
	for(int i=0; i<m_vpScale.count(); i++) {
		ScaleRound* pS = m_vpScale[i];
		pS->Draw(P, iW);
	}
	
	// Draw labels ...
	P.setPen(Qt::white);
	P.translate(iW/2, iH/2);
	
	for(int i=0; i<m_vpLabel.count(); i++) {
		Label* pL = m_vpLabel[i];
		pL->Draw(P,iW);
	}

	return px;
}
// --------------------------------------------------------------------------

void XMLGaugeRound::ResizeBackground()
{
	AbstractGauge::ResizeBackground();
	// We must do something with the needle objects.
	QVector<ScaleRound*>::const_iterator si;
	for(si=m_vpScale.begin(); si!=m_vpScale.end(); si++) {
		(*si)->GetNeedle().SetSize(QSize(38*width()/100,4), 7*width()/100);
	}
}
// --------------------------------------------------------------------------

}
