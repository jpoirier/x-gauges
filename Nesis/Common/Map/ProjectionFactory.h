#ifndef __MAP_PROJECTION_FACTORY_H
#define __MAP_PROJECTION_FACTORY_H
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

#include "MapDefs.h"
#include "AbstractProjection.h"

namespace map {

// -----------------------------------------------------------------------
/** \brief Projection facrory - create desired projection.
  
 */
class ProjectionFactory
{
public:
	//! Constructor.
	ProjectionFactory();
	//! Destructor.
	virtual ~ProjectionFactory();
	
	//! Create projection according to given arguments.
	AbstractProjection* Create(
		const QPointF& ptWGSDeg, Projection eProj, float fdPhiDeg=8.0f) const;
};

// -----------------------------------------------------------------------
} // map namespace

#endif	// __MAP_PROJECTION_FACTORY_H
