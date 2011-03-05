#ifndef INSTRUMENT_XML_GAUGE_ROUND_H
#define INSTRUMENT_XML_GAUGE_ROUND_H

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
#include "Frame.h"
#include "ScaleRound.h"
#include "AbstractGauge.h"

namespace instrument {

// --------------------------------------------------------------------------

class XMLGaugeRound : public AbstractGauge
{
	friend class Frame;
	friend class Bar;
	friend class ScaleRound;
public:
	//! Constructor from XML QDomElement object.
	XMLGaugeRound(const QDomElement& e, QWidget *parent = 0);
	//! Destructor.
	~XMLGaugeRound();

protected:
	//! Draw the foreground.
	virtual void DrawForeground(QPainter& P);
	//! Create background pixmap.
	virtual QPixmap CreateBackground(const QSize& szHint);
	//! Resize the background to a new widget size.
	virtual void ResizeBackground();

protected:
	//! Vector of round scales.
	QVector<ScaleRound*> m_vpScale;
	//! Vector of labels.
	QVector<Label*> m_vpLabel;
	//! Vector of frames
	QVector<Frame*> m_vpFrame;
	//! Vector of bars
	QVector<Bar*> m_vpBar;
};

}

#endif
