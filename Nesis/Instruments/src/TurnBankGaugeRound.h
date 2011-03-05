#ifndef INSTRUMENT_TURN_BANK_ROUND_H
#define INSTRUMENT_TURN_BANK_ROUND_H

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


#include "AbstractGauge.h"

namespace instrument {

// -------------------------------------------------------------------------

class TurnBankGaugeRound : public AbstractGauge
{
public:
	//! Constructor
	TurnBankGaugeRound(const QString& qsPathToAirplane, QWidget *pParent = 0);
	//! Destructor
	~TurnBankGaugeRound();

protected:
	//! Draw the foreground.
	virtual void DrawForeground(QPainter& P);
	//! Create background pixmap.
	virtual QPixmap CreateBackground(const QSize& szHint);
	
	//! We need to resize the turning airplane.
	virtual void ResizeBackground();

private:
	//! Path to the turning airplane image.
	QString m_qsTurnImage;

	//! Turning rate [rad/s]
	float m_fRate;
	//! Ball angle (inclination) [rad]
	float m_fBallAngle;

	//! Dimensionless midpoint radius
	float m_fR;
	//! Dimensionless center point radius.
	float m_fC;
	//! Dimensionless tube thickness.
	float m_fT;
	
	//! Pixmap with turning airplane
	QPixmap m_pxTurn;
};
} // namespace

// -------------------------------------------------------------------------
#endif
