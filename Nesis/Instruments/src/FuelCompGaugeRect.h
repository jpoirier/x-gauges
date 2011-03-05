#ifndef INSTRUMENT_FUEL_COMP_GAUGE_RECT_H
#define INSTRUMENT_FUEL_COMP_GAUGE_RECT_H

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

#include <QImage>
#include "AbstractGauge.h"
#include "Frame.h"

namespace instrument {

// -------------------------------------------------------------------------

class FuelCompGaugeRect : public AbstractGauge
{
public:
	//! Constructor
	FuelCompGaugeRect(QWidget *pParent = 0);
	//! Destructor
	~FuelCompGaugeRect();
	
protected:
	//! Draw the foreground.
	virtual void DrawForeground(QPainter& P);
	//! Create background pixmap.
	virtual QPixmap CreateBackground(const QSize& szHint);
	
private:
	//! Get string for display.
	QString GetString(const Parameter* pPar, int iDec) const;
	
protected:
	
	const Parameter* m_pParRange;			//!< Action range parameter
	const Parameter* m_pParLevel;			//!< Fuel level parameter.
	const Parameter* m_pParFuelFlow;		//!< Fuel flow parameter.
	const Parameter* m_pParAvgFlow;		//!< Average fuel flow parameter.
	const Parameter* m_pParEndurance;	//!< Endurance (time left).
	
	Bar	m_barFuel;
	
	int m_ix1;	//!< Left column
	int m_ix2;  //!< Right column
	int m_ix3;	//!< Fuel column
	int m_ix4;	//!< Right edge
};

} // namespace

// -------------------------------------------------------------------------
#endif
