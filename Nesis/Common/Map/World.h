#ifndef __MAP_WORLD_H
#define __MAP_WORLD_H

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

#include <memory>
#include <QHash>
#include <QVector>
#include <QPointF>
#include <QMutex>
#include "Map/Block.h"

namespace map {
/*! \brief This is singleton class used to access the world map information.
 *
 *  All information provided by this class is compatible to WGS84
 *  (World Geodetic System - see http://en.wikipedia.org/wiki/WGS84).
 *
 *  Only one thread can have access to the class at time. In order
 *  to get access to the class, call the Lock() function. This function
 *  will block until the mutex is available. Once, it is available, it
 *  will return a pointer to the World object. You have the full access
 *  to the object until you call the Unlock(). You must not use the
 *  pointer to the World object after the Unlock(), since some other 
 *  thread may be using it.
 *  When you only 'wish' to have a world object, but you can go without as
 *  well, then use the TryLock(). This will either return non-NULL and 
 *  lock the object on success or return NULL without locking it.
 *
 *  Typically, once a thread locks the object, it will be locked for some
 *  time. In most cases - a complete background painting needs to be
 *  finished before unlocking the class.
 */

class World 
{
	//! Internal container of map::Block objects.
	typedef QHash<quint16, Block*> BlockCon;
//	typedef QHash<quint16, const Block*> ConstBlockCon;

	//! Contructor is hidden - this is a singleton class.
	World();
public:
	//! Destructor is public, so that compiler may access it.
	~World();
	
	//! The only access to the World static object. It locks mutex.
	static World* Lock();
	//! The 'wish' access to the World static object. It locks mutex when returns non-NULL.
	static World* TryLock();
	
	//! Release access to the World static object. In unlocks mutex.
	static void Unlock();

	//! Set the path to the kam files.
	void SetKamPath(const QString& qsKamPath)
	{ m_qsKamPath = qsKamPath; }
	
	//! Set current raster resolution (depends on the scale selection).
	void SetPixPerDegree(int iPixPerDegree);

	//! Prepare the system for a new paint operation.
	void BeginPaint();
	
	//! End painting process.
	void EndPaint(bool bForceGarbageCollection = false);
	
	//! Get the elevation for the given longitude, latitude given in degrees.
	qint16 GetElevation(const QPointF& ptWGS);
	
	//! Get required elevations in x direction from given point in degrees,
	void GetElevation(const QPointF& ptWGS, float fArc, QVector<qint16>& vElevation);

	//! Get the list of the blocks that were used in the last paint.
	QVector<const Block*> GetActiveBlocks() const;
	
private:
	//! Get block for the given point. Update internal objects if the block must be loaded.
	Block* GetBlock(float fLon, float fLat);
	
	//! Perform garbage collection. Remove unused blocks and free memory.
	void CollectGarbage();

private:
	//! We need mutex to prevent simultaneous access to some object functions.
	static QMutex s_mutex;
	//! Wrapper for the only instance of the World object.
	static World* s_pInstance;
	
	//! Internal flag which is true if the painting is active.
	bool m_bPainting;
	
	//! Internal paint situation counter. Used for releasing unused blocks.
	int m_iPaintingCount;

	//! Path to the kam files (used to load the block objects).
	QString m_qsKamPath;
	
	//! Current raster resolution.
	int m_iPixPerDegree;

	//! Container of Block objects.
	BlockCon m_conBlock;
	
	//! We keep a pointer to the active (last accessed) block.
	Block* m_pActiveBlock;

	//! A list (container) of blocks that was accessed recently.
	BlockCon m_conActiveBlock;
	
	//! Last invalid ("sea") block;
	quint16 m_iLastSeaBlock;

	//! List of known sea blocks.
	QVector<quint16> m_conSeaBlocks;
};

} // map namespace

#endif	// __MAP_WORLD_H
