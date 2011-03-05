#ifndef __MAP_VECTOR_LAYER_AIRFIELD_H
#define __MAP_VECTOR_LAYER_AIRFIELD_H

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
#include "Map/VectorLayerPoint.h"


namespace map {
// ------------------------------------------------------------------------ 

/*! \brief The vector layer of airfields. 

	The class is based on the \a VectorLayerPoint class and it 
	implements the airfield specific layer features.
 */

class VectorLayerAirfield : public VectorLayerPoint
{
public:
	//! VectorLayerAirfield contructor.
	VectorLayerAirfield(LayerType eLayer);
	//! Destructor.
	virtual ~VectorLayerAirfield();

	//! Create the required entity and read the data from the stream.
	virtual AbstractLayerEntity* CreateEntity(QDataStream& in);
	
	//! Draw the layer
	virtual void Draw(QPainter* pP, const Mapper* pM) const;

	//! Build list ob text labels used by this layer.
	virtual void GetTextLabels(const Mapper* pM,
										const QRectF& rClip, 
										QList<TextLabel*>& conLabels) const;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_VECTOR_LAYER_AIRFIELD_H
