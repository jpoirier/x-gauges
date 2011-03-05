#ifndef __MAP_LAYER_ENTITY_POLYLINE_H
#define __MAP_LAYER_ENTITY_POLYLINE_H

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

#include "AbstractLayerEntity.h"

namespace map {
// ------------------------------------------------------------------------ 

/** \brief The polyline layer entity. 

 */

class LayerEntityPolyline : public AbstractLayerEntity
{
public:
	//! Contructor takes number of parameters.
	LayerEntityPolyline(unsigned int uiParameterCount);
	//! Destructor.
	virtual ~LayerEntityPolyline();

	//! Read the entity from the stream.
	virtual bool Read(QDataStream& in, const QByteArray& baParam);
	
	//! Draw the polyline
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Calculate and return memory used by this entity. This is just an estimate.
	virtual int GetUsedMemory() const;
	
protected:
	//! X coordinates.
	QVector<float> m_vX;
	//! Y coordinates.
	QVector<float> m_vY;
	//! Polyline segments ofssets.
	QVector<quint16> m_vSeg;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_LAYER_ENTITY_POLYLINE_H
