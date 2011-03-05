/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "MessageAerospace.h"

namespace can {

//-----------------------------------------------------------------------------

MessageAerospace::MessageAerospace() : Message()
{
	SetFrameInfo(8,false,true);
}

MessageAerospace::MessageAerospace(const Message& msg)
: Message(msg)
{
	SetFrameInfo(8,false,true);
}

MessageAerospace::MessageAerospace(
	Identifier I, 				//!< The identifier
	DataType edt, 				//!< Data type format used for the data register.
	uint8_t byServiceCode, 	//!< Service specific code, when used.
	uint8_t byMessageCode,	//!< Message specific code, when used.
	Register rData				//!< Data register.
)
: Message(I, Register(0, (uint8_t)edt, byServiceCode, byMessageCode), rData)
{
	SetFrameInfo(8,false,true);
}

//-----------------------------------------------------------------------------

can::MessageAerospace::MessageAerospace(Identifier I, Register rHead, Register rData)
: Message(I, rHead, rData)
{
	SetFrameInfo(8,false,true);
}


//-----------------------------------------------------------------------------

void MessageAerospace::Set(
	Identifier I, 				//!< The identifier
	DataType edt, 				//!< Data type format used for the data register.
	uint8_t byServiceCode, 	//!< Service specific code, when used.
	uint8_t byMessageCode,	//!< Message specific code, when used.
	Register rData				//!< Data register.
)
{
	SetIdentifier(I);
	SetHeader(Register(0, (uint8_t)edt, byServiceCode, byMessageCode));
	SetData(rData);
}

void MessageAerospace::Set(
	Identifier I, 				//!< The identifier
	Register rHeader, 		//!< Data type format used for the data register.
	Register rData				//!< Data register.
)
{
	SetIdentifier(I);
	SetHeader(rHeader);
	SetData(rData);
}


//-----------------------------------------------------------------------------

} // can namespace


