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

/*! \file Mabu.cpp
    \brief Implementation of asynchronous reading from Mabu port.

    \author  Ales Krajnc
    \date
 */
// --------------------------------------------------------------------------

#include <QDebug>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <iostream>

#include "CommonDefs.h"

#include "Mabu.h"

namespace packetcomm {


// ---------------------------------------------------------------------------

Mabu::Mabu(QObject* pParent)
: QThread(pParent)
{
	m_iHandle    = 0;
	m_bOpen      = false;
}

Mabu::Mabu(const QString& qsPort, QObject* pParent)
: QThread(pParent)
{
	m_qsPort     = qsPort;
	m_iHandle    = 0; // Set invalid value initially.
	m_bOpen      = false;
}


Mabu::~Mabu()
{
// 	if(IsOpen())
// 		Close();
// /*	if(isRunning())
// 		terminate();*/
}

// ---------------------------------------------------------------------------

bool Mabu::Open()
{
	// Open port in blocking mode - the thread will be blocked
	// if no bytes are ready to receive.
	m_iHandle = ::open(
		m_qsPort.toAscii().constData(),
		O_RDWR     // read-write access
	);

	if (m_iHandle < 0) {
		qDebug("Can't open Mabu port %s", m_qsPort.toAscii().constData());
		return false;
	}

	m_bOpen = true;

	return Start();
}

// ---------------------------------------------------------------------------

void Mabu::Close()
{
	qDebug() << "Closing mabu";
	if (isRunning()) {
		m_bOpen = false;
//		::alarm(0);
		if(wait(300)==false) {
			terminate();
			wait(300);
		}
	}

	::close(m_iHandle);
	m_iHandle = 0;
}

// ---------------------------------------------------------------------------

bool Mabu::Write(const char* pcData,  int iSize)
{
	if (IsOpen()) {
		m_muxWrite.lock();
		int iCount = 0;
		do {
			iCount += ::write(GetHandle(), pcData+iCount, iSize-iCount);
	// 		qDebug("Written:%i",iCount);
		} while(iCount < iSize);
		m_muxWrite.unlock();
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------

/*! You need to close the port to terminate the thread.
 */
void Mabu::run()
{
	// qDebug("\nEntering Mabu::ThreadRead::run()");
	const int icBufSize = 128;
	unsigned char aucData[icBufSize+1];
	int iCount;
	// fcntl(m_pSerial->GetHandle(), F_SETFL, FNDELAY);
	// read all the data from the port
	ASSERT(m_apProcessor.get() != NULL);

	do {
		// This is a blocking call.
		iCount = ::read(GetHandle(), aucData, icBufSize);
		if(iCount > 0) {
			//std::cout << std::endl;
			//for(int j=0; j<iCount; j++)
			//	std::cout << " " << (int)aucData[j] << std::flush;

			m_apProcessor->Load(aucData, iCount);
		}
		else {
			std::cerr << "Closing..." << std::endl;
			break;
		}
	} while(IsOpen());
	qDebug() << "Exiting Mabu::ThreadRead::run()" << m_qsPort;
}

// ---------------------------------------------------------------------------

bool Mabu::Start()
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
