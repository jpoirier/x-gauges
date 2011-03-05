#ifndef __PACKETCOMM_ABSTRACT_PORT_H
#define __PACKETCOMM_ABSTRACT_PORT_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

/*! \file AbstractPort.h
    \brief Abstract base class for different ports used in packet communication.

    \author  Ales Krajnc
 */
// ------------------------------------------------------------------------

#include <memory>
#include <QByteArray>

#include "AbstractProcessor.h"	// To handle reading from stream.

// ------------------------------------------------------------------------

namespace packetcomm {

// ------------------------------------------------------------------------

class AbstractPort
{
protected:
	//! Constructor is protected
   AbstractPort();
public:
	//! Destructor
	virtual ~AbstractPort();

//public:
	//! Start the port communication.
	virtual bool Open() = 0;
	//! Stop the port communication.
	virtual void Close()
	{;}

	//! Is the port opened?
	virtual bool IsOpen() const
	{ return false; };

	//! Write block of bytes to serial port buffer. Loop if necessary.
	virtual bool Write(const char* pcData, int iSize) = 0;

	//! Write an array of bytes.
	bool Write(const QByteArray& ba)
	{ return Write(ba.constData(), ba.size()); }

	//! Write just one byte. Return false if the write fails.
 	bool Write(unsigned char uc)
 	{ return Write((const char*)&uc, 1); }

	//! Write just one byte. Return false if the write fails.
 	bool Write(char ch)
 	{ return Write(&ch, 1); }

	//! Set the processor object. We own the passing \a pProcessor object.
	void SetProcessor(AbstractProcessor* pProcessor, bool bAutoDelete=true);

	//! Get number of downloaded bytes from processor.
	int GetDownloadCount()
	{ return m_apProcessor->GetCount(); }

protected:
	bool m_bAutoDelete;
	//! Processor which gets the incoming stream of data to process.
	std::auto_ptr<AbstractProcessor> m_apProcessor;
};

// --------------------------------------------------------------------------

} // namespace

#endif // __PACKETCOMM_ABSTRACT_PORT_H
