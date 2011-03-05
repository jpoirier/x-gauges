#ifndef __MAP_VECTOR_LAYER_H
#define __MAP_VECTOR_LAYER_H

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
#include <QVector>
#include "Map/AbstractLayerEntity.h"
#include "Map/MapDefs.h"
#include "Map/TextLabel.h"


// ------------------------------------------------------------------------ 
// Forward declarations
class QPainter;

namespace map {
	
// Forward declarations
class Mapper;

/*! \brief The abstract base class for the vector layers. 

	 A vector layer is based on the vector information obtained from the
    kam format. Originaly the information was stored in the vamp0 data, 
    which was converted into ESRI shape format, which was then filtered 
	 and converted into stripped kam (kanardia map) format.

	 This is an abstract class. Derived classes are used to store and 
    represent the data efficiently.
 */

class AbstractVectorLayer 
{
protected:
	//! Container of layer entities. (Pointer to entities).
	typedef QVector<AbstractLayerEntity*> ConEntity;
public:
	//! AbstractVectorLayer contructor.
	AbstractVectorLayer(LayerType eLayer);
	//! Destructor.
	virtual ~AbstractVectorLayer();
	
	//! Clear (reset) the layer.
	void Clear();
	
	//! Return true if there are not entites present.
	bool IsEmpty() const
	{ return m_conEntity.empty(); }
	
	//! Set file position and compression flag.
	void SetFilePos(quint64 uiPos, bool bCompress)
	{ m_uiPos = uiPos; m_bCompress = bCompress; } 
	
	//! Return the layer type.
	LayerType GetType() const
	{ return m_eLayer; }

	//! Read the vector data from the given stream.
	virtual bool Read(QIODevice& fIn);
	
	//! Add a layer parameter (type).
	void AddParameter(char cParam)
	{ m_baParameterType.push_back(cParam); }
	
	//! Create an entity from the stream.
	virtual AbstractLayerEntity* CreateEntity(QDataStream& in) = 0;
	
	//! Draw the layer.
	virtual void Draw(QPainter* pP, const Mapper* pM) const = 0;
	
	//! Calculate and return memory used by this layer.
	virtual int GetUsedMemory() const;
	
	//! Build list ob text labels used by this layer.
	virtual void GetTextLabels(const Mapper* /*pM*/, const QRectF& /*rClip*/, 
										QList<TextLabel*>& /*conLabels*/) const
	{;}
	
	
private:
	//! Layer type.
	LayerType m_eLayer;
	//! Limiting scale. A scale, where the layer is not visible anymore.
	float m_fLimitScale;
	//! Is the data on the file compressed?
	bool m_bCompress;
	//! Offset of the layer data on the file.
	quint64 m_uiPos;

protected:
	//! This layer's parameter types.
	QByteArray m_baParameterType;
	//! Layer entities.
	ConEntity m_conEntity;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_VECTOR_LAYER_H
