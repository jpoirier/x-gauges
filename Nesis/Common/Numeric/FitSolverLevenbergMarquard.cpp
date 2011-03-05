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
#include "FitSolverLevenbergMarquard.h"

namespace numeric {

// ---------------------------------------------------------------------------

FitSolverLevenbergMarquard::FitSolverLevenbergMarquard(
	double dEps1, //!< Gradient infinite norm stopping criterion. |g|inf < eps1.
	double dEps2, //!< Change in |x_new-x|euc stopping criterion.` 
	double dTau,  //!< Used to calculate initial size of mi value.
	int ikMax     //!< Maximal number of allowed iterations.
)
{
	m_dTau  = dTau;
	m_dEps1 = dEps1;
	m_dEps2 = dEps2;
	m_ikMax = ikMax;
}

// ---------------------------------------------------------------------------

FitSolverLevenbergMarquard::StoppingCriterion
FitSolverLevenbergMarquard::Solve(
	const BestFit& P, 
	BestFit::Vec& vX
)
{
	// initialize
	int k  = 0;
	int v  = 2;
	StoppingCriterion sc = scInvalid;

	// The code below is not very efficient, since J is calculated twice.
	BestFit::Mat A;
	BestFit::Vec g;

	P.CalcJTJ(vX,A);			// J is calculated here
	P.CalcGradient(vX,g);   // and here.

	double dgMax = g.GetNorm(smx::NormInfinite);
	BestFit::Vec h(g.GetSize());
	A.GetDiagonal(h);
	double dAiiMax = h.GetNorm(smx::NormInfinite);

	double mi = m_dTau*dAiiMax;
	bool bFound = (dgMax <= m_dEps1);
	if(bFound)
		sc = scGradient;

// 	printf("\nX="); vX.Dump(); 
// 	printf("\nA="); A.Dump();
// 	printf("\ng="); g.Dump();
// 	printf("\n");

	while(!bFound && k<m_ikMax) {
		k++;
		h = g;
		h.Multiply(-1);
		for(unsigned int i=0; i<A.GetRowCount(); i++)
			A[i][i] += mi;
		P.SolveSLECholesky(A,h);
	
// 		printf("\nA="); A.Dump();
// 		printf("\nh="); h.Dump();
// 		printf("\ng="); g.Dump();
// 		printf("\n");

		if(h.GetNorm(smx::NormInfinite) < m_dEps2*(vX.GetNorm()+m_dEps2)) {
			sc = scStepSize;
			bFound = true;
		}
		else {
			// calculate the gain ratio dRho
			double dFx = P.GetF(vX);
			vX.Add(h);
			double dFxh = P.GetF(vX);
// 			printf("\nX="); vX.Dump(); 
			BestFit::Vec h1(h);
			h1.Multiply(mi);
			h1.Subtract(g);

// 			printf("\nh1="); h1.Dump();
// 			printf("\nF(x)= %lf, F(x+h)= %lf.", dFx, dFxh); h.Dump();
// 			printf("L(0)-L(h) = %lf", h.Multiply(h1));
// 			printf("\n");

			double dRho = 2*(dFx-dFxh)/(h.Multiply(h1));

			if(dRho > 0) {
				P.CalcJTJ(vX,A);
				P.CalcGradient(vX,g);

				if(g.GetNorm(smx::NormInfinite) <= m_dEps1) {
					bFound = true;
					sc = scGradient; 
				}
				mi = mi * common::GetMax(1./3., 1.0-pow(dRho*2 - 1.0, 3));
				v = 2;
			} 
			else {
				mi = mi*v;
				v = 2*v;
			}
		}
	} // while

	if(bFound == false)
		sc = scMaxIteration;

	return sc;
}

// ---------------------------------------------------------------------------

} // numeric
