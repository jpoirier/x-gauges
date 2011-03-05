#ifndef __NUMERIC_FIT_SOLVER_LM_H
#define __NUMERIC_FIT_SOLVER_LM_H

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

namespace numeric {

/** Solve nonlinear optimization least squares problem based on 
	 Levenberg-Marquard algorhitm.

	 The algorithm is taken from publication (found on internet):

		Methods for Non-linear Least Squares Problems,
		K. Madsen, H.B. Nielsen, O. Tingleff,
		Informatics and MAthematical Modelling
		Technical University of Denmark.

 */

class FitSolverLevenbergMarquard
{
public:
	//! What was the reason for the stop.
	enum StoppingCriterion
	{
		scInvalid,	//!< Invalid value - error indicator.
		scGradient,	//!< Gradient norm was small enough. (This is good.)
		scStepSize, //!< Step size was small enough. (Not necessarily good.)
		scMaxIteration //!< Too many iterations. (Could be problem.)
	};
public:
	//! Constructor
	FitSolverLevenbergMarquard(
		double dEps1  = 1.0E-9, 
		double dEps2  = 1.0E-9, 
		double dTau   = 1.0E-5,
		int ikMax     = 100
	 );

	// TODO: replace BestFit::Vec z VectorBase<double>, which is more generic.
	//! Solve the problem using the L-M algorhitm.
	StoppingCriterion Solve(const BestFit& P, BestFit::Vec& vX);
private:
	//! Tau parameter - used initially.
	double m_dTau;
	//! Gradient stopping criterion.
	double m_dEps1;
	//! Step size stopping criterion.
	double m_dEps2;
	//! Iteration limiter - stopping criterion.
	int m_ikMax;
};

// ---------------------------------------------------------------------------

}

#endif
