#ifndef __MAP_LAYER_ENTITY_POINT_H
#define __MAP_LAYER_ENTITY_POINT_H

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

/** \brief The point layer entity. 

 */

class LayerEntityPoint : public AbstractLayerEntity
{
public:
	//! Contructor takes number of parameters.
	LayerEntityPoint(unsigned int uiParameterCount);
	//! Destructor.
	virtual ~LayerEntityPoint();

	//! Read the entity from the stream.
	virtual bool Read(QDataStream& in, const QByteArray& baParam);
	
	//! Draw the point
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Calculate and return memory used by this entity.
	virtual int GetUsedMemory() const;

protected:
	//! x coordinate.
	float m_fX;
	//! y coordinate.
	float m_fY;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_LAYER_ENTITY_POINT_H
