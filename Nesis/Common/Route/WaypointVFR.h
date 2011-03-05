#ifndef ROUTE_WAYPOINT_VFR_H
#define ROUTE_WAYPOINT_VFR_H
/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QStringList>
#include "Waypoint.h"

namespace route {

/** \brief VFR Waypoint - Waypoint extension with the VFR reporting point info.
  */
	
class WaypointVFR : public Waypoint
{
	enum TypeVFR {
		tNonCompulsory,			//!< 0
		tCompulsory,				//!< 1
		tNonCompulsoryHeli,		//!< 2
		tCompulsoryHeli			//!< 3
	};
public:
	//! Default constructor - invalid and empty turning point.
	WaypointVFR();
	//! Copy constructor.
	WaypointVFR(const WaypointVFR& C);
	//! Constructor from QString object.
 	WaypointVFR(const QString& qsLine);
	//! Destructor
	virtual ~WaypointVFR();

	//! Clone the object, derived classes as well.
	virtual Waypoint* Clone() const
	{ return new WaypointVFR(*this); }
	
	//! Save the data into given binary stream.
	virtual void Write(QDataStream& out) const;
	//! Load the data from given binary stream.
	virtual void Read(QDataStream& in);
	
	//! Draw the symbol.
	void Draw(QPainter& P, const map::Mapper& M) const;
	
protected:
	//! VFR Point type.
	TypeVFR m_eTypeVFR;
	//! The VFR point Refers to strings.
	QStringList m_slRefersTo;
};

// -------------------------------------------------------------------------
} // namespace

#endif
