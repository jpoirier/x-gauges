/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

/*! \file Angle.cpp 
    \brief Implementation file for the Angle class.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

#include <cmath>

#include "CommonDefs.h"
#include "Tolerance.h"
#include "Angle.h"

// ----------------------------------------------------------------------------

using namespace common;
namespace geometry {

// ----------------------------------------------------------------------------
// visible in this file only.

const qreal c2Pi = 2*M_PI;

// ============================================================================

Angle::Angle(qreal dA)
{
   m_dA = Normalize(dA);
}

// ----------------------------------------------------------------------------

qreal Angle::Normalize(qreal dAngle)
{
   // check the interval first
   if(dAngle >= 0.0 && dAngle < c2Pi) 
      return dAngle;

   qreal f = dAngle/c2Pi; // ratio
   // some roundoffs may influence the 2 Pi*angle
   if(tolerance_rel::Equal(f,1.0,1E-12))
      return c2Pi;

   double integ;
   f = modf(f,&integ);    // we need fractional part only
   if(f<0) f+=1;          // make it positive

   return f*c2Pi;        // here is the result
}

// ----------------------------------------------------------------------------
/*! This is a very subtile call and
    the result differs on the CCW parameter. If the CCW is true, the
    method will return angle between arguments aStart and aEnd 
    starting in aStart and going in CCW direction to reach aEnd.
    If the CCW is false, we start in aStart and go in CW direction to reach
    aEnd. E.g.: aStart = 30, aEnd = 0, Diff(aStart,aEnd) = 330 CCW and 30 CW.
 */

Angle Angle::GetDifference(const Angle& aStart, const Angle& aEnd, bool bCCW)
{
   // assume CCW
   qreal dDelta = aEnd - aStart;  // CCW difference
   if(dDelta < 0) dDelta += c2Pi;  // CCW diff normalized
   
   // check the bCCW
   if(bCCW==false) 
      dDelta = c2Pi-dDelta; // The CW solution

   return dDelta;
}

// ----------------------------------------------------------------------------
/*! Calculates the angle which is defined as relative position dRel
    between start angle dAlpha and end angle dBeta. If dRel=0, the retured
    angle is dAlpha and if dRel=1, the returned version is dBeta. The bCCW controls
    the arc between start and end.
 
    \retval Relative angle between dAlpha and dBeta taking account the direction.
 */
Angle Angle::GetRelative(
   qreal dRel,            //!< Relative value from [0,1] interval.
   const Angle& dAlpha,    //!< First angle
   const Angle& dBeta,     //!< Second angle
   bool bCCW               //!< true -> counterclockwise, false -> clockwise.
) 
{
   ASSERT(dRel >= 0 && dRel <= 1.0);
   Angle aDelta = GetDifference(dAlpha,dBeta,bCCW);
   if(bCCW)
      aDelta = dAlpha + dRel*aDelta;
   else
      aDelta = dAlpha - dRel*aDelta;
   return aDelta;
}

// ----------------------------------------------------------------------------
} // namespace geometry
