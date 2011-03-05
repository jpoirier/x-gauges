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
#include "Unit/Manager.h"
#include "XMLTags.h"

#include "XMLGaugeRoundAltitude.h"

namespace instrument {

// -------------------------------------------------------------------------

XMLGaugeRoundAltitude::XMLGaugeRoundAltitude(
	const QDomElement& e, 
	QWidget *pParent
)
: XMLGaugeRound(e, pParent)
{
	Q_ASSERT(m_vpScale.count() == 1);
	Q_ASSERT(m_vpScale[0]->GetParameter()->GetName()==TAG_ALTITUDE);
	
	unit::Manager* pM = unit::Manager::GetInstance();
	
	m_vpScale[0]->EnableBoundScale(false);
	
	m_bThreeNeedles = false;
	bool bThird = (e.attribute(TAG_THIRD_NEEDLE)==TAG_YES);
	if(bThird) {
		int iUserKey = m_vpScale[0]->GetParameter()->GetUnitKeyUser();
		if(iUserKey >= 0) {
			if(pM->GetUnit(iUserKey)->GetSignature()=="feet")
				m_bThreeNeedles = true;
		}
	}
	
	// At this time we know if we have QNH frame.
	// Set number of decimals ...
	for(int i=0; i<m_vpFrame.count(); i++) {
		if(m_vpFrame[i]->GetParameter()->GetName()==TAG_QNH) {
			// Check units
			int iUserKey = m_vpFrame[i]->GetParameter()->GetUnitKeyUser();
			if(iUserKey >= 0) {
				if(pM->GetUnit(iUserKey)->GetSignature()=="inHg") {
					m_vpFrame[i]->SetDecimals(1);
				}
			}
			break;
		}
	}

	m_vpScale[0]->GetNeedle().SetType(Needle::tPointed);
	m_needle3.SetType(Needle::tFatShort);
	m_needle4.SetType(Needle::t10000);
}

// -------------------------------------------------------------------------

void XMLGaugeRoundAltitude::DrawForeground(QPainter& P)
{
	XMLGaugeRound::DrawForeground(P);
	
	float fU = m_vpScale[0]->GetParameter()->GetValueUser();
	m_needle3.Draw(P, 90.0f - 36*fU/1000);
	if(m_bThreeNeedles)
		m_needle4.Draw(P, 90.0f - 36*fU/10000);
}

// -------------------------------------------------------------------------

void XMLGaugeRoundAltitude::ResizeBackground()
{
	XMLGaugeRound::ResizeBackground();
	
	const int iW   = width();
	const int iOff = 7*iW/100;
	m_vpScale[0]->GetNeedle().SetSize(QSize(40*iW/100, 5), iOff);
	m_needle3.SetSize(QSize(28*iW/100, 5*iW/100), iOff);
	m_needle4.SetSize(QSize(46*iW/100, 7*iW/100), iOff);
}

// -------------------------------------------------------------------------


} // namespace

