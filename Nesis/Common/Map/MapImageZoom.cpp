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

#include <QtDebug>
#include <QPainter>
#include "World.h"
#include "MathEx.h"
#include "LambertConformalConicProjection.h"
#include "MapImageZoom.h"

namespace map {

// TODO Define QImage::Format depending on the platform
// TODO Change the constructor - it should take current coordinates as parameters
//      which will be used to calculate projection settings.
// ----------------------------------------------------------------------------

MapImageZoom::MapImageZoom()
{
	m_bTerminating = false;
}

MapImageZoom::~MapImageZoom()
{
}

// ----------------------------------------------------------------------------

void MapImageZoom::AddImage(
	int iZoomFactor,
	const QString& qsZoomName,
	int iSide,
	int iPixPerMeter,
	int iSafetyMargin,
	QPair<int, const char*> ipcPP1, 
	QPair<int, const char*> ipcPP2,
	QPair<float, const char*> fpcScale
)
{
	if(m_conImages.contains(iZoomFactor)) {
		return;
	}

	// TODO Test with the QImage::Format_ARGB32_Premultiplied

	// Create MapImage object ...
	MapImage* pMI = new MapImage(
		iSide,
		iPixPerMeter,
		QImage::Format_ARGB32
	);
	pMI->SetZoomFactor(iZoomFactor);
	pMI->SetZoomName(qsZoomName);
	pMI->SetSafetyMargin(iSafetyMargin);
	pMI->AddPredictionPoint(ipcPP1);
	pMI->AddPredictionPoint(ipcPP2);
	pMI->SetScaleData(fpcScale);
	m_conImages.insert(iZoomFactor, pMI);
}

// --------------------------------------------------------------------------

bool MapImageZoom::GetImage(
	int iKey,
	QImage& img,
	QPoint& ptxOrg,
	float fHeading,
	const QPointF& ptOrgWGS,
	int iRadius,
	QVector<const TextLabel*>& conDrawLabels
)
{
	if(m_bTerminating)
		return false;
// 	std::cout << "GetImage(), key =" << fKey  << std::endl;
	MapImage* pMI = m_conImages.value(iKey, NULL);
	ASSERT(pMI != NULL);

	bool bSuccess = pMI->GetImage(img, ptxOrg, fHeading, ptOrgWGS, iRadius, conDrawLabels);

	// if image was obtained successfully, go and update other
	// zoom levels.
/*	if(bSuccess) {
		ImageCon::iterator i;
		for(i=m_conImages.begin(); i!=m_conImages.end(); i++) {
			// Skip current zoom level
			if(*i != pMI)
				(*i)->UpdateImage(ptOrgWGS, iRadius);
		}
	}*/
	return bSuccess;
}

// --------------------------------------------------------------------------

void MapImageZoom::SetProjection(AbstractProjection* pProj)
{
	m_apProjection = std::auto_ptr<AbstractProjection>(pProj);
	ImageCon::iterator i;
	for(i=m_conImages.begin(); i!=m_conImages.end(); i++) {
		(*i)->SetProjection(pProj);
	}
}
// --------------------------------------------------------------------------

void MapImageZoom::UpdateImages(const QPointF& ptOrgWGS, int iRadius)
{
	if(m_bTerminating)
		return;
		
	// TODO Improve update priority
	foreach(MapImage* pMI, m_conImages)
		pMI->UpdateImage(ptOrgWGS, iRadius);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

SingletonMapImageZoom* SingletonMapImageZoom::s_pInstance = NULL;

// --------------------------------------------------------------------------

SingletonMapImageZoom::SingletonMapImageZoom()
{

}

SingletonMapImageZoom::~SingletonMapImageZoom()
{
}

// --------------------------------------------------------------------------

MapImageZoom* SingletonMapImageZoom::GetInstance()
{
	if(s_pInstance==NULL)
		s_pInstance = new SingletonMapImageZoom();
	// Note - we are returning access to the internal object.
	return &s_pInstance->m_miz;
}

// --------------------------------------------------------------------------

MapImage::Status MapImageZoom::GetStatus(int iZoomFactor) const
{
	MapImage* pMI = m_conImages.value(iZoomFactor, NULL);
	if(pMI) {
		return pMI->GetStatus();
	}
	return MapImage::sEmpty;
}

// --------------------------------------------------------------------------

QStringList MapImageZoom::GetZoomNameList() const
{
	QStringList sl;
	foreach(const MapImage* pI, m_conImages)
		sl << pI->GetZoomName();
	return sl;
}

// --------------------------------------------------------------------------

QVector<int> MapImageZoom::GetZoomFactorList() const
{
	QVector<int> vi;
	foreach(const MapImage* pI, m_conImages)
		vi << pI->GetZoomFactor();
	return vi;
}

// --------------------------------------------------------------------------

QVector<MapImage::Status> MapImageZoom::GetStatusList() const
{
	QVector<MapImage::Status> vs;
	foreach(const MapImage* pI, m_conImages)
		vs << pI->GetStatus();
	return vs;
}
// --------------------------------------------------------------------------

void MapImageZoom::GetZoomData(
	QStringList& sl,
	QVector<int>& vi,
	QVector<MapImage::Status>& vs
) const
{
	foreach(const MapImage* pI, m_conImages) {
		sl << pI->GetZoomName();
		vi << pI->GetZoomFactor();
		vs << pI->GetStatus();
	}
}
// --------------------------------------------------------------------------

void MapImageZoom::SetExternalCoordinates(
	int iZoomFactor,
	const QVector<QPointF>& conCoor
)
{
	MapImage* pMI = m_conImages.value(iZoomFactor, NULL);
	if(pMI) {
		pMI->SetExternalCoordinates(conCoor);
	}
}

// --------------------------------------------------------------------------

QVector<QPoint> MapImageZoom::GetExternalPoints(int iZoomFactor) const
{
	MapImage* pMI = m_conImages.value(iZoomFactor, NULL);
	if(pMI) {
		return pMI->GetExternalPoints();
	}
	return QVector<QPoint>();
}
// --------------------------------------------------------------------------

int MapImageZoom::GetNextZoomFactor(int iZoomFactor, bool bZoomIn) const
{
	ImageCon::const_iterator ci = m_conImages.find(iZoomFactor);
	if(ci==m_conImages.end())
		return 0;

	if(bZoomIn) {
		if(ci!=m_conImages.begin())
			ci--;
	} else {
		if(ci!=(m_conImages.end()-1))
			ci++;
	}

	// Check status
	if(ci.value()->GetStatus()==MapImage::sEmpty) {
		return -ci.value()->GetZoomFactor();
	}
	else {
		return ci.value()->GetZoomFactor();
	}
}

// --------------------------------------------------------------------------

const QVector<QPair<int, const char*> >& MapImageZoom::GetPredictionPoints(
	int iZoomFactor
) const
{
	MapImage* pMI = m_conImages.value(iZoomFactor, NULL);
	Q_ASSERT(pMI != NULL);
	return pMI->GetPredictionPoints();
}
// --------------------------------------------------------------------------

void MapImageZoom::Terminate()
{
	m_bTerminating = true;
	foreach(MapImage* pI, m_conImages)
		pI->Terminate();
	foreach(MapImage* pI, m_conImages) {
		if(pI->isRunning()) {
			//pI->terminate();
			pI->wait();
		}
	}
}
// --------------------------------------------------------------------------

const QImage& MapImageZoom::GetScaleImage(
	int iZoomFactor
) const
{
	MapImage* pMI = m_conImages.value(iZoomFactor, NULL);
	Q_ASSERT(pMI != NULL);
	return pMI->GetScaleImage();
}

// --------------------------------------------------------------------------
} // namespace

