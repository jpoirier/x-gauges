#ifndef __MAP_TEXT_LABEL_H
#define __MAP_TEXT_LABEL_H

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

#include <QString>
#include <QPoint>

namespace map {
	
/*! \brief Combination of text string, coordinate and flags.
 */

class TextLabel 
{
public:
	//! Default constructor.
	TextLabel();
	//! Another constructor using parameters.
	TextLabel(const QString& qsText, const QPointF& ptWGS, int iFlag=0);
	//! Copy constructor.
	TextLabel(const TextLabel& C);
	//! Destructor
	~TextLabel();
	
	//! Copy method.
	void Copy(const TextLabel& C);
	
	//! Get text string.
	const QString& GetText() const 
	{ return m_qsText; }
	
	//! Get WGS coordinates.
	const QPointF& GetCoordinate() const 
	{ return m_ptWGS; }
	
	//! Get position on the image.
	const QPoint& GetPosition() const 
	{ return m_ptImg; }
	//! Set position on the image.
	void SetPosition(const QPoint& ptImg)
	{ m_ptImg = ptImg; }
	
	//! Get drawing position.
	QPoint GetDrawPosition() const
	{ return m_ptDraw; }
	//! Set drawing position.
	void SetDrawPosition(const QPoint& ptDraw)
	{ m_ptDraw = ptDraw; }
	
	//! Are two labels equal? Yes, if their coordinates are the same.
	bool operator==(const TextLabel& C) 
	{ return (m_ptWGS == C.m_ptWGS);	}
	
	//! Return importance flag.
	int GetImportance() const
	{ return m_iFlag; }
	
private:
	//! Text to be displayed.
	QString m_qsText;
	//! Coordinate in WGS (radians).
	QPointF m_ptWGS;
	//! Position in Image (pixels).
	QPoint m_ptImg;
	//! Rotated position used to draw the text (pixels).
	QPoint m_ptDraw;
	//! Importance flag.
	int m_iFlag;
};

} // map namespace

#endif	// __MAP_TEXT_LABEL_H
