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

/*! \file
    \brief

    \author  Ales Krajnc
    \date
 */

#include <algorithm>
#include <QObject>
#include "PackageProcessor.h"

namespace packetcomm {

// ---------------------------------------------------------------------------

PackageProcessor::PackageProcessor()
{
	m_bAutoDelete = false;
}


PackageProcessor::~PackageProcessor()
{
	if(!m_bAutoDelete)
		m_apPackage.release();
}

// ---------------------------------------------------------------------------

void PackageProcessor::SetPackage(
	AbstractPackage* pPackage,
	bool bAutoDelete
)
{
	if(!m_bAutoDelete)
		m_apPackage.release();
	m_apPackage = std::auto_ptr<AbstractPackage>(pPackage);
	m_bAutoDelete = bAutoDelete;
}

// ---------------------------------------------------------------------------

void PackageProcessor::Process(
	const unsigned char* pucBuffer, //!< Buffer with the data received.
	int iCount							//!< Number of bytes in the buffer.
)
{
	// read all the data from the port
	std::copy(pucBuffer, pucBuffer+iCount, std::back_inserter(m_dqBuffer));

	// Process the data in the buffer (deque).
	// Repeat the call as long as the ProcessBuffer keeps returning true.
	while(ProcessBuffer()){;}
}

// ---------------------------------------------------------------------------

bool PackageProcessor::ProcessBuffer()
{
	// end marker
	const DQ::iterator e = m_dqBuffer.end();
	// working iterators
	DQ::iterator b, f;

	// qDebug("DQ size = %i", m_dqBuffer.size());

	// find first head byte.
	b = m_dqBuffer.begin();
	f = std::find(b, e, m_apPackage->GetHeadByte());

	// If the first byte is not found, the whole content can be ignored
	if(f==e) {
		Dump(m_dqBuffer.begin(), m_dqBuffer.end());
		m_dqBuffer.clear();
		return false; // stop processing
	}

	// OK, the first byte was found.
	// First dump all bytes before the first byte, if any
	if(f!=b) {
		Dump(b,f);
		b = m_dqBuffer.erase(b,f);
	}
	// Search for the first tail byte
	f = std::find(b, e, m_apPackage->GetTailByte());

	// not found
	if(f==e) {
		// keep the buffer, maybe the package is not complete, yet.
		return false; // stop processing
	}

	// we have head byte and tail byte, but the package may not be the right one
	while(f<e) {
		f++;
		// The tail is also OK.
		// Give the package chance to validate itself
		if(m_apPackage->Validate(b,f) == true) {
// 			qDebug("Package received");
			// Bingo, we have a package
			m_apPackage->Process(b,f);
			m_dqBuffer.erase(b,f); // erase the package from the buffer
			return true; // try once more
		}
		// well, we have a problem. Maybe there is another tail byte after the one we found
		else {
			// try again
			f = std::find(f, e, m_apPackage->GetTailByte());
		}
	}

	// Well, at least one tail byte was found, but the package is still invalid.
	if(m_dqBuffer.size() < m_apPackage->GetMaxPackageSize())
		return false; // keep this head and wait for more bytes to come from serial port.

	// TODO: Problem!! Serial port stop sending the bytes, the first byte is wrong, but the next
	// one is a good one. We can't reach this one using this algorhithm.

	// We were waiting long enough - remove the first byte and try again
	Dump(b, b+1);
	m_dqBuffer.pop_front();
	return true; // try once, but now locate another head
}

// --------------------------------------------------------------------------

#ifndef QT_NO_DEBUG_OUTPUT
void PackageProcessor::Dump(DQ::const_iterator b, DQ::const_iterator e) const
{
	while(b<e) {
		std::cerr << *b;
		b++;
	}
}
#endif

} // namespace
