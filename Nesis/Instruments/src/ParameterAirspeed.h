#ifndef INSTRUMENT_PARAMETER_AIRSPEED_H
#define INSTRUMENT_PARAMETER_AIRSPEED_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *              število družinskih enot                                                           *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "Parameter.h"

// -------------------------------------------------------------------------

namespace instrument {

/** \brief The class defines the airspeed parameter.

	 The airspeed is an extension of standard parameter. It additionaly
    defines various airspeeds.
 */

class ParameterAirspeed : public Parameter
{
private:
	// Used only to prevent use of the assignement operator.
	const ParameterAirspeed& operator=(const ParameterAirspeed& /*C*/)
	{ return *this; }

public:
	//! Constructor from QDomElement object.
	ParameterAirspeed(const QDomElement& el);
	//! Copy constructor
 	ParameterAirspeed(const ParameterAirspeed& C);
	//! Destructor
	virtual ~ParameterAirspeed();
	
	//! Copy method
 	void Copy(const ParameterAirspeed& C);

	//! Get the white arc range in the scale units.
	const QPair<float,float>& GetWhiteRange() const
	{ return m_rWhite; }

	//! Get best rate of climb.
	float GetVy() const
	{ return m_fVy; }
	//! Get the reference approaching speed (for landing).
	float GetVRef() const
	{ return m_fVRef; }
	
	//! Get max flap speed at position 1.
	float GetVFe1() const
	{ return m_fVFe1;}
	
	//! Get max flap speed at position 2.
	float GetVFe2() const
	{ return m_fVFe2;}
	
	//! Get stall warning speed.
	float GetVStall() const
	{ return m_fVSW; }

	//! Check the internal status. Return true if valid.
	bool IsValid() const;

protected:
	//! Best rate of climb (blue dash)
	float m_fVy;
	//! Reference speed for approach (yellow dash).
	float m_fVRef;
	
	//! Maximal speed with position 1 flaps
	float m_fVFe1;
	//! Maximal speed with position 2 flaps
	float m_fVFe2;
	
	//! Stall warning speed.
	float m_fVSW;

	//! Flaps range (white arc) in the scale units.
	QPair<float,float> m_rWhite; 

};
// -------------------------------------------------------------------------

} // namespace

#endif
