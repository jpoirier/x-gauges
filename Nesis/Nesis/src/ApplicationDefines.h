#ifndef APPLICATION_DEFINES_H
#define APPLICATION_DEFINES_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o.                                 *
 *   Writen by:                                                            *
 *      Aleš Krajnc  [ales.krajnc@kanardia.eu]                             *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Confidential                                                  *
 *                                                                         *
 *   License: All rights reserved                                          *
 *                                                                         *
 ***************************************************************************/

#include "KanardiaHardwareUnits.h"

//! Define the CAN Node id
#define CAN_NODE_ID				KHU_NESIS

//! The version are limited to 16 major and 16 minor. [from 0.0 - 16.16]
#define SET_VERSION(H, L) ((unsigned char)((((H) & 0x0F) << 4) | (L & 0x0F)))

//! Define the latest MAGU software version.
#define SOFTWARE_VERSION		SET_VERSION(1,1)

// --------------------------------------------------------------------------

// Data download services

#define USE_CAN_DDS_A
//#define USE_CAN_DDS_B

// Data upload services

#define USE_CAN_DUS_A
//#define USE_CAN_DUS_B

// Flash programming services

#define USE_CAN_FPS_A
//#define USE_CAN_FPS_B

// Application programming services

#define USE_CAN_APS_A
//#define USE_CAN_APS_B

// Module information service
#define USE_CAN_MIS_A
//#define USE_CAN_MIS_B

// Node id setting service
#define USE_CAN_NIS_A
//#define USE_CAN_NIS_B

// Module config service
#define USE_CAN_MCS_A
//#define USE_CAN_MCS_B

// --------------------------------------------------------------------------

#endif
