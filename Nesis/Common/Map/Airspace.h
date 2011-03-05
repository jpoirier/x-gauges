#ifndef __MAP_AIRSPACE_H
#define __MAP_AIRSPACE_H

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

#include <QString>
#include <QRect>
#include "Geometry/Polygon2D.h"

// Forward declarations
class QPainter;

namespace map {
	
#define AS_NOTAM     	 -3
#define AS_UNLIMITED     -2
#define AS_SURFACE       -1
	
// Forward declarations
class Mapper;
	
// ------------------------------------------------------------------------ 
/*! \brief Airspace class - polygon and information.
	 
	The airspace class stores a closed polygon of coordinates used to
   draw the shape of aispace on the screen plus all additional
   airspace information.
 */

class Airspace {
public:
	enum Type {
		tCTA_CTR,
		tAirways,
		tRestricted,
		tProhibited,
		tDanger,
		tOther,
		tTrainingZone,
		tTrafficInfo,
		tGliderSector,
		tGliderProhibited,
		tMilitaryZone,
		tTransponderMandatoryZone,
		tBoundary,
		tUnknown
	};
	
	enum Class {
		cAll  = 0x3fff,
		cAll_but_GP_W_O = 0x0eff,
		cA		= 0x0001,
		cB		= 0x0002,
		cC		= 0x0004,
		cD		= 0x0008,
		cE		= 0x0010,
		cF		= 0x0020,
		cG		= 0x0040,
		cCTR	= 0x0080,	// I am not sure this is intended here - should be in type.
		cGP	= 0x0100,	// Glider prohibited ... should be in type.
		cR		= 0x0200,	// restricted
		cRestricted = cR,
		cQ		= 0x0400,	// danger
		cDanger = cQ,
		cP		= 0x0800,	// prohibited
		cProhibited = cP,
		cW		= 0x1000,	// wave window
		cWaveWindow = cW,
		cOther= 0x2000		// other
	};
	
public:
	//! Airspace contructor.
	Airspace();
	//! Copy constructor
	Airspace(const Airspace& C);
	//! Destructor.
	virtual ~Airspace();
	
	//! Copy method.
	void Copy(const Airspace& C);
	//! Clear the airspace object into initial state.
	void Clear();
	
	//! Set airspace title.
	void SetTitle(const QString& qsTitle)
	{ m_qsTitle = qsTitle; }
	//! Get airspace title.
	const QString& GetTitle() const
	{ return m_qsTitle; }
	
	//! Set airspace type.
	void SetType(Type eType)
	{ m_eType = eType; }
	//! Get airspace type.
	Type GetType() const
	{ return m_eType; }
	
	//! Set airspace class.
	void SetClass(Class eClass)
	{ m_eClass = eClass; }
	//! Set airspace class - string as argument.
	void SetClass(const QString& qsClass);
	//! Get airspace class.
	Class GetClass() const
	{ return m_eClass; }

	//! Set airspace floor limit.
	void SetFloor(const QString& qsFloor);
	//! Get airspace ceiling limit in text form.
	const QString& GetFloor() const 
	{ return m_qsFloor; }
	//! Return floor altitude in feet.
	int GetFloorFt() const 
	{ return m_iFloor; }
	
	//! Set airspace ceiling limit.
	void SetCeiling(const QString& qsCeiling);
	//! Get airspace ceilking limit in text form.
	const QString& GetCeiling() const 
	{ return m_qsCeiling; }
	//! Get ceiling altitude in feet.
	int GetCeilingFt() const
	{ return m_iCeiling; }
	
	//! Set airspace radio.
	void SetRadio(const QString& qsRadio)
	{ m_qsRadio = qsRadio; }
	//! Get radio freq.
	const QString& GetRadio() const 
	{ return m_qsRadio; }
	
	//! Set airspace active string.
	void SetActive(const QString& qsActive)
	{ m_qsActive = qsActive; }
	
	//! Append one point to the polygon.
	void Append(const QPointF& pt)
	{ m_pgArea.Append(pt); }
	//! Append a polygon to the existing polygon.
	void Append(const QPolygonF& pg);
	//! Clear polygon points only.
/*	void ClearPolygon()
	{ m_pgArea.Clear(); }*/
	//! Get const access to the polygon.
	const geometry::Polygon2D& GetPolygon() const
	{ return m_pgArea; }
	
	//! Return last point in the polygon. It is null, it polygon is empty.
	QPointF GetBackPoint() const;
	
	//! Returns true if airspace seem to be valid.
	bool IsValid() const;
	
	//! Get the bounding rect.
	QRectF GetBounding() const
	{ return m_rcBounding; }
	
	//! Check if at least a part of airspace bounding rect is inside a given rectangle.
	bool IntersectsBounding(const QRectF& rc) const
	{ 	return m_rcBounding.intersects(rc); }
	//! Check if at least a part of given rect is inside airspace polygon (not so fast).
	bool IntersectsPolygon(const QRectF& rc, bool bCheckBoundingFirst=true) const;
	
	//! Check if given point lies inside airspace bounding rect.
	bool IsInsideBounding(const QPointF& ptX) const
	{ 	return m_rcBounding.contains(ptX); }
	
	//! Check if a given point is inside the airspace polygon (not so fast).
	bool IsInside(const QPointF& ptX, bool bCheckBoundingFirst=true) const;
	
	//! Update internal data that are based on the polygon.
	void Update();
	
	//! Return sectors (blocks) used by the airspace polygon.
	QVector<quint16> GetSectors() const
	{ return m_vSectors; }

	//! Draw the polygon without any text.
	void Draw(QPainter* pP, const Mapper* pM) const;
	
	//! Write the data into given binary stream.
	virtual void Write(QDataStream& out) const;
	//! Read the data from given binary stream.
	virtual void Read(QDataStream& in);
	
private:
	//! Convert base and top altitude into number (feet). Negative values indicate AGL.
	int ParseAltitude(const QString& qsAlt);
		
private:
	//! Polygon of wgs coordinates which define the airspace area.
	geometry::Polygon2D m_pgArea;
	//! A rectangle that bounds (surrounds) the polygon.
	QRectF m_rcBounding;
	//! Vector of blocks (sectors) that are covered with the airspace.
	QVector<quint16> m_vSectors;
	
	//! Lower bound of the airspace - in string format.
	QString m_qsFloor;
	//! Lower bound of the airspace in feet. Negative = AGL.
	int m_iFloor;
	
	//! Upper bound of the airspace - in string format.
	QString m_qsCeiling;
	//! Upper bound of the airspace in feet. Negative = AGL.
	int m_iCeiling;
	
	//! Active status (opening hours). Could be empty.
	QString m_qsActive;
	//! Radio(s) - frequency(ies). Could be empty.
	QString m_qsRadio;
	
	//! Title of the namespace.
	QString m_qsTitle;
	//! Type of the airspace.
	Type m_eType;
	//! Class of the airspace.
	Class m_eClass;
};

// ---------------------------------------------------------------------------

} // map namespace

#endif	// __MAP_AIRSPACE_H
