#ifndef __MAP_LAYER_ENTITY_AIRFIELD_H
#define __MAP_LAYER_ENTITY_AIRFIELD_H

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

#include "LayerEntityPoint.h"
#include "TextLabel.h"
#include "Route/Airfield.h"

namespace map {
// ------------------------------------------------------------------------ 

/** \brief The airfield point layer entity. 

 */

class LayerEntityAirfield : public LayerEntityPoint
{
public:
	//! Contructor takes number of parameters.
	LayerEntityAirfield(unsigned int uiParameterCount);
	//! Destructor.
	virtual ~LayerEntityAirfield();

	//! Read the entity from the stream.
	virtual bool Read(QDataStream& in, const QByteArray& baParam);
	
	//! Draw the airfield symbol.
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Get importance. 1 high, 5 low.
	int GetImportance() const;
	
	//! Get entity name.
	const QString& GetName() const
	{ return m_qsName; }
	
	//! Get entity coordinate.
	QPointF GetCoordinate() const
	{ return QPointF(m_fX, m_fY); }
	
	//! Calculate and return memory used by this entity. This is just an estimate.
	virtual int GetUsedMemory() const;
	
	//! Get the TextLabel object for this entity.
	TextLabel GetTextLabel() const;

protected:
	//! Get color of the airfield symbol according to the runway type.
	QColor GetAirfieldColor() const;
		
	//! Do we have two runways?
	bool IsCross() const
	{ return m_iRwyDir1+18 != m_iRwyDir2; } 
	
protected:
	//! Key of the airfield.
	QString m_qsKey;
	//! Airfield name.
	QString m_qsName;
	//! ICAO name - can be empty.
	QString m_qsICAO;
	//! Frequency - can be empty.
	QString m_qsFreq;
	
	//! Airfield category.
	route::Airfield::Category m_eCategory;
	
	//! Runway Type
	route::Airfield::RunwayType m_eRunway;
	//! Runway direction 1.
	int m_iRwyDir1;
	//! Runway direction 2.
	int m_iRwyDir2;
	//! Runway length.
	int m_iRwyLength;
	
	//! Runway elevation
	int m_iElevation;
	//! Airfield info flags.
	int m_iInfoFlags;
	//! Country.
	route::Country m_eCountry;
	
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_LAYER_ENTITY_AIRFIELD_H
