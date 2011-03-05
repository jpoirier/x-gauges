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

#include <QDebug>
#include <QFontInfo>
#include "GraphicsSingleton.h"

#define FONT_FAMILY		"Bitstream Vera Sans"
//#define FONT_FAMILY		"Babelfish" // useful for font error checking

// -----------------------------------------------------------------------

GraphicsSingleton* GraphicsSingleton::s_pInstance = NULL;

GraphicsSingleton::GraphicsSingleton()
{
	// Fonts
	m_aFont[fButton]     = QFont(FONT_FAMILY, 11, QFont::DemiBold);
	m_aFont[fDialog]     = QFont(FONT_FAMILY, 13);
	m_aFont[fDialogSmall]= QFont(FONT_FAMILY, 12);
	m_aFont[fMapStartup] = QFont(FONT_FAMILY, 14, QFont::DemiBold);
	m_aFont[fHSIHeading] = QFont(FONT_FAMILY, 20, QFont::DemiBold);
	m_aFont[fMessageText]= QFont(FONT_FAMILY, 16);
	m_aFont[fBanner]     = QFont(FONT_FAMILY, 10);
	m_aFont[fSmall]      = QFont(FONT_FAMILY, 9);

	// Sizes are given in pixels rather than in points
	m_aFont[fButton].setPixelSize(15);
	m_aFont[fDialog].setPixelSize(17);
	m_aFont[fDialogSmall].setPixelSize(16);
	m_aFont[fBanner].setPixelSize(14);
	m_aFont[fMapStartup].setPixelSize(19);
	m_aFont[fHSIHeading].setPixelSize(27);
	m_aFont[fMessageText].setPixelSize(22);
	m_aFont[fSmall].setPixelSize(13);

	// Font Alias
	m_aFont[fHSIText]    = m_aFont[fButton];
	m_aFont[fVASText]    = m_aFont[fBanner];
	m_aFont[fMapCity]    = m_aFont[fBanner];
	m_aFont[fKnobBuddy]  = m_aFont[fMapStartup];

	// Brushes
	m_aBrush[bBackground]     = QBrush(QColor(76,76,76));
// 	m_aBrush[bBackground]    = QBrush(QColor(0x32, 0x57, 0x71));
	m_aBrush[bAirspace]       = QBrush(QColor(230,10,10,50));
	m_aBrush[bBannerBack]     = QBrush(QColor(0,0,0,180));
	m_aBrush[bVAirspaceBack]  = QBrush(QColor(0,0,0,90));
	m_aBrush[bModernBlackBgd] = QBrush(QColor(10,10,10,255));

	// Pens
	m_aPen[pHSITracking] = QPen(Qt::DotLine);
	m_aPen[pHSITracking].setWidthF(1.5);
	m_aPen[pHSITracking].setColor(QColor(30,30,255,255)); // a kind of blue

	m_aPen[pHSICompass] = QPen(Qt::white);
	m_aPen[pHSICompass].setWidthF(1.5);

// 	m_aPen[pDirectWaypoint] = QPen(QColor(186, 85, 211, 220)); // medium orchid
// 	m_aPen[pDirectWaypoint].setWidth(5);
// 	m_aPen[pDirectWaypoint].setCapStyle(Qt::RoundCap);

	m_aPen[pRouteActiveLeg] = QPen(QColor( 35, 255, 255, 220));
//	m_aPen[pRouteActiveLeg] = QPen(QColor(255, 248, 220, 196)); // white
	m_aPen[pRouteActiveLeg].setWidth(5);
	m_aPen[pRouteActiveLeg].setCapStyle(Qt::RoundCap);

	m_aPen[pRouteInactiveLeg] = QPen(QColor( 0, 0, 0, 255));
//	m_aPen[pRouteInactiveLeg] = QPen(QColor( 105, 105, 105, 220));	// dim gray
	m_aPen[pRouteInactiveLeg].setWidth(2);
	m_aPen[pRouteInactiveLeg].setCapStyle(Qt::RoundCap);

	m_aPen[pDirectWaypoint] = m_aPen[pRouteActiveLeg];

	QFontInfo fi(m_aFont[fButton]);
	qDebug() << "Font Family = " << fi.family();
}

GraphicsSingleton::~GraphicsSingleton()
{
}

// -----------------------------------------------------------------------

GraphicsSingleton* GraphicsSingleton::GetInstance()
{
	if(s_pInstance==NULL) {
		s_pInstance = new GraphicsSingleton();
	}
	return s_pInstance;
}

// -----------------------------------------------------------------------
