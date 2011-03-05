#ifndef COMMON_CRC_8_H
#define COMMON_CRC_8_H

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

namespace common {

/** \brief Simple CRC-8 checksum table based calculation.

	 This class is intended for CRC calculation of short messages.
 */

class CRC8
{
public:
	//! Constructor initilizes the code.
	CRC8()
	{ Reset(); }

	//! Resets the CRC object and prepares it for another CRC run.
	void Reset()
	{ m_byCrc = 0; }

	//! Update the CRC with given byte.
	void Update(unsigned char ucByte)
  	{ m_byCrc = s_auiTable[m_byCrc ^ ucByte]; }

  	//! Update the CRC with given number of bytes.
  	void Update(unsigned char* pucByte, int iN)
  	{ while(iN--) Update(*pucByte++); }

	//! Get the current CRC value.
	unsigned char Get() const
	{ return m_byCrc; }

private:
	//! The crc value.
	unsigned char m_byCrc;

	//! Built in table for a fast lookup.
	static const unsigned char s_auiTable[256];
};

} // namespace common

#endif // __CRC_8_H
