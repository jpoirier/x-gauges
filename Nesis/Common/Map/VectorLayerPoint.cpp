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

#include "LayerEntityPoint.h"
#include "VectorLayerPoint.h"


namespace map {
// ----------------------------------------------------------------------------

VectorLayerPoint::VectorLayerPoint(LayerType eLayer)
: AbstractVectorLayer(eLayer)
{
	m_colorLine = Qt::white; //Qt::black;
	m_colorFill = Qt::white;
}

VectorLayerPoint::~VectorLayerPoint()
{
}

// ----------------------------------------------------------------------------

AbstractLayerEntity* VectorLayerPoint::CreateEntity(QDataStream& in)
{
	// This class is using polyline entities.
	AbstractLayerEntity* pE = new LayerEntityPoint(m_baParameterType.size());
	pE->Read(in, m_baParameterType);
	return pE;
}

// ----------------------------------------------------------------------------

void VectorLayerPoint::Draw(QPainter* pP, const Mapper* pM) const
{
//	qDebug() << "Number of entities in the layers = " << m_conEntity.size();
	pP->setPen(m_colorLine);
	pP->setBrush(m_colorFill);
	bool bWasAntialiasing = (pP->renderHints() & QPainter::Antialiasing);
	pP->setRenderHint(QPainter::Antialiasing, false);
	
	foreach(AbstractLayerEntity* pE, m_conEntity)
		pE->Draw(pP, pM);
	
	pP->setRenderHint(QPainter::Antialiasing, bWasAntialiasing);
}

// ----------------------------------------------------------------------------

int VectorLayerPoint::GetUsedMemory() const
{
	int iMem = AbstractVectorLayer::GetUsedMemory();
	iMem += 2*sizeof(QColor);
	return iMem;
}

// ----------------------------------------------------------------------------
}
