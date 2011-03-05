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

#include <cstring>
#include "AerospaceData.h"

namespace can {

//-----------------------------------------------------------------------------

FlightStateAirData::FlightStateAirData()
{
//	memset(this, sizeof(FlightStateAirData), 0);
	float* pF = &fBodyLongitudinalAcceleration;
	while(pF <= &fTotalPressure) {
		*pF = 0.0f;
		 pF++; 
	}
	
	fBaroCorrection = -999.0f;
};

//-----------------------------------------------------------------------------

FlightControlsData::FlightControlsData()
{
//	memset(this, sizeof(FlightControlsData), 0);
	float* pF = &fPitchControlPosition;
	while(pF <= &fColectiveLeverTrimPosition) {
		*pF = 0.0f;
		 pF++; 
	}
	
	uiCyclicControlStickSwitches = 0;
	
	pF = &fLateralTrimSpeed;
	while(pF <= &fCoPilotRightBrakePedalPosition) {
		*pF = 0.0f;
		 pF++; 
	}	
	
	uiTrimSystemSwitches = 0;
	uiTrimSystemLights   = 0;
	uiCollectiveControlStickSwitches = 0;
	uiStickShakerStallWarningDevice = 0;
}

//-----------------------------------------------------------------------------

EngineFuelSupplyData::EngineFuelSupplyData()
{
//	memset(this, sizeof(EngineFuelSupplyData), 0);
	float* pF;
	pF = &afEngineN1[0];
	while(pF < &afEnginePowerRating[MAX_ENGINES]) {
		*pF = 0.0f;
		 pF++; 
	}

	uint32_t* pU;
	pU = &auiEngineStatus1[0];
	while(pU < &auiEngineStatus2[MAX_ENGINES]) {
		*pU = 0;
		 pU++; 
	}
	
/*	pF = &afEngineN1B[0];
	while(pF < &afEnginePowerRatingB[MAX_ENGINES]) {
		*pF = 0.0f;
		 pF++; 
	}
	
	pU = &auiEngineStatus1B[0];
	while(pU < &auiReservedForFutureUse[32]) {
		*pU = 0;
		 pU++; 
	}*/

	pF = &afFuelPumpFlowRate[0];
	while(pF < &afFuelSystemPressure[MAX_FUEL]) {
		*pF = 0.0f;
		 pF++; 
	}
	
	afFuelTankQuantity[0] = -999.0f;
}

//-----------------------------------------------------------------------------

PowerTransmitionData::PowerTransmitionData()
{
//	memset(this, sizeof(PowerTransmitionData), 0);

	float* pF;
	pF = &afRotorRPM[0];
	while(pF < &afGearboxOilQuantity[MAX_GEARBOXES]) {
		*pF = 0.0f;
		 pF++; 
	}
}

//-----------------------------------------------------------------------------

HydraulicData::HydraulicData()
{
//	memset(this, sizeof(HydraulicData), 0);

	float* pF;
	pF = &afHydraulicSystemPressure[0];
	while(pF < &afHydraulicSystemFluidQuantity[MAX_HYDRAULIC]) {
		*pF = 0.0f;
		 pF++; 
	}
}

//-----------------------------------------------------------------------------

ElectricData::ElectricData()
{
//	memset(this, sizeof(ElectricData), 0);
	
	float* pF;
	pF = &afACSystemVoltage[0];
	while(pF < &afPropIceguardDCCurrent[MAX_ELECTRIC]) {
		*pF = 0.0f;
		 pF++; 
	}
}

//-----------------------------------------------------------------------------

NavData::NavData()
{
//	memset(this, sizeof(NavData), 0);

	float* pF;
	pF = &fGPSAircraftLatitude;
	while(pF <= &fTrueHeading) {
		*pF = 0.0f;
		 pF++; 
	}
	
	uiGPSPredictedRAIM = 0;
	uiGPSModeOfOperation = 0;
	
	for(int i=0; i<MAX_VOR; i++)
		aiDMETimeToGo[i] = 0;
}

//-----------------------------------------------------------------------------

LandingGearData::LandingGearData()
{
//	memset(this, sizeof(LandingGearData), 0);

	uiGearLeverSwitches = 0;
	uiGearLeverLights = 0;
	
	float* pF;
	pF = &afLandingGearTirePressure[0];
	while(pF < &afLandingGearBreakPadThickness[MAX_GEAR]) {
		*pF = 0.0f;
		 pF++; 
	}
}

//-----------------------------------------------------------------------------

MiscellaneousData::MiscellaneousData()
{
//	memset(this, sizeof(MiscellaneousData), 0);
	
	for(int i=0; i<4; i++) {
		acUTC[i] = 0;
		acDate[i] = 0;
	}
	
	fCabinPressure = 0.0f;
	fCabinAltitude = 0.0f;
	fCabinTemperature = 0.0f;
	fLongitudinalCenterOfGravity = 0.0f;
	fLateralCenterOfGravity = 0.0f;
}

//-----------------------------------------------------------------------------

KanardiaData::KanardiaData()
{
	memset(this, 0, sizeof(KanardiaData));

/*	float* pF;
	pF = &afCHT[0];
	while(pF < &afEGT[MAX_DAQU_CYLINDER]) {
		*pF = 0.0f;
		 pF++; 
	}
	
	uiJulianDayGPS = 0;*/
}

//-----------------------------------------------------------------------------
} // can namespace
