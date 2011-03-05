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
#include "MathEx.h"
#include "CommonDefs.h"
#include "LambertConformalConicProjection.h"

namespace map {

// -----------------------------------------------------------------------
	
LambertConformalConicProjection::LambertConformalConicProjection(
	qreal fLam0, 
	qreal fPhi0, 
	qreal fPhi1, 
	qreal fPhi2
)
{
	Set(fLam0, fPhi0, fPhi1, fPhi2);
}

LambertConformalConicProjection::~LambertConformalConicProjection()
{
}
// -----------------------------------------------------------------------
void LambertConformalConicProjection::Set(
	qreal fLam0, //! Central (zero) longitude in radians.
	qreal fPhi0, //! Central (zero) latitude in radians.
	qreal fPhi1, //! First standard parallel (latitude) in radians.
	qreal fPhi2  //! Second standard parallel (latitude) in radians.
)
{
	ASSERT(fLam0 <= M_PI && fLam0 >= -M_PI);
	ASSERT(fPhi0 <= M_PI_2 && fPhi0 >= -M_PI_2);
	ASSERT(fPhi1 <= M_PI_2 && fPhi1 >= -M_PI_2);
	ASSERT(fPhi2 <= M_PI_2 && fPhi2 >= -M_PI_2);
	ASSERT(fPhi1 < fPhi2);

	m_fn = (log( cos(fPhi1) / cos(fPhi2) ))
			/(log( tan(M_PI_4+fPhi2/2) / tan(M_PI_4+fPhi1/2) ));
	m_1_fn = 1.0/m_fn;
	
	m_fF = (cos(fPhi1) * pow(tan(M_PI_4+fPhi1/2), m_fn)) / m_fn;
	
	m_fRho0 = m_fF * pow(tan(M_PI_4+fPhi0/2), -m_fn);
	
	m_fLam0 = fLam0;
}

// -----------------------------------------------------------------------
// TODO Optimize the for the speed!
QPointF LambertConformalConicProjection::ConvertToMap(
	const QPointF& ptWGS
) const
{
	qreal fLam = ptWGS.x();
	qreal fPhi = ptWGS.y();
	
	qreal fRho = m_fF * pow(tan(M_PI_4+fPhi/2), -m_fn);
	qreal fnll0 = m_fn*(fLam-m_fLam0);
		
	return QPointF(
		fRho*sin(fnll0), 
		m_fRho0-fRho*cos(fnll0)
	 );
}

// -----------------------------------------------------------------------
// TODO Optimize the for the speed!
QPointF LambertConformalConicProjection::ConvertToWGS(
	const QPointF& ptMap
) const
{
	using namespace common;
	qreal fx  = ptMap.x();
	qreal fx2 = fx*fx;
	qreal fy  = m_fRho0 - ptMap.y();
	qreal fy2 = fy*fy;
	
	qreal fRho   = Sgn(m_fn)*sqrt(fx2 + fy2);
	qreal fTheta = atan(fx/fy);
	
	return QPointF(
		m_fLam0 + fTheta*m_1_fn,
		2*atan(pow(m_fF/fRho,m_1_fn)) - M_PI_2
	 );
}

// -----------------------------------------------------------------------

}
