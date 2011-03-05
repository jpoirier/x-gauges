/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
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

#include "NesisAerospaceDataManager.h"
#include "WidgetMessage.h"
#include "Settings.h"

#include "Alarms.h"

// --------------------------------------------------------------------

Alarm::Alarm(
	const instrument::Parameter* pPar, 
	AlarmEnum eAlarm, 
	CheckType eCheckType,
	const QString& qsMsg
)
: m_eAlarm(eAlarm)
{
	m_pPar 			= pPar;
	Q_ASSERT(m_pPar);
	m_eCheckType   = eCheckType;
	m_qsPanicMsg   = qsMsg;
	m_eState 		= sClear;

	m_iPanicRepeatTime = 60;
	m_iPanicTime       = 20;
}


Alarm::~Alarm()
{
}
// --------------------------------------------------------------------

bool Alarm::Update()
{
	// Check the alarm conditions.
	bool bValid = IsValidCondition();
	
	// The checks were successful.
	if(bValid) {
		// Reset to clear status.
		if(m_eState != sClear) {
			m_eState = sClear;
			m_tmPanic = QTime();
		}
	}
	// The checks failed. Should we set the alarm?
	else {
		// Is this the first time? Go into waiting state.
		if(m_eState == sClear) {
			m_tmFirst.start();
			m_eState  = sWaiting;
		}
		else {
			// Check is we waited long enough.
			if(m_tmFirst.elapsed() >= GetTriggerTimeMS()) {
				m_eState = sPanic;
				return true;
			}
		}
	}
	return false;
}

// --------------------------------------------------------------------

void Alarm::Process() const
{
	if(m_eState == sPanic) {
		bool bShow = m_tmPanic.isNull();
		if(bShow == false) {
			if(m_tmPanic.elapsed() >= m_iPanicRepeatTime*1000)
				bShow = true;
		}
		
		if(bShow) {
			m_tmPanic = QTime::currentTime();
			WidgetMessage::GetInstance()->SetText(
				m_qsPanicMsg,
				m_iPanicTime,
				WidgetMessage::lCritical
			);
		}
	}
}

// --------------------------------------------------------------------

bool Alarm::IsActive() const
{
	Settings* pS = Settings::GetInstance();
	if(GetType() == instrument::Parameter::atEnabled)
		return true;
	else if(GetType() == instrument::Parameter::atUser) {
		// Check the settings.
		return pS->IsUserAlarmEnabled(GetAlarmEnum());
	}
	return false;
}

// --------------------------------------------------------------------

bool Alarm::IsValidCondition() const
{
	m_pPar->UpdateValueCallback();
	float f = m_pPar->GetValueSystem();
	const QPair<float,float>& L = m_pPar->GetLimits();

	if(m_eCheckType == ctHigh) {
		for(int i=1; i<m_pPar->GetCount(); i++)
			f = qMax(f, m_pPar->GetValueSystem(i));
		return f <= L.second;
	}
	else if(m_eCheckType == ctLow) {
		for(int i=1; i<m_pPar->GetCount(); i++)
			f = qMax(f, m_pPar->GetValueSystem(i));
		return f >= L.first;
	}
	else if(m_eCheckType == ctBoth) {
		float fL = f;
		float fH = f;
		for(int i=1; i<m_pPar->GetCount(); i++) {
			f = m_pPar->GetValueSystem(i);
			fL = qMin(fL, f);
			fH = qMax(fH, f);
		}
		return fH <= L.second && fL >= L.first;
	}
	else
		return true;
}

// --------------------------------------------------------------------

QString Alarm::GetPanicMsg() const
{
	if(m_eCheckType == ctBoth) {
		QString qs = m_qsPanicMsg + " ";
		float f = m_pPar->GetValueSystem();
		if(f > m_pPar->GetLimits().second)
			qs += QObject::tr("HIGH");
		else
			qs += QObject::tr("LOW");
		return qs;
	}
	else
		return m_qsPanicMsg;
}

// ***********************************************************************

AlarmStall::AlarmStall(
	const instrument::ParameterAirspeed* pPar, 
	const QString& qsMsg
)
: Alarm(pPar, aStall, ctOther, qsMsg)
{
	m_bActive = false;
	m_fHighLimit = pPar->GetVStall();
// 	qDebug() << "Stall limit" << pPar->GetVStall();
	m_fLowLimit  = 0.8f * m_fHighLimit;
/*	qDebug() << "Stall warning interval top, bottom" 
				<< m_fHighLimit*3.6f
				<< m_fLowLimit*3.6f << "km/h";*/
}

// --------------------------------------------------------------------

bool AlarmStall::IsValidCondition() const
{
	m_pPar->UpdateValueCallback();
	float fV = m_pPar->GetValueSystem();
	if(m_bActive==false) {
		if(fV > m_fHighLimit)
			m_bActive = true;
		return true;
	}
	// TODO How to prevent false stall warnings on landing, just before or after
	// the touchdown?
	return fV >= m_fHighLimit || fV <= m_fLowLimit;
}

// ***********************************************************************

AlarmCarburetor::AlarmCarburetor(
	const instrument::Parameter* pPar, 
	const instrument::Parameter* pParRPM,
	const QString& qsMsg
)
: AlarmEngineActive(pPar, pParRPM, aCarburetor, ctOther, qsMsg)
{
}

// --------------------------------------------------------------------

bool AlarmCarburetor::IsValidCondition() const
{
	if(m_pParRPM->GetValueSystem() < 400)
		return true;
		
	m_pPar->UpdateValueCallback();
	float f = m_pPar->GetValueSystem();
	QPair<float,float> L = m_pPar->GetYellowRange();
	return (f < L.first || f > L.second);
}

// ***********************************************************************

AlarmEngineActive::AlarmEngineActive(
	const instrument::Parameter* pPar, 
	const instrument::Parameter* pParRPM, 
	AlarmEnum eAlarm, 
	CheckType eCheckType, 
	const QString& qsMsg
)
: Alarm(pPar, eAlarm, eCheckType, qsMsg)
{
	m_pParRPM = pParRPM;
}

// --------------------------------------------------------------------

bool AlarmEngineActive::IsValidCondition() const
{
	if(m_pParRPM->GetValueSystem() < 400)
		return true;
	
	return Alarm::IsValidCondition();
}

// ***********************************************************************
