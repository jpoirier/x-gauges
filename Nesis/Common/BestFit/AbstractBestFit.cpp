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

// ------------------------------------------------------------------------

AbstractBestFit::AbstractBestFit(
	unsigned int uiUnknownCount,	//! Number of unknown coefficients
	unsigned int iDataCount			//! Number of data variables (min 2)
)
:  c_iDataCount(iDataCount), m_vC(uiUnknownCount)
{
}

AbstractBestFit::~AbstractBestFit()
{
}

// ------------------------------------------------------------------------

void AbstractBestFit::Clear()
{
	m_mData.clear();
	m_vC.Reset();
}

// ------------------------------------------------------------------------

void AbstractBestFit::AddData(const common::VectorD& vData)
{
	ASSERT((int)vData.size() == c_iDataCount);
	m_mData.push_back(vData);
}

// ------------------------------------------------------------------------

void AbstractBestFit::AddData(const smx::VectorBase<double>& vData)
{
	ASSERT((int)vData.GetSize() == c_iDataCount);
	common::VectorD v(c_iDataCount);
	for(int i=0; i<c_iDataCount; i++)
		v[i] = vData[i];

	m_mData.push_back(v);
}
// ------------------------------------------------------------------------

void AbstractBestFit::AddData(const smx::VectorBase<float>& vData)
{
	ASSERT((int)vData.GetSize() == c_iDataCount);
	common::VectorD v(c_iDataCount);
	for(int i=0; i<c_iDataCount; i++)
		v[i] = vData[i];

	m_mData.push_back(v);
}

// ------------------------------------------------------------------------

void AbstractBestFit::AddData(double x, double y)
{
	ASSERT(2 == c_iDataCount);
	common::VectorD v(2);
	v[0] = x;
	v[1] = y;

	m_mData.push_back(v);
}

// ------------------------------------------------------------------------

} // namespace
