#ifndef __PACKAGE_PROCESSOR_H
#define __PACKAGE_PROCESSOR_H

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

/*! \file PackageProcessor.h
    \brief Implementation of package processing for package based communication.

    \author  Ales Krajnc 
    \date   
 */

#include <memory>
#include <deque>

#include "CommonDefs.h"
#include "AbstractProcessor.h"
#include "AbstractPackage.h"

namespace packetcomm {
// -------------------------------------------------------------------------

//! Implementation of serial package based communication.
/*! Although the class is not abstract, it is very generic and is based on
    the Package derived objects. Together with the rules of the Package object,
    the class listens to the incoming data until a complite package arrives. 
	 Once a package is complete, it is processed by the package object.

    A package must have a typical head uint8_t, a typical tail uint8_t, limit length,
    and means to validate and process the package. See the class Package for
    more details. 
 */

class PackageProcessor : public AbstractProcessor
{
	//! DQ is double ended queue of bytes.
	typedef std::deque<uint8_t> DQ;
public:
	//! Constructor
   PackageProcessor();
	//! Destructor
   virtual ~PackageProcessor();

	//! Set the package type. We own the passing \a pPackage object if \a bAutoDelete is true. 
	void SetPackage(AbstractPackage* pPackage, bool bAutoDelete = true);

public:
	//! Implementation of reading process required by the abstract base class.
	virtual void Process(
		const unsigned char* pucBuff, //!< Buffer with the data received. 
		int iCount							//!< Number of bytes in the buffer.
	);
private:
	//! Process bytes in the buffer and try to locate the packages.
	bool ProcessBuffer();

#ifdef QT_NO_DEBUG_OUTPUT
	//! Send all bytes that were not recognised as commands to the debug output.
	void Dump(DQ::const_iterator b, DQ::const_iterator e) const
	{ }
#else
	void Dump(DQ::const_iterator b, DQ::const_iterator e) const;
#endif

private:
	//! Incomming buffer of bytes.
	DQ m_dqBuffer;
	//! Package processing and validating object.
	std::auto_ptr<AbstractPackage> m_apPackage;
	//! Should we auto delete the package object.
	bool m_bAutoDelete;
};
// ---------------------------------------------------------------------------

} // namespace
#endif
