#ifndef ABSTRACT_READ_PROCESSOR_H
#define ABSTRACT_READ_PROCESSOR_H

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

/*! \file AbstractProcessor.h
    \brief Abstract implementation of reading and processing of data stream.

    \author  Ales Krajnc
    \date
 */

// --------------------------------------------------------------------------
namespace packetcomm {

/** Abstract class for processing stream of data from ports of serial type.

    Such types are serial port (asynchronous), TCP connection,
	 UDP connection ...

	 This class simply defines one and only pure virtual function, which
	 is called by serial reading classed when data is read and ready to
	 be processed.
 */

class AbstractProcessor
{
public:
	//! Constructor
   AbstractProcessor() {;}
	//! Destructor
   virtual ~AbstractProcessor() {;}

	void Load(const unsigned char* pucBuff, int iCount)
	{
		m_iDataCount += iCount;
		Process(pucBuff, iCount);
	}

	//! Pure virtual request data processing.
	virtual void Process(
		const unsigned char* pucBuff, //!< Buffer with the data received.
		int iCount							//!< Number of bytes in the buffer.
	) = 0;

	int GetCount()
	{ return m_iDataCount; }

private:
	//! How many bytes we have received.
	int m_iDataCount;
};

// --------------------------------------------------------------------------
/** \brief A simple processor which works with serial port in write only mode.
 */

class WriteOnlyProcessor : public AbstractProcessor
{
public:
	//! Constructor
	WriteOnlyProcessor() {;}
	//! Destructor
	~WriteOnlyProcessor() {;}

	//! Throw everthing away.
	void Process(
		const unsigned char* , //!< Buffer with the data received.
		int 							//!< Number of bytes in the buffer.
	) {;}
};

// --------------------------------------------------------------------------
} // namespace

#endif // __ABSTRACT_READ_PROCESSOR_H
