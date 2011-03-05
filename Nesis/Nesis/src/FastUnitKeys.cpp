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

#include <QtDebug>
#include "Unit/Manager.h"
#include "FastUnitKeys.h"

FastUnitKeys* FastUnitKeys::s_pInstance = NULL;

// -------------------------------------------------------------------------

FastUnitKeys::FastUnitKeys()
{
	using namespace unit;
	Manager* pM = Manager::GetInstance();
	
	iKey_rad 		= pM->GetKey("rad");
	iKey_deg 		= pM->GetKey("deg");
	
	iKey_hPa 		= pM->GetKey("hPa");
	iKey_inHg 		= pM->GetKey("inHg");
	iKey_psi 		= pM->GetKey("psi");
	iKey_kPa 		= pM->GetKey("kPa");
	iKey_bar			= pM->GetKey("bar");
	
	iKey_m 			= pM->GetKey("m");
	iKey_ft			= pM->GetKey("ft");
	iKey_feet		= pM->GetKey("feet"); // same as above
	iKey_km 			= pM->GetKey("km");
	iKey_NM 			= pM->GetKey("NM");
	iKey_SM 			= pM->GetKey("SM");
	
	iKey_knots		= pM->GetKey("knots");
	iKey_kts			= pM->GetKey("kts");	 // same as above
	
	iKey_km_h		= pM->GetKey("km/h");
	iKey_mph			= pM->GetKey("mph");
	iKey_m_s			= pM->GetKey("m/s");
	iKey_m_min		= pM->GetKey("m/min");
	iKey_ft_min		= pM->GetKey("ft/min");
	
	iKey_l			= pM->GetKey("l");
	iKey_gal			= pM->GetKey("gal");

	iKey_l_h			= pM->GetKey("l/h");
	iKey_gal_h		= pM->GetKey("gal/h");
	iKey_l_s			= pM->GetKey("l/s");

	iKey_kg			= pM->GetKey("kg");
	iKey_lbs			= pM->GetKey("lbs");
	
	iKey_C			= pM->GetKey("C");
	iKey_F			= pM->GetKey("F");

	iKey_RPM			= pM->GetKey("RPM");

	iKey_V			= pM->GetKey("V");

	iKey_A			= pM->GetKey("A");

	iKey_rad_s		= pM->GetKey("rad/s");
	
	iKey_s			= pM->GetKey("s");
	iKey_min			= pM->GetKey("min");
	iKey_h			= pM->GetKey("h");
}

FastUnitKeys::~FastUnitKeys()
{
}

// -------------------------------------------------------------------------

FastUnitKeys* FastUnitKeys::GetInstance()
{
	if(s_pInstance==NULL)
		s_pInstance = new FastUnitKeys();
	return s_pInstance;
}

// -------------------------------------------------------------------------
