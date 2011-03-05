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

#include <math.h>
#include <QtDebug>
#include <QPainter>
#include "Map/Mapper.h"
#include "CommonDefs.h"
#include "Block.h"

#define FILENAME_EXT		".kam"

namespace map {


// ----------------------------------------------------------------------------
	
Block::Block()
{
	m_uiId 				= 0;
	m_iActivityCount  = 0;
}

Block::~Block()
{
}
// ----------------------------------------------------------------------------
/*! Create the filename and load, read and parse the block file.
 */
bool Block::Load(
	quint16 idBlock,
	const QString& qsPath
)
{
	// the path must end with slash
	Q_ASSERT(qsPath.endsWith('/'));
	
	// Construct the path+filename
	QString qsFileName = qsPath;
	qsFileName += GetFileNameFromId(idBlock, FILENAME_EXT);

	bool bRet = Load(qsFileName);
	Q_ASSERT(m_uiId == idBlock);
	return bRet;
/*
	qDebug() << "Loading: File " << m_qsFileName;
	
	QFile fIn(m_qsFileName);
	// read only, binary, random access.
	if(fIn.open(QIODevice::ReadOnly)==false) {
		//std::cout << "Error: File " << m_qsFileName.toAscii().constData() << " failed to open.\n";
		return false;
	}
	
	// Ok, read the header.
	if(!m_header.Read(fIn)) {
		qDebug() << "Error: File " << m_qsFileName << " failed to read the header.";
		return false;
	}
	
	// The header is already valid. Get the Raster block header.
	if(m_header.IsRasterBlock()) {
		fIn.seek(m_header.GetRasterBlockPos());
		// read the Raster block head (but not the actual data).
		m_raster.ReadHead(fIn);
	}

	// Get the Vector block header.
	if(m_header.IsVectorBlock()) {
		fIn.seek(m_header.GetVectorBlockPos());
		// read the Vector block head (but not the actual data)
		m_vector.ReadHead(fIn);
	}
	
	m_uiId = idBlock;
	return true;*/
}

// ----------------------------------------------------------------------------

bool Block::Load(const QString& qsFile)
{
	m_qsFileName = qsFile;
//	qDebug() << "Loading: File " << m_qsFileName;
	
	QFile fIn(m_qsFileName);
	// read only, binary, random access.
	if(fIn.open(QIODevice::ReadOnly)==false) {
		//std::cout << "Error: File " << m_qsFileName.toAscii().constData() << " failed to open.\n";
		return false;
	}
	
	// Ok, read the header.
	if(!m_header.Read(fIn)) {
		qDebug() << "Error: File " << m_qsFileName << " failed to read the header.";
		return false;
	}
	
	// The header is already valid. Get the Raster block header.
	if(m_header.IsRasterBlock()) {
		fIn.seek(m_header.GetRasterBlockPos());
		// read the Raster block head (but not the actual data).
		m_raster.ReadHead(fIn);
	}

	// Get the Vector block header.
	if(m_header.IsVectorBlock()) {
		fIn.seek(m_header.GetVectorBlockPos());
		// read the Vector block head (but not the actual data)
		m_vector.ReadHead(fIn);
	}
	
	m_uiId = GetIdFromLonLat(m_header.GetOriginLongitude(), m_header.GetOriginLatitude());
	return true;
}

// ----------------------------------------------------------------------------

void Block::DrawVector(QPainter* pP, const Mapper* pM) const
{
	m_vector.Draw(pP, pM);
}

// ----------------------------------------------------------------------------

int Block::GetUsedMemory() const
{
	int iMem = sizeof(Block);
	iMem += m_qsFileName.capacity()*2;
	
	iMem += sizeof(BlockHeader);
	iMem += m_raster.GetUsedMemory();
	iMem += m_vector.GetUsedMemory();
	
	return iMem;
}

// ----------------------------------------------------------------------------

}
