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

#include <QtDebug>
#include <QtGlobal>
#include <QPainter>
#include "World.h"
#include "MathEx.h"
#include "MapImage.h"
#include "Map/AirspaceContainer.h"
#include "Route/WaypointContainerSingleton.h"
#include "Route/WaypointVFR.h"
#include "Route/Airfield.h"

#define CELL_WIDTH	100
#define CELL_HEIGHT	40

namespace map {

// ----------------------------------------------------------------------------
// TODO m_M (mapper) possible conflict due to change of m_M in the update thread.


MapImage::MapImage(
	int iSize,
	int iPixPerMeter,
	QImage::Format fmt
)
: 	m_M(iPixPerMeter, iSize),
	m_image(iSize, iSize, fmt)
{
	using namespace common;
	m_iZoomFactor = 500000;
	m_bTerminating = false;

	//! We have an empty image.
	m_eStatus = sEmpty;

	m_image.setDotsPerMeterX(iPixPerMeter);
	m_image.setDotsPerMeterY(iPixPerMeter);

	// default safety margin is invalid
	m_iSM = 0;
	m_fpcScaleData = qMakePair(0.0f, (const char*)NULL);
}

MapImage::~MapImage()
{
	ClearLabels();
}
// --------------------------------------------------------------------------

void MapImage::ClearLabels()
{
	foreach(TextLabel* ptl, m_conLabels)
		delete ptl;
	m_conLabels.clear();
}

// --------------------------------------------------------------------------
/*! Calculate current origin in the image due to change of the
    ptO coordinate of an aeroplane. If this position is still within the
    limits, image remains untouched - only the origin is updated.
	 However, if the position is outside the limits, either part of the
    image or a complete new image needs to be calculated.
 */
int MapImage::UpdateImage(const QPointF& ptO, int iRadius)
{
	const int iSideSize = m_image.width();
	// Check if the radius and safety margins are not too large.
	ASSERT(iRadius + m_iSM*3/2 <= iSideSize/2);

	// Check if we need to update everything.
	if(m_eStatus == sEmpty) {
// 		std::cout << "SetOriginPoint()" << std::endl;
		// Request full image update
		if(isRunning()==false)
			StartThread(0,0, ptO); // Update complete image.
		return -iSideSize;
	}

	// TODO This line may be problematic.
	// origin point in pixels.
	QPoint ptxO = m_M.ConvertToImage(ptO);

	// do we need to realocate the map.
	int dx = ptxO.x() - iSideSize/2;
	int dy = ptxO.y() - iSideSize/2;

	// How much space do we still have?
	int iFree = iSideSize/2 - common::GetMax(abs(dx),abs(dy)) - iRadius - m_iSM;

	// Not enough, start the background update process.
	if(iFree < 0) {
		// We can update image only if the thread is non running.
//		if(isFinished())
		if(isRunning()==false)
			// Start the thread.
			StartThread(dx, dy, ptO);
		else {
			std::cout << "Warning: Update request for " << m_iZoomFactor << " posponed. "
					    << "Severity is " << (int)(-100.0f*iFree)/m_iSM << "%." << std::endl;
		}
	}

	// We can't move the origin anymore. The only solution is to
	// keep the current map and move the airplane symbol of the center.
	// We only report this condition returning a negative value.
	// The caller is responsible to calculate the position of the
	// airplane symbol.
	return iFree+m_iSM;
}

// --------------------------------------------------------------------------
/*!
 */
void MapImage::StartThread(int dx, int dy, const QPointF& ptONew)
{
	// Start the background process -- see the run() function below.
	ASSERT(isRunning()==false);

	// Store variables
	b_idx = dx;
	b_idy = dy;
	b_ptO = ptONew;

	start(QThread::IdlePriority);
}

// --------------------------------------------------------------------------
/*! This is the background process where a background map image is calculated.
 */
void MapImage::run()
{
	if(m_bTerminating)
		return;
		
	std::cout << ">> Update for zoom factor " << m_iZoomFactor << " started." << std::endl;
// 	std::cout << "Inside the thread - run()" << std::endl;
	Q_ASSERT(m_image.width() == m_image.height());

	// square side size of the final image.
	const int iSideSize = m_image.width();
	const int iEmptyCriteria = 2*iSideSize/3;

	// Required correction.
	const int idy = abs(b_idy);
	const int idx = abs(b_idx);
// 	qDebug() << "idy =" << idy << "idx =" << idx << "emp.crit. =" << iEmptyCriteria; 
	QMutexLocker ml(&m_mutexImage);

	// If correction is larger than some criteria ...
	if(idx > iEmptyCriteria || idy > iEmptyCriteria) {
		// make the image empty - a complete update is required.
		m_eStatus = sEmpty;
		qDebug() << "Warning: Full repaint is required.";
	}

	// Do we need to make a complete update?
	if(m_eStatus == sEmpty) {
// 		std::cout << "Start sEmpty drawing for zoom " << m_fZoomFactor << "." << std::endl;
	// Update the mapper object - we have a new center point.
		m_M.SetScale(1.0f/m_iZoomFactor, b_ptO);

		ClearLabels();
		// Get access to the world object. Wait if necessary.
		World* pW = World::Lock();
		if(m_bTerminating) {
			World::Unlock();
			return;
		}
		pW->SetPixPerDegree(m_M.GetPixPerDegree());
		DrawImagePart(m_image, pW, m_conLabels);
		World::Unlock();

		QList<TextLabel*>::iterator tli;
		for(tli=m_conLabels.begin(); tli!=m_conLabels.end(); tli++) {
			(*tli)->SetPosition(m_M.ConvertToImage((*tli)->GetCoordinate()));
		}
		// The pixmap which shows the current scale image
		UpdateScalePixmap();
//		m_image.save("/tmp/Image.png");
		qDebug() << "<< Full zoom factor" << m_iZoomFactor << "complete!";
		m_eStatus = sReady;
		return;
	}

	// There must be some reason for the update.
	Q_ASSERT(qMax(idx,idy) > 0);

	// No, we only need to make partial update.
	ml.unlock(); // m_image object is not needed for a while.

	// Set the images
	QImage imgVer(idx, iSideSize-idy, m_image.format());
	QImage imgHor(iSideSize, idy, m_image.format());

	// Set the image offsets

	// Case one
	if(b_idx >= 0 && b_idy >=0) {
		imgHor.setOffset(QPoint(b_idx, iSideSize));
		imgVer.setOffset(QPoint(iSideSize, b_idy));
	}
	// Case two
	else if(b_idx < 0 && b_idy < 0) {
		imgHor.setOffset(QPoint(b_idx, b_idy));
		imgVer.setOffset(QPoint(b_idx, 0));
	}
	// Case three
	else if(b_idx >= 0 && b_idy < 0) {
		imgHor.setOffset(QPoint(b_idx, b_idy));
		imgVer.setOffset(QPoint(iSideSize, 0));
	}
	// Case four
	else /* if(b_idx < 0 && b_idy >= 0) */{
		imgHor.setOffset(QPoint(b_idx, iSideSize));
		imgVer.setOffset(QPoint(b_idx, b_idy));
	}

	QList<TextLabel*> conLabelHor;
	QList<TextLabel*> conLabelVer;
	// Get access to the world object. Wait if necessary.
	World* pW = World::Lock();
	if(m_bTerminating) {
		World::Unlock();
		return;
	}
	// Update the world object with the current scale settings.
	// This is needed to call/load correct resolution of the raster map.
	pW->SetPixPerDegree(m_M.GetPixPerDegree());

	// Fill the vertical bar
	DrawImagePart(imgVer, pW, conLabelVer);
//	imgVer.save("/tmp/ImageVer.png");

	// Fill the horizontal bar
	DrawImagePart(imgHor, pW, conLabelHor);
//	imgHor.save("/tmp/ImageHor.png");

	World::Unlock();

	// Now, make the "big image".

	const int iV = iSideSize-idy;
	const int iH = iSideSize-idx;
	Q_ASSERT(iV > 0 && iH > 0);

	// Here we need exclusisve access to the image. Wait if necessary.
	ml.relock();
	// Case one: dx >= 0 and dy >= 0
	if(b_idx >= 0 && b_idy >=0) {
		int iLineBytes = iH*sizeof(QRgb);
		// Reuse the original image.
		for(int iy=0; iy<iV; iy++) {
			QRgb* pSrc = (QRgb*)m_image.scanLine(iy+idy);
			pSrc      += idx;
			QRgb* pDst = (QRgb*)m_image.scanLine(iy);
			if(idy != 0) {
				// memcpy is faster than memmove.
				memcpy(pDst, pSrc, iLineBytes);
			}
			else {
				memmove(pDst, pSrc, iLineBytes);
			}
		}

		// Copy the horizontal image into the big one
		if(!imgHor.isNull()) {
			iLineBytes = imgHor.width()*sizeof(QRgb);
			for(int iy=0; iy<idy; iy++) {
				QRgb* pSrc = (QRgb*)imgHor.scanLine(iy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy+iV);
				memcpy(pDst, pSrc, iLineBytes);
			}
		}

		// Copy the vertical image into the big one
		if(!imgVer.isNull()) {
			iLineBytes = imgVer.width()*sizeof(QRgb);
			for(int iy=0; iy<iV; iy++) {
				QRgb* pSrc = (QRgb*)imgVer.scanLine(iy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy);
				pDst      += iH;
				memcpy(pDst, pSrc, iLineBytes);
			}
		}
	}

	// Case two: dx < 0 and dy < 0
	else if(b_idx < 0 && b_idy < 0) {
		int iLineBytes = iH*sizeof(QRgb);
		// Reuse the original image.
		for(int iy=iV-1; iy>=0; iy--) {
			QRgb* pSrc = (QRgb*)m_image.scanLine(iy);
			QRgb* pDst = (QRgb*)m_image.scanLine(iy+idy);
			pDst      += idx;
			// always memcpy since b_idy is nonzero.
			memcpy(pDst, pSrc, iLineBytes);
		}

		// Copy the horizontal image into the big one
		if(!imgHor.isNull()) {
			iLineBytes = imgHor.width()*sizeof(QRgb);
			for(int iy=0; iy<idy; iy++) {
				QRgb* pSrc = (QRgb*)imgHor.scanLine(iy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy);
				memcpy(pDst, pSrc, iLineBytes);
			}
		}

		// Copy the vertical image into the big one
		if(!imgVer.isNull()) {
			iLineBytes = imgVer.width()*sizeof(QRgb);
			for(int iy=idy; iy<iSideSize; iy++) {
				QRgb* pSrc = (QRgb*)imgVer.scanLine(iy-idy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy);
				memcpy(pDst, pSrc, iLineBytes);
			}
		}
	}

	// Case three: dx >= 0 and dy < 0
	else if(b_idx >= 0 && b_idy < 0) {
		int iLineBytes = iH*sizeof(QRgb);
		// Reuse the original image.
		for(int iy=iV-1; iy>=0; iy--) {
			QRgb* pSrc = (QRgb*)m_image.scanLine(iy);
			QRgb* pDst = (QRgb*)m_image.scanLine(iy+idy);
			pSrc      += idx;
			// always memcpy since b_idy is nonzero.
			memcpy(pDst, pSrc, iLineBytes);
		}

		// Copy the horizontal image into the big one
		if(!imgHor.isNull()) {
			iLineBytes = imgHor.width()*sizeof(QRgb);
			for(int iy=0; iy<idy; iy++) {
				QRgb* pSrc = (QRgb*)imgHor.scanLine(iy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy);
				memcpy(pDst, pSrc, iLineBytes);
			}
		}

		// Copy the vertical image into the big one
		if(!imgVer.isNull()) {
			iLineBytes = imgVer.width()*sizeof(QRgb);
			for(int iy=idy; iy<iSideSize; iy++) {
				QRgb* pSrc = (QRgb*)imgVer.scanLine(iy-idy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy);
				pDst      += iH;
				memcpy(pDst, pSrc, iLineBytes);
			}
		}

	}

	// Case four: dx < 0 and dy >= 0
	else /* if(b_idx < 0 && b_idy >= 0) */{
		int iLineBytes = iH*sizeof(QRgb);
		// Reuse the original image.
		for(int iy=0; iy<iV; iy++) {
			QRgb* pSrc = (QRgb*)m_image.scanLine(iy+idy);
			QRgb* pDst = (QRgb*)m_image.scanLine(iy);
			pDst      += idx;
			if(idy != 0) {
				// memcpy is faster than memmove.
				memcpy(pDst, pSrc, iLineBytes);
			}
			else {
				memmove(pDst, pSrc, iLineBytes);
			}
		}

		// Copy the horizontal image into the big one
		if(!imgHor.isNull()) {
			iLineBytes = imgHor.width()*sizeof(QRgb);
			for(int iy=0; iy<idy; iy++) {
				QRgb* pSrc = (QRgb*)imgHor.scanLine(iy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy+iV);
				memcpy(pDst, pSrc, iLineBytes);
			}
		}

		// Copy the vertical image into the big one
		if(!imgVer.isNull()) {
			iLineBytes = imgVer.width()*sizeof(QRgb);
			for(int iy=0; iy<iV; iy++) {
				QRgb* pSrc = (QRgb*)imgVer.scanLine(iy);
				QRgb* pDst = (QRgb*)m_image.scanLine(iy);
				memcpy(pDst, pSrc, iLineBytes);
			}
		}
	}

	// Update the mapper object - we have a new center point.
	m_M.SetScale(1.0f/m_iZoomFactor, b_ptO);

	// Update the text labels.

	// First update position of the existing text labels.
	// Remove those labels which are not in the image anymore.
	QList<TextLabel*>::iterator i = m_conLabels.begin();
	const QRect rImg(m_image.rect());
	while(i!=m_conLabels.end()) {
		QPoint ptImg = m_M.ConvertToImage((*i)->GetCoordinate());
		if(rImg.contains(ptImg)) {
			(*i)->SetPosition(ptImg);
			i++;
		}
		else {
			// removing from vector is not very efficient.
			delete (*i);
			i = m_conLabels.erase(i);
		}
	}
	// Add labels from horizontal ...
	for(i=conLabelHor.begin(); i!=conLabelHor.end(); i++)
		(*i)->SetPosition(m_M.ConvertToImage((*i)->GetCoordinate()));
	m_conLabels += conLabelHor;
	
	// ... and vertical part of the image. 
	for(i=conLabelVer.begin(); i!=conLabelVer.end(); i++)
		(*i)->SetPosition(m_M.ConvertToImage((*i)->GetCoordinate()));
	m_conLabels += conLabelVer;

	std::cout << "<< Update complete for zoom factor " << m_iZoomFactor << " ." << std::endl;
	
	// Update the scale pixmap
	UpdateScalePixmap();
	// Image is ready.
	m_eStatus = sReady;
}

// --------------------------------------------------------------------------

void MapImage::DrawImagePart(
	QImage& img,
	World* pW,
	QList<TextLabel*>& conTextLabels)
{
	if(img.size().isValid()==false)
		return;
	// Get the contour object.
	ContourColor* pC = ContourColor::GetInstance();

// 	std::cout << "Raster drawing started." << std::endl;

	// Update the world object with the current scale settings.
	// This is needed to call/load correct resolution of the raster map.

	//pW->SetPixPerDegree(m_M.GetPixPerDegree());

	// Reset the paint related bookkeeping in the World object.
	pW->BeginPaint();

	QSize s = img.size();
	QPoint ptPix;
	QPointF ptWGS;
	const int iW = s.width();
	const int iH = s.height();

	// We only need this for the rectangular projection.
	QVector<qint16> vElevation(iW);
	QVector<QRgb>   vColor(iW);

	for(int iy=0; iy<iH; iy++) {
		if(m_bTerminating) break;
		// Get pixel by pixel - a painful way to get a complete map.
		if(m_M.IsRectangularProjection()==false) {
			for(int ix=0; ix<iW; ix++) {
				// Get the elevation
				ptPix = img.offset();
				ptPix += QPoint(ix,iy);
				ptWGS = m_M.ConvertToWGSDegrees(ptPix);
				int iElevation = pW->GetElevation(ptWGS);
				// Convert the elevation into the color and display the pixel.
				img.setPixel(ix, iy, pC->GetColor(iElevation));
			}
		}
		// We have rectangular projection (e.g. Mercator).
		else {
			ptPix = img.offset();
			ptPix.ry() += iy;
			// Ask the mapper for the one pixel step ...
			ptWGS = m_M.ConvertToWGSDegrees(ptPix);
			float fArc = m_M.GetLongitudeOnePixelArcDegree(ptPix);
			// Get as many pixels as we can from the world.
			pW->GetElevation(ptWGS, fArc, vElevation);
			pC->GetColor(vElevation, vColor);
			memcpy(img.scanLine(iy), &vColor[0], iW*4);
		}
	}

	// Do the vector layer drawings, but only, if the zoom is not too large.

	if(m_iZoomFactor <= 10000000) {

		// Get clipping rectangle in WGS coordinates.
		qreal fMinX, fMaxX;
		qreal fMinY, fMaxY;

		// PT 1
		QPoint  ptO   = img.offset();
		QPointF ptWGS = m_M.ConvertToWGS(ptO);
		fMinX = fMaxX = ptWGS.x();
		fMinY = fMaxY = ptWGS.y();

		// PT 2
		ptO.rx() += img.width();
		ptWGS = m_M.ConvertToWGS(ptO);
		fMaxX  = qMax(fMaxX, ptWGS.x());
		fMaxY  = qMax(fMaxY, ptWGS.y());
		fMinX  = qMin(fMinX, ptWGS.x());
		fMinY  = qMin(fMinY, ptWGS.y());

		// PT 3
		ptO.ry() += img.height();
		ptWGS = m_M.ConvertToWGS(ptO);
		fMaxX  = qMax(fMaxX, ptWGS.x());
		fMaxY  = qMax(fMaxY, ptWGS.y());
		fMinX  = qMin(fMinX, ptWGS.x());
		fMinY  = qMin(fMinY, ptWGS.y());

		// PT 4
		ptO.rx() -= img.width();
		ptWGS = m_M.ConvertToWGS(ptO);
		fMaxX  = qMax(fMaxX, ptWGS.x());
		fMaxY  = qMax(fMaxY, ptWGS.y());
		fMinX  = qMin(fMinX, ptWGS.x());
		fMinY  = qMin(fMinY, ptWGS.y());

		// This is clipping rectangle used to speed up drawing.
		QRectF clipRect(fMinX, fMinY, fMaxX-fMinX, fMaxY-fMinY);
		QVector<quint16> vIds = map::GetIdsInRect(
			QRectF(clipRect.topLeft()*(180/M_PI), clipRect.size()*(180/M_PI))
		);
//		foreach(quint16 i, vIds)
// 			qDebug() << "V(ids) =" << vIds.count() << "->" << vIds;

		// Start drawing
		QPainter qP(&img);
//		qP.drawRect(0,0,img.width(), img.height());
		qP.translate(-(img.offset().x()/*+0.5f*/), -(img.offset().y()/*+0.5*/));

		bool bWaypoints 		= m_iZoomFactor <= 1000000;
		bool bAirspaces 		= m_iZoomFactor <= 4000000;
		bool bAntialiasing 	= m_iZoomFactor <= 1000000;

		if(bAntialiasing)
			qP.setRenderHint(QPainter::Antialiasing, true);

		// Ask the World object for the list of blocks used in the background drawing.
		QVector<const Block*> vpBlock = pW->GetActiveBlocks();
		
		
		// Paint each blok and ask it for a list of TextLabels.
		// Build container of active waypoints - block depended.
		route::WaypointContainer::ConstContainer conWpt;
		route::WaypointContainerSingleton* pCW;
		pCW = route::WaypointContainerSingleton::GetInstance();
		
		QVector<const Block*>::const_iterator ib;
		for(ib=vpBlock.begin(); ib!=vpBlock.end(); ib++) {
			if(m_bTerminating) break;	
			(*ib)->DrawVector(&qP, &m_M);
			(*ib)->GetTextLabels(&m_M, clipRect, conTextLabels);
			if(bWaypoints)
				conWpt += pCW->GetSectorWaypoints((*ib)->GetKey());
		}
		
		// Paint airspaces
		if(bAirspaces && !m_bTerminating) {
			map::AirspaceContainer* pAS = map::AirspaceContainer::GetInstance();
			// get list of Airspaces inside clipping rect
			QVector<const Airspace*> vA = pAS->GetAirspaces(
				clipRect,
				Airspace::cAll_but_GP_W_O,
				10000
				/*,
				Airspace::cC |
				Airspace::cE |
				Airspace::cCTR |
				Airspace::cDanger |
				Airspace::cProhibited |
				Airspace::cRestricted,
				1//2000 // ignore airspaces above 12000*/
			);
//			qDebug() << vA.count() << "airspaces are active." << vA[0]->GetTitle();

			if(vA.size()) {
				// draw airspaces ...
				QVector<const Airspace*>::const_iterator ia;
				for(ia=vA.begin(); ia!=vA.end(); ia++) {
					if(m_bTerminating) break;	
					(*ia)->Draw(&qP, &m_M);
				}
			}
		}
		
		// Paint waypoints 
		if(bWaypoints) {
			qP.setRenderHint(QPainter::Antialiasing, false);
			route::WaypointContainer::ConstContainer::const_iterator iw;
			for(iw=conWpt.begin(); iw!=conWpt.end(); iw++) {
				if(m_bTerminating) break;	
				if(clipRect.contains((*iw)->GetCoordinate())) {
					(*iw)->Draw(qP, m_M);
					conTextLabels.prepend(new TextLabel(
						(*iw)->GetShortName(),
						(*iw)->GetCoordinate(),
						1
					));
				}
			}
		}
	}

	// Tell world object that we have stopped painting.
	pW->EndPaint();
}

// --------------------------------------------------------------------------

bool MapImage::GetImage(
	QImage& imgDst, 				//!< [O] Destination image to be created.
	QPoint& ptxOrg, 				//!< [I/O] Desired origin point [I] and actual origin point [O].
	float& fHeading, 				//!< [I/O] True heading in radians [I], angle correction [O].
	const QPointF& ptOrgWGS84,	//!< current coordinate of the airplane.
	int iRadius,					//!< Rotation radius from the origin to the most distant pixel.
	QVector<const TextLabel*>& conDrawLab
)
{
	const int iSideSize = m_image.width();

	// Set new origin position.
	// Start the background process if necessary.
	int iSafe = UpdateImage(ptOrgWGS84, iRadius);

	// If the image is not ready, there is nothing we can do.
	if(m_eStatus != sReady) {
		return false;
	}

	// Check amount of "iSafe" value. If it is too small
	// (too large negative value), we can't draw the image.
	if(iSafe < 0) {
		qDebug() << "iSafe =" << iSafe << ", radius =" << iRadius;
		if(abs(iSafe) > iRadius/2) {
			return false;
		} 
	}

	// Reserve the m_image object for this loop only
	// We don't wait
	if(m_mutexImage.tryLock()==false) {
		qDebug() << "Can't lock the mutex.";
		return false;
	}
	
	// Repeat the origin point calculation inside the mutex.
	QPoint ptxO = m_M.ConvertToImage(ptOrgWGS84);

	// Prepare the rotation constants. They all are made const to help
	// the compiler optimizing the code.
	
	// Origin of the destination image
	const int xii0 = ptxOrg.x();
	const int yii0 = ptxOrg.y();

	// Origin of the source image
	const int x0 = ptxO.x();
	const int y0 = ptxO.y();

	// Integer values of the cosinus and sinus.
	const int C = qRound(cos(-fHeading)*256);
	const int S = qRound(sin(-fHeading)*256);

	// Loop limits
	const int xiiMax = imgDst.width();
	const int yiiMax = imgDst.height();

	// Offset constants
	const int A0 = 256*x0 - xii0*C - yii0*S;
	const int B0 = 256*y0 + xii0*S - yii0*C;

	// Memory addresses
	QRgb* pDst = (QRgb*)imgDst.bits();
	const QRgb* const pSrc = (QRgb*)m_image.bits();

	// The main loop. We tried to code this loop in assembler, but we stopped.
	// The compiler generated code was excellent (we only study the ARM version)
	// and we could not find any real improvement in hand coding, 
	// just complications.
	for(int yii=0; yii<yiiMax; yii++) {
		int xii=0;
		int u = xii*C + yii*S + A0; // y*256
		int w = xii*S + yii*C + B0; // x*256
		for(; xii<xiiMax; xii++) {
			//             y * line size       +  x
			*pDst = pSrc[(w>>8)*iSideSize+(u>>8)];
			u += C;
			w -= S;
			pDst++;
		}
	}

	// List of text labels to plot on the screen.

	// Destination image rectangle
	QRect rDst(imgDst.rect());
	const int iCx = (xii0<<8) - C*x0 + S*y0;
	const int iCy = (yii0<<8) - S*x0 - C*y0;
	QPoint ptR;
	QPoint pt;
	
	// Cells used to optimize the text labels
	const int nC = rDst.width()/CELL_WIDTH + 1;
	const int nR = rDst.height()/CELL_HEIGHT + 1;
	conDrawLab.fill(NULL, nR*nC);

	QList<TextLabel*>::iterator tli;
	for(tli=m_conLabels.begin(); tli!=m_conLabels.end(); tli++) {
		pt = (*tli)->GetPosition();
		const int x = pt.rx();
		const int y = pt.ry();
		ptR.rx() = (C*x - S*y + iCx)>>8;
		ptR.ry() = (S*x + C*y + iCy)>>8;

		// We only take labels which fit into the destination rectangle.
		if(rDst.contains(ptR)) {
			(*tli)->SetDrawPosition(ptR);
			// Put the label into correct cell.
			int iCell = ((ptR.y()-CELL_HEIGHT/2)/CELL_HEIGHT)*nC
						 + (ptR.x() + CELL_WIDTH/2)/CELL_WIDTH;
			
			Q_ASSERT(iCell >=0 && iCell < conDrawLab.size());
			// is cell empty
			if(conDrawLab[iCell] == NULL) {
				conDrawLab[iCell] = (*tli);
			}
			// no, check priority (low is more important)
			else {
				if((*tli)->GetImportance() < conDrawLab[iCell]->GetImportance())
					conDrawLab[iCell] = (*tli);	
			}
		}
	}
	
	// Convert external points
	if(m_conExCoor.isEmpty()==false) {
		const int iN = m_conExCoor.size();
		m_conExPoint.resize(iN);
		for(int i=0; i<iN; i++) {
			pt = m_M.ConvertToImage(m_conExCoor[i]);

			const int x = pt.rx();
			const int y = pt.ry();
			ptR.rx() = (C*x - S*y + iCx)>>8;
			ptR.ry() = (S*x + C*y + iCy)>>8;

			m_conExPoint[i] = ptR;
		}
	}

	// unlock the image.
	m_mutexImage.unlock();

	// This is the case, where we need to change the position of the
	// origin point. This case should be very rare.
	if(iSafe < 0) {
		qWarning("MapImage::GetImage : Origin has to be moved!");
		QPoint pt = m_M.ConvertToImage(ptOrgWGS84);
		int x = pt.x() - x0;
		int y = pt.y() - y0;
		ptxOrg.rx() = (C*x - S*y + (xii0<<8)) >> 8;
		ptxOrg.ry() = (S*x + C*y + (yii0<<8)) >> 8;
	}
	return true;
}

// --------------------------------------------------------------------

void MapImage::SetZoomFactor(int iZoomFactor)
{
	QMutexLocker ml(&m_mutexImage);

	if(m_iZoomFactor!=iZoomFactor) {
		m_iZoomFactor = iZoomFactor;
		m_eStatus = sEmpty;
	}
}

// -------------------------------------------------------------------

#define SW		10

void MapImage::UpdateScalePixmap()
{
	QPointF ptO(b_ptO);
	QPoint pt = m_M.ConvertToImage(ptO);
	ptO.ry() += m_fpcScaleData.first;
	pt -= m_M.ConvertToImage(ptO);
	
	//qDebug() << "Pix =" << pt.y() << m_fpcScaleData.second;
	
	m_imgScale = QImage(50,pt.y()+25, QImage::Format_ARGB32);
	QRgb fc = QColor(0,0,0,0).rgba();
	m_imgScale.fill(fc);
	QPainter P(&m_imgScale);
 		
	QFont font = P.font();
	font.setWeight(QFont::Bold);
	P.setFont(font);
	QRect rcF(0,0, m_imgScale.width(),20);
	P.setBrush(QColor(0,0,0,90));
	P.setPen(Qt::NoPen);
	P.drawRect(rcF);
	P.setPen(Qt::white);
	P.drawText(rcF, Qt::AlignCenter, m_fpcScaleData.second);
	
	P.setPen(Qt::black);
	P.setBrush(Qt::white);
	P.drawRect(0,20,          SW,pt.y());
	// Draw 
	P.setBrush(QColor(Qt::black));
	P.drawRect(SW/2,20,       SW/2,pt.y()/2);
	P.drawRect(0,20+pt.y()/2, SW/2,pt.y()/2); 
	//m_imgScale.save("/home/akrajnc/Test.png");
}

// -------------------------------------------------------------------

} // namespace
