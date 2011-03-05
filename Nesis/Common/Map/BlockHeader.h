#ifndef __MAP_BLOCK_HEADER_H
#define __MAP_BLOCK_HEADER_H

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

#include <QFile>
#include <QDate>

//! Each block header occupies 64 bytes.
#define MBH_HEADER_SIZE	64

namespace map {

// ------------------------------------------------------------------------ 
/*! \brief A block header object of the 'kam' file format. Read, write, parse.
	
	This is a helper class that reads and parses or writes a header of 
   the 'kam' file format. The \a Block class uses this object.

   The header represents first 64 bytes in the 'kam' file. It stores the 
   'magic number', version, location and some file pointers for quick access
   to the underlying data. It also reserves several bytes for possible future
	use.

	The basic principle is that we are able to read any old format, but we 
	always write in the latest one.

	Mostly, the class provides read only methods of internal variables.
	The origin of the block is in the south-west corner of the block.
 */

class BlockHeader {
public:
	//! BlockHeader contructor.
	BlockHeader();
	//! Copy constructor.
	BlockHeader(const BlockHeader& C);
	//! Destructor is public, so that compiler may access it.
	~BlockHeader();
	
	//! Read the header from the stream and parse it.
	bool Read(QIODevice& fIn);
	//! Write the header into the given stream using given parameters.
	static bool Write(QIODevice& out, 
				  			float fOriginLon, float fOriginLat, 
				  			float fArcLon, float fArcLat,
				  			quint64 uiRasterPos, quint64 uiVectorPos);
	
	//! Get header-file version.
	int GetVersion() const
	{ return m_uiVersion; }
	//! Get file creation date stored in the header.
	QDate GetCreationDate() const
	{ return QDate::fromJulianDay(m_iJulianDay); }
	
	//! Get block origin longitude in decimal degrees.
	float GetOriginLongitude() const
	{ return m_fOriginLon; }
	//! Get block origin latitude in decimal degrees.
	float GetOriginLatitude() const
	{ return m_fOriginLat; }
	
	//! Get block longitude arc in decimal degrees.
	float GetArcLongitude() const
	{ return m_fArcLon; }
	//! Get block latitude arc in decimal degrees.
	float GetArcLatitude() const
	{ return m_fArcLat; }
	
	//! Return true if there is a raster block data available.
	bool IsRasterBlock() const
	{ return m_uiRasterPos > 0; }
	//! Return true if there is a vector block data available.
	bool IsVectorBlock() const
	{ return m_uiVectorPos > 0; }
	
	//! Get the raster block position (seek position).
	quint64 GetRasterBlockPos() const
	{ return m_uiRasterPos; }
	//! Get the vector block position (seek position).
	quint64 GetVectorBlockPos() const
	{ return m_uiVectorPos; }
	
	
private:
	//! File version.
	quint16 m_uiVersion;
	//! File creation date in JD format (qint32).
	qint32 m_iJulianDay;
	
	//! Block origin longitude (south-west corner coordinates) in decimal degrees.
	float m_fOriginLon;
	//! Block origin latitude (south-west corner coordinates) in decimal degrees.
	float m_fOriginLat;
	//! Size of block longitude arc in decimal degrees.
	float m_fArcLon;
	//! Size of block latitude arc in decimal degrees.
	float m_fArcLat;
	
	//! File seek position of the first raster data byte in the kam file.
	quint64 m_uiRasterPos;
	//! File seek position of the first vector data byte in the kam file.
	quint64 m_uiVectorPos;
};
// ------------------------------------------------------------------------ 

} // map namespace

#endif	// __MAP_BLOCK_HEADER_H
