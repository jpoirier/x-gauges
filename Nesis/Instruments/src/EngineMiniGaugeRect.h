#ifndef INSTRUMENT_ENGINE_MINI_GAUGE_RECT_H
#define INSTRUMENT_ENGINE_MINI_GAUGE_RECT_H

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

namespace instrument {

// -------------------------------------------------------------------------

class EngineMiniGaugeRect : public AbstractGauge
{
public:
	//! Constructor
	EngineMiniGaugeRect(
		const QDomElement& el,
		QWidget *pParent = 0);
	//! Destructor
	~EngineMiniGaugeRect();
	
protected:
	//! Draw the foreground.
	virtual void DrawForeground(QPainter& P);
	//! Create background pixmap.
	virtual QPixmap CreateBackground(const QSize& szHint);
	
	//! Draw label (center point).
	void DrawLabel(QPainter& P, int iCx, int iCy, const QString& qs, 
						int iW=30, int iH=20);

private:
	Parameter* m_pParCHT;	//!< CHT parameters.
	Parameter* m_pParEGT; //!< EGT parameters.
	Parameter* m_pParOT;	 //!< Oil temperature parameters.
	Parameter* m_pParWT;	 //!< Water temperature parameters.
	Parameter* m_pParCarbT; //!< Carburetor parameters.
	
	Parameter* m_pParOP;	 //!< Oil pressure parameters.
	Parameter* m_pParFP;	 //!< Fuel pressure parameters.
	Parameter* m_pParRPM; //!< RPM parameter tells if engine is running.
	
	bool m_bOilT;		//!< Show oil temperature column.
	bool m_bCarbT; 	//!< Show carburetor column.
	bool m_bWaterT;	//!< Show water column.
	bool m_bOilP;		//!< Show oil pressure column.
	bool m_bFuelP;		//!< Show fuel pressure column.

	int m_ixCht1;	//!< CHT x left.
	int m_ixCht2;	//!< CHT x right.
	 
	int m_ixEgt1;	//!< EGT x left.
	int m_ixEgt2;	//!< EGT x right.
	
	int m_ixOilT1;	//!< Oil x left.
	int m_ixOilT2;	//!< Oil x right.
	
	int m_ixWaterT1;	//!< Water x left.
	int m_ixWaterT2;	//!< Water x right.
	
	int m_ixCarbT1;	//!< Carburetor x left.
	int m_ixCarbT2;	//!< Carburetor x right.
	
	int m_ixOilP1;	//!< Oil pressure x left.
	int m_ixOilP2;	//!< Oil pressure x right.
	
	int m_ixFuelP1;	//!< Fuel pressure x left.
	int m_ixFuelP2;	//!< Fuel pressure x right.
};

} // namespace

// -------------------------------------------------------------------------
#endif
