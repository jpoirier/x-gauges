#ifndef PANEL_MODERN_H
#define PANEL_MODERN_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Aleš Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QDomDocument>
#include <QPixmap>
#include <QImage>

#include "Scale.h"
#include "ScaleRound.h"
#include "ParameterAirspeed.h"
#include "AbstractPanel.h"

// --------------------------------------------------------------------------

namespace modern {

// --------------------------------------------------------------------------

class Attitude : public QRect
{
public:
	//! Constructor
	Attitude(QRect rc);

	//! Set current attitude
	void SetAttitude(float fRoll, float fPitch, float fYaw);
	//! Set slip angle
	void SetSideSlip(float fSlip);
	//! Update active navigation data.
	void UpdateActiveNavigation();

	//! Draw the background image
	void DrawBackground(QPainter& P);
	//! Draw active foreground.
	void DrawForeground(QPainter& P);

private:
	//! Get translated and rotated images for the horizon.
	void CreateHorizonImage(QImage &imDst);
	//! Create scale pixmap.
	void CreateStaticScale();
	//! Create ref W pixmap.
	void CreateStaticRefW();
	//! Create ball pixmap.
	void CreateStaticBall();
	//! Create heading pixmap.
	void CreateStaticHeading();
	//! Create heading label pixmap.
	void CreateStaticHeadLbl();

	//! Create the Ball
	void CreateBall();

	//! Calc x coordinate in heading pixmap that corresponds given heading deg.
	int GetHeadingX(float fDeg) const
	{ return qRound(m_fH0 + fDeg*m_fH1); }
	//! Calc x coordinate in heading pixmap that corresponds given heading rad.
	int GetHeadingXRad(float fRad) const
	{ return qRound(m_fH0 + fRad*m_fH1Rad); }

private:
	//! The roll angle [radians]
	float m_fRoll;
	//! The pitch angle [radians]
	float m_fPitch;
	//! The yaw (heading/tracking) [radians]
	float m_fYaw;

	//! The ball slip
	float m_fSlip;

	//! Image used for the horizon source (a color ramp, actually).
	QImage m_imHorizonSource;
	//! Image object used as horizon destination (rotated and translated ramp)
	QImage m_imHorizonDest;
	
	//! The reference "W" static pixmap.
	QPixmap m_pxRefW;
	//! The W origin point.
	QPoint m_ptRefW;

	//! The ball back static pixmap.
	QPixmap m_pxBallBack;
	//! The ball origin point.
	QPoint m_ptBallBack;

	//! The ball (foreground) pixmap
	QPixmap m_pxBall;

	//! The scale static pixmap.
	QPixmap m_pxScale;
	//! The scale origin point.
	QPoint m_ptScale;
	//! Scale radius
	int m_iR;
	//! The roll scale object.
	instrument::ScaleRound m_scaleRoll;
	//! Attitude font used to draw along horizontal lines.
	QFont m_fontAttitude;	

	//! Heading base pixmap (compass rose).
	QPixmap m_pxHeadingBase;
	//! Heading pixmap to be drawn on the screen = base + bearing label.
	QPixmap m_pxHeading;
	//! The last bearing position.
	int m_iLastBearing;
	//! The polygon used to draw the bearing sign (triangle).
	QPolygon m_pgBearing;
	//! Heading map coefficients.
	float m_fH0, m_fH1, m_fH1Rad;
	//! Origin for heading.
	QPoint m_ptHeading;
	
	//! Heading label pixmap
	QPixmap m_pxHeadLbl;
	//! Origin for heading label.
	QRect m_rcHeadLbl;
};

// --------------------------------------------------------------------------

class Airspeed : public QRect, public instrument::Scale
{
public:
	//! Constructor
	Airspeed(const QDomElement& el, QRect rc);

	//! Draw the background image
	void DrawBackground(QPainter& P);
	//! Draw active foreground.
	void DrawForeground(QPainter& P);

private:
	//! Create the static background image
	void CreateBackgroundPixmap();
	//! Create the velocity arrow pixmap.
	void CreateArrowPixmap(const QSize& sz);

	//! Y coordinate in pixels that corresponds system airspeed.
	int GetSystemY(float fS) const;
	//! Y coordinate in pixels that corresponds scale airspeed.
	int GetScaleY(float fS) const;
	
private:
	//! The airspeed rectangle.
	QRect m_rcAirspeed;
	//! Coefficients which convert airspeed in pixels.
	float m_fC0, m_fC1;
	
	//! The background pixmap.
	QPixmap m_pxBack;
	//! The velocity arrow
	QPixmap m_pxArrow;
	
	//! Font used to draw airspeed in the arrow.
	QFont m_fontArrow;
	//! Font used to draw labels.
	QFont m_fontLabel;

	//! The TAS parameter.
	const instrument::Parameter* m_pParTAS;
	//! The TAS text rectangle.
	QRect m_rcTAS;
};

// --------------------------------------------------------------------------

class Altitude : public QRect, public instrument::Scale
{
public:
	//! Constructor
	Altitude(const QDomElement& ea, const QDomElement& ev, QRect rc);

	//! Draw the background image
	void DrawBackground(QPainter& P);
	//! Draw active foreground.
	void DrawForeground(QPainter& P);

private:
	//! Calculate Altitude coefficients.
	void CalcAltitudeCoef();
	//! Calculate Vario coefficients.
	void CalcVarioCoef();

	//! Create the static background image
	void CreateBackgroundPixmap();
	//! Create the velocity arrow pixmap.
	void CreateArrowPixmap(const QSize& sz);

	//! Y coordinate in pixels that corresponds system altitude.
	int GetSystemY(float fS) const;
	//! Y coordinate in pixels that corresponds scale altitude.
	int GetScaleY(float fS) const;

	//! Y coordinate in pixels that corresponds system vario.
	int GetSystemVarioY(float fS) const;
	//! Y coordinate in pixels that corresponds scale vario.
	int GetScaleVarioY(float fS) const;
	
private:
	//! The altitude rectangle.
	QRect m_rcAltitude;
	//! Coefficients which convert airspeed in pixels.
	float m_fC0, m_fC1;
	
	//! The background pixmap.
	QPixmap m_pxBack;
	//! The velocity arrow
	QPixmap m_pxArrow;
	
	//! Font used to draw airspeed in the arrow.
	QFont m_fontArrow;
	//! Font used to draw labels.
	QFont m_fontLabel;

	//! The vario scale
	instrument::Scale m_sVario;
	//! The vario rectangle.
	QRect m_rcVario;
	//! The vario coefficients which convert vario into pixels.
	float m_fV0, m_fV1;
	//! The vario number rectangle.
	QRect m_rcVarioNumber;
	//! Is this m/s vario
	bool m_bMetersPerSecond;

	//! The QNH parameter.
	const instrument::Parameter* m_pParQNH;
	//! The QNH text rectangle.
	QRect m_rcQNH;
	//! Number of QNH decimals.
	int m_iQNHDecimals;
};

// --------------------------------------------------------------------------
/** \brief The top widget of the Modern panel, which gives flight parameters.
 */
class WidgetFlight : public QWidget
{
Q_OBJECT
public:
	//! Constructor
	WidgetFlight(const QDomDocument& doc, QSize sz, QWidget* pParent = 0);
	//! Destructor
	virtual ~WidgetFlight();

	//! Set the attitude
	void SetAttitude(float fRoll, float fPitch, float fYaw);
	//! Set the sideslip.
	void SetSideSlip(float fSS);
	//! Update active navigation information
	void UpdateActiveNavigation();

	//! Draw the widget.
	virtual void paintEvent(QPaintEvent *pEvent);

private:
	//! Return searched QDomElement which points to correct scale.
	QDomElement FindScaleElement(const QDomElement& el, const QString& qsParam);


private:
	//! The airspeed (background) rectangle.
	Airspeed* m_pVel;

	//! The attitude rectangle.
	Attitude* m_pAtt;

	//! The Altitude rectangle.
	Altitude* m_pAlt;
};

// --------------------------------------------------------------------------

/** \brief The engine part of the Moder panel - the bottom widget.
 */
class WidgetEngine : public QWidget
{
Q_OBJECT
public:
	//! Constructor
	WidgetEngine(const QDomDocument& doc, QSize sz, QWidget* pParent = 0);
	//! Destructor
	virtual ~WidgetEngine();

	//! Draw the widget.
	virtual void paintEvent(QPaintEvent *pEvent);

private:
	//! Create the static background image
	void CreateBackgroundPixmap();
	//! Create needle image.
	void CreateNeedlePixmap();

	//! Draw the background.
	void DrawBackground(QPainter& P);
	//! Draw the foreground.
	void DrawForeground(QPainter& P);

	//! Calculate scale to y factor for given parameter;
	QPair<float, float> CalcScaleFactor(const instrument::Parameter* pPar);
	//! Get y component for given value
	int GetY(const QPair<float,float>& ff, float fS);

	//! Create the background bar and set some parameters. Return label rectang
	void CreateBar(QPainter& P, const QString& qsTxt, const instrument::Parameter* pPar, 
		QRect rcPar, QRect& rcLabel, QPair<float,float>& ffScale);

private:
	QPixmap m_pxBack;  							//! The background pixmap.
	QPixmap m_pxNeedle;							//! The needle pixmap.

	instrument::Parameter* m_pParCHT;		//!< CHT parameters.
	instrument::Parameter* m_pParEGT; 		//!< EGT parameters.
	instrument::Parameter* m_pParOT;	 		//!< Oil temperature parameters.
	instrument::Parameter* m_pParWT;	 		//!< Water temperature parameters.
	instrument::Parameter* m_pParCarbT; 	//!< Carburetor parameters.
	
	instrument::Parameter* m_pParOP;	 		//!< Oil pressure parameters.
	instrument::Parameter* m_pParFP;	 		//!< Fuel pressure parameters.

	instrument::Parameter* m_pParRPM; 		//!< RPMs.
	instrument::Parameter* m_pParMP; 		//!< Manifold pressure.

	instrument::Parameter* m_pParA;			//!< Current flow
	instrument::Parameter* m_pParV;			//!< Voltage
	
	bool m_bOilT;									//!< Show oil temperature column.
	bool m_bCarbT; 								//!< Show carburetor column.
	bool m_bWaterT;								//!< Show water column.
	bool m_bOilP;									//!< Show oil pressure column.
	bool m_bFuelP;									//!< Show fuel pressure column.
	bool m_bVoltage;								//!< Show voltage column?
	bool m_bCurrent;								//!< Show current column?

	QRect m_rcCHT;									//!< The CHT group location.
	QRect m_rcEGT;									//!< The EGT group location.
	QRect m_rcOT;									//!< The oil temperature location.
	QRect m_rcWT;									//!< The water temperature location.
	QRect m_rcCarbT;								//!< The carburator temp. location.
	QRect m_rcOP;									//!< The oil pressure location.
	QRect m_rcFP;									//!< The Fuel pressure location.
	QRect m_rcA;									//!< The current (amperes) location.
	QRect m_rcV;									//!< The voltage location.
	QRect m_rcRPM;									//!< The RPM + manifold location.

	QRect m_rcValCHT;								//!< The value CHT location.
	QRect m_rcValEGT;								//!< The EGT group location.
	QRect m_rcValOT;								//!< The oil temperature location.
	QRect m_rcValWT;								//!< The water temperature location.
	QRect m_rcValCarbT;							//!< The carburator temp. location.
	QRect m_rcValOP;								//!< The oil pressure location.
	QRect m_rcValFP;								//!< The Fuel pressure location.
	QRect m_rcValA;								//!< The current (amperes) location.
	QRect m_rcValV;								//!< The voltage location.
	QRect m_rcValRPM;								//!< The RPM + manifold location.

	// Top and the bottom coordinate of scales
	int m_iyT;										//!< The top coordinate.
	int m_iyB;										//!< The bottom coordinate.

	// The scale parameters
	QPair<float,float> m_ffCHT;				//!< CHT temp. scale to y paramters.
	QPair<float,float> m_ffEGT;				//!< EGT temp. scale to y paramters.
	QPair<float,float> m_ffOT;					//!< Oil temp. scale to y paramters.
	QPair<float,float> m_ffWT;					//!< Water temp. scale to y paramters.
	QPair<float,float> m_ffCarbT;				//!< Carburetor temp. scale to y paramters.
	QPair<float,float> m_ffOP;					//!< Oil press. scale to y paramters.
	QPair<float,float> m_ffFP;					//!< Fuel press. scale to y paramters.
	QPair<float,float> m_ffV;					//!< Voltage scale to y paramters.
	QPair<float,float> m_ffA;					//!< Current scale to y paramters.

	instrument::ScaleRound m_sRPM;
	int m_iR_RPM;

	bool m_bPressBar;								//!< Pressure in bar?
};



// --------------------------------------------------------------------------

class PanelModern : public AbstractPanel
{
Q_OBJECT
public:
	//! Constructor
	PanelModern(const QDomDocument& doc, QSize sz, QWidget *pParent = 0);

	//! Destructor
	virtual ~PanelModern();

	//! Set the draw request for the panel.
	virtual void Draw(bool bMajor);

	// Handle input.
	//! Return list of menu actions names
	QStringList GetMenuLabels() const;
	//! Response on the button pressed action.
	void OnMenuButtonPressed(PanelButton pb);

private:
	//! The top (flight) widget
	WidgetFlight* m_pwFlight;
	//! The bottom (engine) widget
	WidgetEngine* m_pwEngine;
};

// --------------------------------------------------------------------------
} // namespace
#endif // PANEL_MODERN_H
