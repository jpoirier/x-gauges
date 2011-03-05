#pragma once
#ifndef __TOLERANCE_H
#define __TOLERANCE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

/*! \file Tolerance.h 
    \brief Implementation of some relational functions which are 
           based on given tolerance. 
    
    The functions are grouped in two namespaces. The #tolerance_abs
    namespace encapsulates funtions that are based on absolute 
    difference between argument, while the #tolerance_rel namespace
    encapsulates functions that use relative difference between 
    arguments.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

#include "CommonDefs.h"

// -------------------------------------------------------------------

//! Encapsulates relational functions based on absolute tolerance. 
namespace tolerance_abs {

const double TOL = 0.000001; //!< Default tolerance for comparisons

// ==
inline bool Equal(double dLhs, double dRhs, const double dTol=TOL)
{
   return ((dLhs > dRhs-dTol) && (dLhs < dRhs+dTol));
}

// !=
inline bool NotEqual(double dLhs, double dRhs, const double dTol=TOL)
{
   return !Equal(dLhs, dRhs, dTol);
}

// >
inline bool GreaterThan(double dLhs, double dRhs, const double dTol=TOL)
{
    return ((dLhs > dRhs) && NotEqual(dLhs, dRhs, dTol));
}

// >=
inline bool GreaterThanOrEqual(double dLhs, double dRhs, const double dTol=TOL)
{
    return ((dLhs > dRhs) || Equal(dLhs, dRhs, dTol));
}

// <
inline bool LowerThan(double dLhs, double dRhs, const double dTol=TOL)
{
   return ((dLhs < dRhs) && NotEqual(dLhs, dRhs, dTol));
}

// <=
inline bool LowerThanOrEqual(double dLhs, double dRhs, const double dTol=TOL)
{
    return ((dLhs < dRhs) || Equal(dLhs, dRhs, dTol));
}

} // tolerance_abs

// ---------------------------------------------------------------------------
//! Encapsulates relational functions based on relative tolerance. 
namespace tolerance_rel {

const double TOL = 1.0E-10;     //!< Default tolerance for comparisons

//! This function compares two doubles for the equality using relative comparison. 
/*! We use this function when we want smaller precission than the default (built-in)
    a==b operator provides. The required relative precision (tolerance) is passed 
    as argument.

    Returns true if values are equal for given precision.
 */
inline bool Equal(
   double dL,              //!< Left hand value
   double dR,              //!< Right hand value
   const double dTol=TOL)  //!< Tolerance
{
   if (dL==dR) return true;
   // first one is smaller
   if (dL < dR) {
      // Ok, make the first one is larger
      common::Swap(dL, dR);
   }

   // The first one is larger ...
   if(dR != 0) {
      // relative compare normalised by smaller value
      return (fabs((dR-dL)/dR) <= dTol);
   }
   // smaller value is zero
   else {
      // if we compare against zero, use absolute tolerance.
      return tolerance_abs::Equal(dL, dR, dTol);
   }
}

// !=
inline bool NotEqual(double dLhs, double dRhs, const double dTol=TOL)
{
   return !Equal(dLhs, dRhs, dTol);
}

// >
inline bool GreaterThan(double dLhs, double dRhs, const double dTol=TOL)
{
    return ((dLhs > dRhs) && NotEqual(dLhs, dRhs, dTol));
}

// >=
inline bool GreaterThanOrEqual(double dLhs, double dRhs, const double dTol=TOL)
{
    return ((dLhs > dRhs) || Equal(dLhs, dRhs, dTol));
}

// <
inline bool LowerThan(double dLhs, double dRhs, const double dTol=TOL)
{
   return ((dLhs < dRhs) && NotEqual(dLhs, dRhs, dTol));
}

// <=
inline bool LowerThanOrEqual(double dLhs, double dRhs, const double dTol=TOL)
{
    return ((dLhs < dRhs) || Equal(dLhs, dRhs, dTol));
}

} // namespace tolerance_rel
// ---------------------------------------------------------------------------

#endif //__TOLERANCE_H
