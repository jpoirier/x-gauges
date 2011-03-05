#ifndef PACKETCOMM_ABSTRACT_EIA_PACKAGE_H
#define PACKETCOMM_ABSTRACT_EIA_PACKAGE_H

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

#include "PacketComm/AbstractPackage.h"
#include "PacketComm/NesisProtocol.h"
#include <QByteArray>

// ------------------------------------------------------------------------

namespace packetcomm {

// ------------------------------------------------------------------------

//! Process packages received from the uC.
class AbstractEIAPackage : public AbstractPackage
{
public:
	//! Constructor
	AbstractEIAPackage();
	//! Destructor
	virtual ~AbstractEIAPackage(void);

	//! Return package head uint8_t.
	virtual unsigned char GetHeadByte()	const
	{ return EIA_PACKET_HEAD_BYTE; }
	//! Return package tail uint8_t.
	virtual unsigned char GetTailByte() const
	{ return EIA_PACKET_TAIL_BYTE; }
	//! Return maximal package size.
	virtual unsigned int GetMaxPackageSize() const
	{ return EIA_MAX_PACKET_SIZE; }

	//! Validate the package. Return true if b and e define a true package.
	virtual bool Validate(DQ::const_iterator b, DQ::const_iterator e) const;
	//! Process the package - do something with the package.
	virtual void Process(DQ::const_iterator b, DQ::const_iterator e);

	// static functions

	//! Create package for transmition.
	static QByteArray Create(
		uint8_t byCmd,
		const uint8_t*	pbyData,
		quint32 uiSize
	);

	//! Create package for transmition.
	static QByteArray Create(uint8_t byCmd);
	//! Create package for transmition, sending data in array.
	static QByteArray Create(uint8_t byCmd, const QByteArray& baData);
	//! Create package for transmition, sending data as 32 bit unsigned integer.
	static QByteArray Create(uint8_t byCmd, quint32 ui32Data);
	//! Create package for transmition, sending data as one uint8_t.
	static QByteArray Create(uint8_t byCmd, uint8_t byData);

protected:
	//! Do something with the stripped package data.
	virtual void ProcessFinal(uint8_t byCmd, const QByteArray& baData) = 0;
};

// ---------------------------------------------------------------------------
} // namespace
#endif
