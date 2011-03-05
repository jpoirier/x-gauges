#ifndef BEST_FIT_ABSTRACT_BEST_FIT_H
#define BEST_FIT_ABSTRACT_BEST_FIT_H

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

#include "Matrix/VectorHeap.h"
#include "Matrix/MatrixHeap.h"

namespace bestfit {

// ---------------------------------------------------------------------------
/** \brief Abstract base class for best fit problems.

    The class is an abstract base class for the Least Squares Method problems. 
	 This is actually a best fit problem, which may be linear or non-linear.
	 The derived classes must implement any pure virtual functions.

	 This class is NOT intendet to be used in uC code. In order to enhance the
	 precision and simplify the coding double value is assumed for data
	 storage and all calculations.

	 In order to use this class, we must feed it with data points. 
	 Each data point is stored in std::vector<double>. The last element in
    data point vector must be depended variable (e.g. y). 
 */

class AbstractBestFit
{
public:
	//! Shorthand for vector of vectors - data points.
	typedef std::vector<common::VectorD> Data; 
	//! Shorthand for Vector on heap.
	typedef smx::VectorHeap<double> Vec;
	//! Shorthand for Matrix on heap.
	typedef smx::MatrixHeap<double> Mat;

protected:
	//! Protected constructor
	AbstractBestFit(unsigned int uiUnknownCount, unsigned int iDataCount);
public:
	//! Public virtual destructor.
	virtual ~AbstractBestFit();

	//! Clear the content - clear internal variables.
	virtual void Clear();

	//! Add new data point. The last element in \a vData is reference.
	void AddData(const common::VectorD& vData);
	//! Add new data point. The last element in \a vData is reference.
	void AddData(const smx::VectorBase<double>& vData);
	//! Add new data point. The last element in \a vData is reference.
	void AddData(const smx::VectorBase<float>& vData);
	//! Overloaded AddData which takes the \a x any \a y arguments only.
	void AddData(double x, double y);
	
	//! Undo - remove the last inserted data.
	void Undo()
	{ m_mData.pop_back(); }

	//! Return the const access to the Data matrix.
	const Data& GetData() const
	{ return m_mData; }

	//! Pure virtual solver
	virtual bool Solve(const smx::VectorBase<double>* pvC0=NULL) = 0;

	//! Return the results - unknown parameters of the best fit problem.
	const Vec& GetParameters() const
	{ return m_vC; }

	//! Get fitted value for given point
	double CalcFunction(const common::VectorD& vX) const
	{ return CalcFunction(m_vC, vX); }

protected:
	//! Calculate best fit function using given parameters and data point.
	virtual double CalcFunction(const Vec& vC, const common::VectorD& vX) const = 0;

protected:
	//! Number of variables in each data vector. Min 2 (x -> y).
	const int c_iDataCount;
	//! Final result - coefficients of the best fit problem.
	Vec m_vC; 

	//! Matrix of measurements. Each row represents one measurement.
	Data m_mData;	
};

} // namespace

#endif
