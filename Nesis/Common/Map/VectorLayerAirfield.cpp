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

#include <QDataStream>
#include <QtDebug>
#include <QPainter>
#include <limits>

#include "CommonDefs.h"
#include "Map/Mapper.h"
#include "LayerEntityAirfield.h"
#include "VectorLayerAirfield.h"


namespace map {
// ----------------------------------------------------------------------------

VectorLayerAirfield::VectorLayerAirfield(LayerType eLayer)
: VectorLayerPoint(eLayer)
{
}

VectorLayerAirfield::~VectorLayerAirfield()
{
}

// ----------------------------------------------------------------------------

AbstractLayerEntity* VectorLayerAirfield::CreateEntity(QDataStream& in)
{
	// This class is using polyline entities.
	AbstractLayerEntity* pE = new LayerEntityAirfield(m_baParameterType.size());
	pE->Read(in, m_baParameterType);
	return pE;
}

// ----------------------------------------------------------------------------

void VectorLayerAirfield::Draw(QPainter* pP, const Mapper* pM) const
{
	int iZoom = (int)(1.0f/pM->GetScale());

	bool bWasAntialiasing = (pP->renderHints() & QPainter::Antialiasing);
	pP->setRenderHint(QPainter::Antialiasing, false);
	
	foreach(AbstractLayerEntity* pE, m_conEntity) {
		int iImp = ((LayerEntityAirfield*)pE)->GetImportance();
		if(iImp >= 4 && iZoom > 251000)
			continue;
		if(iImp == 3 && iZoom > 1001000) 
			continue;
		pE->Draw(pP, pM);
	}
	
	pP->setRenderHint(QPainter::Antialiasing, bWasAntialiasing);
}

// ----------------------------------------------------------------------------

void VectorLayerAirfield::GetTextLabels(
	const Mapper* pM,
	const QRectF& rClip,  
	QList<TextLabel*>& conLabels) const
{
	int iZoom = (int)(1.0f/pM->GetScale());
	foreach(AbstractLayerEntity* pE, m_conEntity) {
		int iImp = ((LayerEntityAirfield*)pE)->GetImportance();
		if(iImp >= 4 && iZoom > 251000)
			continue;
		if(iImp == 3 && iZoom > 1001000) 
			continue;
		// Is it inside the clipping rectangle?
		if(rClip.contains(((LayerEntityAirfield*)pE)->GetCoordinate()))
			conLabels << new TextLabel(((LayerEntityAirfield*)pE)->GetTextLabel());
	}
}

// ----------------------------------------------------------------------------
}
