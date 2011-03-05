#ifndef __FAST_UNIT_KEYS_H
#define __FAST_UNIT_KEYS_H

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

#include <memory>

class FastUnitKeys
{
private:
	//! Private constructor prevents direct class creation.
	FastUnitKeys();
public:
	//! Public destructor.
	~FastUnitKeys();

	//! The only access to the singleton object.
	static FastUnitKeys* GetInstance();
	
public:
	int iKey_rad;
	int iKey_deg;
	
	int iKey_hPa;
	int iKey_inHg;
	int iKey_psi;
	int iKey_kPa;
	int iKey_bar;
	
	int iKey_m;
	int iKey_ft;
	int iKey_feet;
	int iKey_km;
	int iKey_NM;
	int iKey_SM;
	
	int iKey_knots;
	int iKey_kts;
	int iKey_km_h;
	int iKey_mph;
	int iKey_m_s;
	int iKey_m_min;
	int iKey_ft_min;
	
	int iKey_l;
	int iKey_gal;

	int iKey_l_h;
	int iKey_gal_h;
	int iKey_l_s;
	
	int iKey_kg;
	int iKey_lbs;
	
	int iKey_C;
	int iKey_F;

	int iKey_RPM;

	int iKey_V;

	int iKey_A;

	int iKey_rad_s;
	
	int iKey_s;
	int iKey_min;
	int iKey_h;
private:
	//! The only static singleton object.
	static FastUnitKeys* s_pInstance;
};

#endif
