#ifndef __CHART_LABEL_H
#define __CHART_LABEL_H

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

#include <QFont>
#include <QColor>
#include <QPainter>

// ---------------------------------------------------------------------------

namespace chart {

class Label
{
public:
	//! Constructor
   Label(const QString& qsText = QString(), 
			const QFont& font     = QFont(), 
			const QColor& color   = QColor()
	);
	//! Copy constructor.
	Label(const Label& C);
	//! Destructor.
   ~Label();

	//! Set font parameters.
	void SetFont(const QFont& font)
	{ m_font = font; m_bSizeDirty = true; }
	//! Read/write access to the title font.
	QFont& GetFont()
	{ return m_font; }
	//! Read only access to the title font.
	const QFont& GetFont() const
	{ return m_font; }

	//! Set title text. 
	void SetText(const QString& qsText)
	{ m_qsText = qsText; m_bSizeDirty = true; }
	//! Returns read/write access to the title text.
	QString& GetText()
	{ return m_qsText; }
	//! Returns read only access to the title text.
	const QString& GetText() const
	{ return m_qsText; }

	//! Return true if the label text is empty.
	bool IsEmpty() const
	{ return m_qsText.isEmpty(); }

	//! Set title color.
	void SetColor(const QColor& color)
	{ m_color = color; }
	//! Read/write access to the title color object.	
	QColor& GetColor()
	{ return m_color; }
	//! Read only access to the title color object.
	const QColor& GetColor() const
	{ return m_color; }

	//! Set the text rotation (+ clockwise, -countercloskwise) in degrees.
	void SetRotate(int iRotateDegrees)
	{ m_iRotate = iRotateDegrees; }
	//! Returns the rotation angle.
	int GetRotate() const
	{ return m_iRotate; }

	//! Sets the origin point in the relative coordinates.
	/*! Origin point is definied in relative coordinates which refer
       to the nonrotated rectangle around the text. Point (0,0) refers to
       top left corner of the text, point (1,1) refers to bottom right corner,
		 point (0.5, 0.5) defines midle point of the rectangle. Any value can
       be used for the origin coordinate, however values between 0.0 and 1.0
		 are most useful.
    */
	void SetLocalOrigin(const QPointF& fptOrigin)
	{ m_fptOrigin = fptOrigin; }

	//! Returns the origin point.
	const QPointF& GetLocalOrigin() const
	{ return m_fptOrigin; }

	//! Draws the title using QPainter. 
   /*! The x and y arguments define the position of the relative origin point.
    */
	void Draw(QPainter& P, int x, int y) const;

	//! Draws the title using QPainter. 
   /*! The QPoint argument defines the position of the relative origin point.
    */
	void Draw(QPainter& P, const QPoint& pt) const
	{ Draw(P, pt.x(), pt.y()); }

	//! Returns size required to draw non-rotated text. 
   /*! The text is assumed to be straight (nonrotated) and without
       line breaks and tabs.
    */
	QSize GetSize(QPainter& P) const;

	//! Returns bounding rectangle reqiured to encapsulate rotated text. 
   /*! The text is rotated around local origin but without line breaks 
		 and tabs. Bounding rectangle coordinates are relative to the
		 local origin.
    */
	QRect GetBoundingRect(QPainter& P) const;

protected:
	//! Label text label.
	QString m_qsText;
	//! Label font
	QFont   m_font;
	//! Label text color
	QColor  m_color;
	//! Text rotation angle
	int     m_iRotate;
	//! Relative origin point inside the text rectangle. Default = (bottom, center). 
	QPointF m_fptOrigin;
	//! An internal flag that tels if the calculated and stored size is dirty.
	mutable bool m_bSizeDirty;
	//! Stores the size calculation for later use. Don't use it directly. Use GetSize instead.
	mutable QSize m_size;
};

}	// namespace chart

#endif // __CHART_LABEL_H
