#ifndef INSTRUMENT_XML_GAUGE_ROUND_ALTITUDE_H
#define INSTRUMENT_XML_GAUGE_ROUND_ALTITUDE_H

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

#include "XMLGaugeRound.h"

namespace instrument {

// -------------------------------------------------------------------------

class XMLGaugeRoundAltitude : public XMLGaugeRound
{
public:
	//! Constructor from the XML QDomElement object.
	XMLGaugeRoundAltitude(const QDomElement& e, QWidget *pParent = 0);
	//! Destructor.

protected:
	//! Draw the foreground.
	virtual void DrawForeground(QPainter& P);
	//! Resize the background to a new widget size.
	virtual void ResizeBackground();
	
protected:
	//! Do we have three needles?
	bool m_bThreeNeedles;
	//! Short needle used to display 1000s.
	Needle m_needle3;
	//! Long needle used to display 10000s.
	Needle m_needle4;
};

} // namespace

// -------------------------------------------------------------------------

#endif
