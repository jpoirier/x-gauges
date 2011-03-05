#ifndef WIDGET_HSI_MINI_MAP_H
#define WIDGET_HSI_MINI_MAP_H

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

#include "WidgetHSIMap.h"

// ----------------------------------------------------------------------

class WidgetHSIMiniMap : public WidgetHSIMap
{
Q_OBJECT
public:
	//! Constructor
	WidgetHSIMiniMap(int iArcRadius, const QPointF& ptAirplaneRel,
					 AirspaceVertical* pAS = 0, QWidget *pParent = 0);
	//! Destructor
	virtual ~WidgetHSIMiniMap();

	//! Enable frame.
	void EnableFrame(bool bEnable=true)
	{ m_bDrawFrame = bEnable; }
protected:
	//! Details on the widget resize.
	void resizeEvent(QResizeEvent* pEvent);
	//! Paint the widget.
	void paintEvent(QPaintEvent* pPE);

	//! Draw the top bar (wind, bearing and distance).
	int DrawTopBar(QPainter& P);

	//! Draw the bottom bar.
	int DrawBottomBar(QPainter& P);

protected:
	//! Shall we draw the thick frame around the map.
	bool m_bDrawFrame;
	//! The pixmap of the frame.
	QPixmap m_pxFrame;
};

// ----------------------------------------------------------------------

#endif
