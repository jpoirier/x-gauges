#ifndef __NUMERIC_BEST_FIT_H
#define __NUMERIC_BEST_FIT_H

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
 
#include <vector>
#include "../CommonDefs.h"
#include "../Matrix/MatrixHeap.h"
#include "../Matrix/VectorHeap.h"
#include "../Matrix/Cholesky.h"

namespace numeric
{

/** The class is an abstract base class where we form a Non-linear 
    Least Squares Method problem. This is actually a best fit problem.
	 The derived classes must provide best fit function and a best fit
    function first derivatives.

	 In order to use this class, we must feed it with data points. 
	 Each data point is stored in std::vector<double>. The last element in
    data point vector must be depended variable (e.g. y). 
 */

class BestFit
{
public:
	//! Shorthand for vector of vectors - data points.
	typedef std::vector<common::VectorD> Data; 
public:
	//! Shorthand for Vector implemented on heap.
	typedef smx::VectorHeap<double> Vec;
	//! Shorthand for Matrix implemented on heap.
	typedef smx::MatrixHeap<double> Mat;

public:
	//! Constructor takes the number of unknowns parameters.
	BestFit(unsigned int uiUnknownCount);
	//! Destructor
	virtual ~BestFit();

	//! Clear the content - clear internal variables.
	void Clear();

	//! Add new data point. The last element in \a vData is reference.
	void AddData(const common::VectorD& vData);
	//! Overloaded AddData which takes the \a x any \a y arguments only.
	void AddData(double x, double y);
	//! Return const access to the data.
	const Data& GetData() const
	{ return m_vData; }

	//! Calculate Jacobian matrix. Store it in the \a mJ parameter.
	void CalcJacobian(const Vec& vX, Mat& mJ, bool bTranspose = false) const;

	//! Calculate the "best fit" function for all data points and store it in \a vf.
	void CalcFV(const Vec& vX, Vec& vf) const;

	//! Calculate F (fv'*fv)1/2. - The function that is minimised.
	double GetF(const Vec& vX) const;

	//! Calculate Gradient vector F'. Gradient of the optimization problem.
	void CalcGradient(const Vec& vX, Vec& g) const; 

	//! Calculate J'*J matrix.
	void CalcJTJ(const Vec& vX, Mat& mJTJ) const;

	//! Return number of unknown parameters.
	unsigned int GetUnknownsCount() const
	{ return m_uiUnknowns; }

	//! Solve system of linear equations. This is helper method for the solvers.
	bool SolveSLECholesky(const Mat& mA, Vec& vX) const;  

	/** The "best fit" function call.
		 The vX vector refers to the unknown fitting parameters, while
       vData vector refers to current data point.
		 The method must calculate the fit function based on given parateres. 
    */ 
	virtual double CalcFitFunction(const Vec& vX, const common::VectorD& vData) const = 0;

	//! Calculate the first derivative for the "best fit" function.
	virtual void CalcFitDerivative(const Vec& vX, const common::VectorD& vData, Vec& dv) const = 0;

protected:
	//! Size of the problem - number of unknowns.
	unsigned int m_uiUnknowns;

	//! Matrix of measurements. Each row represents one measurement.
	Data m_vData;	
};

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/*class FitCompass : public BestFit
{
public:
	//! Fit Compass constructor
	FitCompass() : BestFit(3) {};
	virtual double CalcFitFunction(const Vec& vX, const common::VectorD& vData) const;
	virtual void CalcFitDerevative(const Vec& vX, const common::VectorD& vData, Vec& dv) const;
};

// ---------------------------------------------------------------------------

double FitCompass::CalcFitFunction(
	const Vec& vX, 
	const common::VectorD& vData
) const
{
	double x = vData[0];
	double y = vData[1];
	
	double a = vX[0];
	double b = vX[1];
	double c = vX[2];

	return y - (a*sin(x) + b*cos(x) + c);
}

// ---------------------------------------------------------------------------

void FitCompass::CalcFitDerevative(
	const Vec& vX, 
	const common::VectorD& vData, 
	Vec& vd
) const
{
	double x = vData[0];

	assert(m_uiUnknowns==3);
	vd.Resize(m_uiUnknowns);

	vd[0] = -sin(x);
	vd[1] = -cos(x);
	vd[2] = -1;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class FitCompass2 : public BestFit
{
public:
	//! Fit Compass constructor
	FitCompass2() : BestFit(2) {};
	virtual double CalcFitFunction(const Vec& vX, const common::VectorD& vData) const;
	virtual void CalcFitDerevative(const Vec& vX, const common::VectorD& vData, Vec& dv) const;
};

// ---------------------------------------------------------------------------

double FitCompass2::CalcFitFunction(
	const Vec& vX, 
	const common::VectorD& vData
) const
{
	double x = vData[0];
	double y = vData[1];
	
	double c = vX[0];
	double d = vX[1];
//	double e = vX[2];

	return y - (c*sin(2*x) + d*cos(2*x));
}

// ---------------------------------------------------------------------------

void FitCompass2::CalcFitDerevative(
	const Vec& vX, 
	const common::VectorD& vData, 
	Vec& vd
) const
{
	double x = vData[0];

	assert(m_uiUnknowns==2);
	vd.Resize(m_uiUnknowns);

	vd[0] = -sin(2*x);
	vd[1] = -cos(2*x);
//	vd[2] = -1;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class FitCompass3 : public BestFit
{
public:
	//! Fit Compass constructor
	FitCompass3() : BestFit(5) {};
	virtual double CalcFitFunction(const Vec& vX, const common::VectorD& vData) const;
	virtual void CalcFitDerevative(const Vec& vX, const common::VectorD& vData, Vec& dv) const;
};

// ---------------------------------------------------------------------------

double FitCompass3::CalcFitFunction(
	const Vec& vX, 
	const common::VectorD& vData
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

void FitCompass3::CalcFitDerevative(
	const Vec& vX, 
	const common::VectorD& vData, 
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
*/
} // namespace numeric

#endif
