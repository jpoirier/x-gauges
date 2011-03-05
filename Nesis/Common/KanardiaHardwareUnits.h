#ifndef __KANARDIA_HARDWARE_UNITS_H
#define __KANARDIA_HARDWARE_UNITS_H

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

/*! \file KanardiaHardwareUnits.h
    \brief Hardware unit identification constants.

	 This file stores hardware identification constants needed by CAN and
	 EIA communication protocols. Each hardware unit has its own id.
	 Ids must be in 0-255 range (required by CAN).
 */


// NESIS units constants ----------------------------------------------------

//! Magnetic compass and world magnetic model unit.
#define KHU_MAGU							0x01
//! Data acquisition: engine, electic and fuel flow unit.
#define KHU_DAQU							0x02
//! Static, dynamic pressure + Inertial unit.
#define KHU_AIRU							0x03
//! Matherboard unit - home for embedded computer and black box logger.
#define KHU_MABU							0x04
//! Matherboard secondary unit 
#define KHU_MABU_SECONDARY				0x05
//! Servo actuator unit - drives autopilot servos.
#define KHU_SERU							0x06
//! Fuel level unit.
#define KHU_FLU							0x07
//! Can to USB converter - Canu.
#define KHU_CANU							0x08
//! Calibration unut (used internaly).
#define KHU_KALU							0x09
//! NESIS unit - the main unit.
#define KHU_NESIS							0x0a
//! NESIS unit - the seconadary unit.
#define KHU_NESIS_SECONDARY			0x0b

//! The last valid hardware uC based unit
#define KHU_LAST_uC_UNIT				0x0b

//! iCAN unit - special program listening on CAN bus.
//#define KHU_iCAN							0x40
//! SerialNumber - special program used to write serial numbers.
//#define KHU_SERIALNUMBER				0x41
//! Calibrate - special program used for sensor calibration.
//#define KHU_CALIBRATE					0x42
//! Logger - a program used to read and browse logger.
//#define KHU_LOGGER						0x43

#endif // __KANARDIA_HARDWARE_UNITS_H
