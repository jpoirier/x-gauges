#ifndef INSTRUMENT_SCALE_ROUND_H
#define INSTRUMENT_SCALE_ROUND_H

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
#include <cmath>

#include "Scale.h"
#include "Parameter.h"
#include "Needle.h"

#define EPS 0.00001
#define MAJOR_TICK_SIZE			10 
#define MINOR_TICK_SIZE			8 
#define GR_FONT					"Bitstream Vera Sans"


namespace instrument {
/** \brief The class is used to draw a rounded scale and its elements.

	 The scale is constructed from XML file.
 */
class ScaleRound : public Scale
{
public:
	//! Constructor from QDomElement XML.
	ScaleRound(const QDomElement& el);
	//! Constructor parameters.
	ScaleRound(int iStart, int iArcLen, float fS, float fE);
	//! Destructor.
	virtual ~ScaleRound();
	
	//! Draw the scale.
	void Draw(QPainter& P, int iS);
	//! Draw the needle.
	void DrawNeedle(QPainter& P);
	
	//! Draw tick at scale value fX.
	void DrawTickScale(QPainter& P, float fX, int iR, int iL) const;
	//! Draw tick at angle given in radians.
	void DrawTick(QPainter& P, float fAngleRad, int iR, int iL) const;

	//! Draw label at value fX.
	void DrawLabelScale(QPainter& P, float fX, int iR, 
		const QString& qs, bool bInside=true) const;
	//! Draw label at given angle. Used when labels are drawn directly.
	void DrawLabel(QPainter& P, float fAngleRad, int iR, 
		const QString& qs, bool bInside=true) const;
		
	//! Draw arc for given range [fS, fE].
	void DrawArcScale(QPainter& P, float fS, float fE, int iR) const;
	//! Draw arc for given range [fS, fE].
	void DrawArcScale(QPainter& P, QPair<float,float> rR, int iR) const
	{ DrawArcScale(P, rR.first, rR.second, iR); }
	
	//! Draw circle mark at value fX.
	void DrawCircleScale(QPainter& P, float fX, int iR, int ir) const;
	//! Draw circle mark. Used when mark is drawn directly.
	void DrawCircle(QPainter& P, float fAngleRad, int iR, int ir) const;
	
	//! Draw triangle mark at value fX.
	void DrawTriangleScale(QPainter& P, float fX, int iR, int iL) const;
	//! Draw triangle mark (as a tick). Used when mark is drawn directly.
	void DrawTriangle(QPainter& P, float fAngleRad, int iR, int iL) const;
		//! Get access to the needle object.
	Needle& GetNeedle()
	{ return m_needle; }
	
	//! Set scale center point.
	void SetCenter(const QPoint& ptC)
	{ m_ptC = ptC; }
	
protected:
	//! Draw the scale ticks and labels.
	virtual void DrawScale(QPainter& P);
	//! Draw the scale decoration - arcs, marks, etc.
	virtual void DrawDecoration(QPainter& P);

	//! Calculate angle for value x. Angle is given in qt-system.
	int GetAngleQt(qreal x) const
	{ return m_iArcStart + qRound((x-m_fS)/(m_fE-m_fS)*m_iArcLen); }
	//! Calculate angle for value x. Angle is given in radians.
	qreal GetAngleRad(qreal x) const
	{ return GetAngleQt(x)*M_PI/(180*16); }
	//! Calculate angle for value x. Angle is given in degrees.
	qreal GetAngleDeg(qreal x) const
	{ return GetAngleQt(x)/16.0f; }
	
protected:
	//! Scale length stored in qt-system.
	int m_iArcLen;
	//! Scale (arc) start angle in qt-system.
	int m_iArcStart;
	
	//! Font size
	int m_iFontSize;
	
	//! The Center.
	QPoint m_ptC;
	//! Reference radius.
	int m_iR;
	
	//! The needle object
	Needle m_needle;
	//! The cold needle.
	int m_iColdNeedle;
	//! The hot needle.
	int m_iHotNeedle;
	//! Is hot/cold needle in use?
	bool m_bHotColdNeedle;
};

// --------------------------------------------------------------------------

class ScaleRoundAirspeed : public ScaleRound
{
public:
	//! Constructor from QDomElement XML.
	ScaleRoundAirspeed(const QDomElement& el);
	
	//! Convert a system value into scale value.
	float ConvertToScale(float fSys) const;

protected:
	//! Draw the scale ticks and labels.
	void DrawScale(QPainter& P);
	//! Draw the scale decoration - arcs, marks, etc.
	void DrawDecoration(QPainter& P);
	
protected:
	//! Scale factor used to draw scale below the nominal scale.
	float m_fC0;
	//! Scale factor used to draw scale below the nominal scale.
	float m_fC1;
	//! Decision limit for using the C0 and C1 based scale.
	float m_fScaleLimit;
};

}	// namespace
#endif
