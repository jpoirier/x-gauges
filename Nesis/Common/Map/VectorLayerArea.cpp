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

#include <QDataStream>
#include <QtDebug>
#include <QPainter>
#include <limits>

#include "CommonDefs.h"

#include "LayerEntityArea.h"
#include "VectorLayerArea.h"


namespace map {
// ----------------------------------------------------------------------------

VectorLayerArea::VectorLayerArea(LayerType eLayer)
: VectorLayerPolyline(eLayer)
{
}

VectorLayerArea::~VectorLayerArea()
{
}

// ----------------------------------------------------------------------------

AbstractLayerEntity* VectorLayerArea::CreateEntity(QDataStream& in)
{
	// This class is using polyline entities.
	AbstractLayerEntity* pE = new LayerEntityArea(m_baParameterType.size());
	pE->Read(in, m_baParameterType);
	return pE;
}

// ----------------------------------------------------------------------------

void VectorLayerArea::Draw(QPainter* pP, const Mapper* pM) const
{
	pP->setBrush(m_colorLine);
	VectorLayerPolyline::Draw(pP, pM);
}


}
