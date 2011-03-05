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

/*! \file Polygon2D.cpp 
    \brief Implementation file for the Polygon2D class.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

#include <iterator>
#include <limits>
#include <algorithm>
#include "CommonDefsQt.h"
#include "Polygon2D.h"

using namespace std;
using namespace common;
namespace geometry {

// ----------------------------------------------------------------------

// Static variables
unsigned int Polygon2D::s_auIndex[5] = { 0,1,3,6,10 };
qreal Polygon2D::s_adWeight[RES_SIZE] = { 
   -2,                        // 0
   -6, 6,                     // 1
   -12, -24, 12,              // 2
   -20, -60, 60, 20,          // 3
   -30, -120, -180, 120, 30   // 4
};

// ----------------------------------------------------------------------

Polygon2D::Polygon2D()
{
   m_vecV.reserve(20);
   m_uDegree = 4;
   m_bDirty = true;
}

Polygon2D::Polygon2D(const QPolygonF& pg)
{
	QVector<QPointF>::const_iterator b = pg.begin();
	QVector<QPointF>::const_iterator e = pg.end();
	if(*b==*(e-1) && std::distance(b,e)>3) 
		e--;
	
	m_vecV.assign(b,e);
	
	m_uDegree = 4;
	m_bDirty = true;
}

Polygon2D::Polygon2D(const QRectF& rc)
{
   m_vecV.reserve(4);
   m_vecV.push_back(rc.topLeft());
   m_vecV.push_back(rc.bottomLeft());
   m_vecV.push_back(rc.bottomRight());
   m_vecV.push_back(rc.topRight());
	m_uDegree = 4;
	m_bDirty = true;
}

Polygon2D::Polygon2D(VecVertex::const_iterator b, VecVertex::const_iterator e)
{
   if(*b==*(e-1) && std::distance(b,e)>3) 
      e--;
   m_vecV.assign(b,e);
   m_uDegree = 4;
   m_bDirty = true;
}

Polygon2D::Polygon2D(const Polygon2D& C)
{
   Copy(C);
}

// ----------------------------------------------------------------------

Polygon2D::~Polygon2D()
{
}

// ----------------------------------------------------------------------

void Polygon2D::Copy(const Polygon2D& C)
{
   // m_vecV;
   m_vecV.clear();
   m_vecV.reserve(C.m_vecV.capacity());
   copy(C.m_vecV.begin(), C.m_vecV.end(), back_inserter(m_vecV));

   // other
   m_bDirty = C.m_bDirty; 
   m_uDegree = C.m_uDegree;

   // results
   for(int i=0; i<RES_SIZE; i++) {
      m_adResult[i] = C.m_adResult[i];
   }
}

// ----------------------------------------------------------------------

void Polygon2D::SetDegree (unsigned int d)
{
   Q_ASSERT(d <= 4);
   if(d>m_uDegree) {
      m_bDirty = true;
   }
   m_uDegree = d;
}

// ----------------------------------------------------------------------

void Polygon2D::Move(qreal dx, qreal dy)
{
   VecVertex::iterator i;
   for(i=m_vecV.begin(); i!= m_vecV.end(); i++) {
      i->Move(Point2D(dx,dy));
   }
   m_bDirty = true;
}

// ----------------------------------------------------------------------

void Polygon2D::Rotate(qreal dAlpha)
{
   VecVertex::iterator i;
   for(i=m_vecV.begin(); i!= m_vecV.end(); i++) {
      i->Rotate(dAlpha);
   }
   m_bDirty = true;
}

// ----------------------------------------------------------------------

void Polygon2D::Rotate(const Point2D& p, qreal dAlpha)
{
   VecVertex::iterator i;
   for(i=m_vecV.begin(); i!= m_vecV.end(); i++) {
      i->Rotate(p,dAlpha);
   }
   m_bDirty = true;
}

// ----------------------------------------------------------------------
/*! The divider line lineSplit splits this polygon into two separate polygons:
    a) the positive polygon, which lies on the positive side of the lineSplit
       (left of the lineSplit if observed from above - in negative z direction),
    b) the negative polygon.

    If the lineSplit does not cross the polygon, one of the polygons will be empty.
    Each new polygon will have at least three vertices (points).

    \retval int number of polygon and line intersections.
 */
 
int Polygon2D::Split(
   const Line2D& lineSplit, //!< The spliting line. 
   Polygon2D& plgPos,       //!< Resulting positive polygon object.
   Polygon2D& plgNeg) const //!< Resulting negative polygon object.
{
   Q_ASSERT(m_vecV.size()>=3);

   int iCounter = 0;
   const int iSize = m_vecV.size();

   plgPos.Clear(); // make sure that the polygon on positive side is empty.
   plgNeg.Clear(); // the same is true for the negative side.
   plgPos.SetDegree(m_uDegree); // They should have the same degree.
   plgNeg.SetDegree(m_uDegree);

   Point2D t;
   for (int i=0; i<iSize; i++ )  {
      const Point2D& A = m_vecV[i];                      // start point
      const Point2D& B = m_vecV[(i==iSize-1 ? 0 : i+1)]; // end point of current line
      Line2D line (A,B);                                 // current line
      Point2D intPt;                                     // intersection point

      // Intersect the current line with the lineSplit. 
      // Test the returned flag!
      int flag = line.GetIntersection (lineSplit, intPt);

      // Intersection exist and it lies inside the first line (line).
      if (flag & Line2D::intFirst) {
         // The intersection is truly inside the line, not on the edges A or B
         if (intPt!=A && intPt!=B) {
            plgPos.Append(intPt);        // This means that the intesection point
            plgNeg.Append(intPt);        // is a part of both polygons.
            iCounter++;
            continue;
         }
      }

      // Check the end point of the line
      // The end point (B) could be exactly on the split line
      flag = lineSplit.GetPosition(B);

      // Yes it is
      if(flag & Line2D::posOnLine) {
         plgPos.Append(B);        // This means that the intesection point
         plgNeg.Append(B);        // is a part of both polygons.
         iCounter++;
      }
      // is it on the positive side
      else if(flag & Line2D::posPositive) {
         plgPos.Append(B);
      }
      // then it is on the negative side
      else { 
         plgNeg.Append(B);
      }
   } // for (i .. )

   if ( plgPos.GetCount() < 3 ) {
      plgPos.Clear();
   }
   if ( plgNeg.GetCount() < 3 ) {
      plgNeg.Clear();
   }
   return iCounter;
}

// ----------------------------------------------------------------------
/*! In most cases simply returns previously calculated value. If the
    #m_bDirty flag is true, recalculates all integrals up to the requred 
    degree. See also #SetDegree.
 */
qreal Polygon2D::CalcIntegral (
   unsigned int a, //!< degree of x (x^a)
   unsigned int b  //!< degree of y (y^b)
) const
{
   Q_ASSERT(a+b <= GetDegree());
   if(m_bDirty)
      const_cast<Polygon2D*>(this)->Calculate();
   return m_adResult[Index(a,b)];
}

// ----------------------------------------------------------------------

qreal Polygon2D::GetCircumference() const
{
   qreal dL = 0;
   for(unsigned int i=0; i<GetCount(); i++)
      dL += GetLine(i).GetLength();
   return dL;
}

// ----------------------------------------------------------------------
/*!  It sets m_bDirty flag to true and new calculation will be performed
     when reqired.
 */

void Polygon2D::Append(
   const Point2D& p     //!< New point.
)
{
   m_vecV.push_back(p);
   m_bDirty = true;
}

// ----------------------------------------------------------------------
/*! It sets m_bDirty flag to true and new calculation will be performed
    when reqired. If invalid position is given, the new point will be 
    appened. See #Append.
 */

void Polygon2D::Insert(
   unsigned int uPos, //!< Insert position
   const Point2D& p
)
{
   if(uPos < GetCount()) {
      m_vecV.insert(m_vecV.begin()+uPos, p);
      m_bDirty = true;
   }
   else {
      Append(p);
   }
}

// ----------------------------------------------------------------------
/*! It sets m_bDirty flag to true and new calculation will be performed
    when reqired. If invalid position is given the call is ignored.
 */
void Polygon2D::Remove(
   unsigned int uPos //!< Position of the point to be removed.
)
{
   if(uPos<m_vecV.size()) {
      m_vecV.erase(m_vecV.begin()+uPos);
      m_bDirty = true;
   }
}

// ----------------------------------------------------------------------

void Polygon2D::Clear()
{
   m_vecV.clear();
   m_bDirty = true;
   for(int i=0; i<RES_SIZE; m_adResult[i++] = 0.0) ;
}

// ----------------------------------------------------------------------
/*! We construct a horizontal line that starts at point pt and goes into any
    direction. Then we count how many times the line crosses the
    polygon. Odd number of crossings means that the pt is inside
    of polygon.

    \retval true pt is inside the polygon.
    \retval false pt is outside the polygon. 

 */
bool Polygon2D::IsInside(
   const Point2D& pt, //!< point to be tested.
   bool /*bBorder*/       //!< true if polygon border is inside the polygon.
) const
{
	// (1) http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
	const qreal y = pt.GetY();
	const qreal x = pt.GetX();

	const int npol = GetCount()+1;
	int i, j;
	bool bInside = false;

	for (i = 0, j = npol-1; i < npol; j = i++) {
		qreal ypi = m_vecV[i].GetY();
		qreal ypj = m_vecV[j].GetY();
		qreal xpi = m_vecV[i].GetX();
		qreal xpj = m_vecV[j].GetX();

		if (((ypi <= y) && (ypj > y)) || ((ypj <= y) && (ypi > y)))
			if((x < (xpj - xpi) * (y - ypi) / (ypj - ypi) + xpi))
				bInside = !bInside;
   }
	return bInside;
}

// ----------------------------------------------------------------------

void Polygon2D::Write(QDataStream& out) const
{
   /* byte byVer = */ 
	SerializeVersion(out, 1);

	out << (qint32) m_vecV.capacity(); // capacity of the vector
	out << (qint32) m_vecV.size();     // number of vertices
 
   // vertices
	for(unsigned int i=0; i<m_vecV.size(); i++)
		out << m_vecV[i];

   // results
	for(int i=0; i<RES_SIZE; i++)
		out << (double)m_adResult[i];

   // flags
	out << m_bDirty;
	out << (quint32)m_uDegree;
} 

// ----------------------------------------------------------------------

void Polygon2D::Read(QDataStream& in)
{
	double d;
   SerializeVersion(in, 1);

   qint32 qi;
   in >> qi; m_vecV.reserve(qi); // capacity
   in >> qi; m_vecV.resize(qi);

   // vertices
   for(int i=0; i<qi; i++)
   	in >> m_vecV[i];
      
   // results
   for(int i=0; i<RES_SIZE; i++) {
   	in >> d;
   	m_adResult[i] = d;
   }

   // flags
   in >> m_bDirty;
   in >> (quint32&)m_uDegree;
}

// ----------------------------------------------------------------------
/*! All integrals to given degree are evaluated in one single call 
    and results are stored in the cache #m_adResult.
 */ 

void Polygon2D::Calculate()
{
   Q_ASSERT(m_vecV.size()>=3);

   const unsigned int size = m_vecV.size();
   m_bDirty = false;

   // powers will be stored here. We derived our expresions in 
   // z's and y's so we use them rather than x's and y's.
   qreal ya[5], yb[5], za[5], zb[5];
   ya[0] = yb[0] = za[0] = zb[0] = 1.0;

   // clear up results
   for(int i=0; i<RES_SIZE; i++)
      m_adResult[i] = 0.0;

   // the main loop starts ...
   for(unsigned i=0; i<size; i++) {
      const Point2D& A = m_vecV[i];                     // start point
      const Point2D& B = m_vecV[(i==size-1 ? 0 : i+1)]; // end point of current line

      qreal Ya = A.GetX();
      qreal Yb = B.GetX();
      qreal Za = A.GetY();
      qreal Zb = B.GetY();

      // We also need higher powers
      for(int j=1; j<5; j++) {
         ya[j] = ya[j-1] * Ya;
         yb[j] = yb[j-1] * Yb;
         za[j] = za[j-1] * Za;
         zb[j] = zb[j-1] * Zb;
      }

      // this helps a bit
      qreal dy = Ya-Yb;
      qreal dz = Za-Zb;

      // The zero degree ...
      m_adResult[0] += (Ya + Yb) * dz;

      // The first degree ...
      if(m_uDegree>=1) {
         m_adResult[1] += (ya[2] + Ya*Yb + yb[2]) * dz;
         m_adResult[2] += (za[2] + Za*Zb + zb[2]) * dy;
      }

      // The second degree ...
      if(m_uDegree>=2) {
         m_adResult[3] += (Ya+Yb) * (ya[2]+yb[2]) * dz;
         m_adResult[4] += dz * (2*Ya*Yb*(Za+Zb) + ya[2]*(3*Za+Zb) + yb[2]*(Za+3*Zb));
         m_adResult[5] += (Za+Zb) * (za[2]+zb[2]) * dy;
      }

      // The third degree ...
      if(m_uDegree>=3) {
         m_adResult[6] += (ya[4] + ya[3]*Yb + ya[2]*yb[2] + Ya*yb[3] + yb[4]) * dz;
         m_adResult[7] += dz * (ya[3]*(4*Za+Zb) 
                             + ya[2]*Yb*(3*Za+2*Zb)
                             + Ya*yb[2]*(2*Za+3*Zb)
                             + yb[3]*(Za+4*Zb));
         m_adResult[8] += dy * (za[3]*(4*Ya+Yb) 
                             + za[2]*Zb*(3*Ya+2*Yb)
                             + Za*zb[2]*(2*Ya+3*Yb) 
                             + zb[3]*(Ya+4*Yb));
         m_adResult[9] += (za[4] + za[3]*Zb + za[2]*zb[2] + Za*zb[3] + zb[4]) * dy;
      }

      // The fourth degree ...
      if(m_uDegree==4) {
         m_adResult[10]+= (Ya+Yb)*(ya[2]-Ya*Yb+yb[2])*(ya[2]+Ya*Yb+yb[2]) * dz;
         m_adResult[11]+= dz * (ya[4]*(5*Za+Zb) 
                             + 2*ya[3]*Yb*(2*Za+Zb) 
                             + 3*ya[2]*yb[2]*(Za+Zb) 
                             + 2*Ya*yb[3]*(Za+2*Zb) 
                             + yb[4]*(Za+5*Zb));
         m_adResult[12]+= dz * ((10*ya[3]+6*ya[2]*Yb+3*Ya*yb[2]+yb[3]) * za[2]
                             + 2*(Ya+Yb)*(2*ya[2]+Ya*Yb+2*yb[2])*Za*Zb
                             + (ya[3]+3*ya[2]*Yb+6*Ya*yb[2]+10*yb[3]) * zb[2]);
         m_adResult[13]+= dy * (za[4]*(5*Ya+Yb) 
                             + 2*za[3]*Zb*(2*Ya+Yb) 
                             + 3*za[2]*zb[2]*(Ya+Yb) 
                             + 2*Za*zb[3]*(Ya+2*Yb) 
                             + zb[4]*(Ya+5*Yb));
         m_adResult[14]+= (Za+Zb)*(za[2]-Za*Zb+zb[2])*(za[2]+Za*Zb+zb[2]) * dy;
      }
   }

   // we must divide the results with the constants ...
   for(unsigned int i=0; i<RES_SIZE; i++)
      m_adResult[i] /= s_adWeight[i];
}

// ----------------------------------------------------------------------
/*! Find a minimal rectangle that encapsulates the polygon. The
    rectangle is defined by ptMin and ptMax Point2D objects.
 */
void Polygon2D::GetSurroundingRectangle(
   Point2D& ptMin,   //!< Result - stores minimal x and y value. 
   Point2D& ptMax    //!< Result . stores maximal x and y value.
) const
{
   if(m_vecV.size()) {
      VecVertex::const_iterator i=m_vecV.begin();
      ptMin = ptMax = *i;
      // loop over remaining points
      for(i++; i!=m_vecV.end(); i++) {
         ptMin.SetX(GetMin(ptMin.GetX(), i->GetX()));
         ptMin.SetY(GetMin(ptMin.GetY(), i->GetY()));
         ptMax.SetX(GetMax(ptMax.GetX(), i->GetX()));
         ptMax.SetY(GetMax(ptMax.GetY(), i->GetY()));
      }
   }
}

// ----------------------------------------------------------------------
/*! Find a minimal rectangle that encapsulates the polygon. The
    rectangle is returned in the QRectF object.
 */
QRectF Polygon2D::GetSurroundingRectangle() const
{
	QRectF rc;

   if(m_vecV.size()) {
		Point2D pt1, pt2;
		GetSurroundingRectangle(pt1, pt2);
		rc.setCoords(
			pt1.GetX(), pt2.GetY(),	// left, top 
			pt2.GetX(), pt1.GetY()  // right, bottom.
		);
   }

	return rc.normalized();
}

// ----------------------------------------------------------------------
/*!
    \retval Line2D i-th segment of the polygon. 
 */
Line2D Polygon2D::GetLine(
   unsigned int i //!< index of the requested segment.
) const
{
   Q_ASSERT(i<m_vecV.size());
   return Line2D(
      m_vecV[i],                             // start point
      m_vecV[(i==m_vecV.size()-1 ? 0 : i+1)] // end point of segment line
   ); 
}

// ----------------------------------------------------------------------

Polygon2D::Orientation Polygon2D::GetOrientation() const
{
   // We calculate the area and see ...
   qreal dArea = CalcIntegral(0,0);

   // if the area is positive
   if(tolerance_abs::GreaterThan(dArea,0.0,1E-14)) {
      return oCCW; // counterclockwise
   }
   else if(tolerance_abs::LowerThan(dArea,0.0,1E-14)) {
      return oCW;  // clockwise
   }
   return oInvalid;
}
// ----------------------------------------------------------------------

void Polygon2D::SwitchOrientation()
{
   VecVertex vecNew;
   vecNew.reserve(GetCount());
   VecVertex::reverse_iterator ri;

   for(ri=m_vecV.rbegin(); ri!=m_vecV.rend(); ri++) {
      vecNew.push_back(*ri);
   }
   Clear();
   m_vecV = vecNew;
}
// ----------------------------------------------------------------------
/*! For the oCCW (positive) polygon orientation a positive dOff will 
    inflate the polygon. For the oCW (negative) polygon orientation 
    a positive dOff will shrink the polygon.
 */
void Polygon2D::Offset(
   qreal dOff //!< Offset value.
)
{
   VecVertex vecNew;
   vecNew.reserve(GetCount());

   // Move each point
   for(unsigned int i=0; i<GetCount(); i++) {
      Line2D lnA = GetLine(i==0 ? GetCount()-1 : i-1);
      Line2D lnB = GetLine(i);

      // Offset both lines
      lnA.Offset(-dOff);
      lnB.Offset(-dOff);
      // Intersection gives the new point;
      Point2D ptI;
      unsigned int uStatus = lnA.GetIntersection(lnB, ptI);
      if(uStatus & Line2D::intExist) {
         vecNew.push_back(ptI);
      }
      // lnA and lnB are parallel
      else {
         vecNew.push_back(lnA.GetB());
      }
   }
   Clear();
   m_vecV = vecNew;
}
// ----------------------------------------------------------------------
/*! Search for the vertex (point) index in the polygon.
    \retval -1 if such vertex is not found.
    \retval int position of vertex in the polygon.
 */
int Polygon2D::FindVertex(const Point2D& pt) const
{
   int iRet = -1; // not found
   VecVertex::const_iterator f;
   f = std::find(m_vecV.begin(), m_vecV.end(), pt);
   if(f!=m_vecV.end())
      iRet = std::distance(m_vecV.begin(), f);

   return iRet;
}

// ----------------------------------------------------------------------

bool Polygon2D::IsTwisted() const
{
   // We check for the line intersections
   for(unsigned int i=0; i<GetCount()-2; i++) {
      Line2D ln1 = GetLine(i);
      for(unsigned int j=i+2; j<GetCount(); j++) {
         // special case for the i=0
         if(i==0 && j+1==GetCount())
            continue;
         // get the second line and verify the intersection
         Line2D ln2 = GetLine(j);
         // check the intersection
         Point2D ptD;
         if(ln1.GetIntersection(ln2, ptD)==Line2D::intBoth)
            return true;
      }
   }
   return false;
}
// ----------------------------------------------------------------------
/*! First all intersections are calculated. Then duplicates are removed.
    If only points on the line are required, all points that are outside
    the lnX[A,B] are removed. If bParallel is true, parallel segmets are
    checked for overlap and overlap points are included in the solution.
    
    \retval VecVertex container of intersection points. 
 */
QVector<Point2D> Polygon2D::GetIntersection(
   const Line2D& lnX, //!< line that intersects the polygon
   bool bOnLineOnly,  //!< if true, only points between A and B inclusive are counted.
   bool bParallel     //!< check parallel lines for overlap
) const
{
   // results are stored here
   QVector<Point2D> vecInt;

   // for each segment
   for(unsigned int i=0; i<GetCount(); i++) {
      Line2D ln = GetLine(i);
      Q_ASSERT(ln.IsValid());
      Point2D ptInt;
      // Check overlaps, if they are wanted
      if(bParallel && ln.IsParallel(lnX)) {
         Line2D lnO = ln.GetOverlap(lnX);
         if(lnO.IsValid()) {
            vecInt.push_back(lnO.GetA());
            vecInt.push_back(lnO.GetB());
         }
      }

      // check intersections
      if(ln.GetIntersection(lnX, ptInt) & Line2D::intFirst){
         vecInt.push_back(ptInt);
      }
   }

   // Sort the points in the vector. Same point may appear several
   // times.
   Point2DSortLineLess pred(lnX);
   std::sort(vecInt.begin(),vecInt.end(),pred);
   // remove duplicates, if any
   QVector<Point2D>::iterator f = std::unique(vecInt.begin(),vecInt.end());
   vecInt.erase(f,vecInt.end());

   // remove all points that are not inside the line
   if(bOnLineOnly) {
      QVector<Point2D>::iterator i=vecInt.begin();
      while(i!=vecInt.end()) {
         if(!(lnX.GetPosition(*i) & Line2D::posInsideEnd))
            i = vecInt.erase(i);
         else
            i++;
      }
   }

   return vecInt;
}
// ----------------------------------------------------------------------
/*! Distances from the line and vertices are considered. In the case of
    line, perpendicuar distance is used in the case of vertex, radial
    distance is used.  
    \retval qreal minimal distance to the polygon border.
 */
qreal Polygon2D::GetDistance(
   const Point2D& pt //!< distance to this point is calculated.
) const
{
   qreal dD = std::numeric_limits<qreal>::max();
   
   for(unsigned int i=0; i<GetCount(); i++) {
      dD = GetMin(dD, pt.GetDistance(GetVertex(i)));
      Line2D ln = GetLine(i);
      Point2D ptN = ln.GetNearestPoint(pt);
      if(ln.GetPosition(ptN) & Line2D::posInside) 
         dD = GetMin(dD, ptN.GetDistance(pt));
   }
   return dD;
}

// ----------------------------------------------------------------------
/*! For the current polygon a vertex list used by gpc is constructed.
    The caller gets the list and is responsible to free the memory allocated
    in this method. 
    \retval gpc::gpc_vertex_list a list of vertices in the form requred by gpc.
 */
gpc::gpc_vertex_list Polygon2D::GetGPCVertexList() const
{
   // the object
   gpc::gpc_vertex_list vl;
   
   // number of vertices in the polygon
   vl.num_vertices = GetCount();
   
   // reserve the memory required by gpc_vertex_list.
   vl.vertex = static_cast<gpc::gpc_vertex*>
      (malloc(sizeof(gpc::gpc_vertex)*vl.num_vertices));
   Q_ASSERT(vl.vertex);

   // should be always true as well
   if(vl.vertex) {
      // copy vertices
      for(unsigned int i=0; i<GetCount(); i++) {
         vl.vertex[i].x = m_vecV[i].GetX();
         vl.vertex[i].y = m_vecV[i].GetY();
      }
   }

   return vl;
}

// ----------------------------------------------------------------------

void Polygon2D::Copy(const gpc::gpc_vertex_list VL)
{
   Clear();

   if (VL.num_vertices > 0) {
      m_vecV.reserve(VL.num_vertices+2);
      for (int v = 0; v < VL.num_vertices; v++)
         Append(Point2D(VL.vertex[v].x, VL.vertex[v].y));
    }
}

// ----------------------------------------------------------------------

Point2D Polygon2D::GetMassPoint() const
{
   Q_ASSERT(GetDegree()>=1);
   qreal dA  = CalcIntegral(0,0);
   qreal dSx = CalcIntegral(1,0);
   qreal dSy = CalcIntegral(0,1);
   return Point2D(dSx/dA, dSy/dA);
}

// ----------------------------------------------------------------------
/*! Polygon pg is inside this polygon, if there is no intersection 
    between polygons and all vetices of pg are inside this polygon.
 */
bool Polygon2D::IsInside(
	const Polygon2D& pg, 
	bool bBorder,
	qreal dTol			 //!< Tolerance settings.
) const
{
   // No intersection
   if(AnyIntersection(pg, bBorder, dTol)==false) {
      // since there is no intersection, it is enough to test for any
      // point. If any, it could be back as well.
      return IsInside(pg.GetBack(), bBorder);
   }
   // There is an intersection
   return false;
}

// ----------------------------------------------------------------------
/*! Polygon pg is outside this polygon, if there is no intersection 
    between polygons and all vetices of pg are outside this polygon.
 */
bool Polygon2D::IsOutside(
	const Polygon2D& pg, 
	bool bBorder,
	qreal dTol			 //!< Tolerance settings.
) const
{
   // No intersection
   if(AnyIntersection(pg, bBorder, dTol)==false) {
      // since there is no intersection, it is enough to test for any
      // point. If any, it could be back as well.
      return IsInside(pg.GetBack(), bBorder)==false;
   }
   // There is an intersection
   return false;
}

// ----------------------------------------------------------------------

bool Polygon2D::AnyIntersection(
	const Polygon2D& pg, 
	bool bBorder,
	qreal dTol			 //!< Tolerance settings.
) const
{
   for(unsigned int i=0; i<GetCount(); i++) {
      Line2D lnA = GetLine(i);
      for(unsigned int j=0; j<pg.GetCount(); j++) {
         Line2D lnB = pg.GetLine(j);

         // Check the intersection
         Point2D X;
         int iFlag = lnA.GetIntersection(lnB, X, dTol);
         if(iFlag == Line2D::intBoth) {
            // if border is part of the polygon, then there is no doubt
            // the intersection exists (it may only touch)
            if(bBorder)
               return true;
            // if the border is not part of the polygon, then we must
            // check if lines only touches
            else {
               if(lnA.GetPosition(X, dTol) & Line2D::posEnd ||
                  lnB.GetPosition(X, dTol) & Line2D::posEnd) {
                  // they only touch, this is not intersection
                  ; // do nothing
               }
               else {
                  return true;
               }
            }
         }
      }
   }
   return false;
}

// ----------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const Polygon2D& p)
{
   out << "\nNumber of vertices = " << p.GetCount();
   for(unsigned int i=0; i<p.GetCount(); i++) {
      out << "\n" << i << ": " << p.GetVertex(i);
   }
   return out;
}

// ----------------------------------------------------------------------
} // namespace Geom
