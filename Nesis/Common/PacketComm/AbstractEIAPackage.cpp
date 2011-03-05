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

#include <QDebug>
#include "CRC-8.h"

#include "AbstractEIAPackage.h"

namespace packetcomm {
// ---------------------------------------------------------------------------

AbstractEIAPackage::AbstractEIAPackage()
{
}

AbstractEIAPackage::~AbstractEIAPackage(void)
{
}

// ---------------------------------------------------------------------------

bool AbstractEIAPackage::Validate(
	DQ::const_iterator b, //!< Points to the head.
	DQ::const_iterator e  //!< Points to the first uint8_t after tail
) const
{
	// check the first and last uint8_t
	if(*b != GetHeadByte() || *(e-1) != GetTailByte()) {
		qDebug("Invalid head or tail byte");
		return false;
	}

	// check the size
	if(*(b+1) != std::distance(b,e)) {
		// qDebug("Invalid package size.");
		return false;
	}

	// check the CRC
	DQ::const_iterator b1 = b;
	common::CRC8 crc;
	while(b1 != (e-2))
		crc.Update(*b1++);

	// if CRC does not match ...
	if(crc.Get() != *(e-2)) {
		QString qs;
		while(b<e)
			qs += QString::number(*b++, 16) + " ";
		qs += ": should be " + QString::number(crc.Get(), 16);
		qDebug() << "Invalid package CRC: " << qs;
		return false;
	}

	// seems OK
	return true;
}

// ---------------------------------------------------------------------------

void AbstractEIAPackage::Process(
	DQ::const_iterator b, //!< Points to the head.
	DQ::const_iterator e  //!< Points to the first uint8_t after tail
)
{
	ASSERT(*b==GetHeadByte());
	ASSERT(*(e-1)==GetTailByte());
	ASSERT(*(b+1) == std::distance(b,e));

	int iSize = std::distance(b,e);
	ASSERT(iSize <= (int)GetMaxPackageSize());

	std::advance(b,2);
	iSize -= 4;			// Size of the command in the package
	ASSERT(iSize > 0);

	// Make a copy of data in contiguous memory (deque is not contiguos).
	QByteArray baCmdData;
	baCmdData.reserve(iSize);
	for(DQ::const_iterator bb = b+1; bb!=e-2; bb++)
		baCmdData.append(*bb);
		
	ProcessFinal(*b, baCmdData);
}

// ---------------------------------------------------------------------------

QByteArray AbstractEIAPackage::Create(
	uint8_t byCmd, 				//!< Command uint8_t
	const uint8_t* pbyData,		//!< Data associated with the command.
	unsigned int uiSize		//!< Size of the data.
)
{
	ASSERT(5+uiSize <= EIA_MAX_PACKET_SIZE);

	QByteArray ba;

	ba.append(EIA_PACKET_HEAD_BYTE);		// head
	ba.append(5+(unsigned char)uiSize);		// size
	ba.append(byCmd);								// command

	for(unsigned int i=0; i<uiSize; i++)	// data, if any
		ba.append(pbyData[i]);

	common::CRC8 crc;
	for(int i=0; i<ba.size(); i++)
		crc.Update(ba[i]);
	ba.append(crc.Get());

	ba.append(EIA_PACKET_TAIL_BYTE);		// tail

	return ba;
}
// ---------------------------------------------------------------------------

QByteArray AbstractEIAPackage::Create(uint8_t byCmd)
{
	return Create(byCmd, NULL, 0);
}

// ---------------------------------------------------------------------------

QByteArray AbstractEIAPackage::Create(uint8_t byCmd, const QByteArray& baData)
{
	return Create(byCmd, (const uint8_t*)baData.constData(), baData.size());
}

// ---------------------------------------------------------------------------

QByteArray AbstractEIAPackage::Create(uint8_t byCmd, quint32 ui32Data)
{
	ASSERT(sizeof(quint32)==4);
	return Create(byCmd, (const uint8_t*)&ui32Data, sizeof(quint32));
}
// ---------------------------------------------------------------------------

QByteArray AbstractEIAPackage::Create(uint8_t byCmd, uint8_t byData)
{
	ASSERT(sizeof(uint8_t)==1);
	return Create(byCmd, &byData, sizeof(uint8_t));
}

// ---------------------------------------------------------------------------


} // namespace sensorboard
