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

#include "JulianDay.h"

// -------------------------------------------------------------------------

namespace common {

uint32_t JulianDay::Create(
	int32_t iYear, 	//!< Year
	int32_t iMonth, //!< Month - january = 1, december = 12
	int32_t iDay		//!< Day in the month 1 .. 31
)	
{
	iYear += 8000;
   
	if(iMonth < 3) 
		{ iYear--; iMonth+=12; }

   return   (iYear*365) + (iYear/4) - (iYear/100) + (iYear/400) -1200820
        	 + (iMonth*153+3)/5 - 92
        	 + iDay - 1;

/*	if (iYear < 0 )
		iYear++;

	return   iDay - 32075 +
		1461 * (iYear + 4800 + (eMonth - 14) / 12) / 4 +
		367  * (eMonth - 2 - (eMonth - 14) / 12 * 12) / 12 -
		3    * ((iYear + 4900 + (eMonth - 14) / 12) / 100) / 4;*/
}

// --------------------------------------------------------------------

bool JulianDay::IsLeapYear(int32_t iYear)
{
	if (iYear % 400 == 0)
		return true;
 	if (iYear % 100 == 0)
		return false;
 	return (iYear % 4 == 0);
/*
	uint32_t jd1 = Create(iYear, 2, 28);
   uint32_t jd2 = Create(iYear, 3, 1);
   return (jd2 - jd1) > 1;*/
}

// --------------------------------------------------------------------

JulianDay::JulianDay()
{
	m_uiJD = 0;
}

JulianDay::JulianDay(
	int32_t iYear, 	//!< Year
	int32_t iMonth, //!< Month - january = 1, december = 12
	int32_t iDay		//!< Day in the month 1 .. 31
)	
{
	m_uiJD = Create(iYear, iMonth, iDay);
}

// --------------------------------------------------------------------------

JulianDay::JulianDay(
	uint32_t uiGPSExtendedWeek, //!< GPS week count since 6 Januar 1980
	float fWeekSeconds				  //!< seconds in current week.
)
{
	// 2444245 = Create(1980,1,6) 
	m_uiJD = 2444245 
				+ uiGPSExtendedWeek*7 
				+ (int32_t)(fWeekSeconds/(24*3600));
}

// --------------------------------------------------------------------------

JulianDay::JulianDay(const JulianDay& C)
{
	m_uiJD = C.m_uiJD;
}

// --------------------------------------------------------------------------

JulianDay::JulianDay(uint32_t uiJulianDay)
{
	m_uiJD = uiJulianDay;
}

// --------------------------------------------------------------------------

void JulianDay::GetGregorian(int32_t& iYear, int32_t& iMonth, int32_t& iDay) const
{
	int32_t t1, t2, yr, mo;

	t1 = m_uiJD + 68569L;
	t2 = 4L * t1 / 146097L;
	t1 = t1 - ( 146097L * t2 + 3L ) / 4L;
	yr = 4000L * ( t1 + 1L ) / 1461001L;
	t1 = t1 - 1461L * yr / 4L + 31L;
	mo = 80L * t1 / 2447L;
	iDay = (int32_t) ( t1 - 2447L * mo / 80L );
	t1 = mo / 11L;
	iMonth = (int32_t) ( mo + 2L - 12L * t1 );
	iYear = (int32_t) ( 100L * ( t2 - 49L ) + yr + t1 );

	// Correct for BC years
	if (iYear <= 0) iYear--;
}

// --------------------------------------------------------------------------

Day JulianDay::GetDayOfWeek() const
{
	return (Day)(m_uiJD % 7L);
}

// --------------------------------------------------------------------------

int32_t JulianDay::GetDayOfMonth() const
{
	int32_t iY, iM, iD;
	GetGregorian(iY, iM, iD);
   return iD;
}

// --------------------------------------------------------------------------

int32_t JulianDay::GetDayOfYear() const
{
	int32_t iY, iM, iD;
	GetGregorian(iY, iM, iD);
	return (int32_t) (m_uiJD - Create(iY,1,1) + 1);
}

// --------------------------------------------------------------------------

Month JulianDay::GetMonth() const
{
	int32_t iY, iM, iD;
	GetGregorian(iY, iM, iD);
	return (Month)iM;
}

// --------------------------------------------------------------------------

int32_t JulianDay::GetYear() const
{
	int32_t iY, iM, iD;
	GetGregorian(iY, iM, iD);
	return iY;
}

// --------------------------------------------------------------------------

bool JulianDay::IsLeapYear() const
{
	return IsLeapYear(GetYear());
}

// --------------------------------------------------------------------------

float JulianDay::GetDateAsDecimalYear() const
{
	int32_t iY = GetYear();
	float f = GetDayOfYear();
	f /= (IsLeapYear(iY) ? 365 : 366);
	f += iY;
	return f;
}

// --------------------------------------------------------------------------
} // namespace common

