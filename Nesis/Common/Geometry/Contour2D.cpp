/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

/*! \file Contour2D.cpp 
    \brief Implementaion file for the Contour2D class.

     Here we just barely scratch the surface. The real job
     is done in gpc.c (or gpc.cpp).

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

#include "Contour2D.h"

// --------------------------------------------------------------------------

namespace geometry {

using namespace gpc;

// -----------------------------------------------------------------------

Contour2D::Contour2D(void)
{
   m_gpcPoly.num_contours = 0;
   m_gpcPoly.hole         = NULL;
   m_gpcPoly.contour      = NULL;
}

Contour2D::Contour2D(const Polygon2D& p)
{
   m_gpcPoly.num_contours = 0;
   m_gpcPoly.hole         = NULL;
   m_gpcPoly.contour      = NULL;
   Add(p);
}

Contour2D::~Contour2D(void)
{
   gpc_free_polygon(&m_gpcPoly);
}

// -----------------------------------------------------------------------
/*! A new polygon (contour) is added to current set of contours.
    Vertices from polygon are copied into #m_gpcPoly object and
    all other bookkeeping is done. 
 */
void Contour2D::Add(
   const Polygon2D& p, //!< polygon that enters the m_gpcPoly.
   bool bHole          //!< must be true if polygon p is a hole.
)
{
   // Get vertex list from Polygon2D. We own the list here
   gpc_vertex_list vl = p.GetGPCVertexList();

   // Current number of contours in the gpc polygon.
   const int iNC = m_gpcPoly.num_contours;
   
   // we have a new vertex list (contour) that must be added to the polygon

   // let's do the holes first
   m_gpcPoly.hole = static_cast<int*>
      (realloc(m_gpcPoly.hole,sizeof(int)*(iNC+1)));
   ASSERT(m_gpcPoly.hole);
   m_gpcPoly.hole[iNC] = bHole;

   // add a contour
   m_gpcPoly.contour = static_cast<gpc_vertex_list*>
      (realloc(m_gpcPoly.contour,sizeof(gpc_vertex_list)*(iNC+1)));
   ASSERT(m_gpcPoly.contour);
   m_gpcPoly.contour[iNC] = vl;

   m_gpcPoly.num_contours++;
}

// -----------------------------------------------------------------------

unsigned int Contour2D::GetHoleCount() const
{
   unsigned int iHoles = 0;
   for(int i=0; i<m_gpcPoly.num_contours; i++)
      if(m_gpcPoly.hole[i])
         iHoles++;
   return iHoles;
}

// -----------------------------------------------------------------------
/*! Contures in this object are clipped with contours in cB object using
    eCO clipping operation. Result is stored on a heap and returned 
    as pointer to newly created Contour2D object.

    \retval Contour2D* newly created object - result of clipping operation.
 */
Contour2D* Contour2D::Clip(
   ClipOperation eCO,   //!< Type of the clipping, see #ClipOperation. 
   const Contour2D& cB  //!< The clipping contour.
) const
{
   const Contour2D& cA = *this;
   // result
   Contour2D* pcR = new Contour2D;

   gpc_polygon_clip(
      static_cast<gpc_op>(eCO), 
      const_cast<gpc_polygon*>(&cA.m_gpcPoly), 
      const_cast<gpc_polygon*>(&cB.m_gpcPoly), 
      &pcR->m_gpcPoly
   );

   return pcR;
}

// -----------------------------------------------------------------------
} // namespace Geom
