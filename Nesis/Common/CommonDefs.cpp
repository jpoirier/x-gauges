/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

//! \file CommonDefs.cpp implementation file for commonly used functions.

#include <math.h>
#include <float.h>
#include <Tolerance.h>

#include "CommonDefs.h"

namespace common {

// --------------------------------------------------------------------

/*! Log 10 is used to get number of digits. If absolute number of d is greater 
 *  than 1, log10(|d|)+1 returns number of digists before the decimal. If |d| equals
 *  zero, 0 is returned and if |d| < 1, log10(|d|) is returned.
 *
 * \param d is the number for which we would like to define the digit number.
 * \return a positive number - digits before decimal or a negative number of the
           mdRem significant digit after decimal.
 */
int GetExponent(double d)
{
   double num = fabs(d);
   if ( num >= 1 ) {
      return ((int)log10(num))+1;
   }
   else if (num == 0){
      return 1;
   }
   else {
      return ((int)log10(num));
   }
}

// ------------------------------------------------------------------------
//! Calculates the nearest snap point of the given point. 
double Snap( 
   double x,    //!< The input point
   double su )  //!< The snap dUnit
{
   double ipart;
   double rem = modf( x/su, &ipart );
   if ( x >= 0 && rem > 0.5 ) ipart+=1.;
   if ( x < 0 && rem < -0.5 ) ipart-=1.;
   return ipart*su;
}

// --------------------------------------------------------------------------
/*! Calculates the first and the last valid dUnit 
    mark within the [min,max] interval. Result overwrites the inteval.
    passed as parameters.
 */
void SuggestMinMaxInside( 
   double dUnit,  //!< The dUnit used to adapt the interval
   double& _min,  //!< [in,out] The interval lower bound.
   double& _max ) //!< [in,out] The interval upper bound.
{
   if (dUnit) {
      double dMin = _min;
      double dMax = _max;
      double dRem, ipart;             // number of units between dMin & 
      dRem = modf(dMin/dUnit, &ipart);  // zero.
      int minN = (int)ipart;         // if dMin is positive than we must 
      if (dMin > 0)                   // increment solution by one --
         if (tolerance_rel::NotEqual(dRem,0.0)) minN++;  // but not, if the solution is exact.

      dRem = modf(dMax/dUnit, &ipart);  // number of units between max & zero
      int maxN = (int)ipart;         // if max is negative than we must 
      if (dMax < 0)                  // decremenent solution by one --
         if (tolerance_rel::NotEqual(dRem,0.0) ) maxN--;// but not, if the solution is exact.

      _min = minN*dUnit;   
      _max = maxN*dUnit;
   }
}

// --------------------------------------------------------------------------
/*! The interval is passes as parameters [dMin,max].
    Result overwrites the given inteval. 
 */    
void SuggestMinMaxOutside( 
   double dUnit,  // @parm The dUnit used to adapt the interval.
   double& _min,  // @parm [in,out] The interval lower bound.
   double& _max ) // @parm [in,out] The interval upper bound.
{
   if (dUnit) {
      double dMin = _min; 
      double dMax = _max;
      double dRem, ipart;             // number of units between dMin & 
      dRem = modf(dMin/dUnit, &ipart);  // zero.
      int minN = (int)ipart;         // if dMin is negative than we must 
      if (dMin < 0)                   // decrement solution by one --
         if (tolerance_rel::NotEqual(dRem,0)) minN--;  // but not, if the solution is exact.

      dRem = modf(dMax/dUnit, &ipart);  // number of units between dMax & zero
      int maxN = (int)ipart;         // if dMax is positive than we must 
      if (dMax > 0)                  // incremenent solution by one --
         if (tolerance_rel::NotEqual(dRem,0) ) maxN++;// but not, if the solution is exact.

      _min = minN*dUnit;   
      _max = maxN*dUnit;
   }
}

// --------------------------------------------------------------------
// Internal function. Reads characters form in stream until new line or 
// end of stream is reached.
std::istream& NextLine(std::istream& in)
{
   char ch;
   do {
      in.get(ch);
   } while ( ch != '\n' && in.good() );
   return in;
}

// --------------------------------------------------------------------
/*! First, the string is searched for any internal " character. This
    character is replaced with "" pair. The string is then encapsulated
    within "s" pair and put into the out stream.
    \param out output stream.
    \param s   string

    \retval number of characters put into the out stream.
*/
int WriteString(std::ostream& out, const std::string& s)
{
   using namespace std;
   // are there any '"' in string?
   string ss(s); // a copy
   // replace all " with ""
   string::size_type pos = 0;
   while((pos=ss.find_first_of('"',pos))!=string::npos) {
      ss.insert(pos,1,'"');
      pos+=2;
   }
   out << '"' << ss << '"'; // thats it
   return (int) ss.length();
}

// --------------------------------------------------------------------

int ReadString(std::istream& in, std::string& s)
{
   using namespace std;
   s.erase(); //="";
   char ch;

   // skip spaces, tabs, new lines, ...
   in >> ws >> ch;

   // It must be "
   if(ch!='"') {
      return -1; // first quote not found
   }
   
   bool bExit = false;
   while(!in.eof()) {
      in.get(ch);
      // this could be end or double quote "" -> "
      if(ch=='"') {
         if(in.eof()) {
            bExit = true;
            break;
         }
         char ch2;
         in.get(ch2);               // check the second character
         if(ch2!='"') { // It is not the quote
            in.putback(ch2);     // push it back
            bExit = true;           // signal exit
            break;
         }
      }
      s+=ch;
   }
   if(bExit) {
      return (int)s.length();
   }
   // Sorry, we failed!
   return -1; // Error
}

// --------------------------------------------------------------------

std::istream& extws(std::istream& in)
{
   char ch;
   do {
      bool bAgain = false;
      do {
         in.get(ch);
         if (ch == ';') {  // (;) 
            NextLine(in);
            in.get(ch);
            bAgain = (ch == ';');
            if (bAgain)
               in.putback(ch);
         }
      } while(bAgain);
   } while( (isspace(ch) || ch == ',') && in.good() );
   in.putback(ch);
   return in;
}

// --------------------------------------------------------------------

//! Round a value to a given decimal place.
double Round(double dVal, unsigned int nDecimal)
{
   // move the decimal place nDecimal number of times
   dVal *= pow(10.0, (int)nDecimal);

   double dDiff;
   if (dVal > 0) {       // check for -ve or +ve
      dDiff = dVal - floor(dVal);
      if (dDiff >= 0.5)
         dVal = ceil(dVal);      // e.g. 2.6 -> 3.0
      else
         dVal = floor(dVal);     // e.g. 2.2 -> 2.0
   }
   else
   {
      dDiff = fabs(dVal) - floor(fabs(dVal));
         if (dDiff >= 0.5)
            dVal = floor(dVal);    // e.g. -2.6 -> -3.0
         else
            dVal = ceil(dVal);     // e.g. -2.2 -> -2.0
   }

   // move back the decimal place to the original position
   dVal /= pow(10.0, (int)nDecimal);
   return dVal;
}

// --------------------------------------------------------------------

// Sign of value: 1 for +ve, and -1 for -ve
int Sgn(double dVal, const double dTol)
{
    int sgn = 0;
    if (dVal > dTol)
        sgn = 1;
    else if (dVal < -dTol)
        sgn = -1;
    return sgn;
}


} // namespace common
