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

/*! \file Point2D.cpp  
    \brief Implementation file for the Point2D class.

    \author Ales Krajnc 
    \date 1.1.2006 - Ported from the VisualStudio code.
 */ 
// ----------------------------------------------------------------------

#include "MathEx.h"
#include "CommonDefsQt.h"
#include "Point2D.h"

// ----------------------------------------------------------------------

using namespace common;
namespace geometry {

// ----------------------------------------------------------------------

Point2D Point2D::operator-(const Point2D& p) const
{
   Point2D x(*this);
   x-=p;
   return x;
}

Point2D Point2D::operator+(const Point2D& p) const
{
   Point2D x(*this);
   x+=p;
   return x;
}

// ----------------------------------------------------------------------
/*! Serializes the data to the stream in both directions: 
    storing and reading.
 */

void Point2D::Serialize(
   QDataStream& ds) //!< The stream object.
{
   /*int iVer = */SerializeVersion(ds, 1);
   if(IsStoring(ds)) {
      ds << (double)m_dX << (double)m_dY;
   }
   else {
   	// Load via double is needed due to qreal.
		double d;
      ds >> d; m_dX = d;
      ds >> d; m_dY = d; 
   }
}

// ----------------------------------------------------------------------
/*! Calculates the distance between this point and the given point p. 
 */

qreal Point2D::GetDistance(
   const Point2D& p) //!< The given point.
   const
{
   qreal dx = p.m_dX-m_dX;
   qreal dy = p.m_dY-m_dY;
   return sqrt(dx*dx + dy*dy);
}

// ----------------------------------------------------------------------

qreal Point2D::GetAngle(const Point2D& p ) const
{
   return common::ATan2Safe(p.m_dY-m_dY, p.m_dX-m_dX);
}

// ----------------------------------------------------------------------

void Point2D::Rotate (qreal dAlpha)
{
   if (!dAlpha) return;
   qreal CA=cos(dAlpha);
   qreal SA=sin(dAlpha);
   qreal xx = m_dX;

   m_dX = xx*CA - m_dY*SA;
   m_dY = xx*SA + m_dY*CA;
}

// ----------------------------------------------------------------------

void Point2D::Rotate(const Point2D& p, qreal dAlpha)
{
   m_dX -= p.m_dX;
   m_dY -= p.m_dY;
   Rotate(dAlpha);
   m_dX += p.m_dX;
   m_dY += p.m_dY;
}

// ----------------------------------------------------------------------

// void Point2D::SaveXML(QDomNode& nParent)
// {
//    QDomDocument doc = nParent.ownerDocument();
//    DomElement el(doc.createElement("Point2D"));
// 
//    el.SetAttrInt("ver", 1);
//    el.SetAttrDouble("x", m_dX);
//    el.SetAttrDouble("y", m_dY);
// 
//    nParent.appendChild(el);
// }
// ----------------------------------------------------------------------

// void Point2D::LoadXML(QDomElement& elem)
// {
//    DomElement el(elem);
//    int iVer = el.GetAttrInt("ver");
//    m_dX = el.GetAttrDouble("x");
//    m_dY = el.GetAttrDouble("y");
// }

// ----------------------------------------------------------------------
} // namespace geometry
