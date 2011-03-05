#ifndef INSTRUMENT_ATTITUDE_ROUND_H
#define INSTRUMENT_ATTITUDE_ROUND_H

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
#include "ScaleRound.h"


#define HORIZON_DARK_BLUE		qRgb(0x28, 0x5c, 0xbf)
#define HORIZON_LIGHT_BLUE		qRgb(0x45, 0x73, 0xcb)
#define HORIZON_LIGHT_BROWN	qRgb(0xc5, 0x74, 0x3e)
#define HORIZON_DARK_BROWN		qRgb(0xaa, 0x44, 0x00)


namespace instrument {

// -------------------------------------------------------------------------

class AttitudeGaugeRound : public AbstractGauge
{
//Q_OBJECT
public:
	//! Constructor
	AttitudeGaugeRound(QWidget *pParent = 0);
	//! Destructor
	~AttitudeGaugeRound();
	
	//! Set current attitude
	void SetAttitude(float fRoll, float fPitch);
	//! Set slip angle
	void SetBallSlip(float fSlip);
	//! Set current heading rate [rad/s] (used for debug).
	void SetHeadingRate(float fHeadRate)
	{ m_fHeadRate = fHeadRate; }

	//! Draw the instrument
	virtual void paintEvent(QPaintEvent *pEvent);

protected:
	//! Draw the foreground.
	virtual void DrawForeground(QPainter& P);
	//! Create background pixmap.
	virtual QPixmap CreateBackground(const QSize& szHint);
	
	//! Get translated and rotated images for the horizon.
	void CreateHorizonImage(QImage &imDst);

private:
	//! Current pitch [rad]
	float m_fPitch;
	//! Current roll [rad]
	float m_fRoll;
	
	//! Current heading rate [rad/s] used in debug only.
	float m_fHeadRate;
	
	//! Current slip (ball) angle [rad]
	float m_fSlip;
	
	//! Image used for the horizon.
	QImage m_imHorizonSource;
	//! Image object used as destination.
	QImage m_imHorizonDest;
	//! Relative radius used to draw roll triangle
	float m_fR;
	
	//! The roll scale object.
	ScaleRound m_scaleRoll;
};

} // namespace

// -------------------------------------------------------------------------
#endif
