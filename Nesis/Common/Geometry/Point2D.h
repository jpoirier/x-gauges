#ifndef __POINT2D_H
#define __POINT2D_H

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

/*! \file Point2D.h
    \brief Declaration file for the Point2D class.

    \author Ales Krajnc
    \date   1.1.2006 - ported from VisualStudio code.
 */

#include <iostream>
#include <QPoint>

#include "CommonDefs.h"
#include "Tolerance.h"

// ----------------------------------------------------------------------------
//! The geometry namespace combines geometry related classes.

namespace geometry {

// ----------------------------------------------------------------------------
//! This class implements a point in 2D space. Point is based on qreal precision.
/*! It is assumed that this point lies in the X-Y plain and
    positive angles are measured counterclockwise. This defines righthand
    Euclidian space where Z axis seems to be missing. But it is not. Z axis
    is defined by positive \b counterclockwise rotation of positive angles
    (righthand rule).

    The class provides operator conversions to QPoint class.

    The class is member of the #geometry namespace. Related classes are
    Line2D ...

 */

class Point2D
{
public:
   //! Default constructor.
   Point2D(qreal x=0.0, qreal y=0.0)
   { m_dX=x; m_dY=y; }
   //! Construct from QPoint.
   Point2D(QPoint p)
   { m_dX=p.x(); m_dY=p.y(); }
   //! Construct from QPointF.
   Point2D(QPointF p)
   { m_dX=p.x(); m_dY=p.y(); }
   //! Copy constructor.
   Point2D(const Point2D& C)
   { Copy(C); }

   //! Create a copy of given point.
   void Copy(const Point2D& C)
   { m_dX = C.m_dX; m_dY = C.m_dY; }

   //! The assignment operator.
   const Point2D& operator=(const Point2D& p)
   { Copy(p); return *this; }

   //! Sets the x coordinate.
   void SetX(qreal dX)
   { m_dX = dX; }
   //! Sets the y coordinate.
   void SetY(qreal dY)
   { m_dY = dY; }

   //! Returns the x coordinate.
   qreal GetX() const
   { return m_dX; }
   //! Returns the y coordinate.
   qreal GetY() const
   { return m_dY; }

   //! Offsets the current point by a given distance.
   void Move(const Point2D& p)
   { m_dX+=p.m_dX; m_dY+=p.m_dY; }
   //! Rotates the point around the origin for given angle.
   void Rotate(qreal dAlpha);
   //! Rotates the point around given point (move, rotate, move).
   void Rotate(const Point2D& p, qreal dAlpha);

   //! Calculates the distance between this and given point.
   qreal GetDistance(const Point2D& p) const;
   //! Calculates the angle between this point and given point p.
   qreal GetAngle(const Point2D& p) const;

   //! The difference between this and given point p returned as a new point.
   Point2D operator-(const Point2D& p) const;
   //! The sum of this and given point p returned as a new point.
   Point2D operator+(const Point2D& p) const;

   //! Negate the point.
   Point2D operator-() const
   { return Point2D(-m_dX,-m_dY); }

   //! Operator that converts this point to QPoint object.
   operator QPoint() const
   { return QPoint((int)(m_dX+0.5),(int)(m_dY+0.5)); }

   //! Operator that converts this point to QPointF object.
   operator QPointF() const
   { return QPointF(m_dX, m_dY); }

   //! Adds the given coordinates to the existing coordinates. Same as #Move.
   void operator+=(const Point2D& p)
   { m_dX+=p.m_dX; m_dY+=p.m_dY; }
   //! Subtracts the given coordinates from the existing coordinates.
   void operator-=(const Point2D& p)
   { m_dX-=p.m_dX; m_dY-=p.m_dY; }

   //! Compares for equality of two points using default relative tolerance.
   bool operator==(const Point2D& p) const
   { return tolerance_rel::Equal(m_dX,p.m_dX)
         && tolerance_rel::Equal(m_dY,p.m_dY);}
   //! Negation of the operator ==.
   bool operator!=(const Point2D& p) const
   { return !operator==(p); }
   //! Returns true if the first point has smaller coordinates than given point p.
   bool operator<(const Point2D& p) const
   { return m_dX<p.m_dX && m_dY<p.m_dY; }

   //! Compares for equality of two points using specified tolerance.
   bool IsEqual(const Point2D& p, qreal dTolerance) const
   { return tolerance_rel::Equal(m_dX,p.m_dX, dTolerance)
         && tolerance_rel::Equal(m_dY,p.m_dY, dTolerance);}

   //! Serializes the point to the data binary stream.
   void Serialize(QDataStream& ds);
protected:
   //! X coordinate.
   qreal m_dX;
   //! Y coordinate.
   qreal m_dY;
};

//! Outputs the Point2D object to the out stream.
inline std::ostream& operator<<(std::ostream& out, const Point2D& p)
{
   out << "(" << p.GetX() << "," << p.GetY() << ")";
   return out;
}

//! Writes the Point2D object to the data stream object.
inline QDataStream& operator<<(QDataStream& ds, const Point2D& p)
{
   const_cast<Point2D&>(p).Serialize(ds);
   return ds;
}

//! Reads the Point2D object from the data stream object.
inline QDataStream& operator>>(QDataStream& ds, Point2D& p)
{
   p.Serialize(ds);
   return ds;
}

// --------------------------------------------------------
//! Special binary_function that sorts points.
class Point2DSortXLess : std::binary_function<Point2D, Point2D, bool>
{
public:
   Point2DSortXLess() {}

   result_type operator() (first_argument_type a, second_argument_type b)
   {
      return (a.GetX() <  b.GetX()) ? true :
             (a.GetX() == b.GetX()) ? a.GetY() <b.GetY() :
                                  false;
   }
};


}
// ----------------------------------------------------------------------------
#endif // __POINT2D_H
