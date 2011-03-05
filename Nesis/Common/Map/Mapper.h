#ifndef __MAP_MAPPER_H
#define __MAP_MAPPER_H
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

#include <memory>
#include <QPoint>
#include "CommonDefs.h"
#include "AbstractProjection.h"

namespace map {

// -----------------------------------------------------------------------
/** \brief A class that converts between coordinate systems.
	 This object is used to transform coordinates between the WGS sytem
    used by the \a World object and pixel coordinates used by the
	 \a Paper object. All transformations need to be fast and the 
    code optimized for speed.

	 Mapper class needs and external projection object. The projection
    object must be available for the lifetime of this object.
 */
class Mapper
{
public:
	//! Constructor.
	Mapper(int iPixPerMeter, int iPaperDim);
	//! Destructor.
	virtual ~Mapper();
	
	//! Set scaling parameters for given reference central point and scale.
	void SetScale(float fScale, const QPointF& ptWGSc);
	
	//! Convert WGS coordinate into Paper (pixel) coordinate. 
	QPoint ConvertToImage(const QPointF& ptWGS) const;
	//! Convert WGS coordinate (degrees) into Paper (pixel) coordinate. 
	QPoint ConvertToImageDegrees(const QPointF& ptWGS) const;
	//! Convert Paper (pixel) coordinate into WGS coordinate (radians). 
	QPointF ConvertToWGS(const QPoint& ptPaper) const;
	//! Convert Paper (pixel) coordinate into WGS coordinate (degrees). 
	QPointF ConvertToWGSDegrees(const QPoint& ptPaper) const;
	
	//! Set the projection between WGS and flat earth. It does not own the projection.
	void SetProjection(AbstractProjection* pProjection)
	{ m_pProjection = pProjection; }
	
	//! Returns the current scale
	float GetScale() const
	{ return m_fScale; }
	
	//! Calculate how many pixels we need for one degree.
 	int GetPixPerDegree() const;
	
	//! Calculate logitude arc between two pixels in x direction for given pixel point.
	float GetLongitudeOnePixelArcDegree(QPoint ptPix);
	
	//! Returns true if the projection is rectangular.
	bool IsRectangularProjection() const
	{ return m_pProjection->IsRectangular(); }

protected:
	//! Convert flat earth x coordinate into x pixel coordinate.
	int ConvertFE2PaperX(float fX) const
	{ return (int)(m_fAx * fX + m_fBx); }
	//! Convert flat earth y coordinate into y pixel coordinate.
	int ConvertFE2PaperY(float fY) const
	{ return (int)(m_fAy * fY + m_fBy); }
	//! Convert flat earth coordinate point into pixel point.
	QPoint ConvertFE2Paper(const QPointF& pt) const
	{ return QPoint(ConvertFE2PaperX(pt.x()), ConvertFE2PaperY(pt.y())); } 
	
	//! Convert x pixel coordinate into x flat earth coordinate.
	float ConvertPaperX2FE(int iX) const
	{ return m_fIAx * iX + m_fIBx; }
	//! Convert flat earth y coordinate into y pixel coordinate.
	float ConvertPaperY2FE(int iY) const
	{ return m_fIAy * iY + m_fIBy; }
	//! Convert flat earth coordinate point into pixel point.
	QPointF ConvertPaper2FE(const QPoint& pt) const
	{ return QPointF(ConvertPaperX2FE(pt.x()), ConvertPaperY2FE(pt.y())); } 
	
private:
	//! Projection used to convert from wgs to flat earth map.
	AbstractProjection* m_pProjection;
	
	// Parameters needed to calculate scale coefficients
	//! Screen pixels per meter.
	int m_iPixPerMeter;
	//! Dimension of paper square side in meters.
	int m_iL;
	//! Current scale.
	float m_fScale;
	
	//! Coefficients used to convert form flat earth into pixels.
	float m_fAx, m_fBx, m_fAy, m_fBy;
	//! Coefficients used to convert form pixels into flat earth.
	float m_fIAx, m_fIBx, m_fIAy, m_fIBy;
};
// -----------------------------------------------------------------------

inline QPoint Mapper::ConvertToImage(const QPointF& ptWGS) const
{
	ASSERT(m_pProjection!=NULL);
	return ConvertFE2Paper(m_pProjection->ConvertToMap(ptWGS));
}

// -----------------------------------------------------------------------

inline QPoint Mapper::ConvertToImageDegrees(const QPointF& ptWGS) const
{
	QPointF pt(ptWGS);
	pt.rx() *= M_PI/180;
	pt.ry() *= M_PI/180;
	ASSERT(m_pProjection!=NULL);
	return ConvertFE2Paper(m_pProjection->ConvertToMap(pt));
}

// -----------------------------------------------------------------------

inline QPointF Mapper::ConvertToWGS(const QPoint& ptPaper) const
{
	ASSERT(m_pProjection!=NULL);
	return m_pProjection->ConvertToWGS(ConvertPaper2FE(ptPaper));
}

// -----------------------------------------------------------------------

inline QPointF Mapper::ConvertToWGSDegrees(const QPoint& ptPaper) const
{
	QPointF pt = ConvertToWGS(ptPaper);
	pt.rx() *= 180/M_PI;
	pt.ry() *= 180/M_PI;
	return pt;
}

// -----------------------------------------------------------------------

} // map namespace

#endif	// __MAP_MAPPER_H
