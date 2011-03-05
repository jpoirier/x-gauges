#pragma once
#ifndef __GEOMETRY_LINE2D_H
#define __GEOMETRY_LINE2D_H

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

/*! \file Line2D.h 
    \brief Declaration file for the Line2D class.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

#include <functional>
#include "Point2D.h"
#include "MathEx.h"

// --------------------------------------------------------------------------

namespace geometry {

// --------------------------------------------------------------------------
//! The class implements several useful geometry operations on a line. 
/*! The line has a direction from point A->B. The A represents the
    start point, while B represents end point of the line. However, the
    line is endless. A and B are just useful points on a line. Thus the 
    class is a mixture of line and vector behaviour. It stores start 
    and end point together with line equation: ax + by + c = 0. 
    This class implements the line in the 2D (XY) plain. 
 */
    
class Line2D
{
public:
   //! Intersection flags - use with the #GetIntersection method.
   enum Intersect {
      intNone   = 0, //!< There is no intersection. 
      intExist  = 1, //!< Intersection exists
      intFirst  = 2, //!< Intersection point lies within the first line.
      intSecond = 4, //!< Intersection point lies within the second line.
      intBoth   = 7  //!< Combination of all: exist, first and second.
   };

   //! Position flags
   enum Positon {
      posOnLine   =  1, //!< The point lies on the line direction
      posInside   =  2, //!< The point is inside the line (between A and B).
      posPositive =  4, //!< The point lies on the positive halfspace.
      posNegative =  8, //!< The point lies on the negative halfspace.
      posA        = 16, //!< The point lies on the point A
      posB        = 32, //!< The point lies on the point B
      posEnd      = 48, //!< Check if the point lies on A or B
      posInsideEnd = 50, //!< Check if the point is inside, on A or on B.
   };

public:
   //! Default constructor
   Line2D();
   //! Constructor from four parameters that represent two points
   Line2D(qreal dAx, qreal dAy, qreal dBx, qreal dBy);
   //! Contructor from two points
   Line2D(const Point2D& ptA, const Point2D& ptB);
   //! Copy constructor.
   Line2D(const Line2D& line);

public:
   //! Copies the given object into this.
   void Copy(const Line2D& C);
   //! Assignment operator.
   const Line2D& operator=(const Line2D& C)
   { Copy(C); return *this; }
   
   //! The start point of the line.
   const Point2D& GetA() const { return m_ptA; }
   //! The start point of the line. Same as #GetA.
   const Point2D& GetStart() const { return GetA(); }
   //! The end point of the line.
   const Point2D& GetB() const { return m_ptB; }
   //! The end point of the line. Same as #GetB.
   const Point2D& GetEnd() const { return GetB(); }

   //! Sets the new line start (A) and end (B) points.
   void Set(const Point2D& ptA, const Point2D& ptB);
   //! Set line equation coefficients directly
   void Set(qreal dA, qreal dB, qreal dC);
   //! Sets the new start point.
   void SetA(const Point2D& ptA); 
   //! Sets the new end point.
   void SetB(const Point2D& ptB);
   //! Sets the new start point. Same as #SetA.
   void SetStart(const Point2D& ptA)
   { SetA(ptA); } 
   //! Sets the new end point. Same as #SetB.
   void SetEnd(const Point2D& ptB)
   { SetB(ptB); }

   //! For given y coordinate calculates the x coordinate.
   qreal GetX(qreal y) const 
   { return ((a) ? (-c-b*y)/a : 0.); }
   //! For given x coordinate calculates the y coordinate.
   qreal GetY(qreal x) const 
   { return ((b) ? (-c-a*x)/b : 0.); }

   //! Swaps the start/end points. The line changes direction.
   void Swap();

   //! Returns the line length.
   qreal GetLength() const;

   //! Moves the line for given offset (dx,dy).
   void Move(qreal dx, qreal dy );
   //! Moves the line for the given point
   void Move(const Point2D& d)
   { Move(d.GetX(), d.GetY()); }
   //! Moves the line in the current line direction for the dl units.
   void MoveInDirection (qreal dl);
   //! Moves the line in the perpendicular direction for the dt units.
   void Offset (qreal dt);

   //! Rotates the line for the given angle around the origin.
   void Rotate(qreal dAlpha);
   //! Rotates the line for the given angle around given point.
   void Rotate(const Point2D& ptO, qreal dAlpha);
   //! Returns the current line angle in radians.
   qreal GetAngle() const;
   //! Returns the angle between this line and line b.
   qreal GetAngle(const Line2D& liB) const;
   //! Returns the intersection point with the line b. Check the return flags!
   int GetIntersection(const Line2D& liB, Point2D& ptInt, qreal dTol=1E-12) const;
   //! Returns true if given line is parallel to this line.
   bool IsParallel(const Line2D& liB, qreal dTol=1E-12) const;

   //! Returns the intersection point with a perpendicular trough pt.
   Point2D GetNearestPoint(const Point2D& pt) const;

   //! Creates a parallel line which goes trough point t.
   Line2D CreateParallel(const Point2D& t) const
   { return Line2D(t.GetX(), t.GetY(), t.GetX()+b, t.GetY()-a); }
   //! Creates a perpendicular line which goes trough  point t.
   Line2D CreatePerpendicular(const Point2D& p) const;

   //! Creates a direction (unit length) line.
   Line2D CreateDirection(qreal dRelS, qreal dRelAlpha) const;

   //! Calculates the overlap of two lines. 
   Line2D GetOverlap(const Line2D& ln) const;
   
   //! Tells where the point p lies. Returns a combination of flags. There is some tolerance.
   unsigned int GetPosition(const Point2D& p, qreal dTol=1E-12) const;

   //! Get the line point which is dRelS line lengths away from the start point.
   Point2D At(qreal dRelS) const;

   //! Returns number of line lengths needed to reach point p from the start point.
   qreal GetScalar(const Point2D& p, qreal dTol=1E-12) const;

   //! Returns true if line is a nondegenerate line.
   bool IsValid()  const { return a || b; }

   //! Serializes the line to the CArchive stream.
   void Serialize(QDataStream& ar);

   //! Normalizes the line. The start point remains at point A, but
   // the B is recalculated at such way, that length will be 1.
   void Normalize();

   //! Operator == The lines are equal if the points are equal
   bool operator==(const Line2D& line)
   { return GetA()==line.GetA() && GetB()==line.GetB(); }

protected:
   //! Calculates the internal variables according to the start and end point.
   void Setup();

protected:
   //! Start point of the line.
   Point2D m_ptA;
   //! End point of the line.
   Point2D m_ptB;

private:
   // Line coefficients (we violate naming rules here due to historical reasons.
   //! Coefficient a of the ax + by + c = 0 line equation.
   qreal a;
   //! Coefficient b of the ax + by + c = 0 line equation.
   qreal b;
   //! Coefficient c of the ax + by + c = 0 line equation.
   qreal c;
};
// -----------------------------------------------------------------------------

//! Outputs the Line2D object to the out stream.
inline std::ostream& operator<<(std::ostream& out, const Line2D& li)
{
   out << "A" << li.GetA() << "-->B" << li.GetB() 
       << ": L = " << li.GetLength() << " alpha = " << common::Deg(li.GetAngle());
   return out;
}

// -----------------------------------------------------------------------------

//! Special binary_function that sorts points on the line.
class Point2DSortLineLess : std::binary_function<Point2D, Point2D, bool> 
{
public:
   Point2DSortLineLess(const Line2D& ln) 
   { m_line = ln; }

   result_type operator() (first_argument_type a, second_argument_type b);

private:
   Line2D m_line;
};


// -----------------------------------------------------------------------------
} // namespace geometry

#endif // __GEOMETRY_LINE2D_H
