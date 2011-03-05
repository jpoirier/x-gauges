#ifndef PIXMAP_DIRECTION_H
#define PIXMAP_DIRECTION_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QPixmap>
#include <QVector>
#include <memory>

class QPainter;

// ----------------------------------------------------------------------

class PixmapDirection 
{
private:
	//! Constructor
	PixmapDirection();
public:
	//! Destructor
	virtual ~PixmapDirection();
	
	//! Access to the pixmap direction object.
	static PixmapDirection* GetInstance();
	
	//! Draw the direction according to current heading and required bearing.
	void Draw(QPainter& P, int iX, int iY, float fHeading, float fBearing) const;
	
	//! Get the pixmap.
	const QPixmap& GetPixmap(float fHeading, float fBearing) const;
	
	//! Get width of the pixmaps. 
	int GetWidth() const;
	//! Get height of the pixmaps.
	int GetHeight() const;
	
private:
	//! The only object.
	static std::auto_ptr<PixmapDirection> s_apInstance;

	//! Vector of arrows loaded at the startup.
	QVector<QPixmap> m_vpxArrow;
};

// ----------------------------------------------------------------------

#endif
