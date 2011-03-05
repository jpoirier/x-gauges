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

#include <limits>
#include <QDataStream>
#include <QtDebug>
#include <QFile>
#include "CommonDefs.h"
#include "LZO/LZOMini.h"

#include "BlockRaster.h"

//! Used internally to make the low resolution selection more attractive.
/*! Larger value gives more chance that the lower resolution is selected
    for the active resolution. Any value larger than 1 can be used.
	 Value 1 gives 50 - 50 % interval division. Value 2 gives 66 - 34 %. Value 3
    gives 75 - 25 %, etc.
 */
#define POSITIVE_SLOPE_REDUCTION	1 	// % = PSR/(PSR+1)
#define NEGATIVE_SLOPE_REDUCTION	5 	// % = PSR/(PSR+1)

namespace map {
// ----------------------------------------------------------------------------

BlockRaster::Record::Record()
{
	m_uiPos = 0; // this is invalid position.
	m_n     = 0; // invalid raster.
	m_n_1   = 0;
	m_bCompressed = false;
}

// ----------------------------------------------------------------------------

void BlockRaster::Record::Clear()
{
	m_uiPos = 0; // this is invalid position.
	m_n     = 0; // invalid raster.
	m_n_1   = 0;
	m_bCompressed = false;
	m_baData.resize(0);
}

// ----------------------------------------------------------------------------
/*! Calculate the interpolated elevation for given coordinates.
	 The coordinates must be trimmed to the block - they must be in the range [0,1].
	 Coordinate (0,0) represent the SW corner while coordinate (1,1) represent
    the NE corner.

	 This is a more precise but slower function than the similar #GetData() function;

	 \retval qint16 -- interpolated altitude in meters.
 */

qint16 BlockRaster::Record::GetInterpolatedElevation(float fLon, float fLat) const
{
	ASSERT(m_baData.size() == 2*(m_n_1)*(m_n_1));
	ASSERT(fLon >= 0.0f && fLon <=1.0);
	ASSERT(fLat >= 0.0f && fLat <=1.0);

	// resolution step size
	float fS = 1.0/m_n;

	// interpolation bounding box (ixl,iyl) -- (ixh,iyh)
	int ixl = (int)(fLon/fS);
	if(ixl == m_n)
		ixl--;
	int ixh = ixl+1;
	// ksi
	float fdx = 2*(fLon-ixl*fS)-1;

	int iyl = (int)(fLat/fS);
	if(iyl == m_n)
		iyl--;
	int iyh = iyl+1;
	// eta
	float fdy = 2*(fLat-iyl*fS)-1;

	// bilinear interpolation in [-1,1] x [-1,1] square.
	float fz = (1.0f-fdx)*(1.0f-fdy)*GetData(ixl,iyl);
	fz      += (1.0f+fdx)*(1.0f-fdy)*GetData(ixh,iyl);
	fz      += (1.0f-fdx)*(1.0f+fdy)*GetData(ixl,iyh);
	fz      += (1.0f+fdx)*(1.0f+fdy)*GetData(ixh,iyh);

	return (qint16)fz;
}

// ----------------------------------------------------------------------------

bool BlockRaster::Record::Read(QIODevice& fIn)
{
	// Load the data into byte array.
	fIn.seek(m_uiPos);
	QDataStream in(&fIn);

	m_baData = DecompressMiniLZO(in);
	// the resolution and data size must match exactly.
	if(m_baData.size() != 2*pow(m_n_1,2)) {
		std::cerr << "Error(BlockRaster::Record::Read): Resolution and data size don't match.";
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------------

BlockRaster::BlockRaster()
{
	m_pActiveRecord = NULL;
	m_iRecordCount  = 0;
}

BlockRaster::~BlockRaster()
{
}
// ----------------------------------------------------------------------------

void BlockRaster::Clear()
{
	m_iRecordCount = 0;
	m_pActiveRecord = NULL;
	for(int i=0; i<MBR_MAX_RASTER_RECORDS; i++)
		m_aRecord[i].Clear();
}

// ----------------------------------------------------------------------------
/*! Read the raster (elevation) block data from the stream. The file position
    (seek) must be properly set by the caller.
 */
bool BlockRaster::ReadHead(
	QIODevice& fIn 	//!< The binary file stream.
)
{
	// In stream object
	QDataStream in(&fIn);

	// read the version
	quint16 uiVersion;
	in >> uiVersion;
	Q_ASSERT(uiVersion > 0);

	// Clear the object (in the case if we are reusing an old object)
	if(m_iRecordCount)
		Clear();

	// Read record resolutions and offsets.
	for(int i=0; i<MBR_MAX_RASTER_RECORDS; i++) {
		quint64 uiPos;
		qint16  iRes;
		qint8   iZip;

		in >> uiPos;
		in >> iRes;
		in >> iZip;
		if(iRes != 0 && uiPos != 0) {
			m_aRecord[i].m_n     = iRes;
			m_aRecord[i].m_n_1   = iRes + 1;
			m_aRecord[i].m_uiPos = uiPos;
			m_aRecord[i].m_bCompressed = iZip;
			m_iRecordCount++;
		}
		else
			break;
	}

	// Reset active resolution
	m_pActiveRecord = NULL;

	return true;
}

// ----------------------------------------------------------------------------
/*! For given (requested) scale resolution \a iRes_ppd (subject to zoom
    and screen size) given in pixel per degree select the active resolution.

	 \retval int New active resolution. Note that the new active resolution can
    be significantly different from the given resolution.

 */
int BlockRaster::SetResolution(
	int iRequestedRes_ppd //!< Requested resolution (in pixels per degree).
)
{
	Q_ASSERT(iRequestedRes_ppd > 0);

	if(m_iRecordCount == 0) {
		m_pActiveRecord = NULL;
		return 0;
	}

	// Set the only one we have.
	if(m_iRecordCount == 1) {
		m_pActiveRecord = &m_aRecord[0];
		return m_aRecord[0].m_n;
	}

	// Just find the weighted nearest resolution.
	int iMin = std::numeric_limits<int>::max();
	int   iR = 0;
	for(int i=0; i<m_iRecordCount; i++) {
		int iDif = iRequestedRes_ppd - m_aRecord[i].m_n;
		if(iDif > 0)
			iDif /= POSITIVE_SLOPE_REDUCTION;   // reduce the positive slope.
		else
			iDif = -iDif/NEGATIVE_SLOPE_REDUCTION; // change sign of the negative slope.

		if(iDif < iMin) {
			iMin = iDif;
			iR   = i;
		}
	}
	m_pActiveRecord = &m_aRecord[iR];
	return m_aRecord[iR].m_n;
}

// ----------------------------------------------------------------------------

bool BlockRaster::ReadData(
	QIODevice& fIn
)
{
	ASSERT(m_pActiveRecord != NULL);
	return m_pActiveRecord->Read(fIn);
}

// ----------------------------------------------------------------------------

bool BlockRaster::AssureData(const QString& qsFile)
{
	if(IsEmpty()) {
		QFile fIn(qsFile);
		fIn.open(QIODevice::ReadOnly);
		return ReadData(fIn);
	}
	return true;
}

// ----------------------------------------------------------------------------

int BlockRaster::GetUsedMemory() const
{
	int iMem = sizeof(BlockRaster);
	for (int i=0; i<m_iRecordCount; i++) {
		iMem += m_aRecord[i].m_baData.capacity();
	}
	return iMem;
}

// ----------------------------------------------------------------------------

int BlockRaster::GetRecordResolution(int iRec) const
{ 
	Q_ASSERT(iRec<m_iRecordCount && iRec>=0);
	return m_aRecord[iRec].m_n; 
}	
// ----------------------------------------------------------------------------

} // NAMESPACE
