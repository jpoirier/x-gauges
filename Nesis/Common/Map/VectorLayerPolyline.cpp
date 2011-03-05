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

#include "VectorLayerPolyline.h"
#include "LayerEntityPolyline.h"


namespace map {
// ----------------------------------------------------------------------------

VectorLayerPolyline::VectorLayerPolyline(LayerType eLayer)
: AbstractVectorLayer(eLayer)
{
}

VectorLayerPolyline::~VectorLayerPolyline()
{
}

// ----------------------------------------------------------------------------

AbstractLayerEntity* VectorLayerPolyline::CreateEntity(QDataStream& in)
{
	// This class is using polyline entities.
	AbstractLayerEntity* pE = new LayerEntityPolyline(m_baParameterType.size());
	pE->Read(in, m_baParameterType);
	return pE;
}

// ----------------------------------------------------------------------------

void VectorLayerPolyline::Draw(QPainter* pP, const Mapper* pM) const
{
//	qDebug() << "Number of entities in the layers = " << m_conEntity.size();
	pP->setPen(m_colorLine);
	foreach(AbstractLayerEntity* pE, m_conEntity)
		pE->Draw(pP, pM);			 
}

// ----------------------------------------------------------------------------

int VectorLayerPolyline::GetUsedMemory() const
{
	int iMem = AbstractVectorLayer::GetUsedMemory();
	iMem += sizeof(QColor);
	return iMem;
}

// ----------------------------------------------------------------------------
}
