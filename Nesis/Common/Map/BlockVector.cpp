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
#include <QFile>
#include "Map/Mapper.h"
#include "CommonDefs.h"
#include "VectorLayerPolyline.h"
#include "VectorLayerArea.h"
#include "VectorLayerPopulatedPoint.h"
#include "VectorLayerAirfield.h"
#include "BlockVector.h"


namespace map {

// ----------------------------------------------------------------------------

BlockVector::BlockVector()
{
}

BlockVector::~BlockVector()
{
	Clear();
}

// ----------------------------------------------------------------------------

void BlockVector::Clear()
{
	foreach(AbstractVectorLayer* pL, m_conLayer)
		delete pL;
	m_conLayer.clear();
}

// ----------------------------------------------------------------------------

bool BlockVector::ReadHead(
	QIODevice& fIn					//!< Input device
)
{
	// In stream object
	QDataStream in(&fIn);
	
	// read the version
	quint16 uiVersion;
	in >> uiVersion;
	ASSERT(uiVersion > 0);
	
	// Clear the object (in the case if we are reusing an old object)
	Clear();
	
	// Read layer types and offsets.
	for(int i=ltBegin; i<MBV_MAX_LAYERS_COUNT; i++) {
		quint64 uiRecPos;
		quint8  uiLayer;
		qint8   iZip;

		in >> uiRecPos;
		in >> uiLayer;
		in >> iZip;

		if(uiLayer < ltEnd && uiRecPos != 0) {
			AbstractVectorLayer* pLayer = CreateLayer(uiLayer);
			if(pLayer != NULL) {
				pLayer->SetFilePos(uiRecPos, iZip);
				m_conLayer.push_back(pLayer);
			}
			else {
				qDebug() << "Warning(BlockVector::ReadHead): Unsupported layer " << uiLayer << ".";
			}
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

AbstractVectorLayer* BlockVector::CreateLayer(quint8 uiLayerId)
{
	ASSERT(uiLayerId < ltEnd);

	AbstractVectorLayer* pLayer = NULL;

	switch(uiLayerId) {
/*		case ltPoliticalBoundary:
			pLayer = new VectorLayerPolyline((LayerType)uiLayerId);
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("silver"));
			break;*/
		case ltRoad:
			pLayer = new VectorLayerPolyline((LayerType)uiLayerId);
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("red"));
			break;
		case ltTrail:
			pLayer = new VectorLayerPolyline((LayerType)uiLayerId);
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("orange"));
			break;
		case ltPowerline:
			pLayer = new VectorLayerPolyline((LayerType)uiLayerId);
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("darkseagreen"));
			break;
		case ltWaterCourse:
			pLayer = new VectorLayerPolyline((LayerType)uiLayerId);
			pLayer->AddParameter(FTString);  // nam
			pLayer->AddParameter(FTInteger); // hyc
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("royalblue"));
			break;
		case ltInlandWater:
			pLayer = new VectorLayerArea((LayerType)uiLayerId);
			pLayer->AddParameter(FTString);  // nam
			pLayer->AddParameter(FTInteger); // hyc
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("royalblue"));
			break;
		case ltRailroad:
			pLayer = new VectorLayerPolyline((LayerType)uiLayerId);
			pLayer->AddParameter(FTInteger); // fco
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("black"));
			break;
		case ltCityArea:
			pLayer = new VectorLayerArea((LayerType)uiLayerId);
			((VectorLayerPolyline*)pLayer)->SetLineColor(QColor("yellow"));
			break;
		case ltPopPlaces:
			pLayer = new VectorLayerPopulatedPoint((LayerType)uiLayerId);
			((VectorLayerPopulatedPoint*)pLayer)->SetColors(Qt::black, QColor("yellow"));
			break;
		case ltAirfield:
			pLayer = new VectorLayerAirfield((LayerType)uiLayerId);
			// ((VectorLayerAirfield*)pLayer)->SetColors(Qt::black, Qt::white);
			break;
			
/*	ltAdministrativeArea,		 	//!< (text) names of admin. areas.
	ltBoundaryText,				 	//!< (text) gulf names, some port names.
		
	ltSpotElevation,					//!< (point) Some major elevations.
		
	ltAqueductCanal,					//!< (line) Aqueducts, canals, flumes, penstocks.
		
	ltSettlement,						//!< (point) City, town, village, etc.
		
	ltMiscTransport,					//!< (line) lift, ...
	ltRailroad,							//!< (line) railoads
	ltTransportStructure,			//!< (line) bridges, tunnels, etc...
	ltPipeline,							//!< (line) pipelines
*/
		default:
			break;
	}
	return pLayer;
}

// ----------------------------------------------------------------------------

void BlockVector::Draw(QPainter* pP, const Mapper* pM) const
{
// 	qDebug() << "Number of layers to draw = " << m_conLayer.size();
	float fZoom = 1.0f/pM->GetScale();
	foreach(AbstractVectorLayer* pL, m_conLayer) {
		if(IsVisible((int)fZoom, pL->GetType())) 
			pL->Draw(pP, pM);
	}
}

// ----------------------------------------------------------------------------

bool BlockVector::AssureData(const QString& qsFile)
{
	if(!m_conLayer.empty()) {
		QFile fIn(qsFile);
		fIn.open(QIODevice::ReadOnly);
		return ReadData(fIn);
	}
	return true;
}

// ----------------------------------------------------------------------------

bool BlockVector::ReadData(QIODevice& in)
{
	bool bStatus = true;
	foreach(AbstractVectorLayer* pL, m_conLayer) {
		// load only if not already loaded.
		if(pL->IsEmpty()) {
			bool bOK = pL->Read(in);
			if(!bOK) bStatus = false;
		}
	}
	return bStatus;
}

// ----------------------------------------------------------------------------

int BlockVector::GetUsedMemory() const
{
	int iMem = sizeof(BlockVector);
	foreach(AbstractVectorLayer* pL, m_conLayer)
		iMem += pL->GetUsedMemory();
	return iMem;
}

// ----------------------------------------------------------------------------

void BlockVector::GetTextLabels(
	const Mapper* pM,
   const QRectF& rClip,
	QList<TextLabel*>& conLabels
) const
{
	float fZoom = 1.0f/pM->GetScale();
	// airfields first
	foreach(AbstractVectorLayer* pL, m_conLayer) {
		LayerType lt = pL->GetType();
		if(lt==ltAirfield) {
			if(IsVisible((int)fZoom, lt)) {
				pL->GetTextLabels(pM, rClip, conLabels);
			}
			break;
		}
	}
	// populated places second
	foreach(AbstractVectorLayer* pL, m_conLayer) {
		LayerType lt = pL->GetType();
		if(lt==ltPopPlaces) {
			if(IsVisible((int)fZoom, lt)) {
				pL->GetTextLabels(pM, rClip, conLabels);
			}
			break;
		}
	}
}

// ----------------------------------------------------------------------------
		
bool BlockVector::IsVisible(int iZoom, LayerType lType) const
{
	iZoom -= 1000;
	switch(lType) {
/*		case ltPoliticalBoundary:
		break;*/
		case ltRailroad:
		case ltTrail:
			return iZoom <  1000000;
		case ltRoad:
			return iZoom <  2000000;
		case ltWaterCourse:
			return iZoom <  4000000;
		case ltInlandWater:
			return iZoom <  8000000;
		case ltCityArea:
			return iZoom <  4000000;
		case ltPopPlaces:
			return iZoom <  2500000;
		case ltAirfield:
			return false; //iZoom <  1500000;
			
/*	ltAdministrativeArea,		 	//!< (text) names of admin. areas.
			ltBoundaryText,				 	//!< (text) gulf names, some port names.
		
			ltSpotElevation,					//!< (point) Some major elevations.
		
			ltAqueductCanal,					//!< (line) Aqueducts, canals, flumes, penstocks.
		
			ltSettlement,						//!< (point) City, town, village, etc.
		
			ltMiscTransport,					//!< (line) lift, ...
			ltRailroad,							//!< (line) railoads
			ltTransportStructure,			//!< (line) bridges, tunnels, etc...
			ltPipeline,							//!< (line) pipelines
*/
		default:
			return false;
	}
}

// ----------------------------------------------------------------------------
}
