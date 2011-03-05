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

#include "BestFit.h"

namespace numeric
{

using namespace common;

// ---------------------------------------------------------------------------

BestFit::BestFit(unsigned int uiUnknownCount)
{
	m_uiUnknowns = uiUnknownCount;
}

BestFit::~BestFit()
{
}

// ---------------------------------------------------------------------------

void BestFit::Clear()
{
	m_vData.clear();
}

// ---------------------------------------------------------------------------

void BestFit::AddData(
	const VectorD& vData
)
{
	m_vData.push_back(vData);
}

// ---------------------------------------------------------------------------

void BestFit::AddData(
	double dx, 
	double dy
)
{
	VectorD v(2);
	v[0] = dx;
	v[1] = dy;
	AddData(v);
}

// ---------------------------------------------------------------------------

void BestFit::CalcFV(
	const Vec& vX, 
	Vec& vf
) const
{
	vf.Resize(m_vData.size());

	for(unsigned int i=0; i<m_vData.size(); i++) {
		vf[i] = CalcFitFunction(vX, m_vData[i]);
	}
}

// ---------------------------------------------------------------------------

double BestFit::GetF(
	const Vec& vX
) const
{
	Vec vf;
	CalcFV(vX, vf);

	return vf.Multiply(vf);
}

// ---------------------------------------------------------------------------

void BestFit::CalcJacobian(
	const Vec& vX, 
	Mat& mJ,
	bool bTranspose
) const
{
	ASSERT(m_vData.size() > m_uiUnknowns);

	if(bTranspose)
		mJ.Resize(m_uiUnknowns, m_vData.size());
	else
		mJ.Resize(m_vData.size(), m_uiUnknowns);

	Vec vd; // vector of first derivatives
	for(unsigned int i=0; i<m_vData.size(); i++) {
		CalcFitDerivative(vX, m_vData[i], vd);
		for(unsigned int j=0; j<m_uiUnknowns; j++) {
			if(bTranspose)
				mJ[j][i] = vd[j];
			else
				mJ[i][j] = vd[j];
		}
	}
}
// ---------------------------------------------------------------------------

void BestFit::CalcGradient(const Vec& vX, Vec& g) const
{
	Mat mJT;
	CalcJacobian(vX, mJT, true); // true == get transpose of J

	Vec vF;
	CalcFV(vX, vF);

	g.Resize(m_uiUnknowns);
	mJT.PostMultiply(vF,g);
}

// ---------------------------------------------------------------------------

void BestFit::CalcJTJ(const Vec& vX, Mat& mJTJ) const
{
	Mat mJ;
	CalcJacobian(vX, mJ);
	Mat mJT(mJ.GetColCount(), mJ.GetRowCount());
	mJT.Transpose(mJ);

	mJTJ.Resize(m_uiUnknowns,m_uiUnknowns);
	mJTJ.Multiply(mJT,mJ);
}

// ---------------------------------------------------------------------------
/*! Solve system of linear equations using Cholesky decomposition.
    This is helper function for the FitSolver classes. If the return value is
    false, the vX argument is not altered. 

	 \retval true if \a mA is positive definite matrix.
	 \retval false if \a mA is not positive definite matrix.
 */

bool BestFit::SolveSLECholesky(
	const Mat& mA, //!< The matrix of coefficients.
	Vec& vX			//!< [in, out] Right hand side on input, solution on output.
) const
{
	BestFit::Mat mL(mA.GetRowCount(), mA.GetColCount());
	smx::Cholesky<double> C(mL);
	C.Decompose(mA);
	if(C.IsSPD()) {
		C.Solve(vX);
		return true;
	}
	else {
		return false;
	}
} 

// --------------------------------------------------------------------

} // numeric
