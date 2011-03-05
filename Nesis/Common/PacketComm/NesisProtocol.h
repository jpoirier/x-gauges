#ifndef __NESIS_PROTOCOL_H
#define __NESIS_PROTOCOL_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

/*! \file NesisProtocol.h Basis of the communication protocol.
    \brief Definition of communication protocol constants and commands.

    \author  Rok Markovic, Ales Krajnc
    \date
 */


// NESIS units constants ----------------------------------------------------

#define MAGU_UNIT							0x01
#define DAQU_UNIT							0x02
#define AIRU_UNIT							0x03
#define INU_UNIT							0x04
#define NESIS_UNIT						0x05
#define SERU_UNIT							0x06
#define FLU_UNIT							0x07

// NESIS EIA packet description ---------------------------------------------

#define EIA_PACKET_HEAD_BYTE   		0xfa
#define EIA_PACKET_TAIL_BYTE   		0xf3

#define EIA_MAX_PACKET_SIZE    		112
#define EIA_MIN_PACKET_SIZE			5
#define EIA_SIZE_BYTE_POSITION		1
#define EIA_COMMAND_BYTE_POSITION	2
#define EIA_DATA_BEGIN_POSITION		3

// Common commands (for all units) --------------------------------------------

#define EIA_REQUEST_INFO						0x70
#define EIA_REPORT_INFO							0x71
#define EIA_CAN_MESSAGE							0x72
#define EIA_ACKNOWLEDGE							0x73

// Flash related commands -----------------------------------------------------

#define EIA_CMD_FLASH_DATA						0x80

#define FLASH_DATA_BURN							0xfc
#define FLASH_DATA_MAGU_FIRST					0x10

// MAGU commands start at 0xb0 ------------------------------------------------

#define EIA_MAGU_SET_REPORT_INTERVAL		0xb1
#define EIA_MAGU_WMM_REQUEST					0xb2
#define EIA_MAGU_ADD_CALIBRATION_PAIR		0xb4
#define EIA_MAGU_FINISH_CALIBRATION			0xb5
#define EIA_MAGU_REPORT_HEADING				0xb8
#define EIA_MAGU_REPORT_WMM					0xb9
#define EIA_MAGU_REPORT_CALIBRATION			0xba

// MAGU finish calibration options
#define MAGU_FINISH_CANCEL						0
#define MAGU_FINISH_REPORT_ONLY				1
#define MAGU_FINISH_REPORT_BURN				2
#define MAGU_FINISH_RESTORE_DEFAULT			3

// MAGU flash commands
#define MAGU_FLASH_DATA_MOUNT_MATRIX		0 + FLASH_DATA_MAGU_FIRST
#define MAGU_FLASH_DATA_COEF_2D				1 + FLASH_DATA_MAGU_FIRST

// Zlagalnik commands start at 0x90 -------------------------------------------

#define EIA_ZLAG_SET_POSITION					0x90
#define EIA_ZLAG_REPORT_POSITION				0x91

#define EIA_OVEN_REPORT							0x9a

// Reports --------------------------------------------------------------------

#define EIA_IDENT_ATTITUDE						0x10
#define EIA_IDENT_ACCELERATION				0x11
#define EIA_IDENT_RATE							0x12
#define EIA_IDENT_AIRDATA						0x13
#define EIA_IDENT_POSITION						0x14
#define EIA_IDENT_WIND							0x15
#define EIA_IDENT_TEMPERATURE					0x16
#define EIA_IDENT_GPS							0x17


// When adding a new NESIS command, check the size of EXTCOM_CMD_COUNT in
// the extcom.h file.


// TODO Revision of names is required.
#define ALTUS_CMD_CONTROL				0xc0
#define ALTUS_CMD_LIST					0xc1
#define ALTUS_CMD_CONNECT				0xc2
#define ALTUS_CMD_DISCONNECT			0xc3

#define ALTUS_CMD_MASK					0xf0
#define ALTUS_CMD_SIGNATURE			0xc0

#define NESIS_CMD_MASK					0xe0
#define NESIS_CMD_SIGNATURE			0xa0 | 0xb0

// NESIS packets used by Senap ----------------------------------------------

#define NESIS_CMD_TELEMETRY      	0xa0
#define NESIS_CMD_PRINTF				0xa1
#define NESIS_CMD_PRG_WPT				0xa2
#define NESIS_CMD_ERROR					0xa3
#define NESIS_CMD_RAW_DATA				0xa5
#define NESIS_CMD_ENTER_ISP			0xa6
#define NESIS_CMD_FREQ_TELEMETRY		0xa7
#define NESIS_CMD_FFT					0xa8
#define NESIS_CMD_SET_QNH				0xa9
#define NESIS_CMD_MAG_VEC				0xaa


#endif // __NESIS_PROTOCOL_H
