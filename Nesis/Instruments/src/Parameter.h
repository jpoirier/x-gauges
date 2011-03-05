#ifndef INSTRUMENT_PARAMETER_H
#define INSTRUMENT_PARAMETER_H

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

#include <QPair>
#include <QColor>
#include <QDomElement>
#include "Delegate.h"

#define IB_RED    qRgb(255,  40,  12)
#define IB_YELLOW qRgb(255, 220,   0)
#define IB_GREEN  qRgb(  0, 170,  20)
#define IB_BLUE   qRgb( 11,  79, 190)
#define IB_CYAN   qRgb( 17, 221, 255) 
#define IB_ORANGE qRgb(255, 102,   0) 


// -------------------------------------------------------------------------

namespace instrument {

inline bool IsNull(const QPair<float,float>& r)
{ return r.first == (float) 0 && r.second == (float)0; }


/** \brief The class defines an instrument parameter.

	 A parameter object is used to desrcibe parameter limits, name, units,
	 etc. All limits and values are stored in system units (say CAN aerospace).
 */

class Parameter
{
public:
	//! Alarm activity type.
	enum AlarmType {
		atDisabled,			//! Always disabled.
		atEnabled,			//! Always enabled.
		atUser				//! Enabled by default, but user may disable it.
	};
private:
	// Used only to prevent use of the assignement operator.
	const Parameter& operator=(const Parameter& /*C*/)
	{ return *this; }

public:
	//! Constructor from QDomElement object.
	Parameter(const QDomElement& el);
	//! Copy constructor
 	Parameter(const Parameter& C);
	//! Destructor
	virtual ~Parameter();
	
	//! Copy method
 	void Copy(const Parameter& C);

	//! Get number of values within this parameter. Usually one, but may be more.
	int GetCount() const
	{ return m_vValue.count(); }

	//! Return the name of parameter.
	const QString& GetName() const
	{ return m_qsName; }

	//! Set the value callback delegate (fast callback from any c++ class).
	void SetCallback(Delegate0<float> Callback)
	{ m_Callback0 = Callback; }
	//! Return the callback delegate;
	Delegate0<float> GetCallback0() const
	{ return m_Callback0; }

	//! Set the value callback delegate (fast callback from any c++ class).
	void SetCallback(Delegate<float, int> Callback)
	{ m_Callback1 = Callback; }
	//! Return the callback delegate;
	Delegate<float, int> GetCallback1() const
	{ return m_Callback1; }

	//! Update the callback values
	void UpdateValueCallback() const;
	
	//! Get unit key of the callback source.
	int GetUnitKeySystem() const
	{ return m_iKeySystem; }
	
	//! Get current value in user units.
	float GetValueUser(int i=0) const;
	//! Get current value in system units.
	float GetValueSystem(int i=0) const
	{ return m_vValue[i]; }
	
	//! Set the user specific unit.
	void SetUnitKeyUser(int iKey)
	{ m_iKeyUser = iKey; }
	//! Get unit key
	int GetUnitKeyUser() const
	{ return m_iKeyUser; }
	
	//! Get the parameter range.
	QPair<float,float> GetRange() const;

	//! Get the green arc range in the scale units.
	const QPair<float,float>& GetGreenRange() const
	{ return m_rGreen; }

	//! Get the yellow arc range in the scale units.
	const QPair<float,float>& GetYellowRange() const
	{ return m_rYellow; }
	
	//! Get the limits in the system units.
	const QPair<float,float>& GetLimits() const
	{ return m_rLimits; }
	//! Set the limits in the system units.
	void SetLimits(const QPair<float,float> ffL)
	{ m_rLimits = ffL; }

	//! Return true if low limit was given.
	bool IsLowLimit() const
	{ return m_bLowLimit; }
	//! Return true if high limit was given.
	bool IsHighLimit() const
	{ return m_bHighLimit; }
	
	//! Check the internal status. Return true if valid.
	virtual bool IsValid() const;
	
	//! Get alarm trigger delay [s].
	int GetAlarmTriggerDelay() const
	{ return m_iAlarmTriggerDelay; }
	//! Set the alarm trigger delay [s].
	void SetAlarmTriggerDelay(int iT)
	{ m_iAlarmTriggerDelay = iT; }

	//! Set the alarm status.
	void SetAlarmType(AlarmType eAlarmType)
	{ m_eAlarmType = eAlarmType; }
	//! Get the alarm status.
	AlarmType GetAlarmType() const
	{ return m_eAlarmType; }
	
	//! Return a color for given system value fS.
	QColor GetColor(float fSys, QColor colDefault) const;
	
protected:
	//! Initialize the values.
	void Initialize();
	
protected:
	//! Parameter name.
	QString m_qsName;
	//! Values collected from callback.
	mutable QVector<float> m_vValue;

	//! The callback class when only one value exists.
	Delegate0<float> m_Callback0;
	//! The callback class when more than one values exist.
	Delegate<float, int> m_Callback1;

	//! Units returned from the callback system (CANAerospace).
	int m_iKeySystem;
	//! Units required by user - used for fast access to unit manager.
	int m_iKeyUser;

	//! Range of the instrument in the system units.
	QPair<float,float> m_rRange;
	//! green arc in the system units.
	QPair<float,float> m_rGreen;
	//! green arc in the system units.
	QPair<float,float> m_rYellow;
	//! Limits (red marks) in the system units.
	QPair<float,float> m_rLimits;

	//! Is high limit active? We assume yes, if it was set.
	bool m_bHighLimit; 
	//! Is low limit active? We assume yes, if it was set.
	bool m_bLowLimit; 
	
	//! Alarm trigger delay [s]. How many seconds we wait before alarm is triggered.
	int m_iAlarmTriggerDelay;
	//! Status of the alarm.
	AlarmType m_eAlarmType;
};
// -------------------------------------------------------------------------

} // namespace

#endif
