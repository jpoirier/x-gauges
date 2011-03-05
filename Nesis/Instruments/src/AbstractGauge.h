#ifndef INSTRUMENT_ABSTRACT_GAUGE_H
#define INSTRUMENT_ABSTRACT_GAUGE_H

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

#include <QWidget>
#include "Defines.h"

#define GR_BLACK						qRgb(26,26,26)
#define GR_GRAY						qRgb(128,128,128)

#define GR_FONT						"Bitstream Vera Sans"
#define GR_FONT_SIZE_TITLE		 	13
#define GR_FONT_SIZE_TITLE_BIG 	16
#define GR_FONT_SIZE_UNIT			18

#define GR_LABEL_BOX_TEXT_SIZE	15
#define GR_LABEL_BOX_BACKGROUND	qRgb(210,210,210)
#define GR_LABEL_BOX_HEIGHT		18

#define GR_SCALE_FONT_SIZE_PX		18

#define GR_MAJOR_TICK_SIZE			10 
#define GR_MINOR_TICK_SIZE			8 

#define GR_TITLE_Y					64
#define GR_UNIT_Y						87
#define GR_UNIT_Y2					20
#define GR_TITLE_Y2					138
#define GR_TITLE_Y0					115

namespace instrument {

class AbstractGauge : public QWidget
{
//Q_OBJECT
public:
	//! Constructor
	AbstractGauge(QWidget* pParent);
	//! Destructor
	~AbstractGauge();

	//! Set the background file for pixmap.
	void SetBackgroundFile(const QString& qsBackPx)
	{ m_qsBackground = qsBackPx; }

	//! Prepare (resize) the objects.
	virtual void resizeEvent(QResizeEvent *pEvent);

	//! Draw the instrument
	virtual void paintEvent(QPaintEvent *pEvent);

	//! The instrument size hint.
   QSize sizeHint() const
	{ return QSize(208,208); }

protected:
	//! Draw the background (just copy the pixmap)
	virtual void DrawBackground(QPainter& P);
	//! Draw the foreground (here we simply set pens and brushes).
	virtual void DrawForeground(QPainter& P);
	//! Create background pixmap.
	virtual QPixmap CreateBackground(const QSize& szHint) = 0;

	//! Resize the background to a new widget size.
	virtual void ResizeBackground();
	
protected:
	//! The background pixmap source
	QString m_qsBackground;
	//! Pixmap of the instrument base
	QPixmap m_pxBackground;
};

} // namespace

#endif
