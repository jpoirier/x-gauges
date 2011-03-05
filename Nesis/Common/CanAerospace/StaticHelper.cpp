/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "Defines.h"
#include "KanardiaHardwareUnits.h"
#include "StaticHelper.h"

namespace can {

const char* apcDataTypes[] = {
	"No Data",
	"Error",
	"Float",
	"Long",
	"ULong",
	"BLong",
	"Short",
	"UShort",
	"BShort",
	"Char",
	"UChar",
	"BChar",
	"Short2",
	"UShort2",
	"BShort2",
	"Char4",
	"UChar4",
	"BChar4",
	"Char2",
	"UChar2",
	"BChar2",
	"MemId",
	"Chksum",
	"AChar",
	"AChar2",
	"AChar4",
	"Char3",
	"UChar3",
	"BChar3",
	"AChar3",
	"DoubleH",
	"DoubleL"
};


const char* GetDataTypeText(DataType eDataType)
{
	if(eDataType < (int)COUNTOF(apcDataTypes))
		return apcDataTypes[eDataType];
	else
		return "N/A";
}

//-----------------------------------------------------------------------------

const char* GetHardwareName(uint8_t byId)
{
	static const char* pacNames[] = {
		"Invalid unit",
		"Magu",
		"Daqu",
		"Airu",
		"Mabu",
		"Mabu Sec",
		"Seru",
		"Flu",
		"Canu",
		"Kalu",
		"Nesis",
		"Nesis Sec"
	};

	if(byId <= KHU_LAST_uC_UNIT)
		return pacNames[byId];
	else
		return "Unknown unit";
}

//-----------------------------------------------------------------------------

struct Vendor
{
	const char* pcName;
	uint32_t    uiId;
};

Vendor aVendors[] = {
	{ "Kanardia",  0x16748a73 },
	{ "FlyTech",   0x36020a26 },
	{ "Pipistrel", 0x77551400 }
};


int GetVendorCount()
{
	return COUNTOF(aVendors);
}

uint32_t GetVendorHash(int i)
{
	if(i<GetVendorCount())
		return aVendors[i].uiId;
	return 0;
}

const char* GetVendorName(int i)
{
	if(i<GetVendorCount())
		return aVendors[i].pcName;
	return NULL;
}

int FindVendorIndex(uint32_t uiHash)
{
	for(int i=0; i<GetVendorCount(); i++)
		if(GetVendorHash(i) == uiHash)
			return i;
	return -1; // Not found.
}

//-----------------------------------------------------------------------------

} // can namespace

