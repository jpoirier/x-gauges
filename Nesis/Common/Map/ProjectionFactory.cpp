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

#include "MathEx.h"
#include "LambertConformalConicProjection.h"
#include "MercatorProjection.h"
#include "ProjectionFactory.h"

namespace map {

// -----------------------------------------------------------------------
	
ProjectionFactory::ProjectionFactory()
{
}

ProjectionFactory::~ProjectionFactory()
{
}
// -----------------------------------------------------------------------

AbstractProjection* ProjectionFactory::Create(
	const QPointF& ptWGSDeg, 	//!< Projection central coordinate.
	Projection eProj, 	 		//!< Type of projection.
	float fdPhiDeg			 		//!< Extra parameter for the Lambert projection.
) const
{
	using namespace common;
	AbstractProjection* pProj = NULL;

	float fLat = ptWGSDeg.y();
	float fLon = ptWGSDeg.x();
	
	//! Automatic projection selection
	if (eProj == projAutomatic) {
		//! Select mercator for latitudes below 20 degrees.)
		if(fLat > 50.0f) {
			eProj = projLambert60N;
		}
		else if(fLat > 30.0f) {
			eProj = projLambert40N;
		}
		else if(fLat > 15.0f) {
			eProj = projLambert20N;
		}
		else if(fLat < -15.0f) {
			eProj = projLambert20S;
		}
		else if(fLat < -30.0f) {
			eProj = projLambert40S;
		}
		else if(fLat < -50.0f) {
			eProj = projLambert60S;
		}
		// central latitudes between -15, 15 degrees
		else {
			eProj = projMercator;
		}
	}

	// Round the center of projection to one degree and
	// convert it into radians.
	fLat = Rad(floor(fLat));
	fLon = Rad(floor(fLon));

	// Create projection
	switch (eProj) {
	case projLambert60N:
		pProj = new LambertConformalConicProjection(
							fLon, fLat, Rad(60.0f-fdPhiDeg), Rad(60.0f+fdPhiDeg)
						);
		break;
	case projLambert40N:
		pProj = new LambertConformalConicProjection(
							fLon, fLat, Rad(40.0f-fdPhiDeg), Rad(40.0f+fdPhiDeg)
						);
		break;
	case projLambert20N:
		pProj = new LambertConformalConicProjection(
							fLon, fLat, Rad(20.0f-fdPhiDeg), Rad(20.0f+fdPhiDeg)
						);
		break;
	case projLambert20S:
		pProj = new LambertConformalConicProjection(
							fLon, fLat, Rad(-20.0f-fdPhiDeg), Rad(-20.0f+fdPhiDeg)
						);
		break;
	case projLambert40S:
		pProj = new LambertConformalConicProjection(
							fLon, fLat, Rad(-40.0f-fdPhiDeg), Rad(-40.0f+fdPhiDeg)
						);
		break;
	case projLambert60S:
		pProj = new LambertConformalConicProjection(
							fLon, fLat, Rad(-60.0f-fdPhiDeg), Rad(-60.0f+fdPhiDeg)
						);
		break;
	default:
		pProj = new MercatorProjection(fLon);
		break;
	}

	return pProj;
}

// -----------------------------------------------------------------------
} // map
