#ifndef __MAP_MAP_IMAGE_ZOOM_H
#define __MAP_MAP_IMAGE_ZOOM_H
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

#include <QMap>
//#include <QHash>
#include "MapImage.h"
#include "ContourColor.h"

namespace map {

// -----------------------------------------------------------------------
/** \brief A class that is responsible to provide the image of a map.

	 This object is used to provide the a part of map, which will be copied
    to the screen. Internally, it stores one or more parallel images with
    different zoom levels.
 */
class MapImageZoom 
{
	// TODO Use QHash - it is slightly faster. Need to modify the GetNextZoomFactor()
	//! Shorthand for container of map images.
	typedef QMap<int, MapImage*> ImageCon;
public:
	//! Constructor.
	MapImageZoom();
	//! Destructor.
	virtual ~MapImageZoom();
	
	//! Set cartographic projection by taking ownership of the given projection object.
	void SetProjection(AbstractProjection* pProj);
	
	//! Add image for given factor.
	void AddImage(int iZoomFactor, const QString& qsZoomName,
					  int iSide, int iPixPerMeter, int iSafetyMargin,
					  QPair<int, const char*> ipcPP1, QPair<int, const char*> ipcPP2,
					  QPair<float, const char*> fpcScale);
	//! Return number of images in the container.
	int GetImageCount() const
	{ return m_conImages.size(); }
	
	//! Get desired map image.
	bool GetImage(
		int iKey, 
		QImage& img, 
		QPoint& ptxOrg, 
		float fHeading, 
		const QPointF& ptOrgWGS,
		int iRadius,
		QVector<const TextLabel*>& conDrawLabels
	);

	//! Perform the background update of the images.
	void UpdateImages(const QPointF& ptOrgWGS, int iRadius);
	
	//! Return status of the image with
	MapImage::Status GetStatus(int iZoomFactor) const;

	//! Set list of external coordinates that needs to be converted.
	void SetExternalCoordinates(int iZoomFactor, const QVector<QPointF>& conCoor);
	//! Get the list of external points (coordinates) that were converted.
	QVector<QPoint> GetExternalPoints(int iZoomFactor) const;
	
	//! Get prediction position data pair vector (seconds, label).
	const QVector<QPair<int, const char*> >& GetPredictionPoints(int iZoomFactor) const;
	
	//! Get next(previous) zoom factor from given factor.
	int GetNextZoomFactor(int iZoomFactor, bool bZoomIn) const;
	
	//! Get list of zoom names.
	QStringList GetZoomNameList() const;
	//! Get list of zoom factors.
	QVector<int> GetZoomFactorList() const;
	//! Get status list.
	QVector<MapImage::Status> GetStatusList() const;
	//! Get all zoom data in one step.
	void GetZoomData(QStringList& sl, QVector<int>& vi, QVector<MapImage::Status>& vs) const;
	
	//! Terminate 
	void Terminate();
	
	//! Get the scale image.
	const QImage& GetScaleImage(int iZoomFactor) const;
	
private:
	//! Container of parallel map images.
	ImageCon m_conImages;
	//! Projection - one for all images. Images don't have their own projection object.
	std::auto_ptr<AbstractProjection> m_apProjection;
	
	//! Terminating flag.
	bool m_bTerminating;
};

// -----------------------------------------------------------------------

class SingletonMapImageZoom
{
private:
	//! Private constructor.
	SingletonMapImageZoom();
public:
	//! Public destructor.
	~SingletonMapImageZoom();
	
	//! Access to the singleton object.
	static MapImageZoom* GetInstance();
	
private:
	//! The only instance of this class.
	static SingletonMapImageZoom* s_pInstance;	
		
	//! The purpose of this class is to wrap this object.
	MapImageZoom m_miz;
};

// -----------------------------------------------------------------------

} // map namespace

#endif	// __MAP_PAINTER
