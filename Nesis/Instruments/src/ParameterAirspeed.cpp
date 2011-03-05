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
#include <QtXml>
#include "Unit/Manager.h"
#include "CommonDefsQt.h"
#include "ParseHelper.h"
#include "XMLTags.h"

#include "ParameterAirspeed.h"

namespace instrument {

// --------------------------------------------------------------------------

ParameterAirspeed::ParameterAirspeed(const QDomElement& el)
: Parameter(el)
{
	// Initialize
	m_fVy = m_fVRef = m_fVFe1 = m_fVFe2 = m_fVSW = 0.0f;
	
	// White
	m_rWhite = ParseRange(el, TAG_WHITE, m_iKeySystem);

	// velocities
	QDomElement ef = el.firstChildElement(TAG_VELOCITY);
	if(!ef.isNull()) {
		int iKey = ParseUnitKey(ef, TAG_UNIT);
		m_fVy   = ef.attribute(TAG_VY,   "0").toFloat();
		m_fVRef = ef.attribute(TAG_VREF, "0").toFloat();
		m_fVFe1 = ef.attribute(TAG_VFE1, "0").toFloat();
		m_fVFe2 = ef.attribute(TAG_VFE2, "0").toFloat();
		m_fVSW  = ef.attribute(TAG_VSW,  "0").toFloat();
// 		qDebug() << "VSW=" << m_fVSW;
		
		unit::Manager* pM = unit::Manager::GetInstance();
		m_fVy   = pM->Convert(m_fVy, iKey, m_iKeySystem);
		m_fVRef = pM->Convert(m_fVRef, iKey, m_iKeySystem);
		m_fVFe1 = pM->Convert(m_fVFe1, iKey, m_iKeySystem);
		m_fVFe2 = pM->Convert(m_fVFe2, iKey, m_iKeySystem);
		m_fVSW  = pM->Convert(m_fVSW, iKey, m_iKeySystem);
// 		qDebug() << "VSW=" << m_fVSW;
	}
}

// --------------------------------------------------------------------------

ParameterAirspeed::ParameterAirspeed(const ParameterAirspeed& C)
: Parameter(C)
{
	m_rWhite	= C.m_rWhite;
	m_fVy 	= C.m_fVy;
	m_fVRef 	= C.m_fVRef;
	m_fVFe1 	= C.m_fVFe1;
	m_fVFe2  = C.m_fVFe2;
	m_fVSW   = C.m_fVSW;
}

// --------------------------------------------------------------------------

ParameterAirspeed::~ParameterAirspeed()
{
}

// --------------------------------------------------------------------------

void ParameterAirspeed::Copy(const ParameterAirspeed& C)
{
	Parameter::Copy(C);

	m_rWhite	= C.m_rWhite;
	m_fVy 	= C.m_fVy;
	m_fVRef 	= C.m_fVRef;
	m_fVFe1 	= C.m_fVFe1;
	m_fVFe2  = C.m_fVFe2;
	m_fVSW   = C.m_fVSW;
}
// --------------------------------------------------------------------------

bool ParameterAirspeed::IsValid() const
{
	if(!Parameter::IsValid()) {
		return false;
	}

	if(m_rWhite.first > m_rWhite.second) {
		qWarning() << "ParameterAirspeed: Invalid white range [" 
					  << m_rWhite.first << "," 
					  << m_rWhite.second << "].";
		return false;
	}
	return true;
}

// --------------------------------------------------------------------------

} // namespace
