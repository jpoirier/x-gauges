#ifndef __ALTITUDE_H
#define __ALTITUDE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

/*! \file
    \brief

    \author  Ales Krajnc
    \date
 */
 
#include <cmath>

namespace isa {
// ---------------------------------------------------------------------------

/** Class converts between pressure and altitude. It also enables QNH settings.
	 The class is mainly used to convert from pressure received from sensors
    into altitude.

	 Pressure units are hPa - hectopascals.
	 Altitude untis are m - meters.
 */

class Altitude
{
public:
	//! Constructor. By default it takes "flight level" QNH.
	Altitude(float fpQNH = 1013.25)
	{ 	SetQNH(fpQNH); }

	//! Specify new QNH in hPa.
	void SetQNH(float fpQNH)
	{ m_fpQNH = fpQNH; }

	//! Get current QNH pressure in hPa.
	float GetQNH() const
	{ return m_fpQNH; }

	//! Calculate altitude according to given pressure (hPa) and QNH settings.
	float GetAltitude(float fp) const
	{ 	return 44330.8f - 11880.341069940743f*pow(1013.25f + fp - m_fpQNH, 0.190263f); }
//	{ 	return 44330.8f * (1.0f - pow((fp-m_fpQNH+1013.25f)/1013.25f, 0.190263f)); }
	
	//! Calculate standard altitude according to given pressure (hPa).
	float GetStandardAltitude(float fp) const
	{ 	return 44330.8f - 11880.341069940743f*pow(fp, 0.190263f); }

	//! Calculate pressure according to given altitude (m) and QNH settings.
	float GetPressure(float fAlt) const
	{ return m_fpQNH + 1013.25f*(pow(1.0f - 0.2255767998772862E-4f*fAlt, 5.255882646652266f)-1.0f); }
/*	{ 	return 1013.25f * pow((1.0f-fAlt/44330.8f), 1.0f/0.190263f) 
			  + m_fpQNH - 1013.25f; }*/
			  
	//! Calculate derivative dA/dp at given pressure and QNH.
	float GetDerivative(float fp) const
	{ return -2260.39f / pow((fp + 1013.25f - m_fpQNH), 0.809737f); } 

private:
	//! QNH settings in hPa - hectopascals.
	float m_fpQNH;
};

// ---------------------------------------------------------------------------

} // namespace

#endif
