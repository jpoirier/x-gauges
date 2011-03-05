#ifndef BEST_FIT_ABSTRACT_BEST_FIT_LINEAR_H
#define BEST_FIT_ABSTRACT_BEST_FIT_LINEAR_H

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

#include "AbstractBestFit.h"

namespace bestfit {

// ---------------------------------------------------------------------------
/** \brief Linear abstract base class for best fit problems.

    The class is an abstract base class for the LINEAR 
    Least Squares Method problems. 
 */

class AbstractBestFitLinear : public AbstractBestFit
{
protected:
	//! Protected constructor
	AbstractBestFitLinear(unsigned int uiUnknownCount, unsigned int iDataCount);
public:
	//! Public virtual destructor.
	virtual ~AbstractBestFitLinear();

	//! Pure virtual solver
	bool Solve(const smx::VectorBase<double>* pvC0=NULL);
protected:
	//! Calculate the "Xi" vector for best fit function for given vD data point.
	virtual void CalcX(const common::VectorD& vD, smx::VectorBase<double>& vX) const = 0;
	//! Get Y value for given vD data point. Default is the last point.
	virtual double CalcY(const common::VectorD& vD) const
	{ return vD.back(); }
};
// ---------------------------------------------------------------------------

} // namespace

#endif
