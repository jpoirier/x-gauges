#ifndef __MAP_VECTOR_LAYER_POINT_H
#define __MAP_VECTOR_LAYER_POINT_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
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
#include "Map/AbstractVectorLayer.h"


namespace map {
// ------------------------------------------------------------------------ 

/*! \brief The vector layer of polylines. 

	The class is based on the \a AbstractVectorLayer class and it 
	implements the polyline version of the vector layer.
 */

class VectorLayerPoint : public AbstractVectorLayer
{
public:
	//! VectorLayerPoint contructor.
	VectorLayerPoint(LayerType eLayer);
	//! Destructor.
	virtual ~VectorLayerPoint();

	//! Create the required entity and read the data from the stream.
	virtual AbstractLayerEntity* CreateEntity(QDataStream& in);
	
	//! Draw the layer
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Set line and fill colors
	void SetColors(const QColor& colorLine, const QColor& colorFill)
	{ m_colorLine = colorLine; m_colorFill = colorFill; }

	//! Calculate and return memory used by this layer.
	virtual int GetUsedMemory() const;
	
protected:
	//! Line color
	QColor m_colorLine;
	//! Fill color
	QColor m_colorFill;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_VECTOR_LAYER_POINT_H
