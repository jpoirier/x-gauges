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
#include "Map/MapDefs.h"
#include "Map/Mapper.h"
#include "LayerEntityPolyline.h"


namespace map {
// ----------------------------------------------------------------------------

LayerEntityPolyline::LayerEntityPolyline(unsigned int uiParameterCount)
: AbstractLayerEntity(uiParameterCount)
{
}

LayerEntityPolyline::~LayerEntityPolyline()
{
}

// ----------------------------------------------------------------------------

bool LayerEntityPolyline::Read(QDataStream& in, const QByteArray& baParam)
{
	// read number of vertices
	quint16 uiCount;
	in >> uiCount;

	// resize the arrays
	m_vX.resize(uiCount);
	m_vY.resize(uiCount);

	// read the vertices
	for(int i=0; i<uiCount; i++) {
		in >> m_vX[i];
		in >> m_vY[i];
	}

	// read the segments count
	quint16 uiSegCount;
	in >> uiSegCount;
	m_vSeg.clear();
	if(uiSegCount > 1) {
		m_vSeg.resize(uiSegCount);
		// read the segmetns
		for(int i=0; i<uiSegCount; i++) {
			in >> m_vSeg[i];
		}
	}
	// Make one segment, even of it does not exist.
	else {
		m_vSeg.push_back(0);
	}
	m_vSeg.push_back(uiCount);
	

	// read the parameters, if any present
	for(int i=0; i<baParam.size(); i++) {
		// FTString,  FTInteger, FTDouble, FTLogical, FTInvalid
		if(baParam[i] == FTString) {
			QString qsString;
			in >> qsString;
			m_conParameter[i] = QVariant(qsString);
		}
		else if(baParam[i] == FTInteger) {
			qint32 iData;
			in >> iData;
			m_conParameter[i] = QVariant(iData);
		}
		else if(baParam[i] == FTDouble) {
			double dData;
			in >> dData;
			m_conParameter[i] = QVariant(dData);
		}
	}
	
	return (in.status() == QDataStream::Ok);
}

// ----------------------------------------------------------------------------

void LayerEntityPolyline::Draw(QPainter* pP, const Mapper* pM) const
{
	ASSERT(m_vSeg.size() > 1);
	for(int s=0; s<m_vSeg.size()-1; s++) {
		int i =    m_vSeg[s];
		int iEnd = m_vSeg[s+1];
		
		// TODO:This is an error - the MapGenerator should handle this.
 		if(i==iEnd) continue;
			
		QPoint p1 = pM->ConvertToImage(QPointF(m_vX[i], m_vY[i]));
		for(i++; i<iEnd; i++) {
			QPoint p2 = pM->ConvertToImage(QPointF(m_vX[i], m_vY[i]));
			pP->drawLine(p1, p2);
			p1 = p2;
		}
	}
}

// ----------------------------------------------------------------------------

int LayerEntityPolyline::GetUsedMemory() const
{
	int iMem = AbstractLayerEntity::GetUsedMemory();
	
	iMem += 2*(sizeof(m_vX) + m_vX.capacity()*sizeof(float));
	iMem += sizeof(m_vSeg) + m_vSeg.capacity()*sizeof(quint16);
	
	return iMem; 
}

// ----------------------------------------------------------------------------
}

