#ifndef WIDGET_QUICK_MAP_H
#define WIDGET_QUICK_MAP_H

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
#include <QPixmap>
#include <QFont>
#include <QImage>
#include "Map/MapImageZoom.h"

// -------------------------------------------------------------------------

class WidgetQuickMap : public QWidget
{
Q_OBJECT
	
public:
	//! Constructor.
	WidgetQuickMap(QWidget* pParent = NULL);
	//! Destructor.
	~WidgetQuickMap();
	
	//! Set zoom level for current map.
	void SetZoom(int iZoomFactor);

	//! Set zoom level for current map.
// 	bool ChangeZoom(bool bIn);
	
	//! The paint event.
	virtual void paintEvent(QPaintEvent* pPE);

	//! Return current radius of map!
	int GetRadius() const
	{ return m_iRadius; }
	
	//! Draw the map.
	virtual bool Draw(QPainter& P);

	//! Draw the airplane image
 	void DrawAirplane(QPainter& P);

	void SetPosition(QPointF ptPos);

protected:
	//! Set correct center position, radius and image object on resize.
	void resizeEvent(QResizeEvent *pEvent);	

protected:
	//! Map rotation angle
	float m_fMapRotation;

	//! Special map class that performs the background painting.
	map::MapImage* m_pMI;
	//! Currently used zoom factor used as a key to background image.
	int m_iZoomFactor;
	//! Airplane pixmap
	QPixmap m_pxAirplane;
	
	//! We keep required relative airplane center position.
	QPointF m_ptAirplaneRel;
	//! Airplane center position in pixels.
	QPoint m_ptAirplane;
	
	QPointF m_ptPosition;
	
	//! Required map radius in pixels.
	int m_iRadius;
	
	//! Internal image which gets a copy of map.
	QImage m_image;
	/** Current labels that are needed to draw. This variable is global
	    because we do not want to allocate and reallocate it all the time. */
	QVector<const map::TextLabel*> m_conLabels;

};

// --------------------------------------------------------------------------

#endif // __MAPWIDGET_H
// -------------------------------------------------------------------------
