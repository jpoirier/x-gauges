/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

/*! \file
    \brief

    \author  Ales Krajnc
    \date
 */

// ---------------------------------------------------------------------------

#include "Label.h"

// ---------------------------------------------------------------------------

namespace chart {

Label::Label(const QString& qsText, const QFont& font, const QColor& color)
{
	m_qsText  		= qsText;
	m_font    		= font;
	m_color   		= color;
	m_iRotate 		= 0;
	m_fptOrigin 	= QPointF(0.5f,0.5f);
	m_bSizeDirty 	= true;
}

Label::Label(const Label& C)
{
	m_font   		= C.m_font;
	m_qsText 		= C.m_qsText;
	m_color  		= C.m_color;
	m_iRotate 		= C.m_iRotate;
	m_fptOrigin 	= C.m_fptOrigin;
	m_bSizeDirty 	= C.m_bSizeDirty;
	m_size       	= C.m_size;
}

Label::~Label()
{
}

// ---------------------------------------------------------------------------

void Label::Draw(QPainter& P, int x, int y) const
{
	P.save();

	P.setFont(m_font);
	P.setPen(m_color);

	GetSize(P); // m_size is now valid

	// If the text is rotated, we need to modify the coordinate system.
	if(m_iRotate) {
		P.translate(x,y);
		P.rotate(m_iRotate);
		P.translate(
			-m_fptOrigin.x()*m_size.width(),
			-(m_fptOrigin.y()-1.0f)*m_size.height()
		);
		P.drawText(0,0, m_qsText);
	}
	// There is no need to change the coordinate system, if the text is not
   // rotated. This is slightly faster.
	else {
		P.drawText(
			(int) (x-m_fptOrigin.x()*m_size.width() + 0.5f),
			(int) (y-(m_fptOrigin.y()-1.0f)*m_size.height() + 0.5f),
			m_qsText
		);
	}

	P.restore();
}

// ---------------------------------------------------------------------------

QSize Label::GetSize(QPainter& P) const
{
	if(m_bSizeDirty==false)
		return m_size;

	P.save();
	P.setFont(m_font);
// 	QRect r = P.fontMetrics().boundingRect(m_qsText);
	QRect r = P.fontMetrics().tightBoundingRect(m_qsText);
	m_size = r.size();
	P.restore();

	m_bSizeDirty = false;

	return m_size;
}

// ---------------------------------------------------------------------------

QRect Label::GetBoundingRect(QPainter& P) const
{
	QSize size = GetSize(P);
	QRect r(
		qRound(-size.width()*m_fptOrigin.x()), qRound(-size.height()*m_fptOrigin.y()),
		size.width(), size.height()
	);

	QMatrix m;
	m.rotate(m_iRotate);
	r = m.mapRect(r);

	return r;
}

// ---------------------------------------------------------------------------

}
