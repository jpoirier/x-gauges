#ifndef INSTRUMENT_PARSE_HELPER_H
#define INSTRUMENT_PARSE_HELPER_H

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

#include <QPair>
#include <QDomElement>

// -------------------------------------------------------------------------

namespace instrument {

class XMLGaugeRound;

//! Read the unit attribute from the element.
int ParseUnitKey(const QDomElement& el, const QString& qsTag);

//! Read the range (unit, low, high) attributes and convert them into pairs.
QPair<float,float> ParseRange(
	const QDomElement& elParent, const QString& qsTag, int iSystemKey);
	
//! Get required argument.
QString ParseAttribute(const QDomElement& el, const QString& qsTag);

//! Create proper XMLGaugeRound object. The caller owns the object.
XMLGaugeRound* CreateXMLGauge(const QDomElement& el, QWidget* pParent);


} // namespace
// -------------------------------------------------------------------------

#endif
