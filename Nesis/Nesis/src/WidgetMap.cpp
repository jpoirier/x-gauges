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

#include <QtGui>
#include "Settings.h"
#include "MathEx.h"
#include "Map/TextLabel.h"
#include "WidgetMap.h"
#include "GraphicsSingleton.h"
#include "NesisAerospaceDataManager.h"

// --------------------------------------------------------------------------

WidgetMap::WidgetMap(const QPointF& ptAirplaneRel, QWidget* pParent)
:
	QWidget(pParent)
{
	m_ptAirplaneRel = ptAirplaneRel;
	m_iZoomFactor = 500000; // Default
	m_pMIZ = map::SingletonMapImageZoom::GetInstance();
	m_pixAirplane.load(Settings::GetInstance()->GetImagePath()+"Airplane-HS.png");
	m_iRadius = 0;
// 	setAttribute(Qt::WA_NoSystemBackground, true);
	m_conLabels.reserve(400);
	
	m_fMapRotation	= 0; // heading, tracking, north up
}

WidgetMap::~WidgetMap()
{
}

// --------------------------------------------------------------------------

void WidgetMap::resizeEvent(QResizeEvent* /*pEvent*/)
{
	const int iW = width();
	const int iH = height();
	int ix = (int)(m_ptAirplaneRel.x()*iW);
	int iy = (int)(m_ptAirplaneRel.y()*iH);
	m_ptAirplane = QPoint(ix,iy);

	m_iRadius = (int)sqrt(ix*ix + iy*iy)+2;
	
	// Image for the map
	m_image = QImage(iW,iH, QImage::Format_ARGB32);
}

// --------------------------------------------------------------------------

void WidgetMap::paintEvent(QPaintEvent* /*pPE*/)
{
	QPainter P(this);
	Draw(P);
}

// --------------------------------------------------------------------------

bool WidgetMap::Draw(QPainter& P)
{
	// Set font
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	P.setFont(pGS->GetFont(GraphicsSingleton::fMapCity));

	// Get current position!
	NesisAerospaceDataManager* pDM = NesisAerospaceDataManager::GetInstance();
	QPointF ptPosWGS = pDM->GetPosition();
//	qDebug() << "Pos =" << ptPosWGS; 
	// Get our heading (in radians);

	// Container of labels
	m_conLabels.clear();
	
	// Try to get the image
	bool bSuccess = m_pMIZ->GetImage(
			m_iZoomFactor,
			m_image,
			m_ptAirplane,
			m_fMapRotation,
			ptPosWGS,
			m_iRadius,
			m_conLabels
	);

	// Draw the image on success
	if(bSuccess) {
		P.drawImage(QPoint(0,0), m_image);

		foreach(const map::TextLabel* ptl, m_conLabels) {
			if(ptl == NULL) continue;
			QPoint p = ptl->GetDrawPosition();
			/*P.drawText(
				p.x()+5, p.y()-3,
				120, 40,
				Qt::AlignLeft | Qt::AlignTop,
				ptl->GetText()
			);*/
			// This code seems to be faster, but makes long names and
			// is not able to interpret the "\n" in the text.
			// See also "LayerEntityAirfield::GetTextLabel.
			P.drawText(p.x()+5, p.y()-3, ptl->GetText());

			//P.drawLine(p.x()-3, p.y()-3, p.x()+3, p.y()+3);
			//P.drawLine(p.x()+3, p.y()-3, p.x()-3, p.y()+3);*/
		}
	}
	// Draw some text if we are waiting
	else {
		if(m_pMIZ->GetStatus(m_iZoomFactor)==map::MapImage::sEmpty) {
			static int iC = 0;
			iC++;
			P.setFont(pGS->GetFont(GraphicsSingleton::fMapStartup));
			P.setPen(Qt::white);
			P.drawText(0,0, width(), height(), Qt::AlignCenter,
						QString(tr("Please wait... %1")).arg(iC));
		}
	}
	return bSuccess;
}

// --------------------------------------------------------------------------

void WidgetMap::SetZoom(int iZoomFactor)
{
	m_iZoomFactor = iZoomFactor;
	update();
}

// --------------------------------------------------------------------------

bool WidgetMap::ChangeZoom(bool bIn)
{
	// Try to get next level.
	int iNextZoom = m_pMIZ->GetNextZoomFactor(m_iZoomFactor, bIn);
	ASSERT(iNextZoom != 0);

	if(iNextZoom > 0) {
		if(iNextZoom != m_iZoomFactor) {
			SetZoom(iNextZoom);
		}
		return true;
	}
	return false;
}

// --------------------------------------------------------------------------

void WidgetMap::DrawAirplane(QPainter& P)
{
	//P.resetMatrix();
	// TODO check the pixmap size first and then calculate the offset.
	P.drawPixmap(m_ptAirplane.x()-20, m_ptAirplane.y()-16, m_pixAirplane);
}

// --------------------------------------------------------------------------
