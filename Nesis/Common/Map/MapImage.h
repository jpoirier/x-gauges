#ifndef __MAP_IMAGE_H
#define __MAP_IMAGE_H
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

#include <QThread>
#include <QMutex>
#include <QImage>
#include "CommonDefs.h"
#include "ContourColor.h"
#include "Mapper.h"
#include "TextLabel.h"
#include "Route/Waypoint.h"

namespace map {

class World;

// -----------------------------------------------------------------------
/** \brief A class that is respresents "paper map" in pixels.

	The "image map" is hidden buffer where we are performing background
   drawing of the world surface. The class acts as a source class for
   the Painter object that maps pixels to the actuall screen.
 */
class MapImage : public QThread
{
public:
	//! Status of the paper object.
	enum Status {
		sEmpty,    		//!< The object is empty - it needs to be repainted.
		sReady,			//!< We have an image ready to copy from.
	};
	
public:
	//! Constructor.
	MapImage(int iSize, int iPixPerMeter, QImage::Format fmt);
	//! Destructor.
	virtual ~MapImage();

	//! Set safety margin f.
	void SetSafetyMargin(int iF)
	{ m_iSM = iF; }

	//! Set zoom factor (this is inverse of scale).
	void SetZoomFactor(int iZoomFactor);
	//! Return zoom factor value.
	int GetZoomFactor() const
	{ return m_iZoomFactor; }

	//! Set zoom name
	void SetZoomName(const QString& qsZoomName)
	{ m_qsZoomName = qsZoomName; }
	//! Return zoom name.
	const QString& GetZoomName() const
	{ return m_qsZoomName; }
	//! Return image status
	Status GetStatus() const;

	//! Get the image to be painted on the screen.
	bool GetImage(
			QImage& img,
			QPoint& ptxOrg,
			float& fHeading,
			const QPointF& ptOrgWGS84,
			int iRadius,
			QVector<const TextLabel*>& conDrawLabels);

	//! Set projection for the mapper object. Projection is external object.
	void SetProjection(AbstractProjection* pProj)
	{ m_M.SetProjection(pProj); }

	//! Set current airplane (origin position).
	int UpdateImage(const QPointF& ptO, int iRadius);

	//! Set external coordinates for conversion.
	void SetExternalCoordinates(const QVector<QPointF>& conCoor)
	{ m_conExCoor = conCoor; }
	//! Get converted external image points.
	const QVector<QPoint>& GetExternalPoints() const
	{ return m_conExPoint; }
	
	//! Set prediction points (seconds, label)
	void AddPredictionPoint(QPair<int, const char*> ipcPP)
	{ m_vPred << ipcPP; }
	
	//! Get prediction point data pairs.
	const QVector<QPair<int, const char*> >& GetPredictionPoints() const
	{ return m_vPred; }
	
	//! Terminate the thread.
	void Terminate()
	{ m_bTerminating = true; }
	
	//! Set the scale data.
	void SetScaleData(const QPair<float, const char*>& fpcData)
	{ m_fpcScaleData = fpcData; }
	
	//! Get the scale image.
	const QImage& GetScaleImage() const
	{ return m_imgScale; }

private:
	//! Prepare a new image. Copy what we can and calculate the rest.
	void StartThread(int dx, int dy, const QPointF& ptONew);

	//! Runs in the background thread - updating the new image.
	virtual void run();

	//! Draw given image. MapImage must be correct size. Current map settings are used.
	void DrawImagePart(
		QImage& img, World* pW,
		QList<TextLabel*>& conTextLabels);
		
	//! Clear text labels.
	void ClearLabels();
	
	//! Create/update the scale pixmap.
	void UpdateScalePixmap();

private:
	//! Mutex for exclusive access to the image.
	mutable QMutex m_mutexImage;

	//! Current zoom factor.
	int m_iZoomFactor;
	//! Current zoom name - a zoom factor string used on display.
	QString m_qsZoomName;

	//! Status of the internal image.
	Status m_eStatus;
	
	//! The termination flag. Exit the thread immediately.
	bool m_bTerminating;

	//! Current origin - position of the airplane point in pixel points.
// 	QPoint m_ptxO;
	//! Each image has its own individual scale and projection.
	Mapper m_M;
	//! Internaly, the paper map is stored as a pixel image.
	QImage m_image;

	//! In some cases we want to get image coordinates of external points.
	QVector<QPointF> m_conExCoor;
	//! These are converted points by the GetImage() operation.
	QVector<QPoint> m_conExPoint;

	//! List of TextLabels of this image. We own the label objects.
	QList<TextLabel*> m_conLabels;

	//! This safety margin tells us when we need to start updating the map.
	int m_iSM;
	
	//! Prediction points vector.
	QVector<QPair<int, const char*> > m_vPred;
	
	//! Data used to calculate the scale
	QPair<float, const char*> m_fpcScaleData;
	
	//! Scale pixmap - to draw over the final image when needed.
	QImage m_imgScale;
	
	// The background process needs some parameters ...
	//! How much must we move relative to the current image (x direction).
	int b_idx;
	//! How much must we move relative to the current image (y direction).
	int b_idy;
	//! The new map origin W84 point.
	QPointF b_ptO;
};

// -----------------------------------------------------------------------

inline MapImage::Status MapImage::GetStatus() const
{
// 	QMutexLocker ml(&m_mutexImage);
	return m_eStatus;
}

// -----------------------------------------------------------------------
} // map namespace

#endif	// __MAP_PAINTER
