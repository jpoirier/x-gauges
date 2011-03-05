#ifndef COMMON_SPHERE_NAVIGATION_H
#define COMMON_SPHERE_NAVIGATION_H

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

#include <math.h>
#include <limits>
#include "MathEx.h"
#include "CommonDefs.h"

#define EARTH_RADIUS_KM 6372.795		// Kilometers
#define EARTH_RADIUS_M  6372795.0	// Meters

namespace common {

// -------------------------------------------------------------------------

// --------------------------------------------------------------------
/** \brief Get great circle distance between two points on the unit sphere.
 	 
	The algorithm is taken from the 
	<A HREF="http://en.wikipedia.org/wiki/Great-circle_distance">Wikipedia</A>.
	The algorithm comes in three forms, acos, asin and atan2 form. We use
   the atan2 form, since it seems the most roboust and numericaly suitable
   for large and small distances.

	All arguments must be given in radians.

	\retval T Distance between start and destination point on unit sphere.
 */

template<class T>
T GetDistanceGreatCircle(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	const T fSinFi1 = sin(fLat1);
	const T fCosFi1 = cos(fLat1);
	const T fSinFi2 = sin(fLat2);
	const T fCosFi2 = cos(fLat2);
	const T fDLam   = fLon2-fLon1;
	const T fSinDLam = sin(-fDLam);
	const T fCosDLam = cos(-fDLam);

   const T fy = sqrt(
		pow(fCosFi2*fSinDLam,2) + 
      pow(fCosFi1*fSinFi2-fSinFi1*fCosFi2*fCosDLam,2)
	);
	const T fx = fSinFi1*fSinFi2 + fCosFi1*fCosFi2*fCosDLam;

	// This one is good, if points are not too close.
	// T r1 = acos(fSinFi1*fSinFi2 + fCosFi1*fCosFi2*fCosDLam);
	// This one is good, if points are not too far - opposite.
	// T r2 = 2*asin(sqrt(pow(sin((fLat2-fLat1)/2),2) + fCosFi1*fCosFi2*pow(sin(fDLam/2),2)));

	// The atan version is the most roboust (but slow).
	return ATan2Safe(fy, fx);
} 

// -------------------------------------------------------------------------
/** \brief Get great circle start bearing between two points on the unit sphere.
 	 
	The algorithm is taken from the 
	<A HREF="http://williams.best.vwh.net/avform.htm">Ed Williams' Aviation page</A>.
	Bearing between two points on a sphere connected by a great circle is not
   constant but is changing from point to point.
	Since it is atan2 bases, the algorithm seems roboust and may be used to calculate
	small angles as well.

	All arguments must be given in radians.

	\retval T Start bearing between start and destination point on unit sphere.
 */

template<class T>
T GetBearingGreatCircle(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	return Mod(
		(T)ATan2Safe(
			sin(fLon2-fLon1)*cos(fLat2),
         cos(fLat1)*sin(fLat2)-sin(fLat1)*cos(fLat2)*cos(fLon2-fLon1)
		), 
	(T)(2*M_PI));
}


// -------------------------------------------------------------------------
/** \brief Get great circle bearing and distance between two points 
           on the unit sphere.
 	 
	The distance algorithm is taken from the 
	<A HREF="http://en.wikipedia.org/wiki/Great-circle_distance">Wikipedia</A>,
	while the bearing algorithm is taken from the 
	<A HREF="http://williams.best.vwh.net/avform.htm">Ed Williams' Aviation page</A>.

	All arguments must be given in radians.

	\retval std::pair<T,T> Bearing, Distance pair between start and destination 
                          point on unit sphere.
 */

template<class T>
std::pair<T,T> GetBearingDistanceGreatCircle(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	const T fEps = std::numeric_limits<T>::min()*5;

	const T fSinFi1 = sin(fLat1);
	const T fCosFi1 = cos(fLat1);
	const T fSinFi2 = sin(fLat2);
	const T fCosFi2 = cos(fLat2);
	const T fDLam   = fLon2-fLon1;
	const T fSinDLam = sin(-fDLam); // Formulas are West is positive, hence 
	const T fCosDLam = cos(-fDLam); // we change sing since East is positive in our case.

   const T fy = sqrt(
		pow(fCosFi2*fSinDLam,2) + 
      pow(fCosFi1*fSinFi2-fSinFi1*fCosFi2*fCosDLam,2)
	);
	const T fx = fSinFi1*fSinFi2 + fCosFi1*fCosFi2*fCosDLam;

	// The atan version is the most roboust (but slow).
	const T fD = ATan2Safe(fy, fx);

	// Bearing
	T fH;
	// Handle the pole situation first
	if (fCosFi1 < fEps) {   // EPS a small number ~ machine precision
  		if(fLat1 > 0)
     		fH = M_PI;        //  starting from N pole
  		else
     		fH = 2*M_PI;      //  starting from S pole
   }

	//  For starting points other than the poles
	else {
		fH = Mod(
			(T)ATan2Safe(
				-fSinDLam*fCosFi2,
         	fCosFi1*fSinFi2-fSinFi1*fCosFi2*fCosDLam
			), 
			(T)(2*M_PI)
		);
/*		if(fSinDLam < 0)
   		fH = ACosSafe((fSinFi2-fSinFi1*cos(fD))/(sin(fD)*fCosFi1)); 
		else {
   		fH =2*M_PI-ACosSafe((fSinFi2-fSinFi1*cos(fD))/(sin(fD)*fCosFi1));
		}*/
	}

	return std::make_pair(fH, fD);
}

// -------------------------------------------------------------------------
/** \brief Rhumb (Loxodrome) bearing and distance between two points 
           on the unit sphere.
 	
	The rhumb (loxodrome) version keeps constant heading to the destination for
   the sake of longer distance. The algorithm is taken from the 
	<A HREF="http://williams.best.vwh.net/avform.htm">Ed Williams' Aviation page</A>.
	
	All arguments must be given in radians.

	\retval std::pair<T,T> Bearing, Distance pair between start and destination 
                          point on unit sphere.
 */

template<class T>
std::pair<T,T> GetBearingDistanceRhumb(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	const T fEps = std::numeric_limits<T>::min()*5;

	const T flon_W = Mod(fLon1-fLon2, (T)(2*M_PI));
  	const T flon_E = Mod(fLon2-fLon1, (T)(2*M_PI));

	const T fphi = log(tan(fLat2/2+M_PI_4)/tan(fLat1/2+M_PI_4));
  
	T q;
 	T fH;
	T fD;

	if (fabs(fLat2-fLat1) < sqrt(fEps)) {
		q = cos(fLat1);
	} 
	else {
		q = (fLat2-fLat1)/fphi;
	}
  	// If westerly rhumb line is the shortest
	if (flon_W < flon_E) {
    	fH = Mod((T)ATan2Safe(-flon_W, fphi), (T)(2*M_PI));
      fD = sqrt((T)pow(q*flon_W,2) + pow(fLat2-fLat1,2));
	} 
	else {
    	fH = Mod((T)ATan2Safe(flon_E, fphi), (T)(2*M_PI));
      fD = sqrt((T)pow(q*flon_E,2) + pow(fLat2-fLat1,2));
   }
	return std::make_pair(fH, fD);
}

// -------------------------------------------------------------------------
/** \brief Rhumb (Loxodrome) bearing between two points 
           on the unit sphere.
 	
	The rhumb (loxodrome) version keeps constant heading to the destination for
   the sake of longer distance. The algorithm is taken from the 
	<A HREF="http://williams.best.vwh.net/avform.htm">Ed Williams' Aviation page</A>.
	
	All arguments must be given in radians.

	\retval T Bearing between start and destination point on the unit sphere.
 */

template<class T>
T GetBearingRhumb(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	return GetBearingDistanceRhumb(fLon1, fLat1, fLon2, fLat2).first;
}

// -------------------------------------------------------------------------
/** \brief Rhumb (Loxodrome) distance between two points 
           on the unit sphere.
 	
	The rhumb (loxodrome) version keeps constant heading to the destination for
   the sake of longer distance. The algorithm is taken from the 
	<A HREF="http://williams.best.vwh.net/avform.htm">Ed Williams' Aviation page</A>.
	
	All arguments must be given in radians.

	\retval T Rhumb distance between start and destination point on the unit sphere.
 */

template<class T>
T GetDistanceRhumb(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	return GetBearingDistanceRhumb(fLon1, fLat1, fLon2, fLat2).second;
}

// -------------------------------------------------------------------------
/** \brief Flat Earth aproximation for short range on the unit sphere.
 	
	We make a tangent plane on the starting point. The longitudes are
   corrected and standard trigonometry is used to calculate distance and
   bearing between point. The algorithm is simple and fast but it is only valid
   if the destination point is not too far from the start point. 
	
	All arguments must be given in radians.

	\retval T Flat Earth distance between start and destination point on the unit sphere.
 */


template<class T>
T GetDistanceFlat(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	return sqrt(
		pow(fLat2-fLat1,2) + 
		pow((fLon2-fLon1)*cos(fLat1),2)
	);
}

// -------------------------------------------------------------------------
/** \brief Flat Earth aproximation for short range on the unit sphere.
 	
	We make a tangent plane on the starting point. The longitudes are
   corrected and standard trigonometry is used to calculate distance and
   bearing between point. The algorithm is simple and fast but it is only valid
   if the destination point is not too far from the start point. 
	
	All arguments must be given in radians.

	\retval T Flat Earth bearing between start and destination point on the unit sphere.
 */

template<class T>
T GetBearingFlat(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	return Mod((T)ATan2Safe(cos(fLat1)*(fLon2-fLon1), fLat2-fLat1), (T)(2*M_PI));
}

// -------------------------------------------------------------------------
/** \brief Flat Earth aproximation for short range on the unit sphere.
 	
	We make a tangent plane on the starting point. The latitudes are
   corrected and standard trigonometry is used to calculate distance and
   bearing between point. The algorhitm is simple and fast but it is only valid
   if the destination point is not too far from the start point. 
	
	All arguments must be given in radians.

	\retval std::pair<T,T> Bearing, Distance pair between start and destination 
                          point on unit sphere.
 */

template<class T>
std::pair<T,T> GetBearingDistanceFlat(
	const T fLon1, //!< Longitude of the start point.
	const T fLat1, //!< Latitude of the start point.
	const T fLon2, //!< Longitude of the destination (second) point.
	const T fLat2  //!< Latitude of the destination (second) point.
)
{
	const T fdLonW = Mod(fLon1-fLon2, (T)(2*M_PI));
  	const T fdLonE = Mod(fLon2-fLon1, (T)(2*M_PI));

	const T fdLat = fLat2-fLat1;
	const T fCos  = cos(fLat1);

	if(fdLonE < fdLonW) {
		return std::make_pair(
			Mod((T)ATan2Safe(fCos*fdLonE, fdLat), (T)(2*M_PI)),
			sqrt(fdLat*fdLat + pow(fdLonE*fCos,2))
		);
	}
	else {
		return std::make_pair(
			Mod((T)ATan2Safe(-fCos*fdLonW, fdLat), (T)(2*M_PI)),
			sqrt(fdLat*fdLat + pow(fdLonW*fCos,2))
		);
	}
}

// -------------------------------------------------------------------------
/** \brief Combination of Great Circle and Flat Earth bearing and distance.
 	
	If the distance between coordinates is small enough (very small)
	Flat Earth version is used. For all other cases Great Circle version
   is used.

	\retval std::pair<T,T> Bearing, Distance pair between start and destination 
                          point on unit sphere.
 */

template<class T>
std::pair<T,T> GetBearingDistance(T fLon1, T fLat1, T fLon2, T fLat2)
{
	// TODO We could pass the swithcing treshold in the call.
	if(GetMax(fabs(fLon1-fLon2),fabs(fLat1-fLat2)) > 0.0001f)
		return GetBearingDistanceGreatCircle(fLon1, fLat1, fLon2, fLat2);
	else
		// TODO - use some tricks to improve resolution of the solution.
		return GetBearingDistanceFlat(fLon1, fLat1, fLon2, fLat2);
}

// -------------------------------------------------------------------------

} // namespace

#endif
