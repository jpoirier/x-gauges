#ifndef INSTRUMENT_FRAME_H
#define INSTRUMENT_FRAME_H

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

#include <QDomElement>
#include <QPainter>
#include "Parameter.h"

namespace instrument {

// --------------------------------------------------------------------------

class Label
{
public:
	//! Constructor reads form QDomElement
	Label(const QDomElement& el);
	//! Draw the label.
	void Draw(QPainter& P, int iW);
	
private:
	QString m_qsText;			//!< Text to display.
	float m_fR;					//!< Radius from center.
	float m_fPhi;				//!< Angle from 0 degrees.
	int   m_iFontSize;		//!< Size of font.
	bool  m_bUnit;				//!< Is this unit?
};

// --------------------------------------------------------------------------

class Frame
{
public:
	//! Constructor from XML QDomElement.
	Frame(const QDomElement& el);
	//! Draw the frame.
	void Draw(QPainter& P, int iW);
	
	//! Set number of decimals.
	void SetDecimals(int iDec)
	{ m_iDec = iDec; }
	//! Return number of decimals.
	int GetDecimals() const
	{ return m_iDec; }
	
	//! Get Parameter object.
	const Parameter* GetParameter() const
	{ return m_pPar; }
	
private:
	//! Parameter which we want to display in the frame.
	const Parameter* m_pPar;
	
	int m_iDec;					//!< Number of decimals to show
	int m_iUnitKey;			//!< We need to convert into this unit.
	int m_iUnitKeySource;	//!< This is the unit that we get from source.
	
	bool m_bPosKnown;			//!< True, when final position is known.
	QRect m_rc;					//!< Final position coordinates.
	float m_fR;					//!< Radius from center.
	float m_fPhi;				//!< Angle from 0 degrees.
	float m_fWidth;			//!< Width of the rectanlge in % of total width.
	
	QString m_qsOld;			//!< Old text that was displayed during last call.
};

// --------------------------------------------------------------------------

class Bar
{
public:
	//! The type of the bar
	enum Type { 
		tLeftRight,			//!< The bar grows from left to right.
		tRightLeft,			//!< The bar grows from right to left.
		tTopBottom,			//!< The bar grows from top to bottom.
		tBottomTop			//!< The bar grows from bottom to top.
	};

public:
	//! Constructor from XML.
	Bar(const QDomElement& el);
	//! Constructor from parameters.
	Bar(Type eType, float fR, float fPhi, 
		float fW, float fH, bool bLabel, float fGap=0.4f);
	//! Draw the bar
	void Draw(QPainter& P, int iS);
	
	//! Set parameter.
	void SetParameter(const Parameter* pPar)
	{ m_pPar = pPar; }
	
	//! Draw the bar in given rect. This is made static.
	static void DrawBar(QPainter& P, const Parameter* pPar, Type eBarType, 
		const QRect& rc, float fS, int iAlpha);
protected:
	//! Create the background pixmap.
	void CreateBackground(int iS);
	//! Draw the label
	void DrawLabel(QPainter& P, const QRect& rc, float fU);
	
	//! Is vertical bar?
	bool IsVertical() const
	{ return m_eBarType >= tTopBottom; }
	//! Is horizontal bar?
	bool IsHorizontal() const
	{ return !IsVertical(); }
	
	//! Get gap and bar thickness.
	void GetGapAndT(int& iGap, int& iThickness);
	
	//! Get the color.
	QColor GetColor(float fS, QColor colDefault);
	
private:
	//! The source for the information.
	const Parameter* m_pPar;
	
	bool m_bPosKnown;			//!< True, when final position is known.
	QRect m_rc;					//!< Final position coordinates.
	
	float m_fR;					//!< Radius from center.
	float m_fPhi;				//!< Angle from 0 degrees.
	
	float m_fWidth;			//!< Width of the rectangle in % of total width.
	float m_fHeight;			//!< Height of the rectangle in % of total width.
	float m_fGap;				//!< Relative gap value to the bar thickness.
	bool  m_bLabel;			//!< Should we draw the label as well?
	
	Type m_eBarType;			//!< Type of the bar
	
	QVector<QRect> m_vR;
	
	//! The background pixmap.
	QPixmap m_pxBack;
};

// --------------------------------------------------------------------------

}

#endif
