#ifndef COMMON_CRC_32_H
#define COMMON_CRC_32_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <stdint.h>

namespace common {

class CRC32
{
public:
	//! Constructor initilizes the code.
	CRC32() 
	{ Reset(); }

	//! Resets the CRC object and prepares it for another CRC run.
	void Reset()
	{ m_uiCrc = 0xFFFFFFF; }

	//! Update the CRC with given byte.
	void Update(unsigned char ucByte)
	{ m_uiCrc = ((m_uiCrc>>8) & 0x00FFFFFF) ^ c_auiTable[(m_uiCrc^ucByte) & 0xFF ]; }
	
  	//! Update the CRC with given number of bytes.
  	void Update(unsigned char* pucByte, int iN)
  	{ while(iN--) Update(*pucByte++); }

	//! Get the current CRC value.
	uint32_t Get() const
	{ return (m_uiCrc & 0xFFFFFFFF); }

private:
	//! The crc value.
	uint32_t m_uiCrc;

	//! Built in table for a fast lookup.
	static const uint32_t c_auiTable[256];
};

} // namespace common

#endif // __CRC_32_H
