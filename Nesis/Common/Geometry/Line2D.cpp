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

/*! \file Line2D.cpp
    \brief Implementation of the class Line2D.

    \author Ales Krajnc
    \date 1.1.2006 - Ported from the VisualStudio
 */
// --------------------------------------------------------------------------

#include <assert.h>
#include <limits>

#include "CommonDefsQt.h"
#include "math.h"
#include "Line2D.h"

// --------------------------------------------------------------------------

using namespace common;
namespace geometry {

// --------------------------------------------------------------------------

Point2DSortLineLess::result_type
Point2DSortLineLess::operator() (
   first_argument_type a,
   second_argument_type b)
{
   Q_ASSERT(m_line.GetPosition(a) & Line2D::posOnLine);
   Q_ASSERT(m_line.GetPosition(b) & Line2D::posOnLine);
   qreal xa = m_line.GetScalar(a);
   qreal xb = m_line.GetScalar(b);
   return xa < xb;
}

// --------------------------------------------------------------------------

Line2D::Line2D()
{
   a=b=c = 0.0; // not a valid line
}

Line2D::Line2D(qreal x1, qreal y1,  qreal x2, qreal y2)
: m_ptA(x1,y1), m_ptB(x2,y2)
{
   Setup();
}

Line2D::Line2D(const Point2D& ptA, const Point2D& ptB)
: m_ptA(ptA), m_ptB(ptB)
{
   Setup();
}

Line2D::Line2D(const Line2D& C)
{
   Copy(C);
}

// --------------------------------------------------------------------------

void Line2D::Copy(const Line2D& C)
{
   m_ptA = C.m_ptA;
   m_ptB = C.m_ptB;
   a = C.a;
   b = C.b;
   c = C.c;
   // Setup not needed.
}

// --------------------------------------------------------------------------
/*! This method calculates constants needed to describe the line
    equation. The direction goes from A to m_ptB.
    The equations looks like: ax + by + c = 0;
    a, b, c are member variables. They violate the m_ naming rule.
 */

void Line2D::Setup()
{
   a = m_ptA.GetY() - m_ptB.GetY();    // a == -ay (vector component)
   b = m_ptB.GetX() - m_ptA.GetX();    // b ==  ax (vector component)
   c = m_ptA.GetX()*m_ptB.GetY() - m_ptB.GetX()*m_ptA.GetY();
}

// --------------------------------------------------------------------------
/*! It calculates the distance between point A and point m_ptB.
    \retval qreal the distance. It is just the Pitagora rule.
 */

qreal Line2D::GetLength () const
{
   return sqrt( a*a + b*b );
}

// --------------------------------------------------------------------------
/*! Move the line for given dx in x direction and given dy in y direction.
    After the move, we recalc the internal a,b,c variables.
    \param dx the offset in x direction.
    \param dy the offset in y direction.
 */

void Line2D::Move (qreal dx, qreal dy)
{
   Point2D delta(dx,dy);
   m_ptA+=delta;
   m_ptB+=delta;
   Setup();
}

// --------------------------------------------------------------------------
/*! Keeps current direction and length. It just moves the lines
   (points A and B, actualy) for the dl units in current direction.
   \param dl units to move in the current line direction.
 */
void Line2D::MoveInDirection (qreal dl)
{
   qreal dAlpha = GetAngle();
   Move(dl*cos(dAlpha),dl*sin(dAlpha));
}

// --------------------------------------------------------------------------
/*! Moves the line for the dl units in the perpendicular direction.
    Positive dl value moves the line to the positive side. The positive
    side is left side of the line if we look from point A (start) to point B (end).
    \param dt offset value. See the method description.
 */

void Line2D::Offset (qreal dt)
{
   qreal dAlpha = GetAngle()+M_PI_2;
   Move (dt*cos(dAlpha), dt*sin(dAlpha));
}

// --------------------------------------------------------------------------
/*! We simply rotate both points around the origin.
    \param dAlpha rotation angle (positive angle is counterclockwise).
 */

void Line2D::Rotate(qreal dAlpha)
{
   m_ptA.Rotate(dAlpha);
   m_ptB.Rotate(dAlpha);
   Setup();
}
// --------------------------------------------------------------------------
/*! Rotation about an origin point. We rotate both points and calculate new
    line.
 */
void Line2D::Rotate(
   const Point2D& ptO,  //!< Origin point.
   qreal dAlpha        //!< Rotation angle (positive is counterclockwise).
)
{
   m_ptA.Rotate(ptO,dAlpha);
   m_ptB.Rotate(ptO,dAlpha);
   Setup();
}

// --------------------------------------------------------------------------

void Line2D::Swap()
{
   Point2D tmp(m_ptA);
   m_ptA = m_ptB;
   m_ptB = tmp;
   Setup();
}

// --------------------------------------------------------------------------

void Line2D::Set(const Point2D& ptA, const Point2D& ptB)
{
   m_ptA = ptA;
   m_ptB = ptB;
   Setup();
}

// --------------------------------------------------------------------------

void Line2D::Set(qreal dA, qreal dB, qreal dC)
{
   ASSERT(dA!=0 || dB!=0); // at least one must be nonzero
   a = dA;
   b = dB;
   c = dC;
   // we need some valid values for ptA and ptB as well
   if(fabs(b) > fabs(a)) {
      m_ptA = Point2D(0,GetY(0));
      m_ptB = Point2D(1,GetY(1));
   }
   else {
      m_ptA = Point2D(GetX(0),0);
      m_ptB = Point2D(GetX(1),1);
   }
   ASSERT(GetPosition(m_ptA)&posOnLine);
   ASSERT(GetPosition(m_ptB)&posOnLine);
}
// --------------------------------------------------------------------------

void Line2D::SetA (const Point2D& ptA)
{
   m_ptA = ptA;
   Setup();
}

void Line2D::SetB (const Point2D& ptB )
{
   m_ptB=ptB;
   Setup();
}
// --------------------------------------------------------------------------

qreal Line2D::GetAngle() const
{
   // b=Bx-Ax
   qreal fi = acos (b/GetLength());
   return -a > 0 ? fi : 2*M_PI-fi;
}

// --------------------------------------------------------------------------
/*! The solution comes strictly from the dot product.

   \retval qreal Angle in radians between this and liB line parameter.
 */

qreal Line2D::GetAngle(
   const Line2D& liB    //!< Other line.
) const
{
   // It comes strictly from dot product
   qreal fi = acos((a*liB.a + b*liB.b)/(GetLength()*liB.GetLength()));
   return (a*liB.b-liB.a*b) >= 0 ? fi : 2*M_PI - fi;
}

// --------------------------------------------------------------------------
/*! Calculates the point of intesection of this line with the line
    liB. The additional information is returned in the returned value.

    bit 0 (intExist) is set if intersection exists.
    bit 1 (intFirst) is set if the intersection lies on the first(this) line.
    bit 2 (intSecond) is set if the intersection lies on the second(liB) line.
 */

int Line2D::GetIntersection(
   const Line2D& liB,   //!< Second line.
   Point2D& X,          //!< The resulting point.
   qreal dTol          //!< Tolerance (default = 1E-12)
) const
{
   qreal det = a*liB.b-b*liB.a;
   if (tolerance_abs::Equal(det, 0.0, dTol)) {
      return 0;  // No intersection
   }
   else {
      int ret = intExist;
      X.SetX((b*liB.c-c*liB.b)/det);
      X.SetY((c*liB.a-a*liB.c)/det);

      qreal l_a = m_ptA.GetDistance(X);
      qreal l_b = m_ptB.GetDistance(X);
      ret |= tolerance_rel::Equal(l_a+l_b, GetLength(), dTol) ? intFirst : 0;

      l_a = liB.m_ptA.GetDistance(X);
      l_b = liB.m_ptB.GetDistance(X);
      ret |= tolerance_rel::Equal(l_a+l_b, liB.GetLength(), dTol) ? intSecond : 0;
      return ret;
   }
}
// --------------------------------------------------------------------------
/*! Calculate if lines (vectors) are colinear. We use the vector product.
    If vector product is zero, lines are parallel. Since we do not live in
    a perfect wold, we may relax zero to almost zero, where a tolerance
    from a perfect zero is allowed.
    \retval true lines are parallel to the required tolerance.
    \retval false lines are not parallel to the required tolerance.
*/
bool Line2D::IsParallel(
   const Line2D& liB, //!< The other line.
   qreal dTol        //!< Tolerance (default = 1E-12)
) const
{
   return tolerance_abs::Equal(a*liB.b-b*liB.a,0.0, dTol);
}

// --------------------------------------------------------------------------
/*! We seek a point on the line (not limited by the start and end
    points of the line) which is the neares to the given point t.
    We get this point if we construct a perpendicular line trough point
    t to this line. The intersetion of the lines is the solution.
    The solution is pretty simple. We follow two rules:

    \li The scalar product of the perpendicular line and this line is zero.
    \li The point must lie on the current line, hence ax + by + c must be
        zero.

    \param t Given point.

    \retval Point2D the nearest point to t, which lies on the line.

 */
Point2D Line2D::GetNearestPoint(const Point2D& t) const
{
   // The solution below is correct, but a bit difficult to understand.
   // We have a point t, from which we want to construct perpendicular line to
   // existing line. This is true if scalar product of this line and
   // new line is zero. For this reason we must find a point (x,y) that
   // 1: makes scalar product zero and
   // 2: is member of this line.
   // This line vector: (ax, ay) == (b,-a);
   // New line vector:  (t.x-x, t.y-y)
   // 1: (b,-a).(t.x-x, t.y-y) = 0
   // 2: a x + b y + c = 0
   // After arrangement:
   //
   // | b, -a | |x|   | b*t.x - a*t.y |
   // |       |*| | = |               |
   // | a,  b | |y|   | -c            |

   qreal cp = b*t.GetX() - a*t.GetY();
   qreal det = a*a+b*b;
   assert(det); // zero only if a and b are both zero.

   return Point2D((b*cp-c*a)/det, (-c*b-a*cp)/det);
}

// --------------------------------------------------------------------------
/*! Returns a line, which lies perpendicular to this line. The
    direction of perpendicular line goes from intersection (A) to the
    point it must go trough (B).

   \retval Line2D a line perpendicular to this line. The perpendiclar line
           starts at intersection and ends in given point ptB.

 */

Line2D Line2D::CreatePerpendicular(
   const Point2D& ptB   //!< Point trough which we construt the perpendicular line.
) const
{
   Point2D ptA = GetNearestPoint(ptB);
   // if p is on the line, we will get invalid line (a=b=0);
   return Line2D (ptA, ptB);
}

// --------------------------------------------------------------------------
/*! Unit direction (unit vector) which starts at relative point dRelS on the
    line and ends at a point which is one unit far from start point following
    relative direction dRelAlpha from current line direction.

    The method At defines the start point.
    The resulting angle is current angle plus dRelAlpha angle.
    The length of resulting line is one.

    \retval Line2D Unit line at specific position and relative direction.
 */

Line2D Line2D::CreateDirection(
   qreal dRelS,     //!< Defines the starting point (relative to the current line).
   qreal dRelAlpha  //!< Defines resulting angle relative to the current angle.
) const
{
   Point2D ptA = At(dRelS); // this is a start point.
   dRelAlpha += GetAngle(); // this is a new angle
   // calculate the second point ...
   Point2D ptB(cos(dRelAlpha), sin(dRelAlpha));
   ptB += ptA;
   return Line2D(ptA,ptB);
}

// --------------------------------------------------------------------------
/*! Check the position of point t. Is it on the line, on the left side,
    or maybe on the right side. A tolerance is used to calculate if
    the point pt is on the line or not.
    The return value can be a combination of several bits. See position flags.

    \retval unsigned int Combination of position bits.
 */

unsigned int Line2D::GetPosition(
   const Point2D& pt,  //!< Point under consideration.
   qreal dTol         //!< Tolerance.
) const
{
   // The point is on the line (or at least within the tolerance)
   if (tolerance_abs::Equal(a*pt.GetX() + b*pt.GetY() + c, 0.0, dTol)) {
      unsigned int uRet = posOnLine;      // it is definitely on the line

      qreal da = m_ptA.GetDistance(pt);  // distance from start point
      qreal db = m_ptB.GetDistance(pt);  // distance from end point
      if(pt==m_ptA) uRet |= posA;   // TODO: tolerance from ptA?
      if(pt==m_ptB) uRet |= posB;   // TODO: tolerance from ptB?

      if(tolerance_rel::Equal(da+db, GetLength()) && !(uRet&posEnd)) // is beween A and B
         uRet |= posInside;
      return uRet;
   }
   // The point is not on the line.
   else {
      Line2D tmp(m_ptA,pt);
      return (a*tmp.b-tmp.a*b > 0)? posPositive : posNegative;
   }
}

// --------------------------------------------------------------------------

Point2D Line2D::At(qreal dRelS) const
{
   qreal dx = b;  // m_ptB.GetX()-m_ptA.GetX();
   qreal dy = -a; // m_ptB.GetY()-m_ptA.GetY();
   return Point2D(m_ptA.GetX()+dx*dRelS, m_ptA.GetY()+dy*dRelS);
}

// --------------------------------------------------------------------------
#undef max

qreal Line2D::GetScalar(const Point2D& t, qreal dTol) const
{
   if (tolerance_abs::Equal(a*t.GetX()+b*t.GetY()+c, 0.0f, dTol)) {
      qreal dx = b;  // m_ptB.GetX()-m_ptA.GetX();
      qreal dy = -a; // m_ptB.GetY()-m_ptA.GetY();
      qreal dsx = t.GetX()-m_ptA.GetX();
      qreal dsy = t.GetY()-m_ptA.GetY();
      
      qreal l2 = a*a + b*b; 
      return (dx*dsx+dy*dsy)/(l2);
//      qreal l = GetLength();
//      return (dx*dsx+dy*dsy)/(l*l);
   }
   else
      return -std::numeric_limits<qreal>::max();
}

// --------------------------------------------------------------------------

void Line2D::Serialize(QDataStream& ar)
{
   /*int iVer =*/ SerializeVersion(ar, 1);

   m_ptA.Serialize(ar);
   m_ptB.Serialize(ar);

   if(IsStoring(ar)) {
      ar << a << b << c;
   }
   else {
      ar >> a >> b >> c;
   }
}

// --------------------------------------------------------------------------

void Line2D::Normalize()
{
   qreal l = GetLength();
   Point2D ptB(b/l,-a/l);
// qreal dAlpha = GetAngle();
// Point2D ptB(cos(dAlpha), sin(dAlpha));
   ptB += m_ptA;
   SetB(ptB);
}

// --------------------------------------------------------------------------

Line2D Line2D::GetOverlap(const Line2D& lnX) const
{
   Line2D lnOverlap;
   if(IsParallel(lnX) && GetPosition(lnX.GetA()) & posOnLine) {
      qreal xA = GetScalar(lnX.GetA());
      qreal xB = GetScalar(lnX.GetB());
      if(xA>xB) ::Swap(xA,xB);
      xA = GetMax((qreal)0.0, xA);
      xB = GetMin((qreal)1.0, xB);
      if(xA < xB) {
         lnOverlap.Set(At(xA), At(xB));
      }
   }
   return lnOverlap;
}

// --------------------------------------------------------------------------

#ifdef _DEBUG
void Line2D::Dump(CDumpContext& dc) const
{
   dc << "\nLine2D: a,b,c: "
      << a << " " << b << " " << c;
   dc << "\n   Point A:" << m_ptA.GetX() << "," << m_ptA.GetY();
   dc << "\n   Point B:" << m_ptB.GetX() << "," << m_ptB.GetY();
}
#endif

// ----------------------------------------------------------------------

} // namespace Geom
