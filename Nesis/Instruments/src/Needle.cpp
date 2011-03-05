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
#include "Needle.h"

// --------------------------------------------------------------------------

namespace instrument {

Needle::Needle()
{
	m_eType = tPlain;
	m_iOffset = 0;
}


Needle::~Needle()
{
}

// --------------------------------------------------------------------------

void Needle::SetSize(QSize sz, int iOffset)
{ 
	m_size = sz; 
	m_iOffset = iOffset; 
	if(m_eType != tPlain) {
		CreatePixmap();
	}
}

// --------------------------------------------------------------------------

void Needle::Draw(
	QPainter& P, 		//! The painter object
	qreal fAngleDeg 	//! Needle angle given in degrees.
)
{
	P.save();
	
	P.rotate(-fAngleDeg);
	if(m_eType != tPlain && m_eType != tPlainLabel) {
		bool bActive = P.testRenderHint(QPainter::SmoothPixmapTransform);
		if(!bActive)
			P.setRenderHint(QPainter::SmoothPixmapTransform, true);
		P.drawPixmap(QPointF(0.0,-m_size.height()/2.0), m_px);
		if(!bActive)
			P.setRenderHint(QPainter::SmoothPixmapTransform, false);
	}
	else {
		P.setPen(QPen(Qt::white, m_size.height()));
		P.drawLine(m_iOffset, 0, m_size.width(), 0);
	}
	if(m_eType == tPlainLabel) {
		qreal fH = 9;
		P.setPen(QPen(Qt::white, 2));
		P.drawEllipse(0.5*m_size.width()-fH/2, -fH/2, fH,fH);
	}

	P.restore();
}

// --------------------------------------------------------------------------

void Needle::CreatePixmap()
{
	// polygon points for short needle (used in altimeter)
	static const QPointF aptShortFat[5] = {
		QPointF(0.0, -0.2),
		QPointF(0.7, -0.5),
		QPointF(1.0,  0.0),
		QPointF(0.7,  0.5),
		QPointF(0.0,  0.2),
	};

	// polygon points for 10000 ft needle
	static const QPointF apt10000[10] = {
		QPointF(0.00,  0.10),
		QPointF(0.25,  0.10),
		QPointF(0.25,  0.05),
		QPointF(0.90,  0.05),
		QPointF(1.00,  0.50),
		QPointF(1.00, -0.50),
		QPointF(0.90, -0.05),
		QPointF(0.25, -0.05),
		QPointF(0.25, -0.10),
		QPointF(0.00, -0.10)
	};

	// Resize and reset the pixmap
	if(m_px.size() != m_size)
		m_px = QPixmap(m_size);
	m_px.fill(Qt::transparent);

	// Set the drawing parameters.
	QPainter P(&m_px);
	P.setRenderHints(QPainter::Antialiasing, true);
	P.translate(QPointF(m_iOffset, m_size.height()/2.0));
	
//	P.setPen(Qt::darkGray);
	P.setPen(Qt::white);
	P.setBrush(Qt::white);

	const qreal fH   = m_size.height();
	const qreal fH_2 = fH/2.0;
	const qreal fL = m_size.width()-m_iOffset;
	QRect rc;

	// Draw polygons / lines into the pixmap
	switch(m_eType) {
	case tPointed:
		{
			QPolygonF pg;
			pg << QPointF(0.0, fH_2);
			pg << QPointF(fL - 1.6*fH_2,  fH_2);
			pg << QPointF(fL,  0.0);
			pg << QPointF(fL - 1.6*fH_2, -fH_2);
			pg << QPointF(0.0,-fH_2);
			P.drawConvexPolygon(pg);
			break;
		}

	case t10000:
		P.scale(fL, fH);
		P.drawConvexPolygon(apt10000, 10);
		break;

	case tFatShort:
		P.scale(fL, fH);
		P.drawConvexPolygon(aptShortFat, 5);
		break;
		
	case tPlainLabel:
		P.setPen(QPen(Qt::white, 3));
		P.drawLine(0, 0, fL-6, 0);
		P.setPen(Qt::transparent);
		rc = QRect(0.4*(fL-fH), -fH/2, fH,fH);
		P.drawEllipse(rc);
		break;	
		
	case tPlain:
	default:
		P.setPen(QPen(Qt::white, m_size.height()));
		P.drawLine(0, 0, fL-fH, 0);
		break;
	};
}

} // namespace
