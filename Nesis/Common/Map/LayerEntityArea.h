#ifndef __MAP_LAYER_ENTITY_AREA_H
#define __MAP_LAYER_ENTITY_AREA_H

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

#include "LayerEntityPolyline.h"

namespace map {
// ------------------------------------------------------------------------ 

/** \brief The area layer entity. 

 */

class LayerEntityArea : public LayerEntityPolyline
{
public:
	//! Contructor takes number of parameters.
	LayerEntityArea(unsigned int uiParameterCount);
	//! Destructor.
	virtual ~LayerEntityArea();

	//! Draw the area
	virtual void Draw(QPainter* pP, const Mapper* pM) const;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_LAYER_ENTITY_AREA_H
