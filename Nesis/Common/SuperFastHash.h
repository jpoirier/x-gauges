#ifndef SUPER_FAST_HASH_H
#define SUPER_FAST_HASH_H

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
 *   Modified for the use in C++ using original code Paul Hsieh.           *
 *                                                                         *
 * By Paul Hsieh (C) 2004, 2005.  Covered under the Paul Hsieh derivative  *
 * license. See:                                                           *
 * http://www.azillionmonkeys.com/qed/weblicense.html for license details. *
 * http://www.azillionmonkeys.com/qed/m_uiHash.html                            *
 *                                                                         *
 ***************************************************************************/

#include <stdint.h>

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

namespace common {

class SuperFastHash
{
public:
	//! Constructor initilizes the code.
	SuperFastHash() 
	{ Reset(); }

	//! Resets the object and prepares it for another run.
	void Reset() { m_uiHash = 0; }

	//! Update the m_uiHash with given byte. Try to avoid this call and rather use the block version.
	void Update(unsigned char ucByte)
	{ Update(&ucByte, 1); }

	//! Update the m_uiHash with given block of bytes.
	void Update(const unsigned char *pbyData, int iLen);

	//! Get the current CRC value.
	uint32_t Get() const
	{ return m_uiHash; }

private:
	//! The m_uiHash value.
	uint32_t m_uiHash;
};

// ------------------------------------------------------------------------------------

/** Implementation of the SuperFastHash block update. This is just s slight 
    modification of original code developed by mr. Paul Hsieh.
 */

inline void SuperFastHash::Update(const unsigned char *pbyData, int iLen) 
{
	Q_ASSERT(iLen > 0 && pbyData != NULL);

	uint32_t uiTmp;
	int iRem = iLen & 3;
	iLen >>= 2;

	/* Main loop */
	for (;iLen > 0; iLen--) {
		m_uiHash  += get16bits (pbyData);
		uiTmp    = (get16bits (pbyData+2) << 11) ^ m_uiHash;
		m_uiHash   = (m_uiHash << 16) ^ uiTmp;
		pbyData  += 2*sizeof (uint16_t);
		m_uiHash  += m_uiHash >> 11;
	}

		/* Handle end cases */
	switch (iRem) {
		case 3:	m_uiHash += get16bits (pbyData);
				m_uiHash ^= m_uiHash << 16;
				m_uiHash ^= pbyData[sizeof (uint16_t)] << 18;
				m_uiHash += m_uiHash >> 11;
				break;
		case 2:	m_uiHash += get16bits (pbyData);
				m_uiHash ^= m_uiHash << 11;
				m_uiHash += m_uiHash >> 17;
				break;
		case 1: m_uiHash += *pbyData;
				m_uiHash ^= m_uiHash << 10;
				m_uiHash += m_uiHash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	m_uiHash ^= m_uiHash << 3;
	m_uiHash += m_uiHash >> 5;
	m_uiHash ^= m_uiHash << 4;
	m_uiHash += m_uiHash >> 17;
	m_uiHash ^= m_uiHash << 25;
	m_uiHash += m_uiHash >> 6;
}

} // namespace common

#endif // __SUPER_FAST_HASH_H
