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
 
#include "FitCompass.h"

namespace numeric
{
using namespace common;

// --------------------------------------------------------------------

double FitCompass::CalcFitFunction(
	const Vec& vX, 
	const VectorD& vData
) const
{
	double x = vData[0];
	double y = vData[1];
	
	double a = vX[0];
	double b = vX[1];
	double c = vX[2];
	double d = vX[3];
	double e = vX[4];

	return y - (a*sin(x) + b*cos(x) + c*sin(2*x) + d*cos(2*x) + e);
}

// ---------------------------------------------------------------------------

void FitCompass::CalcFitDerivative(
	const Vec& /*vX*/, 
	const VectorD& vData, 
	Vec& vd
) const
{
	double x = vData[0];

	assert(m_uiUnknowns==5);
	vd.Resize(m_uiUnknowns);

	vd[0] = -sin(x);
	vd[1] = -cos(x);
	vd[2] = -sin(2*x);
	vd[3] = -cos(2*x);
	vd[4] = -1;
}
// ---------------------------------------------------------------------------

} // numeric
