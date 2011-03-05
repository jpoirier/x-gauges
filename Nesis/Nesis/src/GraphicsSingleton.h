#ifndef __GRAPHICS_SINGLETON_H
#define __GRAPHICS_SINGLETON_H

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

#include <memory>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QPen>

class GraphicsSingleton
{
public:
	//! Enumeration of fonts used in the application.	
	enum Font {
		fButton = 0,	//! Font used for buttons.
		fDialog,			//! Font used in dialogs.
		fDialogSmall,	//! Font used in dialogs, smaller.
		fMapCity,		//! Font used to draw city text on a map
		fMapStartup,   //! Font used during map startup.
		fHSIHeading,   //! Used to draw big white numbers on HSI
		fHSIText,		//! Used to draw text on HSI
		fVASText,      //! Used to draw vertical airspace text.
		fMessageText,  //! Used to draw messages.
		fBanner,			//! Used in the top and bottom banner.
		fSmall,			//! Small text font (used in mini HSI).
		fKnobBuddy,		//! Font used to display the knob selection in edit input.
		fLast				//! End of font enumeration marker.
	};
	
	//! Enumeration of brushes used in the application.
	enum Brush {
		bBackground,	//! Brush color used for basic background.
		bAirspace,
		bBannerBack, 	//! Banner background
		bVAirspaceBack,//! Vertical airspace background
		bModernBlackBgd,
		bLast				//! End of brush enumeration marker.
	};

	//! Enumeration of pens used in the application
	enum Pen {
		pHSITracking = 0,
		pHSICompass, 
		pDirectWaypoint,
		pRouteActiveLeg,
		pRouteInactiveLeg,
		pLast					//! End of pen enumeration marker
	};
	
	//! Private constructor.
	GraphicsSingleton();
public:
	//! Destructor
	~GraphicsSingleton();
	
	//! Access to the singleton object.
	static GraphicsSingleton* GetInstance();
	
	//! Return required font.
	const QFont& GetFont(Font font) {
		return m_aFont[font];
	}
	//! Return required brush;
	const QBrush& GetBrush(Brush brush) {
		return m_aBrush[brush];	
	}
	//! Return required pen.
	const QPen& GetPen(Pen pen) {
		return m_aPen[pen];
	}
private:
	//! The singleton object.
	static GraphicsSingleton* s_pInstance;
	
public:
	//! Fonts
	QFont m_aFont[fLast];
	//! Fonts
	QBrush m_aBrush[bLast];
	//! Pens
	QPen m_aPen[pLast];
};

#endif
