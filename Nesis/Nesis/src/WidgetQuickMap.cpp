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
#include "WidgetQuickMap.h"
#include "GraphicsSingleton.h"
#include "NesisAerospaceDataManager.h"
#include "Map/ProjectionFactory.h"

// --------------------------------------------------------------------------

WidgetQuickMap::WidgetQuickMap(QWidget* pParent)
:
	QWidget(pParent)
{
	Settings* pSettings = Settings::GetInstance();

	m_ptAirplaneRel = QPointF(0.5f, 0.5f);
	m_ptPosition = QPointF(1.0, 1.0);
	
 	m_pMI = new map::MapImage(220, pSettings->GetScreenResolution(), QImage::Format_ARGB32);
	m_pxAirplane.load(Settings::GetInstance()->GetImagePath()+"Airplane-HS.png");
	m_iRadius = 0;
// 	setAttribute(Qt::WA_NoSystemBackground, true);
	m_conLabels.reserve(400);
	setFixedSize(200,200);
	m_fMapRotation	= 0; 		// north up

	// Map projection
	QPointF ptWGS = NesisAerospaceDataManager::GetInstance()->GetPosition();
	if(ptWGS.isNull())
		ptWGS = pSettings->GetMapStartPosition();

	map::ProjectionFactory projectionFactory;
	map::AbstractProjection* pProj;
	pProj = projectionFactory.Create(ptWGS, pSettings->GetProjection());
	m_pMI->SetProjection(pProj);
	SetZoom(2000000);
}
// --------------------------------------------------------------------------

WidgetQuickMap::~WidgetQuickMap()
{
}

// --------------------------------------------------------------------------

void WidgetQuickMap::resizeEvent(QResizeEvent* /*pEvent*/)
{
	const int iW = width();
	const int iH = height();
	int ix = (int)(m_ptAirplaneRel.x()*iW);
	int iy = (int)(m_ptAirplaneRel.y()*iH);
	m_ptAirplane = QPoint(ix,iy);

	m_iRadius = qMax(iW, iH)/2+10;
	
	// Image for the map
	m_image = QImage(iW,iH, QImage::Format_ARGB32);
}

// --------------------------------------------------------------------------

void WidgetQuickMap::paintEvent(QPaintEvent* /*pPE*/)
{
	QPainter P(this);
	Draw(P);
	DrawAirplane(P);
}

// --------------------------------------------------------------------------

bool WidgetQuickMap::Draw(QPainter& P)
{
	// Set font
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	P.setFont(pGS->GetFont(GraphicsSingleton::fMapCity));

	// Get current position!
// 	NesisAerospaceDataManager* pDM = NesisAerospaceDataManager::GetInstance();
// 	QPointF ptPosWGS = pDM->GetPosition();
// 	qDebug() << "Pos =" << ptPosWGS;
	m_ptAirplane.rx() =(int)(m_ptAirplaneRel.x()*width());
	m_ptAirplane.ry() =(int)(m_ptAirplaneRel.y()*height());

	// Container of labels
	m_conLabels.clear();
	
	// Try to get the image
	bool bSuccess = m_pMI->GetImage(
			m_image,
			m_ptAirplane,
			m_fMapRotation,
			m_ptPosition,
			m_iRadius,
			m_conLabels
	);

	// Draw the image on success
	if(bSuccess) {
		P.drawImage(QPoint(0,0), m_image);

		foreach(const map::TextLabel* ptl, m_conLabels) {
			if(ptl == NULL) continue;
			QPoint p = ptl->GetDrawPosition();
			P.drawText(
				p.x()+5, p.y()-3,
				120, 40,
				Qt::AlignLeft | Qt::AlignTop,
				ptl->GetText()
			);
			// This code seems to be faster, but makes long names and
			// is not able to interpret the "\n" in the text.
			// See also "LayerEntityAirfield::GetTextLabel.
// 			P.drawText(p.x()+5, p.y()-3, ptl->GetText());

// 			P.drawLine(p.x()-3, p.y()-3, p.x()+3, p.y()+3);
// 			P.drawLine(p.x()+3, p.y()-3, p.x()-3, p.y()+3);
			
			// draw airplane
			P.drawPixmap(m_ptAirplane.x()-20, m_ptAirplane.y()-16, m_pxAirplane);

		}
	}
	// Draw some text if we are waiting
	else {
		if(m_pMI->GetStatus()==map::MapImage::sEmpty) {
			static int iC = 0;
			iC++;
			P.setFont(pGS->GetFont(GraphicsSingleton::fMapStartup));
			P.setPen(Qt::white);
			P.drawText(0,0, width(), height(), Qt::AlignCenter,
						QString(tr("Calculating...")));
		}
	}
	return bSuccess;
}

// --------------------------------------------------------------------------

void WidgetQuickMap::SetZoom(int iZoomFactor)
{
	m_pMI->SetZoomFactor(iZoomFactor);
}

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------

void WidgetQuickMap::DrawAirplane(QPainter& P)
{
	//P.resetMatrix();
	// TODO check the pixmap size first and then calculate the offset.
	P.drawPixmap(m_ptAirplane.x()-20, m_ptAirplane.y()-16, m_pxAirplane);
}

// --------------------------------------------------------------------------

void WidgetQuickMap::SetPosition(QPointF ptPos)
{
	m_ptPosition = ptPos*M_PI/180.0;
}

