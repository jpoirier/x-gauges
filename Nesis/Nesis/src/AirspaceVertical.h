#ifndef __AIRSPACE_VERTICAL_WIDGET_H
#define __AIRSPACE_VERTICAL_WIDGET_H

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

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include "Map/ActiveAirspaceContainer.h"

// ----------------------------------------------------------------------

class AirspaceVertical : public QObject
{
Q_OBJECT
public:
	//! Constructor
	AirspaceVertical(QObject *pParent = 0);
	//! Destructor
	virtual ~AirspaceVertical();

	//! Painting the airspace.
	virtual void Draw(QPainter& P, const QRect& rc) const;

private slots:
	//! Update airspace container
	void OnUpdateAirspaces();
					
protected:
	//! Current active airspace container.
	map::ActiveAirspaceContainer m_conAS;	
	//! Airplane pixmap
	QPixmap m_pixAirplane;
};

// ----------------------------------------------------------------------

#endif
