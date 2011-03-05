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
#include <limits>
#include "CommonDefs.h"
#include "LZO/LZOMini.h"

#include "AbstractVectorLayer.h"


namespace map {
// ----------------------------------------------------------------------------

AbstractVectorLayer::AbstractVectorLayer(LayerType eLayer)
{
	// make the layer always visible by default.
	m_fLimitScale = std::numeric_limits<float>::max(); 
	m_eLayer = eLayer;
	m_bCompress = false;
	m_uiPos = 0;
}

AbstractVectorLayer::~AbstractVectorLayer()
{
	Clear();
}
// ----------------------------------------------------------------------------

void AbstractVectorLayer::Clear()
{
	foreach(AbstractLayerEntity* pE, m_conEntity)
		delete pE;
	m_conEntity.clear();
	m_baParameterType.clear();
}

// ----------------------------------------------------------------------------

bool AbstractVectorLayer::Read(QIODevice& fIn)
{
	Q_ASSERT(m_conEntity.isEmpty()==true);

	// Move the pointer to the correct possition.
	if(fIn.seek(m_uiPos)==false)
		return false;

	// Start reading ...
	QDataStream in(&fIn);

	// vector layer data
	QByteArray baData;
	Q_ASSERT(m_bCompress);
	baData = DecompressMiniLZO(in);
	// There must be something ...
	if(baData.isEmpty())
		return false;

/*	in >> baData;

	// Check if we got anything.
	if(baData.isEmpty()) {
		qDebug() << "Error(AbstractVectorLayer::Read): Failed to read data.";
		return false;
	}

	// Uncompress if necessary.
	if(m_bCompress) {
		baData = qUncompress(baData);
		if(baData.isEmpty()) {
			qDebug() << "Error(AbstractVectorLayer::Read): Failed to uncompress the data.";
			return false;
		}
	}*/

	// The actual data is in the byte array. 
	QDataStream inBa(baData); // read only
	
	// Read number of entities.
	quint16 uiEntitiesCount;
	inBa >> uiEntitiesCount;

	// Read number of parameters (attributes) to monitor.
	quint16 uiParameterCount;
	inBa >> uiParameterCount;

	// Get parameters.
	m_baParameterType.resize(uiParameterCount);
	for(int i=0; i<m_baParameterType.size(); i++) {
		qint8 iType;
		inBa >> iType;
		m_baParameterType[i] = iType;
	}

	// Load entities.
	m_conEntity.resize(uiEntitiesCount);
	for(int i=0; i<uiEntitiesCount; i++) {
		m_conEntity[i] = CreateEntity(inBa);
		ASSERT(m_conEntity[i]!=NULL);
	}
	return true;
}

// ----------------------------------------------------------------------------

int AbstractVectorLayer::GetUsedMemory() const
{
	int iMem = sizeof(AbstractVectorLayer);
	iMem += m_baParameterType.capacity() + 2;
	
	foreach(AbstractLayerEntity* pE, m_conEntity) {
		iMem += pE->GetUsedMemory();
	}
	
	return iMem;
}

// ----------------------------------------------------------------------------
}
