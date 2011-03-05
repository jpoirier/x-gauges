#ifndef __MAP_ABSTRACT_LAYER_ENTITY_H
#define __MAP_ABSTRACT_LAYER_ENTITY_H

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

#include <QVariant>
#include <QVector>
#include <QDataStream>

// ------------------------------------------------------------------------ 
// Forward declarations
class QPainter;

namespace map {
	
// Forward declarations
class Mapper;

/*! \brief The abstract base class for the layer entities. 

	 This is the base class for the layer entities line polyline, text, 
    point, which are also the derives classed \a LayerEntityPolyline,
    \a LayerEntityPoint, \a LayerEntityText.

	 This is an abstract class. Derived classes are used to store and 
    represent the data efficiently.
 */

class AbstractLayerEntity 
{
public:
	//! AbstractLayerEntity contructor.
	AbstractLayerEntity(unsigned int uiParameterCount);
	//! Destructor.
	virtual ~AbstractLayerEntity();

	//! Read the entity from the stream.
	virtual bool Read(QDataStream& in, const QByteArray& baParam) = 0;
	
	//! Draw the entity.
	virtual void Draw(QPainter* pP, const Mapper* pM) const = 0;
	
	//! Calculate and return memory used by this entity. This is just an estimate.
	virtual int GetUsedMemory() const;

protected:
	//! Parameter values.
	QVector<QVariant> m_conParameter;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_ABSTRACT_LAYER_ENTITY_H
