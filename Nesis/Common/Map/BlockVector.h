#ifndef __MAP_BLOCK_VECTOR_H
#define __MAP_BLOCK_VECTOR_H

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

#include <QVector>
#include <QRect>
#include "Map/MapDefs.h"
#include "Map/TextLabel.h"

//! Each block vector header occupies 256 bytes.
#define MBV_HEADER_SIZE	         256
//! Maximal number of vector layers inside the vector block.
#define MBV_MAX_LAYERS_COUNT	   25
	

// Forward declarations
class QPainter;

namespace map {
	
// Forward declarations
class Mapper;
class AbstractVectorLayer;

// ------------------------------------------------------------------------ 
/*! \brief A vector block 1 deg by 1 deg of various vector information.
 
	The vector blocks origin from the vmap0. 
	vmap0 = Vector Map (VMap) Level 0 is an updated and improved version 
	of the National Imagery and Mapping Agency's (NIMA) Digital Chart of
	the World (DCW®). The VMap Level 0 database provides worldwide 
	coverage of vector-based geospatial data which can be viewed at
	1:1,000,000 scale, i.e. 1cm=10km. It consists of geographic, 
	attribute, and textual data.

	This data was filtered, modified and converted into 1x1 degree 
	blocks.
 */

class BlockVector {
	typedef QVector<AbstractVectorLayer*> ConLayer;
public:
	//! BlockVector contructor.
	BlockVector();
	//! Destructor.
	~BlockVector();
	
	//! Clear (reset) the vector block object.
	void Clear();
	
	//! Read the vector head data from the given stream.
	bool ReadHead(QIODevice& in);
	
	//! Make sure that the vector data is ready to use.
	bool AssureData(const QString& qsFileName);
	
	//! Read the vector data from the given stream.
	bool ReadData(QIODevice& in);
	
	//! Draw the vector layers.
	void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Calculate and return memory used by this block.
	int GetUsedMemory() const;
	
	//! Build list ob text labels used by this block.
	void GetTextLabels(const Mapper* pM, 
							 const QRectF& rClip, 
							 QList<TextLabel*>& conLabels) const;

private:
	//! Create layer for given layer type.
	AbstractVectorLayer* CreateLayer(quint8 uiLayerId);
	
	//! Check visibility of layer at given zoom.
	bool IsVisible(int iZoom, LayerType lType) const;

private:
	//! Vector layers stored in RAM.
	ConLayer m_conLayer;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_BLOCK_VECTOR_H
