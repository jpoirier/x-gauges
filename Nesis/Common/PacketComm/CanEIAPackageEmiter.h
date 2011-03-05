#ifndef PACKETCOMM_CAN_EIA_PACKAGE_EMITER_H
#define PACKETCOMM_CAN_EIA_PACKAGE_EMITER_H

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

#include <QObject>
#include "PacketComm/AbstractEIAPackage.h"

namespace packetcomm {

// ------------------------------------------------------------------------

/** \brief Process Can packages received from uC and emit signal.

	When a can package is received from serial port, we emit a signal
	which distributes the package to the interested parties.

	Any console packages are printed on the std::cout.
 */
class CanEIAPackageEmiter : public QObject, public AbstractEIAPackage
{
Q_OBJECT
public:
	//! Constructor
	CanEIAPackageEmiter(QObject* pParent);
	//! Destructor
	virtual ~CanEIAPackageEmiter();

signals:
	//! CAN message received.
	void CanReceived(uint uiId, uint uirA, uint uirB);

protected:
	//! Display console and emit can messages.
	virtual void ProcessFinal(uint8_t byCmd, const QByteArray& baData);
};

// ---------------------------------------------------------------------------
} // namespace
#endif
