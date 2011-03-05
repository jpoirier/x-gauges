#pragma once
#ifndef __CONTOUR2D_H
#define __CONTOUR2D_H

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

/*! \file Contour2D.h 
    \brief Declaration file for the Contour2D class.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

#include "Polygon2D.h"
#include "gpc.h"

// --------------------------------------------------------------------------

namespace geometry {

//! The class encapsulates gpc "C" structures and methods.
/*! gpc means Generic Polygon Clipper. 
    See http://www.cs.man.ac.uk/~toby/alan/software/ for more details about
    GPC. Since gpc uses several defines, structs and functions, we have
    put it into gpc namespace.

    The Contour2D class is build above the gpc::gpc_polygon and provides
    the same clipping operations as gpc does. 
 */

class Contour2D
{
public:
   //! The clipping operation used in the clip command
   enum ClipOperation {
      cDifference   = gpc::GPC_DIFF, //!< Difference (this LESS clip)
      cIntersection = gpc::GPC_INT,  //!< Intersection (this AND clip)
      cXOR          = gpc::GPC_XOR,  //!< Exclusice OR (this XOR clip)
      cUnion        = gpc::GPC_UNION //!< Union (this OR clip)
   };

public:
   //! Default constructor
   Contour2D(void);
   //! Copy constructor
   Contour2D(const Polygon2D& p);
   //! Destructor
   virtual ~Contour2D(void);

   //! Adds a Polygon2D structure into polygon contour set.
   void Add(const Polygon2D& p, bool bHole=false);

   //! Returns number of contours.
   unsigned int GetCount() const
   { return m_gpcPoly.num_contours; }

   //! Retuns number of holes.
   unsigned int GetHoleCount() const;

   //! Assures const access to a contour iC.
   const gpc::gpc_vertex_list GetContour(unsigned int iC) const
   { ASSERT(iC < GetCount()); return m_gpcPoly.contour[iC]; }

   //! Tells if contour iC is a hole or not.
   bool IsHole(unsigned int iC) const
   { ASSERT(iC < GetCount()); return m_gpcPoly.hole[iC]!=0; }

   //! The clip operation - this is the heart of the Contour2D object.
   Contour2D* Clip(ClipOperation eCO, const Contour2D& cB) const;

protected:
   //! The gpc struct used in gpc opreations
   gpc::gpc_polygon m_gpcPoly;
};

} // namespace Geom

#endif // __CONTOUR2D_H
