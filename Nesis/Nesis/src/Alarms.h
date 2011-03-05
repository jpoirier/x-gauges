#ifndef ALARMS_H
#define ALARMS_H

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

#include "ParameterAirspeed.h"

#include <QTime>
#include "Parameter.h"

// -------------------------------------------------------------------------
/** \brief Base class for alarms used in nesis.
 */

class Alarm 
{
public:
	enum State {
		sClear,	 //!< Alarm conditions are not present.
		sWaiting, //!< Alarm conditions are present, but we wait for a while.
		sPanic	 //!< Alarm must be set.
	};
	
	enum AlarmEnum {
		aCHT,						//!< High CHT alarm.
		aEGT,						//!< High EGT alarm.
		aFuelLevel,				//!< Low fuel level alarm.
		aFuelPressure,			//!< High/Low fuel pressure alarm.
		aOilPressure,			//!< High/Low oil pressure alarm.
		aOilTemperature, 		//!< High oil temperature alarm.
		aRPM,						//!< High RPM alarm.
		aStall,					//!< Stall warning alarm.
		aVNE,						//!< Max velocity alarm.
		aWaterTemperature,	//!< High water (coolant) temperature alarm.
		aCurrent,				//!< High current alarm.
		aVoltage,				//!< High/Low voltage alarm.
		aCarburetor,			//!< Carburetor temperature alarm.
		aEnd						//!< Not an alarm, just end of enum marker.
	};

	enum CheckType {
		ctLow,			//!< Check low limit
		ctHigh,			//!< Check high limit
		ctBoth,			//!< Check both limits
		ctOther			//!< Some other check.
	};
	
public:
	//! Constructor, protected.
	Alarm(const instrument::Parameter* pPar, 
		AlarmEnum eAlarm, CheckType eCheckType, const QString& qsMsg);
	//! Destructor.
	virtual ~Alarm();
	
	//! Set the panic message time [s].
	void SetPanicMsgTime(int iT)
	{ m_iPanicTime = iT; }
	//! Set the panic condition repeat time [s].
	void SetPanicRepeatTime(int iT)
	{ m_iPanicRepeatTime = iT; }
	
	//! Update the alarm state. Return true, if alarm condition is met.
	virtual bool Update();
	
	//! Process the alarm. Shows message by default.
	virtual void Process() const;
	
	//! Return current state of the alarm.
	State GetState() const
	{ return m_eState; }

	//! Return current type of the alarm.
	instrument::Parameter::AlarmType GetType() const
	{ return m_pPar->GetAlarmType(); }

	//! Return enum identifier of the alarm.
	AlarmEnum GetAlarmEnum() const
	{ return m_eAlarm; }

	//! Return true if the alarm is active.
	bool IsActive() const;

	//! Get trigger time [s]
	int GetTriggerTimeMS() const
	{ return m_pPar->GetAlarmTriggerDelay()*1000; }

	//! Construct the "panic" message string.
	virtual QString GetPanicMsg() const;
	
	//! Get current condition. Return true if current condition valid.
	virtual bool IsValidCondition() const;
	
	//! Get the parameter object.
	const instrument::Parameter* GetParameter() const
	{ return m_pPar; }
	
private:
	//! Timestamp of the first alarm occurence.
	QTime m_tmFirst;
	//! Current state of the alarm conditions.
	State m_eState;
	
	//! Time of the last panic message.
	mutable QTime m_tmPanic;
	//! Panic message duration in seconds.
	int m_iPanicTime;
	//! Time between the panic processing is repeated.
	int m_iPanicRepeatTime;
	
protected:
	//! Parameter that we monitor.
	const instrument::Parameter* m_pPar;
	//! Type of the alarm.
	const AlarmEnum m_eAlarm;
	//! Type of the check to perform.
	CheckType m_eCheckType;
	//! The alarm message.
	QString m_qsPanicMsg;
};

// -------------------------------------------------------------------------

/** \brief The stall speed alarm.

	The stall speed alarm implementation is a bit different.
 */

class AlarmStall : public Alarm
{
public:
	//! Constructor
	AlarmStall(const instrument::ParameterAirspeed* pPar, const QString& qsMsg);
	
	//! Get condition. Return true if condition is valid.
	virtual bool IsValidCondition() const;
	
private:
	//! Stall warning is active only when stall speed was crossed.
	mutable bool m_bActive;
	float m_fHighLimit;
	float m_fLowLimit;
};

// -------------------------------------------------------------------------

/** \brief Some alarms may require active engine.

	 The specific of oil pressure alarm is in that it is active only
	 if engine is running (some non-zero RPM is detected).
 */

class AlarmEngineActive : public Alarm
{
public:
	//! Constructor
	AlarmEngineActive(
		const instrument::Parameter* pPar, const instrument::Parameter* pParRPM,
		AlarmEnum eAlarm, CheckType eCheckType, const QString& qsMsg);
	
	//! Get condition. Return true if condition is valid.
	virtual bool IsValidCondition() const;
	
protected:
	//! RPM engine parameter which tells if engine is active.
	const instrument::Parameter* m_pParRPM;
};

// -------------------------------------------------------------------------

/** \brief The Carburetor temperature alarm.
 */

class AlarmCarburetor : public AlarmEngineActive
{
public:
	//! Constructor
	AlarmCarburetor(const instrument::Parameter* pPar, 
		const instrument::Parameter* pParRPM, const QString& qsMsg);
	
	//! Get condition. Return true if condition is valid.
	virtual bool IsValidCondition() const;
};


#endif
