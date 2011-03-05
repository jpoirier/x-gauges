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
#include "CommonDefs.h"
#include "BlockHeader.h"

namespace map {

#define MAGIC_NUMBER   0xCA6BB6AC // 32 bit magic number	
#define KAM_VERSION    1

// ----------------------------------------------------------------------------
	
BlockHeader::BlockHeader()
{
	m_uiVersion  = 0;	// invalid version
	
	m_fOriginLon = m_fOriginLat = 0.0f; // invalid origin
	m_fArcLon    = m_fArcLat    = 0.0f; // invalid arcs
	
	m_uiRasterPos = m_uiVectorPos = 0;  // no raster not vector data
}

BlockHeader::BlockHeader(const BlockHeader& C)
{
	m_uiVersion  = C.m_uiVersion;
	m_iJulianDay = C.m_iJulianDay;
	
	m_fOriginLon = C.m_fOriginLon;
	m_fOriginLat = C.m_fOriginLat;
	m_fArcLon    = C.m_fArcLon;
	m_fArcLat    = C.m_fArcLat;
	
	m_uiRasterPos = C.m_uiRasterPos;
	m_uiVectorPos = C.m_uiVectorPos;
}

BlockHeader::~BlockHeader()
{
}

// ----------------------------------------------------------------------------
/*! Read and parse the header block from the given input device stream. 
    The stream must be random access, binary and positioned to 0.

	 \retval true the header was read successfully.
	 \retval false the header failed to read for some reason.
 */
bool BlockHeader::Read(
	QIODevice& fIn	//! Input stream (binary, random access).
							 )
{
	ASSERT(fIn.isSequential()==false);
	ASSERT(fIn.isReadable()==true);
	ASSERT(fIn.isTextModeEnabled()==false);
	ASSERT(fIn.pos() == 0);
	ASSERT(sizeof(float) == 4);
	
	QDataStream in(&fIn);
	// Check the magic number
	quint32 uiMagic;
	in >> uiMagic;
	// OK, try to read the rest of the header.
	if(uiMagic == MAGIC_NUMBER) {
		in >> m_uiVersion;
		in >> m_iJulianDay;
		
		in >> m_fOriginLon;
		in >> m_fOriginLat;
		in >> m_fArcLon;
		in >> m_fArcLat;
		
		in >> m_uiRasterPos;
		in >> m_uiVectorPos;
		
		// The next 24 bytes are reserved. 
		// They should be zero, but we do not read them, just skip them.
		// Some additional checks are fast and harmless.
		if(   m_uiVersion >= 1 
		   && m_iJulianDay > 2454110 // 9.Jan.2007
			&& m_fArcLon >= 1.0f
			&& m_fArcLat >= 1.0f
			&& fIn.size() >= MBH_HEADER_SIZE
		  ) {
			fIn.seek(MBH_HEADER_SIZE); // position the file pointer to the 64-th byte.
			return true;
		}
	}
	// failed in any other case.
	return false;
}

// ----------------------------------------------------------------------------
/*! Write the header into the output stream. This is static function and thus
    does not affect any class member. It is meant to be used by MapGenerator 
    program, which will generate '.kam' formated files from various source files.

	 The \a out (output device) must have random-access, binary and writable 
    attributes.

	 \retval true on success.
	 \retval false on failure.
 */
bool BlockHeader::Write(
	QIODevice& fOut, 		//!< Output device.
	float fOriginLon, 	//!< Origin longitude (west edge of the block).
	float fOriginLat, 	//!< Origin latitude (south edge of the block).
	float fArcLon,			//!< Longitude arc (size) of the block. 
	float fArcLat,			//!< Latitude arc (size) of the block.
	quint64 uiRasterPos, //!< File position of the zero byte of the raster data block.
	quint64 uiVectorPos	//!< File position of the zero byte of the vector data block.
)
{
	ASSERT(fOut.isSequential()==false);
	ASSERT(fOut.isWritable()==true);
	ASSERT(fOut.isTextModeEnabled()==false);
	ASSERT(fOut.pos() == 0);
	ASSERT(sizeof(float) == 4);
	
	QDataStream out(&fOut);
	
	quint32 uiMagic = MAGIC_NUMBER;
	quint16 uiVersion = KAM_VERSION;
	qint32  iJulianDay = QDate::currentDate().toJulianDay();
	
	out << uiMagic;
	out << uiVersion;
	out << iJulianDay;
		
	out << fOriginLon;
	out << fOriginLat;
	out << fArcLon;
	out << fArcLat;
		
	out << uiRasterPos;
	out << uiVectorPos;
	
	// write zero bytes to complete the block header.
	while(fOut.pos() < MBH_HEADER_SIZE) {
		out << (qint8)0;
	}
	return true;
}

}
