#ifndef CAN_MESSAGE_H
#define CAN_MESSAGE_H

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

#include <stdint.h>

//! Zero means bradcast to everybody.
#define 	EVERYBODY			0

//! Register A byte definitions.
#define NODE_ID_COUNTER			0
#define DATA_TYPE_INDEX			1
#define SERVICE_CODE_INDEX		2
#define MESSAGE_CODE_INDEX		3

namespace can {

//-----------------------------------------------------------------------------

/** \brief Can register. It is a 32 bit value storage array.

    These 32 bits can be interpreted in different ways. The union allows
    us to have a direct access to different standard variable types.
 */

union Register
{
public:
	//! Default constructor.
	Register() { ui32 = 0; }
	//! Copy constructor
	Register(const Register& C) { ui32 = C.ui32; }
	//! Constructor from unsigned int.
	Register(uint32_t uiR) { ui32 = uiR; }
	//! Constructor from integer.
	Register(int32_t iR) { i32 = iR; }
	//! Constructor from float.
	Register(float fR) { f = fR; }
	//! Constructor from two 16 bit unsigned integers.
	Register(uint16_t ui1, uint16_t ui2) { us2.v1 = ui1; us2.v2 = ui2; }
	//! Constructor from two 16 bit integers.
	Register(int16_t i1, int16_t i2) { s2.v1 = i1; s2.v2 = i2; }
	//! Constructor from four bytes.
	Register(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
	{ by[0] = b1; by[1] = b2; by[2] = b3; by[3] = b4; }
	//! Constructor from four chars.
	Register(char c1, char c2, char c3, char c4)
	{ ch[0] = c1; ch[1] = c2; ch[2] = c3; ch[3] = c4; }

	operator float() const	{ return f;}

// Don't forget that this is union!
public:
	uint32_t 		ui32;			//!< unsigned 32 bit integer.
	int32_t 			i32;			//!< signed 32 bit integer.
	char				ch[4];		//!< array of four chars.
	uint8_t			by[4];		//!< array of four bytes (unsigned chars)
	float 			f;				//!< 32 bit floating point
	//! Two 16 bit unsigned integers packed together.
	struct {
		uint16_t v1;					//!< First 16 bit unsigned integer.
		uint16_t v2;					//!< Second 16 bit unsigned integer.
	} us2;
	//! Two 16 bit integers packed together.
	struct {
		int16_t v1;					//!< First 16 bit integer.
		int16_t v2;					//!< Second 16 bit integer.
	} s2;
};

//-----------------------------------------------------------------------------
//! Parallel levels

enum Level
{
	lPrimary,
	lSecondary,
	lTertiary,
	lQuaternary
};

//-----------------------------------------------------------------------------

union Identifier
{
public:
	//! Default constructor.
	Identifier(){ ui32 = 0; }
	//! Direct constructor.
	Identifier(uint32_t uiIdent) { ui32 = uiIdent; }
	//! Bit fields constructor.
	Identifier(Level eLevel, uint32_t uiId, uint8_t uiSender, uint8_t uiReceiver)
	{
		ui32		 = 0;
		B.uiLevel	 = eLevel;
		B.uiId		 = uiId;
		B.uiSender 	 = uiSender;
		B.uiReceiver = uiReceiver;
	};

/*	//! Get the id inside the identifier.
	uint32_t GetId() const { return B.uiId; }
	//! Get the sender.
	uint8_t GetSender() const { return B.uiSender; }
	//! Get the receiver.
	uint8_t GetReceiver() const { return B.uiReceiver; }
	//! Get the level.
	Level GetLevel() const { return (Level)B.uiLevel; }*/

public:
	//! Struct of bit fields
	struct {
		uint8_t  uiReceiver;				//! The receiving node
		uint8_t  uiSender;				//! The sending node
		uint32_t uiId      : 11;		//! Id of the can message
		uint32_t uiLevel   :  2;		//! Redundancy level
		uint32_t uiNotUsed :  3;		//! Not used by can - must be zero.
	} B;
	//! Single 32 bit identifier.
	uint32_t 		ui32;
	char				ch[4];		//!< array of four chars.
	uint8_t			by[4];		//!< array of four bytes (unsigned chars)
};

//-----------------------------------------------------------------------------

/** \brief CAN Message Structure.

	 Low level CAN message structure. The data size is fixed to eight bytes.
	 A CAN message consists of:
		- Frame information,
		- Identifier,
		- 32 bit data register A (bytes 0-3),
		- 32 bit data register B (bytes 4-7).
 */

class Message {

public:
	//! Default constructor.
	Message()
	{
		m_FrameInfo.Reg = 0;
	}

	//! Constructor taking all four CAN registers.
	Message(uint32_t uiFrame, Identifier I, Register rA, Register rB)
	{
		m_FrameInfo.Reg = uiFrame;
		m_I = I;
		m_rA = rA;
		m_rB = rB;
	}

	//! Simplified constructor taking id and both data registers.
	Message(Identifier I, Register rA, Register rB)
	{
		m_FrameInfo.Reg = 0;
		m_I  = I;
		m_rA = rA;
		m_rB = rB;
	}

	//! Set main message parameters explicitely.
	void Set(Identifier I, Register rA, Register rB)
	{
		m_I  = I;
		m_rA = rA;
		m_rB = rB;
	}

	//! Returns \a m_FrameInfo value in register representation.
	uint32_t GetFrameInfo() const { return m_FrameInfo.Reg; }

	//! Method returns identifier from message.
	Identifier GetIdentifier() const { return m_I; }
	//! Set indetifier when forming a message.
	void SetIdentifier(Identifier I) { m_I = I; }

	//! Who is sending the message (sender)?
	uint8_t GetSender() const { return m_I.B.uiSender; }
	//! Who is receiving the message (receiver)?
	uint8_t GetReceiver() const { return m_I.B.uiReceiver; }
	//! What is the message id?
	uint32_t GetId() const { return m_I.B.uiId; }
	//! Get message level.
	Level GetLevel() const { return (Level)m_I.B.uiLevel; }

	//! Set the sender.
	void SetSender(uint8_t bySender)
	{ m_I.B.uiSender = bySender; }

	//! Return the first register - A (data bytes 0-3).
	Register GetRegisterA() const	{ return m_rA; }

	//! Returns the second register - B (data bytes 4-7).
	Register GetRegisterB() const	{ return m_rB; }

	//! Sets the priority field in FrameInfo
	void SetPriority(uint8_t btPriority) { m_FrameInfo.Tx.Priority = btPriority; }

	//! Sets the data length, remote frame and ext ident into FrameInfo.
	void SetFrameInfo(
			uint32_t DataLength,
			bool RemoteFrame,
			bool ExtIdent )
	{
		m_FrameInfo.Tx.DataLength = DataLength;
		m_FrameInfo.Tx.RemoteFrame = RemoteFrame;
		m_FrameInfo.Tx.ExtendedId = ExtIdent;
	}

	//! Set individual byte in the message. Position may be in the [0,7] interval.
	void SetByte(unsigned int ui, uint8_t by)
	{
		if(ui<4)
			m_rA.by[ui] = by;
		else
			m_rB.by[ui-4] = by;
	}
	//! Get individual byte from the message. Position may be in the [0,7] interval.
	uint8_t GetByte(unsigned int ui) const
	{
		return ui<4 ? m_rA.by[ui] : m_rB.by[ui-4];
	}

protected:
	//! Data structure that represents Frame info register.
	union FrameInfo {
		struct TxFrameInfo {
			uint32_t
				Priority 	: 8,	//!< Priority for the message (not used).
				pad1 			: 8,
				DataLength 	: 4,	//!< Number of data bytes we are sending (0...8).
				pad2 			: 10,
				RemoteFrame : 1,	//!< If true we are requesting remote frame.
				ExtendedId	: 1;	//!< If true we are sending extended frame.
		} Tx;
		struct RxFrameInfo {
			uint32_t
				Index	 		: 10,
				Bypass		: 1,
				pad1 			: 5,
				DataLength 	: 4,
				pad2 			: 10,
				RemoteFrame : 1,
				ExtendedId	: 1;
		} Rx;
		uint32_t Reg;	//!< Used to direct access.
	};

protected:
	//! Message frame info.
	FrameInfo m_FrameInfo;

	//! Identifier (29 bits are used).
	Identifier m_I;

	//! Register A (covers first four bytes 0-3).
	Register m_rA;
	//! Register B (covers second four bytes 4-7).
	Register m_rB;
};

//-----------------------------------------------------------------------------

} // can namespace

#if defined(QT_CORE_LIB)
	#include <QMetaType>
	Q_DECLARE_METATYPE(can::Message)
#endif

#endif
