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

#include <QtGui>
#include "Map/Mapper.h"
#include "LayerEntityArea.h"


namespace map {
// ----------------------------------------------------------------------------

LayerEntityArea::LayerEntityArea(unsigned int uiParameterCount)
: LayerEntityPolyline(uiParameterCount)
{
}

LayerEntityArea::~LayerEntityArea()
{
}

// ----------------------------------------------------------------------------

void LayerEntityArea::Draw(QPainter* pP, const Mapper* pM) const
{
	ASSERT(m_vSeg.size() > 1);
	for(int s=0; s<m_vSeg.size()-1; s++) {
		int i =    m_vSeg[s];
		int iEnd = m_vSeg[s+1];
		
		QPolygon poly;
		poly.reserve(iEnd - i);
		for(; i<iEnd; i++) {
			poly << pM->ConvertToImage(QPointF(m_vX[i], m_vY[i]));
		}
		
		pP->drawPolygon(poly);
	}
}

// ----------------------------------------------------------------------------
}


