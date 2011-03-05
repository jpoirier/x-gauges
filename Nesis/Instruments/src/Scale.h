#ifndef INSTRUMENT_SCALE_H
#define INSTRUMENT_SCALE_H

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
#include "Parameter.h"

namespace instrument {
/** \brief The class is used to handle the basic scale parameters.

	 The scale is constructed from XML file.
 */
class Scale
{
protected:
	enum LimitType {
		ltNone,
		ltTick,
		ltArc
	};
public:
	//! Constructor from QDomElement XML.
	Scale(const QDomElement& el);
	//! Constructor which only takes start and end scale value.
	Scale(float fS, float fE);
	//! Destructor.
	virtual ~Scale();
	
	//! Get Parameter object.
	const Parameter* GetParameter() const
	{ return m_pPar; }
	
	//! Set scale limits
	void SetScaleLimits(float fS, float fE);
	//! Set scale limits
	void SetScaleLimits(const QPair<float,float> rSE)
	{ SetScaleLimits(rSE.first, rSE.second); }
	//! Get the scale start.
	float GetStart() const
	{ return m_fS; }
	//! Get the scale end.
	float GetEnd() const
	{ return m_fE; }

	//! Return the major tick scale step.
	float GetMajorTickStep() const
	{ return m_fMajorTickStep; }
	//! Return number of minor intervals between the major step.
	int GetMinorIntervalCount() const
	{ return m_iMinorIntervalCount; }
	//! Return the label scale step.
	float GetLabelStep() const
	{ return m_fLabelStep; }

	//! Return true if there is low limit (red) active.
	bool IsLowLimit() const
	{ return m_bLowLimit; }
	//! Return true if there is high limit (red) active.
	bool IsHighLimit() const
	{ return m_eHighLimit != ltNone; }
	
	//! Convert a system value into scale value.
	virtual float ConvertToScale(float fS) const;
	//! Convert pair of system values into scale value.
	QPair<float,float> ConvertToScale(const QPair<float,float>& P);
	
	//! Set the scale bound parameter.
	void EnableBoundScale(bool bBound)
	{ m_bBoundScale = bBound; }
	
	//! Update values from Parameter object and dump them.
	void UpdateValue();
	//! Get dumping value.
	float GetDumpingCoefficient() const
	{ return m_fDumping; }
	//! Get dumped system value.
	float GetDumpedSystemValue(int i=0) const
	{ return m_vDump[i]; }
	
protected:
	//! The scale parameter.
	const Parameter* m_pPar;
	
	//! Draw yellow arc?
	bool m_bYellow;
	//! Draw green arc?
	bool m_bGreen;
	//! Draw high limit?
	LimitType m_eHighLimit;
	//! Draw low limit?
	bool m_bLowLimit;
	
	//! Number of decimals shown on scale.
	int m_iDecimals;
	
	//! Value at the start of the scale.
	float m_fS;
	//! Value at the end of the scale.
	float m_fE;
	
	//! Major tick step in the scale units.
	float m_fMajorTickStep;
	//! Number of minor intervals inside a major interval.
	int m_iMinorIntervalCount;
	//! Label step in the scale units.
	float m_fLabelStep;
	
	//! Multiplier used to get scale value from the real value.
	float m_fMultiplier;
	
	//! Should we bound the scale?
	bool m_bBoundScale;
	
	//! The scale dumping parameter.
	float m_fDumping;
	//! Dump values.
	QVector<float> m_vDump;
};

// --------------------------------------------------------------------------

}	// namespace
#endif
