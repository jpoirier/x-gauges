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
#include "Map/Mapper.h"
#include "LayerEntityPopulatedPoint.h"
#include "VectorLayerPopulatedPoint.h"


namespace map {
// ----------------------------------------------------------------------------

VectorLayerPopulatedPoint::VectorLayerPopulatedPoint(LayerType eLayer)
: VectorLayerPoint(eLayer)
{
}

VectorLayerPopulatedPoint::~VectorLayerPopulatedPoint()
{
}

// ----------------------------------------------------------------------------

AbstractLayerEntity* VectorLayerPopulatedPoint::CreateEntity(QDataStream& in)
{
	// This class is using polyline entities.
	AbstractLayerEntity* pE = new LayerEntityPopulatedPoint(m_baParameterType.size());
	pE->Read(in, m_baParameterType);
	return pE;
}

// ----------------------------------------------------------------------------

void VectorLayerPopulatedPoint::Draw(QPainter* pP, const Mapper* pM) const
{
	int iZoom = (int)(1.0f/pM->GetScale());

	pP->setPen(m_colorLine);
	pP->setBrush(m_colorFill);
	bool bWasAntialiasing = (pP->renderHints() & QPainter::Antialiasing);
	pP->setRenderHint(QPainter::Antialiasing, false);
	
	foreach(AbstractLayerEntity* pE, m_conEntity) {
		int iImp = ((LayerEntityPopulatedPoint*)pE)->GetImportance();
		if(iImp >= 4 && iZoom > 251000)
			continue;
		if(iImp == 3 && iZoom > 1001000) 
			continue;
		pE->Draw(pP, pM);
	}
	
	pP->setRenderHint(QPainter::Antialiasing, bWasAntialiasing);
	
}

// ----------------------------------------------------------------------------

void VectorLayerPopulatedPoint::GetTextLabels(
	const Mapper* pM,
	const QRectF& rClip,  
	QList<TextLabel*>& conLabels) const
{
	int iZoom = (int)(1.0f/pM->GetScale());
	foreach(AbstractLayerEntity* pE, m_conEntity) {
		int iImp = ((LayerEntityPopulatedPoint*)pE)->GetImportance();
		if(iImp >= 4 && iZoom > 251000)
			continue;
		if(iImp == 3 && iZoom > 3001000) 
			continue;
		if(!((LayerEntityPopulatedPoint*)pE)->IsCity() && iZoom > 2001000)
			continue;
		// Is it inside the clipping rectangle?
		if(rClip.contains(((LayerEntityPopulatedPoint*)pE)->GetCoordinate()))
			conLabels << new TextLabel(((LayerEntityPopulatedPoint*)pE)->GetTextLabel());
	}
}

// ----------------------------------------------------------------------------
}
