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

#include <algorithm>
#include <QtDebug>
#include "World.h"

namespace map {

QMutex World::s_mutex;
World* World::s_pInstance = NULL;

// ----------------------------------------------------------------------------

World::World()
{
	m_conBlock.reserve(100);
	m_pActiveBlock = NULL;
	m_iPixPerDegree = 120; // 30"
	m_bPainting = false;
	m_iPaintingCount = 0;
}

World::~World()
{
	foreach(Block* pBlock, m_conBlock)
		delete pBlock;
}
// ----------------------------------------------------------------------------

World* World::Lock()
{
	s_mutex.lock();
	if(!s_pInstance)
		s_pInstance = new World;
	return s_pInstance;
}

// ----------------------------------------------------------------------------

World* World::TryLock()
{
	if(s_mutex.tryLock()) {
		if(!s_pInstance) {
			s_pInstance = new World;
		}
/*		if(s_pInstance->m_bPainting == true) {
			s_mutex.unlock();
			return NULL;
		}*/
		return s_pInstance;
	}
	return NULL;
}

// ----------------------------------------------------------------------------

void World::Unlock()
{
	s_mutex.unlock();
}

// ----------------------------------------------------------------------------

void World::SetPixPerDegree(int iPixPerDegree)
{
	// remember current scale
	m_iPixPerDegree = iPixPerDegree;
	foreach(Block* pBlock, m_conBlock)
		pBlock->SetResolution(m_iPixPerDegree);
}

// ----------------------------------------------------------------------------

void World::BeginPaint()
{
	Q_ASSERT(m_bPainting == false);
	// clear the list of active blocks.
	m_conActiveBlock.clear();
	// Set active block to zero
	m_pActiveBlock = NULL;
	// Sea block
	m_iLastSeaBlock = 0;

	// Mark painting
	m_bPainting = true;

	// Update painting counter
	m_iPaintingCount++;
}
// ----------------------------------------------------------------------------

void World::EndPaint(bool bForceGarbageCollection)
{
	Q_ASSERT(m_bPainting);

// 	std::cout << "Number of blocks " << m_conBlock.size() << std::endl;

	// Each block that was participating in the last paint gets one point.
	foreach(Block* pB, m_conActiveBlock)
		pB->IncreaseActivity();
	m_bPainting = false;

	// Do we need to perform garbage collection
	if(m_iPaintingCount == 10
		|| bForceGarbageCollection
		|| m_conBlock.size() > 20) {
		CollectGarbage();
		m_iPaintingCount = 0;
	}
}

// ----------------------------------------------------------------------------

Block* World::GetBlock(float fLon, float fLat)
{
	quint16 uiBlockKey = GetIdFromLonLat(fLon, fLat);

	// Try to reuse the last block
	if(m_pActiveBlock) {
		if(m_pActiveBlock->GetKey()==uiBlockKey) {
			// Bingo ...
			return m_pActiveBlock;
		}
	}

	// Check last sea block again.
	if(uiBlockKey == m_iLastSeaBlock)
		return NULL;

	// Locate the block
	Block* pB = m_conBlock.value(uiBlockKey, NULL);

	// Load the block if not found.
	if(pB == NULL) {
		// Before we open the file, search the known sea blocks.
		// This takes some time, but it is significantly faster than
		// search for a non-existing file.
		QVector<quint16>::iterator isb;
		isb = std::lower_bound(m_conSeaBlocks.begin(), m_conSeaBlocks.end(), uiBlockKey);
		if(isb != m_conSeaBlocks.end()) {
			if(*isb == uiBlockKey) {
				m_iLastSeaBlock = uiBlockKey;
				return NULL;
			}
		}

		// Try to open the file
		pB = new Block();
		// if it fails, it is a sea block. Remember it.
		if(pB->Load(uiBlockKey, m_qsKamPath) == false) {
			delete pB;
			m_iLastSeaBlock = uiBlockKey;
			m_conSeaBlocks.insert(isb, uiBlockKey);
			return NULL;
		}
		// insert the block into container
		m_conBlock.insert(uiBlockKey, pB);
		// Set current resolution and thus set correct raster record.
		pB->SetResolution(m_iPixPerDegree);
	}

	// Put the block on the active block list
	if(m_conActiveBlock.value(uiBlockKey, NULL)==NULL) {
		m_conActiveBlock.insert(uiBlockKey, pB);
		// Load raster and vector data if necessary.
		if(pB->AssureData()==false) {
			qWarning() << "Corrupted block data in file" << pB->GetFileName();
			// For some reason the block information is corrupted.
			// This is most probably due to corrupted file.
			// Take block out of the active blocks list.
			m_conActiveBlock.remove(uiBlockKey);
			// Put block into "sea list". This is sorted list.
			QVector<quint16>::iterator isb;
			isb = std::lower_bound(m_conSeaBlocks.begin(), m_conSeaBlocks.end(), uiBlockKey);
			m_conSeaBlocks.insert(isb, uiBlockKey);
			m_iLastSeaBlock = uiBlockKey;
			// Remove the block from container of blocks.
			m_conBlock.remove(uiBlockKey);
			// Delete the block object.
			delete pB;
			return NULL;
		}
	}

	// make the last block the active one.
	m_pActiveBlock = pB;
	return pB;
}

// ----------------------------------------------------------------------------

qint16 World::GetElevation(const QPointF& ptWGS)
{
	float fLon = ptWGS.x();
	float fLat = ptWGS.y();

	Block* pB = GetBlock(fLon, fLat);
	if(pB)
		return pB->GetElevation(fLon, fLat);
	else
		return SEA_SRTM_ELEVATION;
}

// ----------------------------------------------------------------------------

void World::GetElevation(
	const QPointF& ptWGS,
	float fArc,
	QVector<qint16>& vElevation
)
{
	float fLon = ptWGS.x();
	float fLat = ptWGS.y();

	QVector<qint16>::iterator i = vElevation.begin();
	const QVector<qint16>::const_iterator e = vElevation.end();

	while(i!=e) {
		Block* pB = GetBlock(fLon, fLat);
		// while sea
		while(pB==NULL && i!=e) {
			*i++ = SEA_SRTM_ELEVATION;
			fLon+=fArc;
			pB = GetBlock(fLon, fLat);
		}
		// a block
		if(i!=e) {
			fLon = pB->GetElevation(fLon, fLat, fArc, i, e);
		}
	}
}

// ----------------------------------------------------------------------------

QVector<const Block*> World::GetActiveBlocks() const
{
	QVector<const Block*> vpBlock;
	foreach(const Block* pB, m_conActiveBlock)
		vpBlock.push_back(pB);

	return vpBlock;
}

// ----------------------------------------------------------------------------

void World::CollectGarbage()
{
	// Get (approximate memory used).
/*	int iMemUsed = 0;*/
	QVector<quint16> conBlocksToRemove;

	foreach(Block* pB, m_conBlock) {
// 		iMemUsed += pB->GetUsedMemory();
		pB->DecreaseActivity();
		if(pB->GetActivity() <= 0)
			conBlocksToRemove << pB->GetKey();
	}

// 	std::cout << "Initial memory used by " << m_conBlock.count() << " blocks " << iMemUsed/(1024*1024) << "MB.\n";

	std::cout << "Garbage collection" << std::endl;

	foreach(quint16 key, conBlocksToRemove) {
		delete m_conBlock.take(key);
	}

/*	iMemUsed = 0;
	foreach(Block* pB, m_conBlock) {
		iMemUsed += pB->GetUsedMemory();
		qDebug() << "Block id " << pB->GetKey() << " - " << pB->GetActivity();
	}
	std::cout << "Memory used by " << m_conBlock.count() << " blocks after cleanup = "
			    << iMemUsed/(1024*1024) << "MB.\n";*/
}

// ----------------------------------------------------------------------------
}
