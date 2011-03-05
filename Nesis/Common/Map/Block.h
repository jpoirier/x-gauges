#ifndef __MAP_BLOCK_H
#define __MAP_BLOCK_H

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

#include <QString>
//#include <QVector>
#include "BlockHeader.h"
#include "BlockRaster.h"
#include "BlockVector.h"
#include "TextLabel.h"

// Forward declarations
class QPainter;

namespace map {
	
// Forward declarations
class Mapper;
	
// ------------------------------------------------------------------------ 
/*! \brief A block 1 deg by 1 deg of geographics information.
 
   Objects of class Block work hand in hand with the \a World singleton 
   object. A block is a dynamic entity which is loaded on demand and
   discarded whnen it is not needed anymore. The block provides geographics
	information within its region. The block size is 1 deg by 1 deg.

	Information inside the block consists of vector data (several vector
   layers), raster data and block header data.
 */

class Block {
public:
	//! Block contructor.
	Block();
	//! Destructor.
	~Block();
	
	//! Load block data for given block key and folder path.
	bool Load(quint16 uiBlock, const QString& qsPath);
	//! Load block data from given file name.
	bool Load(const QString& qsFile);
	
	//! Return this objects key.
	quint16 GetKey() const
	{ return m_uiId; }

	//! Return the block file name.
	const QString& GetFileName() const
	{ return m_qsFileName; }
	
	//! Increase activity counter.
	void IncreaseActivity();
	//! Decrease activity counter.
	void DecreaseActivity()
	{ m_iActivityCount--; }
	//! Return activity counter.
	int GetActivity() const 
	{ return m_iActivityCount; }

	//! Set raster resolution for given "pixels per degree" scale.
	int SetResolution(int iPixPerDegree);
	//! Assure that raster and vector data is ready for use. Load from file if necessary.
	bool AssureData();
	
	//! Get the elevation for the given longitude, latitude.
	qint16 GetElevation(float fLon, float fLat) const;
	
	//! Get vector of elevations limited by block, i and e iterators. 
	float GetElevation(float fLon, float fLat, float fArc, 
							QVector<qint16>::iterator& i, 
							const QVector<qint16>::const_iterator& e) const;
	
	//! Draw the vector graphics.
	void DrawVector(QPainter* pP, const Mapper* pM) const;
	
	//! Calculate and return memory used by this block.
	int GetUsedMemory() const;
	
	//! Build list ob text labels used by this block.
	void GetTextLabels(const Mapper* pM, const QRectF& rClip, QList<TextLabel*>& conLabels) const;

	//! Return read only access to the header part.
	const BlockHeader& GetHeader() const
	{ return m_header; }
	//! Return read only access to the raster part.
	const BlockRaster& GetRaster() const
	{ return m_raster; }
	//! Return read only access to the vector part.
	const BlockVector& GetVector() const
	{ return m_vector; }
		
private:
	//! Block id - may be used as a key.
	quint16 m_uiId;
	//! Block activity counter (how many times it was used in painting.)
	int m_iActivityCount;
	//! Store the file name after load. We need it for raster and vector data. 
	QString m_qsFileName;
	//! The block information.
	BlockHeader m_header;
	//! The Raster information.
	BlockRaster m_raster;
	//! The Vector information.
	BlockVector m_vector;
};

// ------------------------------------------------------------------------ 

inline qint16 Block::GetElevation(float fLon, float fLat) const
{
	fLon -= GetLonFromId(m_uiId);
	fLat -= GetLatFromId(m_uiId);
	// This is the job for the raster block.
	return m_raster.GetElevation(fLon, fLat);
}

inline float Block::GetElevation(
	float fLon, float fLat, float fArc, 
	QVector<qint16>::iterator& i, const QVector<qint16>::const_iterator& e) const
{
	float fL0 = GetLonFromId(m_uiId);
	fLon -= fL0;
	fLat -= GetLatFromId(m_uiId);
	// This is the job for the raster block.
	return m_raster.GetElevation(fLon, fLat, fArc, i, e) + fL0;
}

// ----------------------------------------------------------------------------

inline int Block::SetResolution(int iPixPerDegree)
{
	return m_raster.SetResolution(iPixPerDegree);
}
// ----------------------------------------------------------------------------

inline bool Block::AssureData()
{
	// TODO: Can we open the file just once and pass the IODevice instead?
	bool bVec = m_vector.AssureData(m_qsFileName);
	bool bRas = m_raster.AssureData(m_qsFileName);
	return (bVec && bRas); 
}

// ----------------------------------------------------------------------------

inline void Block::GetTextLabels(
	const Mapper* pM, 
	const QRectF& rClip, 
	QList<TextLabel*>& conLabels
) const
{
	m_vector.GetTextLabels(pM, rClip, conLabels);
}

// ----------------------------------------------------------------------------

inline void Block::IncreaseActivity()
{ 
	m_iActivityCount++; 
	if(m_iActivityCount > 10) 
		m_iActivityCount = 10; 
}


} // map namespace

#endif	// __MAP_BLOCK_H
