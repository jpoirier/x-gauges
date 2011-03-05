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

#include <QPainter>
#include "Settings.h"
#include "PixmapBattery.h"

// -----------------------------------------------------------------------

std::auto_ptr<PixmapBattery> PixmapBattery::s_apInstance;

// -----------------------------------------------------------------------

PixmapBattery::PixmapBattery()
{
	Settings* pS = Settings::GetInstance();
	const QString& qsPath = pS->GetImagePath();
	// Load pixmaps
	m_vpxOnBat << QPixmap(qsPath + "Battery-20.png");
	m_vpxOnBat << QPixmap(qsPath + "Battery-40.png");
	m_vpxOnBat << QPixmap(qsPath + "Battery-60.png");
	m_vpxOnBat << QPixmap(qsPath + "Battery-80.png");
	m_vpxOnBat << QPixmap(qsPath + "Battery-100.png");
	
	m_vpxCharge << QPixmap(qsPath + "BatteryCharging-20.png");
	m_vpxCharge << QPixmap(qsPath + "BatteryCharging-40.png");
	m_vpxCharge << QPixmap(qsPath + "BatteryCharging-60.png");
	m_vpxCharge << QPixmap(qsPath + "BatteryCharging-80.png");
	m_vpxCharge << QPixmap(qsPath + "BatteryCharging-100.png");
}

PixmapBattery::~PixmapBattery()
{
}
// -----------------------------------------------------------------------

PixmapBattery* PixmapBattery::GetInstance()
{
	if(s_apInstance.get()==NULL) {
		s_apInstance = std::auto_ptr<PixmapBattery>(new PixmapBattery);
	}
	return s_apInstance.get();
}

// -----------------------------------------------------------------------

void PixmapBattery::Draw(
	QPainter& P, 	//!< The object used to draw things.
	int iX, 			//!< X left coordinate of pixmap
	int iY, 			//!< Y top coordinate of pixmap
	Mode eMode, 	//!< The battery mode.
	int iCapacity	//!< Capacity in % [0-100].
) const
{
	int i = iCapacity / 20;
	if(i>4) i=4;
	
	if(eMode==mCharge) {
		P.drawPixmap(iX, iY, m_vpxCharge[i]);
	}
	else if(eMode==mOnBattery) {
		P.drawPixmap(iX, iY, m_vpxOnBat[i]);
	}
}

// -----------------------------------------------------------------------

int PixmapBattery::GetWidth() const
{
	return m_vpxOnBat[0].width();
}

// -----------------------------------------------------------------------

int PixmapBattery::GetHeight() const
{
	return m_vpxOnBat[0].height();
}

// -----------------------------------------------------------------------
