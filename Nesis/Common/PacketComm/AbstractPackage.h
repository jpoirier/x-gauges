#ifndef __ABSTRACT_PACKAGE_H
#define __ABSTRACT_PACKAGE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

#include <deque>
#include "CommonDefs.h"

namespace packetcomm {

// --------------------------------------------------------------------------
//! Abstract package class to be used with SerialAbstractPackage class.
/*! This class defines the basic requirements needed by the AbstractPackageProcessor
    class to be able to identify and validate a package inside a stream
    ob bytes. Derived classes must implement the details.
 */

class AbstractPackage
{
protected:
	//! Shorthand for double ended queue used to store incoming bytes.
	typedef std::deque<uint8_t> DQ;
public:
	//! Constructor.
	AbstractPackage() {};
	//! Destructor.
	virtual ~AbstractPackage(void) {};

	//! Head uint8_t of a package.
	virtual uint8_t GetHeadByte() const = 0;
	//! Tail uint8_t of a package.
	virtual uint8_t GetTailByte() const = 0;
	//! Maximal size of a package.
	virtual unsigned int  GetMaxPackageSize() const = 0;
	//! Validate a package candidate.
	virtual bool          Validate(DQ::const_iterator b, DQ::const_iterator e) const = 0;
	//! Process a package (which was previously successfully validated).
	virtual void          Process(DQ::const_iterator b, DQ::const_iterator e)  = 0;
};

// ---------------------------------------------------------------------------
} // namespace

#endif
