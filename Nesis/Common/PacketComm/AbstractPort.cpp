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

/*! \file AbstractPort.cpp
    \brief Implementation of asynchronous reading from serial port.

    \author  Ales Krajnc
    \date
 */
// --------------------------------------------------------------------------

#include "CommonDefs.h"
#include "AbstractPort.h"

namespace packetcomm {


// ---------------------------------------------------------------------------

AbstractPort::AbstractPort()
{
	m_bAutoDelete = true;
}


AbstractPort::~AbstractPort()
{
	if(m_bAutoDelete==false)
		m_apProcessor.release();
// 	if(IsOpen())
// 		Close();
}

// ---------------------------------------------------------------------------

void AbstractPort::SetProcessor(
	AbstractProcessor* pProcessor, 
	bool bAutoDelete
)
{ 
	if(!m_bAutoDelete)
		m_apProcessor.release();
	m_apProcessor = std::auto_ptr<AbstractProcessor>(pProcessor); 
	m_bAutoDelete = bAutoDelete;
}

// ---------------------------------------------------------------------------
} // namespace
