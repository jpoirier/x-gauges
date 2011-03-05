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

#include "Parameter.h"

namespace instrument {

// --------------------------------------------------------------------------

Parameter::Parameter(const QDomElement& el)
{
	Initialize();

	m_qsName   = el.attribute(TAG_PARAM);
	int iCount = el.attribute(TAG_COUNT, "1").toInt();
	m_vValue.resize(iCount);

	// We need the system unit first. Not having a system-unit is
	// a fatal error. The system-unit is CANaerospace (Nesis) base unit.
	m_iKeySystem = ParseUnitKey(el, TAG_SYSTEM_UNIT);
	if(m_iKeySystem < 0)
		return;

	// Green, yellow, limits (read) and recomended range
	m_rGreen = ParseRange(el, TAG_GREEN, m_iKeySystem);
	m_rYellow = ParseRange(el, TAG_YELLOW, m_iKeySystem);
	m_rLimits = ParseRange(el, TAG_LIMITS, m_iKeySystem);
	m_rRange = ParseRange(el, TAG_RANGE, m_iKeySystem);

	QDomElement ef = el.firstChildElement(TAG_LIMITS);
	m_bLowLimit = m_bHighLimit = false;
	if(!ef.isNull()) {
		m_bLowLimit = (ef.attribute(TAG_LOW) != "");
		m_bHighLimit = (ef.attribute(TAG_HIGH) != "");
	}
	
	// Alarm settings
	ef = el.firstChildElement(TAG_ALARM);
	if(!ef.isNull()) {
		m_iAlarmTriggerDelay = ef.attribute(TAG_DELAY, "2").toInt();
		QString qsA = ef.attribute(TAG_STATUS);
		if(!qsA.isEmpty()) {
			m_eAlarmType = qsA == QLatin1String(TAG_USER) ? atUser :
								qsA == QLatin1String(TAG_DISABLED) ? atDisabled :
								atEnabled;
		}
	}
}

// --------------------------------------------------------------------------

Parameter::Parameter(const Parameter& C)
{
	Copy(C);
}

// --------------------------------------------------------------------------

Parameter::~Parameter()
{
}

// --------------------------------------------------------------------------

void Parameter::Initialize()
{
	m_rRange  = qMakePair((float)0,(float)0);
	m_rGreen  = qMakePair((float)0,(float)0);
	m_rYellow = qMakePair((float)0,(float)0);
	m_rLimits = qMakePair((float)0,(float)0);

	m_bLowLimit = m_bHighLimit = false;
	
	m_iAlarmTriggerDelay = 2; // two seconds by default 
	m_eAlarmType = atEnabled;
	
	m_iKeyUser   = -1;
	m_iKeySystem = -1;
}

// --------------------------------------------------------------------------

void Parameter::Copy(const Parameter& C)
{
	m_qsName						= C.m_qsName;

	m_iKeyUser 					= C.m_iKeyUser;
	m_iKeySystem				= C.m_iKeySystem;
	m_rRange						= C.m_rRange;
	m_rGreen						= C.m_rGreen;
	m_rYellow					= C.m_rYellow;
	m_rLimits					= C.m_rLimits;
	
	m_iAlarmTriggerDelay 	= C.m_iAlarmTriggerDelay; 
	m_eAlarmType 				= C.m_eAlarmType;

	m_Callback0					= C.m_Callback0;
	m_Callback1					= C.m_Callback1;

	m_bLowLimit 				= C.m_bLowLimit;
	m_bHighLimit 				= C.m_bHighLimit;

}
// --------------------------------------------------------------------------

QPair<float,float> Parameter::GetRange() const
{
	// If range was not given, calculate one from the limits.
	if(IsNull(m_rRange)) {
		QPair<float,float> r = m_rLimits;
		r.second *= 1.1;
		if(r.first != 0) {
			r.first = 0;
		}
		return r;
	}
	return m_rRange;
}

// --------------------------------------------------------------------------

bool Parameter::IsValid() const
{
	if(m_iKeySystem < 0) {
		qWarning() << "Parameter: Invalid system key" << m_iKeySystem;
		return false;
	}
	if(m_rGreen.first > m_rGreen.second) {
		qWarning() << "Parameter: Invalid green range [" 
					  << m_rGreen.first << "," 
					  << m_rGreen.second << "].";
		return false;
	}
	if(m_rYellow.first > m_rYellow.second) {
		qWarning() << "Parameter: Invalid yellow range [" 
					  << m_rYellow.first << "," 
					  << m_rYellow.second << "].";
		return false;
	}
	if(m_rLimits.first > m_rLimits.second && m_rLimits.second!=0) {
		qWarning() << "Parameter: Invalid limits (red) range [" 
					  << m_rLimits.first << "," 
					  << m_rLimits.second << "].";
		return false;
	}
	
	return true;
}

// --------------------------------------------------------------------------

void Parameter::UpdateValueCallback() const
{
	if(!m_Callback1.IsNull()) {
		const int iN = m_vValue.count();
		for(int i=0; i<iN; i++)
			m_vValue[i] = m_Callback1(i);
	}
	else {
		Q_ASSERT(m_vValue.count()==1 && m_Callback0.IsNull()==false);
		m_vValue[0] = m_Callback0();
	}
}

// --------------------------------------------------------------------------

float Parameter::GetValueUser(int i) const
{ 
	return unit::Manager::GetInstance()->Convert(m_vValue[i], m_iKeySystem, m_iKeyUser); 
}

// --------------------------------------------------------------------------
QColor Parameter::GetColor(float fS, QColor colDefault) const
{
	using namespace common;
	if(IsInRange(GetLimits(), fS)==false)
		return IB_RED;
	else if(IsInRange(GetGreenRange(), fS))
		return IB_GREEN;
	else {
		// If yellow range is not defined, this means yellow.
// 		if(IsNull(GetYellowRange()))
			return IB_YELLOW;
		// If yellow range is defined ...
/*		if(IsInRange(GetYellowRange(), fS))
			return IB_YELLOW;*/
	}
	return colDefault;
}

// --------------------------------------------------------------------------

} // namespace
