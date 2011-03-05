#ifndef PIXMAP_BATTERY_H
#define PIXMAP_BATTERY_H

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

#include <QPixmap>
#include <QVector>
#include <memory>

class QPainter;

// ----------------------------------------------------------------------

class PixmapBattery 
{
public:
	//! Battery modes - same as the modes in sys::Power
	enum Mode {
		mIdle 		= 0,	//!< Not active
		mCharge 		= 1,	//!< Charging
//		mDischarge	= 2,  //!< Discharging
		mGasGauge	= 3,	//!< Running on battery.
		mOnBattery  = 3
	};
	
private:
	//! Constructor
	PixmapBattery();
public:
	//! Destructor
	virtual ~PixmapBattery();
	
	//! Access to the pixmap battery object.
	static PixmapBattery* GetInstance();
	
	//! Draw the battery pixamp according to the battery mode and relative capacity.
	void Draw(QPainter& P, int iX, int iY, Mode eMode, int iCapacity) const;
	
	//! Get width of the pixmaps. 
	int GetWidth() const;
	//! Get height of the pixmaps.
	int GetHeight() const;
	
private:
	//! The only object.
	static std::auto_ptr<PixmapBattery> s_apInstance;

	//! Vector of charging pixmaps loaded at the startup.
	QVector<QPixmap> m_vpxCharge;
	//! Vector of "On Battery" pixmaps loaded at the startup.
	QVector<QPixmap> m_vpxOnBat;
};

// ----------------------------------------------------------------------

#endif
