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

#include <QtDebug>
#include <QFile>
#include <QStringList>
#include "CommonDefs.h"
#include "MathEx.h"
#include "Geometry/Angle.h"
#include "Geometry/Polygon2D.h"

#include "AirspaceContainer.h"
#include "ActiveAirspaceContainer.h"

namespace map {

// ----------------------------------------------------------------------------

ActiveAirspaceContainer::ActiveAirspaceContainer()
{
}

ActiveAirspaceContainer::~ActiveAirspaceContainer()
{
}

// ----------------------------------------------------------------------------

void ActiveAirspaceContainer::Update(const QPointF& ptX, float fRadiusKm)
{
	AirspaceContainer* pAS = AirspaceContainer::GetInstance();
	
	// longitude correction factor (flat earth)
	qreal f  = 1.0f/cos(ptX.y());
	// surroundings rectangle
	qreal fy = fRadiusKm/6372.0f;
	qreal fx = fy * f;
	QRectF rcSurr(ptX.x()-fx, ptX.y()-fy, 2*fx, 2*fy); 
	
	// Get new airspaces
	m_conAS = pAS->GetAirspaces(rcSurr);
}

// ----------------------------------------------------------------------------

QVector<const Airspace*> ActiveAirspaceContainer::GetAirspaces(
	const QPointF& ptX 
) const
{
	using namespace geometry;
	QVector<const Airspace*> vA;
	
	foreach(const Airspace* pA, m_conAS) {
		const Polygon2D& pg = pA->GetPolygon();
		if(pg.IsInside(ptX))
			vA << pA;
	}
	
	return vA;
}

// ----------------------------------------------------------------------------
} // namespace
