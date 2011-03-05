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
#include <QPainter>
#include <QTimer>

#include "WidgetHSIMap.h"
#include "WidgetMain.h"

#include "PanelHSIMap.h"

PanelHSIMap::PanelHSIMap(QSize sz, QWidget *pParent)
 : AbstractPanel(sz, pParent)
{
	setFixedSize(sz);
	const int iW = width();
	const int iH = height();

	m_pAS = new AirspaceVertical(this);

	m_pwMap = new WidgetHSIMap(iH*0.45, QPointF(0.5, 0.65), m_pAS, this);
	m_pwMap->setGeometry(0,0, iW,iH);
}


PanelHSIMap::~PanelHSIMap()
{
}

// -----------------------------------------------------------------------

void PanelHSIMap::Draw(bool bMajor)
{
/*	static int iCount=0;
	if(iCount++%5 == 0)*/
	if(bMajor)
		m_pwMap->repaint();
}
// -----------------------------------------------------------------------

void PanelHSIMap::SetZoom(int iZoomFactor)
{
	((WidgetMap*)m_pwMap)->SetZoom(iZoomFactor);
}

// -----------------------------------------------------------------------

bool PanelHSIMap::ChangeZoom(bool bIn)
{
	return ((WidgetMap*)m_pwMap)->ChangeZoom(bIn);
}

// -----------------------------------------------------------------------

QStringList PanelHSIMap::GetMenuLabels() const
{
	QStringList sl;
	sl << tr("QNH");
	sl << QString(); //tr("Altitude");
	sl << tr("Airfield");
	sl << tr("Waypoint");
	sl << tr("Route");
	return sl;
}

// -----------------------------------------------------------------------

void PanelHSIMap::OnMenuButtonPressed(PanelButton ePB)
{
	switch(ePB) {
		case pbBtn1:
			GetWidgetMain()->OnSetQNH();
			break;
		case pbBtn2:
			//GetWidgetMain()->OnSetAltitude();
			break;
		case pbBtn3:
//			GetWidgetMain()->OnSetHeading();
			GetWidgetMain()->OnSelectNearestAirfield();
			break;
		case pbBtn4:
			GetWidgetMain()->OnSelectWaypoint();
			break;
		case pbBtn5:
			GetWidgetMain()->OnSelectRoute();
			break;
		default:
			break;
	}
	SetDirectMode();
}

// -----------------------------------------------------------------------
