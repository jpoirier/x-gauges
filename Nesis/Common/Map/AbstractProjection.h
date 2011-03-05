#ifndef MAP_ABSTRACTPROJECTION_H
#define MAP_ABSTRACTPROJECTION_H
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

#include <QPoint>

namespace map {

// -----------------------------------------------------------------------
/** \brief Abstract class for all projections.
 */
class AbstractProjection
{
public:
	//! Constructor.
	AbstractProjection();
	//! Destructor.
	virtual ~AbstractProjection();
	
	//! Convert wgs coordinates on the sphere (lat, lon) into (x, y). 
	virtual QPointF ConvertToMap(const QPointF& ptWGS) const = 0;
	//! Convert flat earth map (x,y) coordinates into wgs sphere (lat, lon). 
	virtual QPointF ConvertToWGS(const QPointF& ptMap) const = 0;
	
	//! Returns true if the projection is rectangular (Mercator, for instance).
	virtual bool IsRectangular() const = 0;
};

// -----------------------------------------------------------------------

} // map namespace

#endif	// __MAP_ABSTRACTPROJECTION_H
