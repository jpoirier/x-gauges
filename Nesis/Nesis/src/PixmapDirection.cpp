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

#include <QPainter>
#include "Settings.h"
#include "PixmapDirection.h"

// -----------------------------------------------------------------------

std::auto_ptr<PixmapDirection> PixmapDirection::s_apInstance;

// -----------------------------------------------------------------------

PixmapDirection::PixmapDirection()
{
	Settings* pS = Settings::GetInstance();
	const QString& qsPath = pS->GetImagePath();
	// Load pixmaps
	m_vpxArrow << QPixmap(qsPath + "ArrowN.png");
	m_vpxArrow << QPixmap(qsPath + "ArrowNE.png");
	m_vpxArrow << QPixmap(qsPath + "ArrowE.png");
	m_vpxArrow << QPixmap(qsPath + "ArrowSE.png");
	m_vpxArrow << QPixmap(qsPath + "ArrowS.png");	
	m_vpxArrow << QPixmap(qsPath + "ArrowSW.png");	
	m_vpxArrow << QPixmap(qsPath + "ArrowW.png");	
	m_vpxArrow << QPixmap(qsPath + "ArrowNW.png");	
}

PixmapDirection::~PixmapDirection()
{
}
// -----------------------------------------------------------------------

PixmapDirection* PixmapDirection::GetInstance()
{
	if(s_apInstance.get()==NULL) {
		s_apInstance = std::auto_ptr<PixmapDirection>(new PixmapDirection);
	}
	return s_apInstance.get();
}

// -----------------------------------------------------------------------

const QPixmap& PixmapDirection::GetPixmap(
	float fHeading, 	//!< Current heading.
	float fBearing		//!< Required bearing.
) const
{
	float fA = fBearing - fHeading;
	if(fA < 0)
		fA += 2*M_PI;
		
	int i = qRound(fA/M_PI_4);
	if(i<0) i=0;
	if(i>=m_vpxArrow.count()) i=0;
	
	return m_vpxArrow[i];
}

// -----------------------------------------------------------------------

void PixmapDirection::Draw(
	QPainter& P, 		//!< The object used to draw things.
	int iX, 				//!< X left coordinate of pixmap
	int iY, 				//!< Y top coordinate of pixmap
	float fHeading, 	//!< Current heading.
	float fBearing		//!< Required bearing.
) const
{
	P.drawPixmap(iX, iY, GetPixmap(fHeading, fBearing));
}

// -----------------------------------------------------------------------

int PixmapDirection::GetWidth() const
{
	return m_vpxArrow[0].width();
}

// -----------------------------------------------------------------------

int PixmapDirection::GetHeight() const
{
	return m_vpxArrow[0].height();
}

// -----------------------------------------------------------------------
