#ifndef MATHEX_H
#define MATHEX_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o.                                 *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Licence:                                                              *
 *      GNU - General Public License as published by the                   *
 *            Free Software Foundation                                     *
 *                                                                         *
 ***************************************************************************/

/*! \file MathEx.h 
    \brief Implementation of some math-like functions.

    \author Ales Krajnc 
    \date   15.6.2006
 */

#include <cmath>
#include <cstdlib>
#include <assert.h>

// --------------------------------------------------------------------

namespace common {

// --------------------------------------------------------------------
//! Returns the max of given arguments. Used to avoid conflicts in various libraries.
template <class T>
const T& GetMax(const T& a, const T& b) { return a>b ? a : b; }

//! Returns the max of given arguments. Used to avoid conflicts in various libraries.
template <class T>
const T& GetMax(const T& a, const T& b, const T& c) 
{ return a>b ? GetMax(a,c) : GetMax(b,c); }

//! Returns the max of given arguments. Used to avoid conflicts in various libraries.
template <class T>
const T& GetMax(const T& a, const T& b, const T& c, const T& d) 
{ 
	const T& x = GetMax(a,b);
	const T& y = GetMax(c,d);
	return GetMax(x,y); 
}

// --------------------------------------------------------------------

//! Returns the min of given arguments. Used to avoid conflicts in various libraries.
template <class T>
const T& GetMin(const T& a, const T& b) { return a<b ? a : b; }

//! Returns the min of given arguments. Used to avoid conflicts in various libraries.
template <class T>
const T& GetMin(const T& a, const T& b, const T& c) 
{ return a<b ? GetMin(a,c) : GetMin(b,c); }

//! Returns the min of given arguments. Used to avoid conflicts in various libraries.
template <class T>
const T& GetMin(const T& a, const T& b, const T& c, const T& d) 
{ 
	const T& x = GetMin(a,b);
	const T& y = GetMin(c,d);
	return GetMin(x,y); 
}

// --------------------------------------------------------------------
//! Returns a value between fMin and fMax interval.
/*! Returns x, if x is within the interval. If this is not 
    the case, lower or upper interval bound is returned instead.
 */
template<class T>
T GetMinMax(
   const T& x,    //!< x argument, which supposed to be inside the interval.
   const T& fMin, //!< lower bound
   const T& fMax) //!< upper bound
{
   assert(fMin < fMax);
   if(x < fMin)
      return fMin;
   if(x > fMax)
      return fMax;
   return x;
}

// --------------------------------------------------------------------
//! Returns the sign of the x. 
/*! \retval -1 if x is negative.
    \retval  1 if x is positive.
    \retval  0 if x is zero.
 */
template<class T>
int Sgn(const T& x)
{
   if(x>0) 
      return 1;
   else if (x<0)
      return -1;
   else
      return 0;
}

// --------------------------------------------------------------------
//! Converts degrees to radians.
inline float Rad(float fDeg) { return fDeg*((float)(M_PI)/180.0f); }
inline double Rad(double dDeg) { return dDeg*(M_PI/180.0); }
//QPointF Rad(const 
// --------------------------------------------------------------------
//! Converts radians to degrees.
inline float Deg(float fRad) { return fRad*(180.0f/((float)M_PI)); } 
inline double Deg(double dRad) { return dRad*(180.0/M_PI); } 

// --------------------------------------------------------------------
//! Standard interpolation. For two given points, we want to get y for known x. 
template<class T>
T Interpolate(
   const T& dX1, //!< Point 1 x coordinate.
   const T& dY1, //!< Point 1 y coordinate.
   const T& dX2, //!< Point 2 x coordinate.
   const T& dY2, //!< Point 2 y coordinate.
   const T& dX   //!< x argument on the line. y value is returned.
) 
{
   assert(dX2-dX1);
   return (dY2-dY1)/(dX2-dX1)*(dX-dX1) + dY1;
}

// ---------------------------------------------------------------------
//! Gives a random value from a given interval.
template<class T>
T GetRandom(const T& fMin, const T& fMax)
{
   assert(fMax > fMin);
   return (fMin + (fMax-fMin)*drand48());
}

// ---------------------------------------------------------------------
//! Safe version of the arcus cosinus. 

template<class T>
T ACosSafe(T x)
{
	return acos(GetMinMax(x, (T)-1.0, (T)1.0));
}

// ---------------------------------------------------------------------
//! Safe version of the arcus sinus. 

template<class T>
T ASinSafe(T x)
{
	return asin(GetMinMax(x, (T)-1.0, (T)1.0));
}

// -------------------------------------------------------------------------
//! Limits the argument value y into the [0,x] range.

template<class T>
T Mod(T y, T x)
{ 
	return y - x*floor(y/x); 
} 

// ---------------------------------------------------------------------
//! Safe version of arcus tanges 2

template<class T>
T ATan2Safe(const T& y, const T& x)
{
	if(x==(T)0.0 && y==(T)0.0)
		return (T)0.0;
	else
		return atan2(y,x);
}

// ---------------------------------------------------------------------
} // namespace common
#endif // MATHEX_H
