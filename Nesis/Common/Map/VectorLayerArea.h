#ifndef __MAP_VECTOR_LAYER_AREA_H
#define __MAP_VECTOR_LAYER_AREA_H

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

#include <memory>
#include <QByteArray>
#include <QColor>
#include "Map/MapDefs.h"
#include "Map/VectorLayerPolyline.h"


namespace map {
// ------------------------------------------------------------------------ 

/*! \brief The vector layer of areas. 

	The class is based on the \a VectorLayerPolyline class and it 
	implements the area version of the vector layer.
 */

class VectorLayerArea : public VectorLayerPolyline
{
public:
	//! VectorLayerArea contructor.
	VectorLayerArea(LayerType eLayer);
	//! Destructor.
	virtual ~VectorLayerArea();

	//! Create the required entity and read the data from the stream.
	virtual AbstractLayerEntity* CreateEntity(QDataStream& in);
	
	//! Draw the layer
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Set the fill color
	void SetFillColor(QColor colorFill)
	{ m_colorFill = colorFill; }

protected:
	//! Fill color.
	QColor m_colorFill;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_VECTOR_LAYER_AREA_H
