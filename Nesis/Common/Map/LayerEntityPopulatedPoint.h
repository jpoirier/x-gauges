#ifndef __MAP_LAYER_ENTITY_POPULATED_POINT_H
#define __MAP_LAYER_ENTITY_POPULATED_POINT_H

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

#include "LayerEntityPoint.h"
#include "TextLabel.h"

namespace map {
// ------------------------------------------------------------------------ 

/** \brief The populated point layer entity. 

 */

class LayerEntityPopulatedPoint : public LayerEntityPoint
{
public:
	//! Contructor takes number of parameters.
	LayerEntityPopulatedPoint(unsigned int uiParameterCount);
	//! Destructor.
	virtual ~LayerEntityPopulatedPoint();

	//! Read the entity from the stream.
	virtual bool Read(QDataStream& in, const QByteArray& baParam);
	
	//! Draw the point
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Get importance. 1 high, 5 low.
	int GetImportance() const
	{ return m_iImportance; }

	//! Is it city?
	bool IsCity() const
	{ return m_bIsCity; }
	
	//! Get entity name.
	const QString& GetName() const
	{ return m_qsName; }
	
	//! Get entity coordinate.
	QPointF GetCoordinate() const
	{ return QPointF(m_fX, m_fY); }
	
	//! Calculate and return memory used by this entity. This is just an estimate.
	virtual int GetUsedMemory() const;
	
	//! Get the TextLabel object for this entity.
	TextLabel GetTextLabel() const
	{ return TextLabel(m_qsName, QPointF(m_fX, m_fY), m_iImportance); }
	
protected:
	//! A flag indicating city area (point is not drawn).
	bool m_bIsCity;
	//! Importance of the point (1-5), 1 very, 5 minor.
	int m_iImportance;
	//! Populated point name.
	QString m_qsName;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_LAYER_ENTITY_POPULATED_POINT_H
