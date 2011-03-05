#ifndef __HSI_MAP_WIDGET_H
#define __HSI_MAP_WIDGET_H

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

#include "WidgetMap.h"
#include "AirspaceVertical.h"

// ----------------------------------------------------------------------

class Settings;
class FastUnitKeys;
class NesisAerospaceDataManager;
class GraphicsSingleton;
class ActiveNavigation;
class PixmapBattery;

class WidgetHSIMap : public WidgetMap
{
Q_OBJECT
public:
	//! Constructor
	WidgetHSIMap(int iArcRadius, const QPointF& ptAirplaneRel, AirspaceVertical* pAS = 0, QWidget *pParent = 0);
	//! Destructor
	virtual ~WidgetHSIMap();

protected:
	//! On the resize we create the frame pixmap.
	void resizeEvent(QResizeEvent *pEvent);
	//! Painting of the widget.
	virtual void paintEvent(QPaintEvent* pPE);

	//! Draw the HSI scale
	void DrawHSI(QPainter& P, float fHeadingDeg);
	//! Draw the tracking line and two prediction points.
	void DrawTrackingLine(QPainter& P, const QVector<QPoint>& vEP,
								 const QVector<QPair<int, const char*> >& vPP);
	//! Draw the active route.
	void DrawRoute(QPainter& P, const QVector<QPoint>& vEP, int iFirst, float fRelPosLeg);
	//! Draw the direct to path to the active waypoint.
	void DrawDirectToWaypoint(QPainter& P, const QPoint& ptD);
	//! Draw wind direction arrow
	void DrawWindDirection(QPainter& P);

	//! Draw heading numbers label
	virtual void DrawHeadingNumbers(QPainter& P, int iTopOffset);
	//! Draw scale
	void DrawScale(QPainter& P, int iLeftOffset);

	//! Draw the top bar information, return the bottom coordinate.
	virtual int DrawTopBar(QPainter& P);
	//! Draw the bottom bar information, return the top coordinate.
	virtual int DrawBottomBar(QPainter& P);
	
protected:
	//! Heading triangle pixmap
	QPixmap m_pixHeadingTriangle;
	//! Heading bug pixmap
//	QPixmap m_pixHeadingBug;
	//! Should we draw vertical airspace as well - we do not own the object!
	const AirspaceVertical* m_pAS;

	//! Radius of the arch
	int m_iArcRadius;
	//! Wind arrow polygon
	QPolygon m_pgWind;
	//! Frame thickness - if frame exists.
	int m_iFrameThickness;

	//! Heading label background.
	QPolygon m_pgHeading;

	// Pointers to singletons.

	Settings* 						m_pS;		//!< Application settings.
	FastUnitKeys* 					m_pFUK;	//!< UnitManager key integers.
	NesisAerospaceDataManager* m_pND;	//!< Aerospace data manager.
	GraphicsSingleton* 			m_pGS;	//!< Common graphics object.
	ActiveNavigation* 			m_pAN;	//!< Active navigation data.
	PixmapBattery*             m_pPixBat; //!< Battery status pixmaps.
};

// ----------------------------------------------------------------------

#endif
