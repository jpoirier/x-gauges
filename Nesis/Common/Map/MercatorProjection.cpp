/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include <QPoint>
#include "CommonDefs.h"
#include "MathEx.h"
#include "MercatorProjection.h"

namespace map {

// -----------------------------------------------------------------------
	
MercatorProjection::MercatorProjection(qreal fLam0)
{
	Set(fLam0);
	const int Nc=166;
	m_fs = 1.0/128.0;
	
	m_vC0.resize(Nc);
	m_vC1.resize(Nc);
//	m_vC2.resize(Nc);
	for(int i=0; i<Nc; i++) {
		double x = i*m_fs;
		double xx = M_PI_4 + x/2;
		m_vC0[i] = log(tan(xx));
		m_vC1[i] = 1.0/cos(x);
//		m_vC2[i] = ( -pow(1.0/sin(xx),2) + pow(1.0/cos(xx),2) )/4.0;
	}
	
	const int Ni=256;
	
	m_vI0.resize(Ni);
	m_vI1.resize(Ni);
//	m_vI2.resize(Ni);
	for(int i=0; i<Ni; i++) {
		double x = i*m_fs;
		m_vI0[i] = 2*atan(exp(x)) - M_PI_2;
		m_vI1[i] = 2*exp(x)/(1+exp(2*x));
//		m_vI2[i] = -(2*exp(x)*(-1+exp(2*x)))/pow(1+exp(2*x),2);
	}
}

MercatorProjection::~MercatorProjection()
{
}
// -----------------------------------------------------------------------

void MercatorProjection::Set(
	qreal fLam0 //! Central (zero) longitude in radians.
)
{
	ASSERT(fLam0 <= M_PI && fLam0 >= -M_PI);
	m_fLam0 = fLam0;
}

// -----------------------------------------------------------------------

QPointF MercatorProjection::ConvertToMap(
	const QPointF& ptWGS
) const
{
/*	qreal f1 = log(tan(M_PI_4 + ptWGS.y()/2));
	return QPointF(
			ptWGS.x()-m_fLam0, 
			f1
	);*/
	// TODO Make GetYFromLatitude and GetLatitudeFromY truly inverse.
	
	float f2 = GetYFromLatitude(ptWGS.y());
	return QPointF(
		ptWGS.x()-m_fLam0, 
		f2
	);
}

// -----------------------------------------------------------------------

QPointF MercatorProjection::ConvertToWGS(
	const QPointF& ptMap
) const
{
/*	qreal f1 = exp(ptMap.y());
	f1 = 2*atan(f1) - M_PI_2;
	return QPointF(
			m_fLam0 + ptMap.x(),
			f1
	);*/
	float f2 = GetLatitudeFromY(ptMap.y());
	
	return QPointF(
		m_fLam0 + ptMap.x(),
		f2 //f1
	);
}

// -----------------------------------------------------------------------

float MercatorProjection::GetYFromLatitude(float fLat) const
{
	int iSig = common::Sgn(fLat);
	float x = fabs(fLat);
	int ind = (int)floor((x+m_fs/2)/m_fs);
	float dx = x-ind*m_fs;
	if(ind < 166)
		return iSig*(m_vC0[ind] + m_vC1[ind]*dx);
	else
		return log(tan(fLat/2+M_PI_4));
}

// -----------------------------------------------------------------------

float MercatorProjection::GetLatitudeFromY(float y) const
{
	int iSig = common::Sgn(y);
	float x = fabs(y);
	int ind = (int)floor((x+m_fs/2)/m_fs);
	float dx = x-ind*m_fs;
	if(ind < 256)
		return iSig*(m_vI0[ind] + m_vI1[ind]*dx);
	else
		return 2*atan(exp(y)) + M_PI_2;
	
}

// -----------------------------------------------------------------------

}
