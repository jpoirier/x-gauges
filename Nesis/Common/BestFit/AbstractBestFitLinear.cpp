/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "AbstractBestFitLinear.h"
#include "Matrix/Cholesky.h"

namespace bestfit {

// ------------------------------------------------------------------------

AbstractBestFitLinear::AbstractBestFitLinear(
	unsigned int uiUnknownCount,	//! Number of unknown coefficients
	unsigned int iDataCount			//! Number of data variables (min 2)
)
:  AbstractBestFit(uiUnknownCount, iDataCount)
{
}

AbstractBestFitLinear::~AbstractBestFitLinear()
{
}

// ------------------------------------------------------------------------

bool AbstractBestFitLinear::Solve(
	const smx::VectorBase<double>* /*pvC0*/ //!< We dont need initial solution here.
)
{
	const int iN = m_vC.GetSize();
	// Working vectors, matrix
	smx::VectorHeap<double> vX(iN);
	smx::MatrixHeap<double> mA(iN,iN);
	m_vC.Reset();

	// Setup the problem matrix mA and right side vector m_vC
	for(unsigned int i=0; i<m_mData.size(); i++) {
		CalcX(m_mData[i], vX);					// Xi
		mA.Outer(vX, vX, smx::aoAdd);		   // A += Xi'Xi
		vX.Multiply(CalcY(m_mData[i]));		// yi Xi
		m_vC.Add(vX);								// Y += yi Xi
	}
	// The matrix should be symmetric due to Outer function, but
	// there may be some roundoff errors. Hence we make it symmetric by
	// by force.
	mA.ForceSymmetry();

	// Solve the problem using Cholesky decomposition.
	smx::MatrixHeap<double> mC(iN,iN); // working (internal) matrix
	smx::Cholesky<double> C(mC);
	bool bSuccess = C.Decompose(mA);

	if(bSuccess)
		C.Solve(m_vC);
	else
		m_vC.Reset();

	return bSuccess;
}

// ------------------------------------------------------------------------

} // namespace
