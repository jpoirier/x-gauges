#ifndef __MABU_H
#define __MABU_H

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

/*! \file Mabu.h
    \brief Qt based wrapper for asynchronous reading of Mabu port.

    \author  Ales Krajnc
    \date
 */
// ------------------------------------------------------------------------

#include <string>
#include <QThread>
#include <QMutex>

#include "AbstractProcessor.h"	// To handle reading from stream.
#include "AbstractPort.h"

namespace packetcomm {

// ------------------------------------------------------------------------

class Mabu : public AbstractPort, public QThread
{
public:
	//! Constructor.
	Mabu(QObject* pParent=0);
	//! Constructor takes the opening parameters.
	Mabu(const QString& qsPort, QObject* pParent=0);
	//! Destructor;
   virtual ~Mabu();

public:
	//! Open the port using given name.
	virtual bool Open();
	//! Close the port.
	virtual void Close();

	//! Write block of bytes to Mabu port buffer. Loop if necessary.
	virtual bool Write(const char* pcData, int iSize);

	//! Is the port opened?
	virtual bool IsOpen() const
	{ return m_bOpen; }

	//! Set Mabu port name.
	void SetPort(const QString& qsPort)
	{ m_qsPort = qsPort; }

	//! Get port Mabu name.
	const QString& GetPort() const
	{ return m_qsPort; }

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
	//! The port handle.
	int m_iHandle;
	//! Is port opened.
	bool m_bOpen;

	//! This object may be used in different threads and the mutex prevents write collision.
	QMutex m_muxWrite;
};

// --------------------------------------------------------------------------

} // namespace

#endif // __Mabu_H
