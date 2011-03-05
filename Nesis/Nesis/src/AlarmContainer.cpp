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
#include "ParameterContainer.h"
#include "XMLTags.h"

#include "AlarmContainer.h"

// -------------------------------------------------------------------------

//! The static instance.
std::auto_ptr<AlarmContainer> AlarmContainer::s_apInstance;

// -------------------------------------------------------------------------

AlarmContainer::AlarmContainer()
{
	using namespace instrument;
	ParameterContainer* pPC = ParameterContainer::GetInstance();
	Alarm* pAA;
	
	// VNE
	m_conAlarm << new Alarm(
		pPC->GetParameter(TAG_TAS), 
		Alarm::aVNE, 
		Alarm::ctHigh,
		QObject::tr("AIRSPEED HIGH")
	);
// 	qDebug() << "TAS lim=" << pPC->GetParameter(TAG_TAS)->GetLimits();
	

	// Stall
	pAA = new AlarmStall(
		dynamic_cast<const instrument::ParameterAirspeed*>(pPC->GetParameter(TAG_AIRSPEED)),
		QObject::tr("STALL   STALL   STALL")
	);
	pAA->SetPanicMsgTime(3);
	pAA->SetPanicRepeatTime(4);
	m_conAlarm << pAA;
	
	// RPM
	m_conAlarm << new Alarm(
		pPC->GetParameter(TAG_RPM), 
		Alarm::aRPM, 
		Alarm::ctHigh,
		QObject::tr("RPM HIGH")
	);
	
	// Oil pressure
	m_conAlarm << new AlarmEngineActive(
		pPC->GetParameter(TAG_OIL_PRESS),
		pPC->GetParameter(TAG_RPM),
		Alarm::aOilPressure,
		Alarm::ctBoth,
		QObject::tr("OIL PRESS.")
	);

	// CHT
	m_conAlarm << new AlarmEngineActive(
		pPC->GetParameter(TAG_CHT), 
		pPC->GetParameter(TAG_RPM),
		Alarm::aCHT, 
		Alarm::ctHigh,
		QObject::tr("CHT HIGH")
	);
	
	// EGT
	m_conAlarm << new AlarmEngineActive(
		pPC->GetParameter(TAG_EGT), 
		pPC->GetParameter(TAG_RPM),
		Alarm::aEGT, 
		Alarm::ctHigh,
		QObject::tr("EGT HIGH")
	);
	 
	// Oil temperature
	m_conAlarm << new Alarm(
		pPC->GetParameter(TAG_OIL_TEMP), 
		Alarm::aOilTemperature, 
		Alarm::ctHigh,
		QObject::tr("OIL T. HIGH.")
	);

	// Water (coolant) temperature
	m_conAlarm << new Alarm(
		pPC->GetParameter(TAG_WATER_TEMP), 
		Alarm::aWaterTemperature, 
		Alarm::ctHigh,
		QObject::tr("WATER T. HIGH.")
	);

	// Fuel level
	m_conAlarm << new Alarm(
		pPC->GetParameter(TAG_FUEL_LEVEL), 
		Alarm::aFuelLevel, 
		Alarm::ctLow,
		QObject::tr("FUEL LOW")
	);

	// Fuel pressure
	m_conAlarm << new AlarmEngineActive(
		pPC->GetParameter(TAG_FUEL_PRESS),
		pPC->GetParameter(TAG_RPM),
		Alarm::aFuelPressure,
		Alarm::ctBoth,
		QObject::tr("FUEL PRESS.")
	);

	// Current (amperes)
	m_conAlarm << new Alarm(
		pPC->GetParameter(TAG_CURRENT), 
		Alarm::aCurrent, 
		Alarm::ctHigh,
		QObject::tr("CURRENT HIGH")
	);

	// Voltage
	m_conAlarm << new AlarmEngineActive(
		pPC->GetParameter(TAG_VOLTAGE), 
		pPC->GetParameter(TAG_RPM),
		Alarm::aVoltage, 
		Alarm::ctBoth,
		QObject::tr("VOLTAGE")
	);

	// Carburetor
 	m_conAlarm << new AlarmCarburetor(
		pPC->GetParameter(TAG_CARBURETOR), 
		pPC->GetParameter(TAG_RPM),
		QObject::tr("CARBURETOR ICING CHECK")
	);

	// Debug - print the list of alarms
	qDebug() << "List of alarms.";
	for(int i=0; i<m_conAlarm.count(); i++) {
		const Alarm* pA = m_conAlarm[i];
		const instrument::Parameter* pP = pA->GetParameter();
		qDebug() << "Alarm " 
					<< pA->GetAlarmEnum()
					<< pP->GetName()
					<< pP->GetLimits()
					<< pA->IsActive();
	}
	
	// Build the list of active alarms!
	UpdateActive();
	
/*	qDebug() << "List of ACTIVE alarms.";
	for(int i=0; i<m_conActive.count(); i++) {
		const Alarm* pA = m_conActive[i];
		const instrument::Parameter* pP = pA->GetParameter();
		qDebug() << "Alarm " 
					<< pA->GetAlarmEnum()
					<< pP->GetName();
	}*/
}

// -------------------------------------------------------------------------

AlarmContainer::~AlarmContainer()
{
	foreach(Alarm* pAA, m_conAlarm)
		delete pAA;
}

// -------------------------------------------------------------------------

AlarmContainer* AlarmContainer::GetInstance()
{
	if(!s_apInstance.get()) {
		s_apInstance = std::auto_ptr<AlarmContainer>(new AlarmContainer);
	}
	return s_apInstance.get();
}

// -------------------------------------------------------------------------

bool AlarmContainer::Update()
{
	bool bAlarm = false;
	for(Con::iterator a=m_conActive.begin(); a!=m_conActive.end(); a++) {
		if((*a)->Update())
			bAlarm = true;
	}
	return bAlarm;
}

// -------------------------------------------------------------------------

AlarmContainer::ConstCon AlarmContainer::GetPanic() const
{
	ConstCon conPanic;
	for(Con::const_iterator a=m_conActive.begin(); a!=m_conActive.end(); a++) {
		if((*a)->GetState() == Alarm::sPanic)
			conPanic << *a;
	}
	
	return conPanic;
}

// -------------------------------------------------------------------------

void AlarmContainer::Process()
{
	ConstCon conPanic = GetPanic();
	if(conPanic.isEmpty())
		return;
		
	// Only one panic message
	if(conPanic.count()==1) {
		conPanic.front()->Process();
	}
	// More panic messages. Limited to max 3.
	else {
		int iCount = 0;
		QString qs = "  ";
		foreach(const Alarm* pAA, conPanic) {
			if(++iCount > 3)
				break;
			qs += pAA->GetPanicMsg();
			qs += "  ";
		}
		WidgetMessage::GetInstance()->SetText(
			qs, 2,
			WidgetMessage::lCritical
		);
	}
}

// -------------------------------------------------------------------------

void AlarmContainer::UpdateActive()
{
	m_conActive.clear();
	foreach(Alarm* pAA, m_conAlarm) {
		if(pAA->IsActive())
			m_conActive << pAA;
	}
// 	qDebug() << m_conActive.count() << "alarms are active.";
}

// -------------------------------------------------------------------------

AlarmContainer::ConstCon AlarmContainer::GetAlarms(instrument::Parameter::AlarmType eAT) const
{
	ConstCon con;
	foreach(Alarm* pAA, m_conAlarm) {
		if(pAA->GetType() == eAT)
			con << pAA;
	}
	return con;
}

// -------------------------------------------------------------------------
