#ifndef __NUMERIC_FIT_COMPASS_H
#define __NUMERIC_FIT_COMPASS_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

/*! \file 
    \brief 

    \author  Ales Krajnc 
    \date   
 */

// --------------------------------------------------------------------
 
#include "BestFit.h"

namespace numeric
{

// ---------------------------------------------------------------------------
/** Fit compass error (hard + soft iron) + offset error.
    The error fit function is defined as:

	 y = a sin(x) + b cos(x) + c sin(2x) + d cos(2x) + e 

	 Here, {a,b,c,d,e} are problem unknowns. The problem can be solved using
    one of the FitSolvers.
 */
class FitCompass : public BestFit
{
public:
	//! Fit Compass constructor
	FitCompass() : BestFit(5) 
	{;}
	//! The fit function. (vX are coefficients, and vData is x,y argument.)
	virtual double CalcFitFunction(const Vec& vX, const common::VectorD& vData) const;
	//! The derivatives over coefficients.
	virtual void CalcFitDerivative(const Vec& vX, const common::VectorD& vData, Vec& dv) const;
};

// ---------------------------------------------------------------------------
} // namespace numeric

#endif
