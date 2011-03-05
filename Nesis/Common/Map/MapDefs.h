#ifndef MAP_MAP_DEFS_H
#define MAP_MAP_DEFS_H

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

#define DEST_KAM_PATH	   "/home/akrajnc/map/kam/"
#define SOURCE_SRTM3_PATH	"/home/akrajnc/map/SRTM/SRTM3/"
#define SOURCE_SRTM30_PATH	"/home/akrajnc/map/SRTM/SRTM30/"
#define SOURCE_SWBD_PATH   "/home/akrajnc/map/SRTM/SWBD/"
#define SHAPE_PATH			"/home/akrajnc/map/shape/"
#define NON_US_GEONAMES    "/home/akrajnc/map/geonames/geonames.txt"
#define US_GEONAMES        "/home/akrajnc/map/geonames/us.txt"
#define DEST_GEONAMES_PATH "/home/akrajnc/map/geonames/"
#define SOURCE_AIRFIELD_PATH "/home/akrajnc/map/Airfields/Airfields.wpt"

#define SRTM30_NODATA		-9999
#define SEA_SRTM_ELEVATION  8990
#define SRTM3_DATA_VOID    -32768

#include <cstdlib>
#include <QFileInfo>
#include <QByteArray>
#include <QVector>
#include <QRectF>
#include "CommonDefs.h"

namespace map {

// --------------------------------------------------------------------------

/** For given longitude and latitude calculate the block id.
	This id is used to identify the block in a unique way.
	The class \a map::World is using the id as a key.

	\retval quint16 16 bit unsigned integer which identifies the block.
 */
inline quint16 GetIdFromLonLat(
	float fLon,	//!< longitude in the range [-180.0, 180]
	float fLat	//!< latitude in the range [-90.0, 90]
)
{
// 	qDebug("Lon=%f, Lat=%f", fLon, fLat);
	Q_ASSERT(fLon <= 180.0f && fLon >= -180.0f);
	Q_ASSERT(fLat <= 90.0f && fLat >= -90.0f);
	return (((int)floor(fLon))+180)*180 + (((int)floor(fLat))+90);
}

// --------------------------------------------------------------------------
//! Same as above, but the integer version.
inline quint16 GetIdFromLonLat(
	int iLon,	//!< longitude in the range [-180, 180]
	int iLat		//!< latitude in the range [-90, 90]
)
{
	Q_ASSERT(iLon <= 180 && iLon >= -180);
	Q_ASSERT(iLat <= 90  && iLat >= -90);
	return ((iLon+180)*180 + iLat+90);
}

// --------------------------------------------------------------------------
//! Calculate block origin longitude for given block id.
inline qint16 GetLonFromId(quint16 uiId)
{ return uiId/180 - 180; }

// --------------------------------------------------------------------------
//! Calculate block origin latitude for given block id.
inline qint16 GetLatFromId(quint16 uiId)
{ return uiId%180 - 90; }

// --------------------------------------------------------------------------
//! Create a filename from given Block id using extension \a pExt.
/*! Construct a file name and return it in a QByteArray block. The
		name is constructed from the block id and given name extension.
 */
inline QByteArray GetFileNameFromId(
	quint16 uiId,		//!< block id.
	const char* pExt	//!< file name extension.
)
{
	Q_ASSERT(pExt != NULL);

	qint16 iLon = GetLonFromId(uiId);
	qint16 iLat = GetLatFromId(uiId);

	int iSize = strlen(pExt) + 8;
	QByteArray baFileName(iSize,'\0');

	sprintf(baFileName.data(), "%c%02u%c%03u%s",
	  (iLat < 0 ? 'S' : 'N'),
	  abs(iLat),
	  (iLon < 0 ? 'W' : 'E'),
	  abs(iLon),
	  pExt
	);
	return baFileName;
}

// --------------------------------------------------------------------------
//! Create vector of block ids that can be found in given range.
inline QVector<quint16> GetIdsInRect(const QRectF& rc)
{
	// Some airways and line may have invalid rect, but they are still OK,
	// just empty due to line following a parallel or ...
/*	Q_ASSERT(rc.isValid());*/
	QVector<quint16> v;
	for(int iY = (int)floor(rc.top()); iY <= (int)floor(rc.bottom()); iY++) {
		for(int iX = (int)floor(rc.left()); iX <= (int)floor(rc.right()); iX++) {
			v << GetIdFromLonLat(iX, iY);
		}
	}
	return v;
}

// --------------------------------------------------------------------------
//! Create a block id from given filename.
inline quint16 GetIdFromFileName(
	const QString& qsFileName	//!< The file name can be a complete path.
)
{
	// We need just the base name.
	QString qsBase = QFileInfo(qsFileName).baseName();
	// Why not use the sscanf?
	char chX, chY;
	int iX, iY;
	sscanf(qsBase.toAscii(), "%c%d%c%d", &chY, &iY, &chX, &iX);
	if(chY=='S') iY = -iY;
	if(chX=='W') iX = -iX;
	
	return GetIdFromLonLat(iX, iY);
}

// --------------------------------------------------------------------------
//! Create block rect from given id. Resulting rect is in degrees.

inline QRectF GetRectFromId(quint16 id)
{
	return QRectF(GetLonFromId(id), GetLatFromId(id), (qreal)1.0,(qreal)1.0);
}

// --------------------------------------------------------------------------

//! Enumeration of the available layers.
enum LayerType
{
	ltBegin,								//!< Special value to be used in loops.
	ltPoliticalBoundary = ltBegin,//!< (line) boundary lines on the surface.
	ltAdministrativeArea,		 	//!< (text) names of admin. areas.
	ltBoundaryText,				 	//!< (text) gulf names, some port names.

	ltSpotElevation,					//!< (point) Some major elevations.

	ltAqueductCanal,					//!< (line) Aqueducts, canals, flumes, penstocks.
	ltWaterCourse,						//!< (line) Rivers, streams, ...
	ltInlandWater,						//!< (area) Lakes, ...

	ltCityArea,							//!< (area) Cities and larger towns.
	ltPopPlaces,						//!< (point) City, town, village, etc.

	ltMiscTransport,					//!< (line) lift, ...
	ltRailroad,							//!< (line) railoads
	ltRoad,								//!< (line) roads
	ltTrail,								//!< (line) trails, path
	ltTransportStructure,			//!< (line) bridges, tunnels, etc...
	ltPipeline,							//!< (line) pipelines
	ltPowerline,						//!< (line) power transmition, telephone, ...

	ltAirfield,							//!< (point-symbol) Airfields from the Welt2000 database.

	ltEnd									//!< Special value to be used in loops.
};

// --------------------------------------------------------------------------
/**  A duplicate of the enum in the shapefil.h file used to break the
     link with the shapefil library.
 */
typedef enum {
	FTString,	//!< QString type.
	FTInteger,	//!< qint32 type.
	FTDouble,	//!< double type.
	FTLogical,	//!< not used.
	FTInvalid	//!< not used.
} DBFFieldType;

// --------------------------------------------------------------------------
/** Possible Geographics projections to be used in the code.
  */
typedef enum {
	projAutomatic,		//!< Projection is automaticaly selected.
	projLambert60N,
	projLambert40N,
	projLambert20N,
	projMercator,
	projLambert20S,
	projLambert40S,
	projLambert60S
} Projection;

// --------------------------------------------------------------------------

}
#endif	// __MAP_DEFS_H
