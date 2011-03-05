#pragma once
#ifndef __GEOMETRY_POLYGON2D_H
#define __GEOMETRY_POLYGON2D_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

/*! \file Polygon2D.h 
    \brief Declaration file for the Polygon2D class.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

#include <vector>
#include <QRect>
#include <QPolygon>
#include "Line2D.h"
#include "gpc.h"

// --------------------------------------------------------------------------

namespace geometry {

#define RES_SIZE 15

// -----------------------------------------------------------------------
//! The class performs operations on the polygon of Point2D objects.
/*! One of the role is calculation of integrals over the area using
    Green-Gauss formula. The area is defined by polygon of points.
    (point = vertex). Integration is performed over polynomial members in
    the form of (x^a * y^b). The maximal degree of polynomial is (a+b)=4. 
    The other role of the class is geometric role. The polygon can split itself
    into two halfs. 

    Special connection is provided to the gpc helper functions and structures.
    See also Contour2D class.
 */

class Polygon2D
{
public:
   //! Definition of the container which stores vertices (points - Point2D objects).
   typedef std::vector<Point2D> VecVertex;

   //! The polygon orientation directions
   enum Orientation {
      oCW  = 0,              //!< clockwise
      oCCW = 1,              //!< counterclockwise
      oClockwise = 0,        //!< clockwise (long version)
      oCounterclockwise = 1, //!< counterclockwise (long version)
      oInvalid = 2
   };

public:
   //! Constructor - creates empty polygon.
   Polygon2D();
	//! Constructor from QPolygonF.
	Polygon2D(const QPolygonF& pg);
	//! Constructor from QRectF.
	Polygon2D(const QRectF& rc);
   //! Constructor - creates polygon from given iterator range.
   Polygon2D(VecVertex::const_iterator b, VecVertex::const_iterator e);
   //! Copy constructor.
   Polygon2D(const Polygon2D& C);
   //! Copy constructor from vertex list (using gpc)
   Polygon2D(const gpc::gpc_vertex_list VL)
   { Copy(VL), m_uDegree = 4; }
   //! Destructor
   virtual ~Polygon2D();

public:
   //! The copy method, which creates a copy of an existing polygon.
   void Copy(const Polygon2D& C);

   //! The copy method, which creates a copy of an existing gpc vertex list.
   void Copy(const gpc::gpc_vertex_list VL);

   //! Clone creates a full copy on the heap. The caller owns the object.
   virtual Polygon2D* Clone() const { return new Polygon2D(*this); }

   //! Sets the highest degree of the polynomial that must be calculated by the polygon. 
   /*! During the integration (Green-Gauss) all possible integrals are evaluated and
       stored. To speed up the calculation, it is wise to specify the smallest
       degree that still satisfies our needs. But, then again, the calculation
       is pretty fast, and one may consider to use maximal degree.
    */
   void SetDegree (unsigned int d);

   //! Returns the current degree of integration for the polynom.
   unsigned int GetDegree() const { return m_uDegree; }

   //! Moves all points for given delta (dx, dy).
   void Move (qreal dx, qreal dy);

   //! Rotates all points around the origin for the specified angle.
   void Rotate (qreal dAlpha);
   //! Rotates all points around the specified point for the given angle.
   void Rotate(const Point2D& pt, qreal dAlpha);

   //! Creates two polygons. The splitLine argument presents a divider line.
   int Split(const Line2D& splitLine, Polygon2D& plgPos, Polygon2D& plgNeg) const;

   //! Calculates the integral of polynom P= x^a * y^b over the polynom area.
   qreal CalcIntegral (unsigned int a, unsigned int b) const;

   //! Returns the circumference of the polygon.
   qreal GetCircumference() const;

   //! This method appends new point p to the polygon.
   void Append(const Point2D& p);

   //! This method inserts new point p before the point at given position.
   void Insert(unsigned int uPos, const Point2D& p);

   //! This method removes a point from the polygon at required position.
   void Remove(unsigned int uPos);

   //! Const access to the points stored in the polygon.
   const Point2D& GetVertex(unsigned int i) const { return m_vecV[i]; }
   //! Full access to the points stored in the polygon. Sets the dirty flag.
   Point2D& GetVertex(unsigned int i) { m_bDirty=true; return m_vecV[i]; }

   //! Sometimes we must know number of items in the polygon.
   unsigned int GetCount() const
   { return m_vecV.size(); }

   //! Sometimes we need to know the last point in the polygon. 
   const Point2D& GetBack() const 
   { return m_vecV.back(); }

   //! Sometimes we need to know the first point in the polygon. 
   const Point2D& GetFront() const 
   { return m_vecV.front(); }

   //! Clears the cached results and empties the polygon.
   void Clear();

   //! Tests the position of point p. Returns true if p is inside. 
   bool IsInside(const Point2D& p, bool bBorder = true) const;

   //! Tests the position of polygon pg. Returns true if pg is inside. 
   bool IsInside(const Polygon2D& pg, bool bBorder = true, qreal dTol=1E-12) const;

   //! Tests the position of polygon pg. Returns true if pg is outside. 
   bool IsOutside(const Polygon2D& pg, bool bBorder = true, qreal dTol=1E-12) const;

   //! Tests of the polygon pg intersects or touches this polygon. 
   bool AnyIntersection(const Polygon2D& pg, bool bBorder = true, qreal dTol=1E-12) const;

   //! Returns the surrounding rect - ptMin min (x,y) & ptMax max(x,y).
   void GetSurroundingRectangle(Point2D& ptMin, Point2D& ptMax) const;

   //! Returns the surrounding rect - QRectF.
   QRectF GetSurroundingRectangle() const;

   //! Returns the line of i-th segment.
   Line2D GetLine(unsigned int i) const;

   //! Write the polygon to the binary stream.
	virtual void Read(QDataStream& out);
   //! Read the polygon from the binary stream.
	virtual void Write(QDataStream& in) const;

   //! Returns true of the polygon is empty (has less than three elements)
   bool IsEmpty() const
   { return GetCount() < 3; }

   //! Returns the polygon orientation
   Orientation GetOrientation() const;

   //! Swithces orientation form CW to CCW and vice versa
   void SwitchOrientation();

   //! Offsets the polygon for given "delta" value.
   void Offset(qreal dOff);

   //! Search the polygon for given point.
   int FindVertex(const Point2D& pt) const;

   //! Returns true if the polygon is twisted (crosses self at least once);
   bool IsTwisted() const;

   //! Returns true if the given point is a vertex point.
   bool IsVertex(const Point2D& pt) const
   { return FindVertex(pt)>=0; }

   //! Returns the collection of intersection points with the given line.
   QVector<Point2D> GetIntersection(const Line2D& ln, bool bOnLineOnly, bool bParallel) const;

   //! Returns the distance of given point to the polygon edge.
   qreal GetDistance(const Point2D& pt) const;

   //! Returns the mass point of the polygon
   Point2D GetMassPoint() const;

   //! Creates a gpc_vertex_list structure out of our polygon.
   gpc::gpc_vertex_list GetGPCVertexList() const;

protected:
   //! This method does the internal calculation.
   void Calculate();

   //! Helper function.
   unsigned int Index(int a, int b) const
   { return s_auIndex[a+b]+b; }

protected:
   //! The vertices of polygon. This is a stl vector of Point2D objects. 
   VecVertex m_vecV;

   //! Tells the polygon status. If dirty, then the integrals must be recalculated.
   bool m_bDirty;

   //! The results are stored here. 
   qreal m_adResult[RES_SIZE];
   //! The weights of sums are stored here. 
   static qreal s_adWeight[RES_SIZE];

   //! Required degree of the polygon. 
   unsigned int m_uDegree;

   //! Returns an index that points to the first position in m_adResult.
   static unsigned int s_auIndex[5];
};
// --------------------------------------------------------------------

//! Output the polygon vertices to the ascii stream
std::ostream& operator<<(std::ostream& out, const Polygon2D& p);

// --------------------------------------------------------------------

//! Store the polygon to the binary data stream by forwarding the call to Polygon2D::Serialize. 
inline QDataStream& operator<<(QDataStream& ds, const Polygon2D& p)
{
   p.Write(ds);
   return ds;
}
// --------------------------------------------------------------------

//! Convinient operator for appending new points into the polygon object.
inline Polygon2D& operator <<(Polygon2D& pg, const Point2D& pt)
{
	pg.Append(pt);
	return pg;
}

// --------------------------------------------------------------------

//! Read the polygon from the binary data stream by forwarding call to Polygon2D::Serialize.
inline QDataStream& operator>>(QDataStream& ds, Polygon2D& p)
{
   p.Read(ds);
   return ds;
}

} // namespace geometry
#endif // __GEOMETRY_POLYGON2D_H
