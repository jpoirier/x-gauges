#ifndef __DEFINES_H
#define __DEFINES_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

/*! \file Defines.h
    \brief Implementation of some commonly used defines.
 */
// -------------------------------------------------------------------------

// PRINTF is used in uC programs and PC programs.

#if defined(EMBEDDED_ARM7)
	#include "Core/console.h"
	#define PRINTF		Console::Printk
/*#elif defined(QT_CORE_LIB)
	#include <QtGlobal>
	#define PRINTF		qDebug*/
#else
	#include <cstdio>
	#define PRINTF		printf
#endif
// -------------------------------------------------------------------------

#if defined(QT_NO_FPU) || defined(QT_ARCH_ARM) || defined(QT_ARCH_WINDOWSCE)
	#define Q_MODF 		modff
	#define Q_SINCOS 		sincosf
#else
	#define Q_MODF 		modf
	#define Q_SINCOS 		sincos
#endif

// -------------------------------------------------------------------------
//! Used to determine number of elements for static arrays.
#define COUNTOF(s)   (sizeof(s) / sizeof((s)[0]))

//! Last element of static array.
#define LASTOF(s)			(s[(sizeof(s) / sizeof((s)[0]) - 1)])

//! Test of given float/double argument is NAN.
#define ISNAN(f)			((f)!=(f))

//! Return absolute value macro.
#define ABS(x)			( ((x)<0) ? (-(x)) : (x) )

// -------------------------------------------------------------------------
// Definition of NULL pointer.
#if !defined(NULL)
 #define NULL 0
#endif

// -------------------------------------------------------------------------

#include <assert.h>
#undef ASSERT
#define ASSERT assert

// parsing helper macro - use with care.
#define EXTCOM_READ(var, pData) \
	memcpy(&var, pData, sizeof(var));\
	pData += sizeof(var)

// -------------------------------------------------------------------------
//! Maximum positive 32 bit value
#define MAX_POSITIVE_32				0x7fffffff

//! Invalid angle or invalid position. 
#define INVALID_ANGLE				-999.9

// -------------------------------------------------------------------------

#endif // __DEFINES_H
