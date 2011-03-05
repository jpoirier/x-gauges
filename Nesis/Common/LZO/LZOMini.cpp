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

#include <QDebug>
#include "CommonDefs.h"
#include "minilzo.h"
#include "LZO.h"

// --------------------------------------------------------------------------
/** Decompress data from \a in stream. Decompression is in-place.
    This is a convenience function for in-place LZO decompression 
    of data which comes from stream. The data in stream must 
    be preprared in proper order. Actual compressed data follows
	 behind of special 8 bytes, which tell size of block and
    size of uncompressed data.
	 
    0-3 : 4 bytes = size of total block (without this 4 bytes),
	 4-7 : 4 bytes = size of uncompressed data (already in the block),
	 8-* : compressed block of data.

	 \param in Input stream of data.

	 \retval QByteArray - uncompressed data or empty array on error.
 */
QByteArray DecompressMiniLZO(QDataStream& in)
{
	Q_ASSERT(in.device()->isOpen() && in.device()->isReadable());
	// Input format is as follows
	// 0-3 : 4 bytes = size of total block (without this 4 bytes),
	// 4-7 : 4 bytes = size of uncompressed data (already in the block),
	// 8-* : compressed block of data.
	
	// Size of overhead (required by decompressor)
	const uint uiOverhead = 16;
	
	// Read byte array size (compressed size)
	quint32 uiBaSize;
	in >> uiBaSize;
	
	// Read decompressed size
	quint8 acDataDS[4];
	in.readRawData((char*)acDataDS,4);
	quint32 uiDecomSize = 
		(acDataDS[0] << 24) | (acDataDS[1] << 16) |
		(acDataDS[2] <<  8) | (acDataDS[3]      );
	
/*	quint32 uiDecomSize;
	in >> uiDecomSize;*/
	
	// We have already read 4 bytes in block (decompressed size).
	uiBaSize -= 4;

	// Construct QByteArray for in-place decompression.
	QByteArray ba;
	ba.resize(qMax(uiDecomSize, uiBaSize)+uiOverhead);
	
	// Offset tells where to copy the first by of the compressed block.
	quint32 uiOffset = ba.size()-uiBaSize;
	
	// Read byte array into the position (read 1 Mb blocks or less).
	const quint32 uiStep = 1024 * 1024;
	quint32 uiAllocated = 0;
	do {
		int iBlockSize = qMin(uiStep, uiBaSize - uiAllocated);
		if (in.readRawData(ba.data() + uiAllocated + uiOffset, iBlockSize) != iBlockSize) {
			ba.clear();
			in.setStatus(QDataStream::ReadPastEnd);
			return ba; // this indicates an error.
		}
		uiAllocated += iBlockSize;
	} while (uiAllocated < uiBaSize);
	
	// Now, do the in-place decompression inside the ba object.
	lzo_uint lzoDecomSize;
	unsigned char* acData = (unsigned char*)ba.constData();
	int iRet = lzo1x_decompress(
		acData+uiOffset,	// pointer to the first compressed byte.
		uiBaSize,			// size of compressed block.
		acData,				// pointer to the first decompressed byte.
		&lzoDecomSize,		// pointer to the uint which receives the uncompressed size.
		NULL					// working buffer.
	);
	
	if(iRet != LZO_E_OK) {
		qDebug() << "LZO Decompression failed! Ret =" << iRet;
		return QByteArray();
	}
	
	Q_ASSERT(lzoDecomSize == uiDecomSize);
	
	// Resize the ba object -- drop the overhead.
	ba.resize(lzoDecomSize);
			
	return ba;
}

// --------------------------------------------------------------------------
