#pragma once
#ifndef __COMMONDEFS_H
#define __COMMONDEFS_H

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

/*! \file CommonDefs.h
    \brief Declaration and implementation of some commonly used
           defines, constants and functions.

    The functions are grouped in the common namespace.

    \author Ales Krajnc
    \date   1.1.2006 - ported from VisualStudio code.
 */
// -------------------------------------------------------------------------

#include <string>
#include <vector>
#include <iostream>

#include <cmath>
#include <stdint.h>

#include "Defines.h"

// -------------------------------------------------------------------------
// Some typedefs

//! In many cases byte is more convinient than unsigned char
//typedef unsigned char byte;

//! Common namespace groups different useful utilities and functions.
namespace common {

//! stl vector of unisgned integers
typedef std::vector<unsigned int>   VectorU;
//! stl vector of ints
typedef std::vector<int>            VectorI;
//! stl vector of doubles
typedef std::vector<double>         VectorD;
//! stl vector of floats
typedef std::vector<float>          VectorF;
//! stl vector of stl string objects
typedef std::vector<std::string>    VectorSS;
//! stl vector of bytes
typedef std::vector<unsigned char>	VectorB;

//! stl pair of two doubles.
typedef std::pair<double,double>    PairDD;
//! stl pair of two floats.
typedef std::pair<float,float>      PairFF;
//! stl pair of two integers.
typedef std::pair<int,int>    		PairII;


// -------------------------------------------------------------------------

//! Swaps the arguments using third (hidden) copy argument.
template <class T>
void Swap(T& a, T& b) { T c=a; a=b; b=c; }

//! Shifs values between three variables, where d is a new value.
template <class T>
void Shift(T& a, T& b, T& c, T d) { a=b; b=c; c=d; }

// --------------------------------------------------------------------
//! Encapsulate given string with "" and write it to the out stream.
int WriteString(std::ostream& out, const std::string& s);

// --------------------------------------------------------------------
//! Extract string within "" from the in stream.
int ReadString(std::istream& in, std::string& s);

// --------------------------------------------------------------------
//! Calculates exponent value in the given value d.
int GetExponent(double);

// --------------------------------------------------------------------
//! Some libraries (math.h) don't define ArcSinh.
inline double ArcSinh(double x)
{
   return asinh(x);
}

// --------------------------------------------------------------------
//! Some libraries (math.h) don't define ArcCosh.
inline double ArcCosh(double x)
{
   return acosh(x);
}

// --------------------------------------------------------------------
//! Some libraries (math.h) don't define ArcTanh.
inline double ArcTanh(double x)
{
   return atanh(x);
}

// --------------------------------------------------------------------
//! Calculates snap point for given x and unit.
double Snap(double x, double su);

// --------------------------------------------------------------------
//! Calculates the first and last valid valid dUnit mark inside the interval.
void SuggestMinMaxInside(double unit, double& _min, double& _max);

// --------------------------------------------------------------------
//! Calculates the first and the last valid dUnit mark beyond the interval.
void SuggestMinMaxOutside(double unit, double& _min, double& _max);

// --------------------------------------------------------------------
//! Round values to given number of decimals.
double Round(double dVal, int nDecimal);

// --------------------------------------------------------------------
//! Gravity model: latitude in radians, altitude in meters.

inline double GetGravity(double dLat_rad, double dAlt_m)
{
	return 9.7803185*(1.0 + 0.005278895 * pow(sin(dLat_rad),2)
								 - 0.000023462 * pow(sin(dLat_rad),4)
						  )*(1.0-0.3086E-6*dAlt_m);
//	return 9.80621*(1-0.00259*cos(2*dLat_rad))*(1.0-0.3086E-6*dAlt_m);
}

// --------------------------------------------------------------------
/** Mercury density model: argument temperature in degrees Celsius.
	 The model is linear and is valid for temperatures above 10C and
	 below 40C.
 */
inline double GetHgDensity(double fT_C)
{
	return 13595.5 * (1.0-0.00018057 * fT_C);
}

// --------------------------------------------------------------------
/** Water density model: argument temperature in degrees Celsius.
	 The model is quadratic and is valid for temperatures above 10C and
	 below 40C.
 */
inline double GetH2ODensity(double fT_C)
{
	return 1000.4 - 0.005*fT_C*fT_C - 0.0058*fT_C;
}

// --------------------------------------------------------------------
} // namespace common

#endif // __COMMONDEFS_H
