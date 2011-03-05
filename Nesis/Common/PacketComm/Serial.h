#ifndef PACKETCOMM_SERIAL_H
#define PACKETCOMM_SERIAL_H

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

/*! \file serial.h
    \brief Qt based wrapper for asynchronous reading of serial port.

    \author  Ales Krajnc
    \date
 */
// ------------------------------------------------------------------------

#include <string>
#include <QThread>
#include <QMutex>

#include <termios.h>

#include "AbstractProcessor.h"	// To handle reading from stream.
#include "AbstractPort.h"

// TODO: Parity and stop bits ...
namespace packetcomm {

// ------------------------------------------------------------------------

//! Baudrate enumeration - See also /usr/include/bits/termios.h
enum Baudrate {
	bHangUp    = B0,
	baud0      = B0,
	// ignoring B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800
	baud2400   = B2400,
	baud4800   = B4800, //	GPS - NMEA uses this speed.
	baud9600   = B9600,
	baud19200  = B19200,
	baud38400  = B38400,
	baud57600  = B57600,
	baud115200 = B115200,
	baud230400 = B230400,
// XXX:
//	baud460800 = B460800,
	// ignoring B500000, B576000, B921600, B1000000, B1152000,
	// B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
};

class Serial : public AbstractPort, public QThread
{
public:
	//! Constructor.
	Serial(QObject* pParent=0);
	//! Constructor takes the opening parameters.
	Serial(const QString& qsPort, Baudrate eBaud, QObject* pParent=0);
	//! Constructor for non standard baud rates.
	Serial(const QString& qsPort, uint uiBaudrate, QObject* pParent=0);
	//! Destructor;
   virtual ~Serial();

public:
	//! Open the port using given name.
	virtual bool Open();
	//! Close the port.
	virtual void Close();

	//! Write block of bytes to serial port buffer. Loop if necessary.
	virtual bool Write(const char* pcData, int iSize);

	//! Is the port opened?
	virtual bool IsOpen() const
	{ return m_bOpen; }

	//! Set serial port name.
	void SetPort(const QString& qsPort)
	{ m_qsPort = qsPort; }

	//! Get port serial name.
	const QString& GetPort() const
	{ return m_qsPort; }

	//! Set nonstandard baudrate.
	void SetBaudrate(uint uiBaudrate)
	{
		m_uiBaudrate = uiBaudrate;
		m_eBaudrate = baud38400;
	}

	//! Set standard baudrate.
	void SetBaudrate(Baudrate eBaudrate)
	{
		m_eBaudrate = eBaudrate;
		m_uiBaudrate = 0;
	}

	//! Set standard baudrate.
	void SetResetOnOpen(bool bReset)
	{
		m_bResetOnOpen = bReset;
	}


protected:
	//! The thread is runing here.
	virtual void run();

	//! Start the thread and reading process.
	bool Start();

	//! Get the handle. Use the handle with care.
	int GetHandle() const
	{ return m_iHandle; }

private:
	//! Port name.
	QString m_qsPort;
	//! Baudrate
	Baudrate m_eBaudrate;
	//! Not standard baud rate.
	uint m_uiBaudrate;
	//! The port handle.
	int m_iHandle;
	//! Is port opened.
	bool m_bOpen;

	bool m_bResetOnOpen;

	//! This object may be used in different threads and the mutex prevents write collision.
	QMutex m_muxWrite;
};

// --------------------------------------------------------------------------

} // namespace

#endif // PACKETCOMM_SERIAL_H
