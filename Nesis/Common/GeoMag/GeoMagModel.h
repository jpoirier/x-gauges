	/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o.                                 *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *      May - July 2006                                                    *
 *                                                                         *
 *   Licence:                                                              *
 *      GNU - General Public License as published by the                   *
 *            Free Software Foundation                                     *
 *                                                                         *
 *   Note:                                                                 *
 *      The code was adapted from the "geomag60.c", which is credit to     *
 *      Stefan Maus 8-24-2004. However, the text given below is found in   *
 *      the original file. The copyright of the original code in not       *
 *      specified. I assume it is public domain.                           *
 *                                                                         *
 *   Web: (where the original model was taken from)                        *
 *      http://www.ngdc.noaa.gov/seg/geomag/models.shtml                   *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *      This program, originally written in FORTRAN, was developed using   *
 *      subroutines written by                                             *
 *      A. Zunde                                                           *
 *      USGS, MS 964, Box 25046 Federal Center, Denver, Co.  80225         *
 *      and                                                                *
 *      S.R.C. Malin & D.R. Barraclough                                    *
 *      Institute of Geological Sciences, United Kingdom.                  *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *      Translated                                                         *
 *      into C by    : Craig H. Shaffer                                    *
 *                     Lockheed Missiles and Space Company                 *
 *                     Sunnyvale, California                               *
 *                     (408) 756 - 5210                                    *
 *                     29 July, 1988                                       *
 *                                                                         *
 *      Contact      : Greg Fountain                                       *
 *                     Lockheed Missiles and Space Company                 *
 *                     Dept. MO-30, Bldg. 581                              *
 *                     1111 Lockheed Way                                   *
 *                     P.O. Box 3504                                       *
 *                     Sunnyvale, Calif.  94088-3504                       *
 *                                                                         *
 *      Rewritten by : David Owens                                         *
 *                     dio@ngdc.noaa.gov                                   *
 *                     For Susan McClean                                   *
 *                                                                         *
 *      Contact      : Susan McLean                                        *
 *                     sjm@ngdc.noaa.gov                                   *
 *                     National Geophysical Data Center                    *
 *                     World Data Center-A for Solid Earth Geophysics      *
 *                     NOAA, E/GC1, 325 Broadway,                          *
 *                     Boulder, CO  80303                                  *
 *                                                                         *
 *      Original                                                           *
 *      FORTRAN                                                            *
 *      Programmer   : National Geophysical Data Center                    *
 *                     World Data Center-A for Solid Earth Geophysics      *
 *                     NOAA, E/GC1, 325 Broadway,                          *
 *                     Boulder, CO  80303                                  *
 *                                                                         *
 *      Contact      : National Geophysical Data Center                    *
 *                     World Data Center-A for Solid Earth Geophysics      *
 *                     NOAA, E/GC1, 325 Broadway,                          *
 *                     Boulder, CO  80303                                  *
 *                                                                         *
 ***************************************************************************/

// Note: index starts with 1!, (from old Fortran)

// ---------------------------------------------------------------------------
//! Maximal degree of a spherical harmonic model.
#define MAXDEG 12
//! Maximal number of numeric coefficients used for a given degree.
#define MAXCOEFF (MAXDEG*(MAXDEG+2)+1)

// ---------------------------------------------------------------------------
//! The class models magnetic field of the earth.
/*! The model is valid for a limited time only and needs to be updated
    every couple of years. I hope it is only coefficients that gets updated.

	 This class version is aimed for the uC specific use. Hence all the
    model coefficients are hardcoded in the static const variables.
    (in order to get them to the .text part of the compiled code.)

	 When new coefficients are ready (in 2010, I guess),
	 the following procedure must be used for the update:

	 a) Edit the new model file in such a way, that the
       ReadAndPrintCoefficients() will be able to read it. (I hope,
       no modification will be necessary.)

    b) Check the maximal degree of coefficients used in the new model.
       (first line, third and forth argument). Modify the MAXDEG constant,
       and static variables s_iNM, s_iNC and s_fModelDate.

	 c) Uncomment the ReadAndPrintCoefficients() method in .h and .cpp

	 d) Modify the .cpp part in order to load the proper model file. Check the
       fopen() instruction.

    e) Write a program (or modify an existing one) and call the function to
       get the output to the stdout. A run in a debugger is also OK.

	 f) Copy the output into the source and edit the lines.
		 Include a copy of model file as well (as a comment of course).

	 g) Compile the new version and do not forget to double check the
       test results.
 */

class GeoMagModel
{
public:
	//! Constructor - it sets the model for year 2007.5 (July 2007).
	GeoMagModel();

	//! Calculates internal magnetic field coefficients for given date.
	void SetDate(float fDate);

	//! Evaluates the magnetic field components for given geodetic coordinates.
	void Evaluate(
		float fLat, float fLon, float fElev,
		int   iExt =  0, float fExt1 = 0, float fExt2 = 0,	float fExt3 = 0
	) const;

	//! Returns x (northward) magnetic field component [nT].
	float GetX() const
	{ return m_fx; }
	//! Returns y (eastward) magnetic field component [nT].
	float GetY() const
	{ return m_fy; }
	//! Returns z (downward) magnetic field component [nT].
	float GetZ() const
	{ return m_fz; }

	//! Once x,y,z are known we can calculate the inc, dec [rad] and strength [nT].
	void CalcIDH(float& fInc, float& fDec, float& fStrength) const;
	
	//! Return the first valid date for the model.
	float GetFirstValidDate() const
	{ return s_fModelDate; }
	//! Return the last valid date for the model.
	float GetLastValidDate() const
	{ return s_fModelDate + 5.0f; }
	//! Return true if the date is a valid date.
	bool IsValidDate(float fDate)
	{ return fDate >= GetFirstValidDate() && fDate <= GetLastValidDate(); }

	//! Reads the model coefficients and prints them in the C++ form.
	// void ReadAndPrintCoefficients();
private:
	//! Returns max degree according to the given model.
	int GetMaxDegree() const
	{ return s_iNM > s_iNC ? s_iNM : s_iNC; }

private:
	//! Coefficients of magnetic field for model year (e.g. 2005)
	static const float s_afGHM[MAXCOEFF];
	//! Coefficients of estimated magnetic field change for period (e.g 2005-2010)
	static const float s_afGHC[MAXCOEFF];
	//! Max degree in the main model
	static const int s_iNM;
	//! Max degree in the field change model
	static const int s_iNC;
	//! Model year /in decimal years/ (e.g. 2005)
	static const float s_fModelDate;
	//! Current date for the model.
	static float s_fDate;
	

	//! Coefficients for the given date (today) based on the model.
	float m_afGH[MAXCOEFF];

	//! x (northward) magnetic field component [nT] (result of evaluation).
	mutable float m_fx;
	//! y (eastward) magnetic field component [nT] (result of evaluation).
	mutable float m_fy;
	//! z (downward) magnetic field component [nT] (result of evaluation).
	mutable float m_fz;
};
// ---------------------------------------------------------------------------
