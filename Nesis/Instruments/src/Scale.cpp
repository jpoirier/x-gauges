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

#include <QtXml>
#include <QPainter>
#include "Unit/Manager.h"

#include "ParseHelper.h"
#include "ParameterContainer.h"
#include "XMLTags.h"
#include "MathEx.h"
#include "CommonDefsQt.h"

#include "Scale.h"

namespace instrument {

// -------------------------------------------------------------------------

Scale::Scale(const QDomElement& el)
{
	m_bBoundScale = true;
	
	Q_ASSERT(el.tagName() == TAG_SCALE);
	QString qsA = ParseAttribute(el, TAG_PARAM);
	m_pPar = ParameterContainer::GetInstance()->GetParameter(qsA);
	if(m_pPar == NULL) {
		qCritical() << "Scale parameter" << qsA << "does not exist in ParameterContainer.";
	} 
		
	m_bYellow		= el.attribute(TAG_YELLOW)!=QLatin1String(TAG_NO);
	m_bGreen			= el.attribute(TAG_GREEN)!=QLatin1String(TAG_NO);
	m_bLowLimit		= el.attribute(TAG_LO_LIMIT)!=QLatin1String(TAG_NO);
	qsA = el.attribute(TAG_HI_LIMIT);
	m_eHighLimit	= qsA==QLatin1String(TAG_NO) ? ltNone : 
						  qsA==QLatin1String(TAG_ARC) ? ltArc : ltTick;
						  
	m_fDumping     = el.attribute(TAG_DUMPING, "0").toFloat();
	m_vDump.resize(m_pPar->GetCount());
	
	// Scale type
	Q_ASSERT(m_pPar->GetUnitKeyUser() >= 0);
	bool bFound = false;
	QDomElement et = el.firstChildElement(TAG_TYPE);
	for(; !et.isNull(); et = et.nextSiblingElement(TAG_TYPE)) {
		// We are only interested in user selected units
		bFound = (ParseUnitKey(et, TAG_UNIT) == m_pPar->GetUnitKeyUser());
		if(bFound) {
			m_fS = ParseAttribute(et, TAG_LOW).toFloat();
			m_fE = ParseAttribute(et, TAG_HIGH).toFloat();
	
			m_fMajorTickStep      = ParseAttribute(et, TAG_MAJOR).toFloat();
			m_iMinorIntervalCount = ParseAttribute(et, TAG_MINOR).toInt();
			m_fLabelStep          = ParseAttribute(et, TAG_LABEL).toFloat();
			m_fMultiplier         = et.attribute(TAG_MULT,"1").toFloat();
			m_iDecimals           = el.attribute(TAG_DECIMALS).toInt();
			
			break;
		}
	}
}		
// --------------------------------------------------------------------------

Scale::Scale(
	float fS, 
	float fE
)
{
	m_fS = fS;
	m_fE = fE;
		
	m_pPar = NULL;
	
	m_bYellow = m_bGreen = m_bLowLimit = false;
	m_eHighLimit = ltNone;
	m_iDecimals = 0;
	
	m_fMajorTickStep = 1;
	m_iMinorIntervalCount = 5;
	m_fLabelStep = 1;
	
	m_fMultiplier = 1;
	m_bBoundScale = true;
	
	m_fDumping = 0.0;
}

// --------------------------------------------------------------------------

Scale::~Scale()
{
}

// --------------------------------------------------------------------------

void Scale::SetScaleLimits(float fS, float fE)
{
	m_fS = fS;
	m_fE = fE;
}

// --------------------------------------------------------------------------

float Scale::ConvertToScale(float f) const
{
	f = unit::Manager::GetInstance()->Convert(
		f, 
		m_pPar->GetUnitKeySystem(),
		m_pPar->GetUnitKeyUser()
	) / m_fMultiplier;
	
	if(m_bBoundScale)
		f = qBound(m_fS, f, m_fE);
	
	return f;
}

// --------------------------------------------------------------------------
	
QPair<float,float> Scale::ConvertToScale(const QPair<float,float>& P)
{
	return qMakePair(
		ConvertToScale(P.first),
		ConvertToScale(P.second)
	);
}

// --------------------------------------------------------------------------

void Scale::UpdateValue()
{
	m_pPar->UpdateValueCallback();
	
	for(int i=0; i<m_pPar->GetCount(); i++) {
		float f = ConvertToScale(m_pPar->GetValueSystem(i));
		m_vDump[i] = m_vDump[i]*m_fDumping + f*(1.0f-m_fDumping);
	}
}

// --------------------------------------------------------------------------

} // namespace
