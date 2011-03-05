#ifndef ROUTE_NAVAIDS_H
#define ROUTE_NAVAIDS_H
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

/** \brief Radio navigation aids info.
  */
	
class NavAid : public Waypoint
{
	//! Type of the Radio Navigation Aid.
	enum TypeNA {
		tUnknown,		//!< 0
		tVORDME,			//!< 1
		tNDB				//!< 2
	};
public:
	//! Default constructor - invalid and empty turning point.
	NavAid();
	//! Copy constructor.
	NavAid(const NavAid& C);
	//! Constructor from QString object.
 	NavAid(const QString& qsLine);
	//! Destructor
	virtual ~NavAid();

	//! Clone the object, derived classes as well.
	virtual Waypoint* Clone() const
	{ return new NavAid(*this); }
	
	//! Save the data into given binary stream.
	virtual void Write(QDataStream& out) const;
	//! Load the data from given binary stream.
	virtual void Read(QDataStream& in);
	
	//! Draw the symbol.
	void Draw(QPainter& P, const map::Mapper& M) const;
	
protected:
	//! NavAid type.
	TypeNA m_eTypeNA;
	//! The VFR point Refers to strings.
	float m_fFrequency;
};

// -------------------------------------------------------------------------
} // namespace

#endif
