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

/*! \file Serial.cpp
    \brief Implementation of asynchronous reading from serial port.

    \author  Ales Krajnc
    \date
 */
// --------------------------------------------------------------------------

#include <QDebug>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
// XXX:
//#include <linux/serial.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <iostream>

#include "CommonDefs.h"
#include "Serial.h"

namespace packetcomm {


// ---------------------------------------------------------------------------

Serial::Serial(QObject* pParent)
: QThread(pParent)
{
	m_eBaudrate  = baud0;
	m_uiBaudrate = 0;
	m_iHandle    = 0;
	m_bOpen      = false;
	m_bResetOnOpen = false;
}

Serial::Serial(const QString& qsPort, Baudrate eBaud, QObject* pParent)
: QThread(pParent)
{
	m_qsPort     = qsPort;
	m_eBaudrate  = eBaud;
	m_uiBaudrate = 0;
	m_iHandle    = 0; // Set invalid value initially.
	m_bOpen      = false;
	m_bResetOnOpen = false;
}


Serial::Serial(const QString& qsPort, uint uiBaudrate, QObject* pParent)
: QThread(pParent)
{
	m_qsPort     = qsPort;
	m_uiBaudrate = uiBaudrate;
	m_eBaudrate  = baud38400;
	m_iHandle    = 0; // Set invalid value initially.
	m_bOpen      = false;
	m_bResetOnOpen = false;
}

Serial::~Serial()
{
	if(IsOpen())
		Close();
/*	if(isRunning())
		terminate();*/
}

// ---------------------------------------------------------------------------

bool Serial::Open()
{
	// Open port in blocking mode - the thread will be blocked
	// if no bytes are ready to receive.
	m_iHandle = ::open(
		m_qsPort.toAscii().constData(),
		O_RDWR |      // read-write access
		O_NOCTTY      // don't be the "controlling terminal" on input
	);					  // (ignore keyboard abort signals and so forth)

	if (m_iHandle < 0){
		qDebug("Can't open serial port %s\n",
			m_qsPort.toAscii().constData()
		);
		return false;
	}

	// general terminal interface - control asynchronous communications ports
	termios tio;
	if(tcgetattr(m_iHandle, &tio) < 0) {
		qDebug("Can't read attributes for serial port %s\n",
			m_qsPort.toAscii().constData()
		);
		return false;
	}

	// Control modes
	tio.c_cflag =
		m_eBaudrate  | // baud rate
		CS8    |       // 8 bit
		CLOCAL |       // Local line - do not change "owner" of port
		CREAD;         // enable receiver

	// Input modes
	tio.c_iflag = IGNPAR; // Ignore framing errors and parity errors.

	// Output modes
	tio.c_oflag = 0;

	// local flags
	tio.c_lflag = 0;

	// Control characters
	tio.c_cc[VMIN]=1;  // Minimum number of characters for non-canonical read.
	tio.c_cc[VTIME]=0; // Timeout in deciseconds for non-canonical read.

	if (m_uiBaudrate != 0) {
// XXX:
//		struct serial_struct ss;

// XXX:
//		ioctl(m_iHandle, TIOCGSERIAL, &ss);

// XXX:
//		ss.custom_divisor = ss.baud_base / m_uiBaudrate;
// XXX:
//		ss.flags &= ~ASYNC_SPD_MASK;
//		ss.flags |= ASYNC_SPD_MASK & ASYNC_SPD_CUST;

// XXX:
//		qDebug("Non standard baud rate %d, baud base = %d, divisor = %d",
//			m_uiBaudrate, ss.baud_base, ss.custom_divisor);

// XXX:
//		ioctl(m_iHandle, TIOCSSERIAL, &ss);
	}

	tcflush(m_iHandle, TCIOFLUSH);
	tcsetattr(m_iHandle,TCSANOW, &tio);

// contol DTR and RTS [rok] -----------------
	if (m_bResetOnOpen) {
		int iStatus;

		if (ioctl(m_iHandle, TIOCMGET, &iStatus) != 0)
			qDebug("ioctl get failed");

	// 	iStatus |=  TIOCM_DTR;
		iStatus &= ~TIOCM_DTR;

	// 	iStatus |=  TIOCM_RTS;
		iStatus &= ~TIOCM_RTS;

		if (ioctl(m_iHandle, TIOCMSET, &iStatus) != 0)
			qDebug("ioctl set failed");
	}
//-------------------------------------

	m_bOpen = true;

	return Start();
}

// ---------------------------------------------------------------------------

void Serial::Close()
{
// 	struct serial_struct ss;

// 	ioctl(m_iHandle, TIOCGSERIAL, &ss);
// 	ss.flags &= ~ASYNC_SPD_MASK;
// 	ioctl(m_iHandle, TIOCSSERIAL, &ss);

	if (isRunning()) {
		m_bOpen = false;
// 		::alarm(1);
		if(wait(300)==false) {
			terminate();
			wait(300);
		}
	}

	::close(m_iHandle);
	m_iHandle = 0;
}

// ---------------------------------------------------------------------------

bool Serial::Write(const char* pcData,  int iSize)
{
	// qDebug() << "Serial" << (void*)pcData;
	m_muxWrite.lock();
	int iCount = 0;
	do {
		iCount += ::write(GetHandle(), pcData+iCount, iSize-iCount);
	} while(iCount < iSize);
	m_muxWrite.unlock();
	return true;
}

// ---------------------------------------------------------------------------
/*! You need to close the port to terminate the thread.
 */
void Serial::run()
{
	// qDebug("\nEntering Serial::ThreadRead::run()");
	const int icBufSize = 1023;
	unsigned char aucData[icBufSize+1];
	int iCount;
	// fcntl(m_pSerial->GetHandle(), F_SETFL, FNDELAY);
	// read all the data from the port
	ASSERT(m_apProcessor.get() != NULL);

	do {
		// This is a blocking call.
		iCount = ::read(GetHandle(), aucData, icBufSize);
		if(iCount > 0) {
			// std::cout << "X" << std::endl;
			//for(int j=0; j<iCount; j++)
			//	std::cout << " " << (int)aucData[j] << std::flush;

			m_apProcessor->Load(aucData, iCount);
		}
		else {
			std::cerr << "Closing..." << std::endl;
		}
	} while(IsOpen());
	qDebug() << "Exiting Serial::ThreadRead::run()" << m_qsPort;
}

// ---------------------------------------------------------------------------

bool Serial::Start()
{
	// Start the thread if not already running.
	if(isRunning()==false) {
		start(/*QThread::InheritPriority*/);
		return true;
	}
	// The thread is already running. Can't open two threads for one port.
	return false;
}

// ---------------------------------------------------------------------------

} // namespace
