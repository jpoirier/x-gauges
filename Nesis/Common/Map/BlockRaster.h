#ifndef __MAP_BLOCK_RASTER_H
#define __MAP_BLOCK_RASTER_H

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

#include <QByteArray>
#include "CommonDefs.h"

namespace map {
// ------------------------------------------------------------------------ 

//! Maximal number of raster records inside the raster block.
#define MBR_MAX_RASTER_RECORDS	8
//! Each block raster header occupies 128 bytes.
#define MBR_HEADER_SIZE	         128
	
// ------------------------------------------------------------------------ 
/*! \brief A raster block 1 deg by 1 deg of elevation information.
 
	The raster blocks origin from the SRTM (Shuttle Radat Topography Mission)
	data provided by the NASA. We simply repacked the same data with
	additional resources from GLOBE, GTOPO30 and from
	http://www.viewfinderpanoramas.org/dem3.html#hgt internet resource.
 */

class BlockRaster {
	//! Internal raster record class.
	class Record {
	public:
		//! Constructor.
		Record();
		//! Clear the record - reset the record.
		void Clear();
		
		//! Get data - elevation data for given (trimmed!) longitude, latitude.
		qint16 GetElevation(float fLon, float fLat) const;
		
		//! Get interpolated data - elevation data for given longitude, latitude.
		qint16 GetInterpolatedElevation(float fLon, float fLat) const;

		//! Return true if data has not been loaded yet.
		bool IsEmpty() const
		{ return m_baData.isEmpty(); }

		//! Read the data from the file.
		bool Read(QIODevice& fIn);
	private:
		//! Get correct elevation for given pixel coordinates (ix, iy). Note: iy points up!
		qint16 GetData(int ix, int iy) const
		{
			int p = ((m_n-iy)*(m_n_1)+ix);
			return ((qint16*)(m_baData.constData()))[p]; 	
// old big endian code
//			int p = ((m_n-iy)*(m_n+1)+ix) << 1;
//			return (qint16)((((quint16)m_baData[p])<< 8) | ((quint8)m_baData[p+1])); 	
		}
	public:
		//! Position of the record in the file.
		quint64 m_uiPos;
		//! Array of record bytes - the elevation data itself.
		QByteArray m_baData;
		//! Resolution - number of elements in one record line.
		int m_n;
		//! Resolution + 1
		int m_n_1;
		//! Data compression flag.
		bool m_bCompressed;
	};
public:
	//! BlockRaster contructor.
	BlockRaster();
	//! Destructor.
	~BlockRaster();
	
	//! Clear (reset) the raster block object.
	void Clear();

	//! Check is the active data is empty -- the data has not been loaded yet.
	bool IsEmpty() const;
	
	//! Set active resolution according to the given scale resolution (pixel per degree).
	int SetResolution(int iRequestedRes_ppd);

	//! Assure that raster data is ready for use. Load from file if necessary.
	bool AssureData(const QString& qsFile);

	//! Read the raster head data from the given stream.
	bool ReadHead(QIODevice& fIn);

	//! Read the raster data for the active resolution.
	bool ReadData(QIODevice& fIn);

	//! Get elevation for given coordinates.
	qint16 GetElevation(float fLon, float fLat) const;
		
	//! Get elevation vector ...
	float GetElevation(
			float fLon, float fLat, float fArc, 
			QVector<qint16>::iterator& i, 
			const QVector<qint16>::const_iterator& e) const;

	//! Get interpolated elevation for given coordinates (slower, but better).
	qint16 GetInterpolatedElevation(float fLon, float fLat) const;
	
	//! Calculate memory used by this object.
	int GetUsedMemory() const;

	//! Return number of internal raster records available.
	int GetRecordCount() const
	{ return m_iRecordCount; }
	//! Return resolution of the i-th record.
	int GetRecordResolution(int iRec) const;

private:
	//! Raster file records stored in RAM. Each record has different resolution.
	Record m_aRecord[MBR_MAX_RASTER_RECORDS];
	//! Currently active record.
	Record* m_pActiveRecord;
	//! Number of valid records.
	int m_iRecordCount;
};
// ------------------------------------------------------------------------ 

inline bool BlockRaster::IsEmpty() const
{	
	ASSERT(m_pActiveRecord!=NULL);
	return m_pActiveRecord->IsEmpty(); 
}

// ------------------------------------------------------------------------ 

inline qint16 BlockRaster::GetElevation(float fLon, float fLat) const
{
	ASSERT(m_pActiveRecord!=NULL);
//	return m_pActiveRecord->GetInterpolatedElevation(fLon, fLat); 
	return m_pActiveRecord->GetElevation(fLon, fLat); 
}

// ------------------------------------------------------------------------ 

inline qint16 BlockRaster::GetInterpolatedElevation(float fLon, float fLat) const
{
	ASSERT(m_pActiveRecord!=NULL);
	return m_pActiveRecord->GetInterpolatedElevation(fLon, fLat); 
}

// ------------------------------------------------------------------------ 

/*! Calculate the elevation for given coordinates. The coordinates must be
    trimmed to the block - they must be in the range [0,1].
	 Coordinate (0,0) represent the SW corner while coordinate (1,1) represent
    the NE corner.

	 \retval qint16 -- altitude of the nearest raster data in meters.
 */
inline qint16 BlockRaster::Record::GetElevation(
	float fLon,	//!< Longitude trimmed to this raster block [0,1].
	float fLat	//!< Latitude trimmed to this raster block [0,1].
) const
{
	return GetData(fLon*m_n, fLat*m_n);
// 	ASSERT(m_baData.size() == 2*(m_n+1)*(m_n+1));
// 	ASSERT(fLon >= 0.0f && fLon <=1.0);
// 	ASSERT(fLat >= 0.0f && fLat <=1.0);

/*	int ix = (int)(fLon*m_n + 0.5f);
	int iy = (int)(fLat*m_n + 0.5f);*/
// 	ASSERT(ix<=m_n && iy<=m_n);

// 	return GetData(ix, iy);
}

// ----------------------------------------------------------------------------

inline float BlockRaster::GetElevation(
	float fLon,
	float fLat,
	float fArc,
	QVector<qint16>::iterator& i, const QVector<qint16>::const_iterator& e
) const
{
	while(i!=e && fLon<=1.0) {
// 		int iE = GetElevation(fLon, fLat);
		*i++ = GetElevation(fLon, fLat);
		fLon += fArc;
	}
	return fLon;
}

// ----------------------------------------------------------------------------

} // map namespace

#endif	// __MAP_BLOCK_RASTER_H
