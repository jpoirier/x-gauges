#ifndef __JULIAN_DAY_H
#define __JULIAN_DAY_H

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

#include <stdint.h>

namespace common {

// --------------------------------------------------------------------

//! Days of week
enum Day {
	dMonday = 0,
	dTuesday,
	dWednesday,
	dThursday,
	dFriday,
	dSaturday,
	dSunday
};

//! Months
enum Month {
	mJanuary = 1,
	mFebruary,
	mMarch,
	mApril,
	mMay,
	mJune,
	mJuly,
	mAugust,
	mSeptember,
	mOctober,
	mNovember,
	mDecember
};

/** The Julian day is the integer number of days that have elapsed since
    the initial epoch defined as noon Universal Time (UT)
    Monday, January 1, 4713 BC in the proleptic Julian calendar.

	 Here we provide a JulianDay class which represents dates as
	 Julian day numbers (days since 1 Jan 4713 BC).
	 The class can handle all dates from 1 Jan 4713BC to 31 Dec 4999.

	 Years AD are positive, years BC are negative. There is
	 no year 0AD, it goes from 1BC to 1AD. A year of 0 will be treated
	 as 1BC. No attempt is made to validate ranges. Physical errors
	 will not result from insane day-month combinations like the 87th
	 day of the 45th month, but the results will obviously not make
	 much sense.

	 Credits:
	 Date conversion routines  by Eric bergman-Terrell,
	 Computer Language, Dec 1990.
 */

class JulianDay
{
public:
	//! Default constructor constructs jd=0;
	JulianDay();
	//! Constructor requires year, month and day according to Gregorian calendar.
	JulianDay(int32_t iYear, int32_t iMonth, int32_t iDayInMonth);
	//! Constructor requires (extended - 13 bit format) GPS week and week seconds.
	JulianDay(uint32_t uiGPSExtendedWeek, float fWeekSeconds);
	//! Copy constructor.
	JulianDay(const JulianDay& C);
	//! Constructor from a known julian day constant.
	JulianDay(uint32_t uiJulianDay);

	//! Return Julian day value as integer.
	int32_t GetJulianDay() const
	{ return (int32_t)m_uiJD; }

	//! Convert Julian day into Gregorian form.
	void GetGregorian(int32_t& iYear, int32_t& iMonth, int32_t& iDay) const;

	//! Return day in the week.
	Day GetDayOfWeek() const;
	//! Return day of month
	int32_t GetDayOfMonth() const;
	//! Return day in the year.
	int32_t GetDayOfYear() const;

	//! Return month in the year.
	Month GetMonth() const;
	//! Return year.
	int32_t GetYear() const;

	//! Is it a leap year?
	bool IsLeapYear() const;

	//! Return date expressed as decimal year. E.g. 30.6.2006 = 2006.495890411
	float GetDateAsDecimalYear() const;

	// Operators

	//! Assignment operator.
	const JulianDay& operator = (const JulianDay& R) { m_uiJD = R.m_uiJD; return *this; }
	//! Equality operator.
	bool operator == (const JulianDay& R) const { return m_uiJD == R.m_uiJD; }
	//! Non-equality operator.
	bool operator != (const JulianDay& R) const { return m_uiJD != R.m_uiJD; }

	//! Is smaller?
	bool operator < (const JulianDay& R) const { return m_uiJD < R.m_uiJD; }
	//! Is smaller or equal?
	bool operator <= ( const JulianDay& R ) const { return m_uiJD <= R.m_uiJD; }
	//! Is greater?
	int32_t operator > (const JulianDay& R) const { return m_uiJD > R.m_uiJD; }
	//! Is greater or equal?
	int32_t operator >= (const JulianDay& R) const { return m_uiJD >= R.m_uiJD; }

	//! Increment operator.
	const JulianDay& operator++() { m_uiJD++; return *this; }
	//! Decrement operator.
   const JulianDay& operator--() { m_uiJD--; return *this; }

	//! Add days to current date.
	const JulianDay& operator += (int32_t R) { m_uiJD += R; return *this; }
	//! Subtract days from current date;
	JulianDay& operator-=(int32_t R) { m_uiJD -= R; return *this; }

	//! Difference between two JulianDays.
   friend int32_t operator-(const JulianDay& L, const JulianDay& R)
   { return L.GetJulianDay() - R.GetJulianDay(); }

	// Static functions

	//! Create JulianDay from given arguments.
	static uint32_t Create(int32_t iYear, int32_t iMonth, int32_t iDayInMonth);
	//! Test is given year is a leap year.
   static bool IsLeapYear(int32_t iYear);

private:
	//! Julian day number as unsigned integer.
	uint32_t m_uiJD;
};

} // namespace common

#endif // __JULIAN_DAY_H
