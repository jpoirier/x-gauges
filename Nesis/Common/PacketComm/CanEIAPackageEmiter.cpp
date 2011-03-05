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
#include "PacketComm/packet.h"

#include "CanEIAPackageEmiter.h"


namespace packetcomm {
// ---------------------------------------------------------------------------

CanEIAPackageEmiter::CanEIAPackageEmiter(QObject* pParent)
: QObject(pParent)
{
}

CanEIAPackageEmiter::~CanEIAPackageEmiter()
{
}

// ---------------------------------------------------------------------------

void CanEIAPackageEmiter::ProcessFinal(
	uint8_t byCmd, 				//!< Command
	const QByteArray& baData	//!< Data (stripped)
)
{
	// Some debuging stuff
	static uint uiMsgCount = 0;
	static uint16_t uiLastCount = 0;
	static uint8_t uiTxWait = 0;
	static uint8_t uiRxWait = 0;

	PacketCAN* pktCan;

	switch (byCmd) {

	case EIA_CAN_MESSAGE:
      Q_ASSERT(sizeof(PacketCAN) == baData.size());
		pktCan = reinterpret_cast<PacketCAN*>(const_cast<char*>(baData.constData()));

		// Check the message counter.
		if(++uiLastCount != pktCan->m_uiCount) {
			qDebug() << "Packet missed.";
			uiLastCount = pktCan->m_uiCount;
		}

		// Check the transmit and receive situation
		if(pktCan->m_uiTxWait > uiTxWait) {
			uiTxWait = pktCan->m_uiTxWait;
			qDebug() << "Tx wait =" << uiTxWait;
		}
		if(pktCan->m_uiRxWait > uiRxWait) {
			uiRxWait = pktCan->m_uiRxWait;
			qDebug() << "Rx wait =" << uiRxWait;
		}

		if(uiMsgCount++%3000 == 0)
			uiRxWait = uiTxWait = 0;

		// Emit the received message.
		emit CanReceived(
			pktCan->m_uiIdentifier,
			pktCan->m_uiHeader,
			pktCan->m_uiData
		);
		break;

	case NESIS_CMD_PRINTF:
		std::cout << baData.constData() << std::flush;
		break;

	default:
		qDebug("CanEIAPackageEmiter: Unknown package 0x%x", byCmd);
	}
}

// ---------------------------------------------------------------------------


} // namespace
