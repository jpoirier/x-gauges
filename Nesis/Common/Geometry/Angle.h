#pragma once
#ifndef __GEOMETRY_ANGLE_H
#define __GEOMETRY_ANGLE_H

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

#include <QDataStream>

/*! \file Angle.h 
    \brief Declaration file for the Angle class.

    \author Ales Krajnc 
    \date   1.1.2006 - ported from VisualStudio code.
 */

// --------------------------------------------------------------------------

namespace geometry {

// -----------------------------------------------------------------------
//! Implements calculations related to the angles.
/** The goal of this class is to keep angles normalized in [0,2Pi)
    range. Additionaly, it performs some aritmetics on angles.
  */

class Angle
{
public:
   //! Constructor
   Angle(qreal a=0);
   //! Destructor
   ~Angle() {};
   
public:
   //! Returns the angle so the class may be used as a qreal
   operator qreal() const { return m_dA; }

   //! Assign qreal
   const Angle& operator=(qreal a)
   { m_dA = Normalize(a); return *this; }

   //! Difference of two angles. 
   static Angle GetDifference(const Angle& aAlfa, const Angle& aBeta, bool bCCW=true);

   //! Calculates the relative angle between two angles.
   static Angle GetRelative(qreal dRel, const Angle& aAlfa, const Angle& aBeta, bool bCCW=true);
   
   //! Normalizes the angle in the [0,2Pi) range.
   static qreal Normalize(qreal dA);

protected:
   //! Angle - guarded in the range.
   qreal m_dA;
};

// -----------------------------------------------------------------------

inline QDataStream& operator<<(QDataStream& ds, const Angle& a)
{
   ds << (qreal)a;
   return ds;
}

inline QDataStream& operator>>(QDataStream& ds, Angle& a)
{
   qreal dA;
   ds >> dA; a = dA;
   return ds;
}

// -----------------------------------------------------------------------

} // namespace geometry

#endif //__GEOMETRY_ANGLE_H
