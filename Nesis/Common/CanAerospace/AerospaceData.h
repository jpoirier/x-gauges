#ifndef CAN_AEROSPACE_H
#define CAN_AEROSPACE_H

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

/** \brief CAN aerospace structures

	 Our CAN aerospace structures follow the specification of CAN identifiers
	 proposed by Stock Flight Systems (version 1.7).
	 See http://www.stockflightsystems.com/ for more details.
 */


#include <stdint.h>

// TODO Check defines
#define MAX_FUEL	8
#define MAX_VOR	4
#define MAX_ILS	4
#define MAX_ADF	4
#define MAX_FD		2
#define MAX_COM	4
#define MAX_TRANSPONDER	4

#define MAX_ENGINES			4
#define MAX_GEARBOXES		8
#define MAX_HYDRAULIC		8
#define MAX_ELECTRIC			10

#define MAX_GEAR				3

// DAQU specific defines
#define MAX_DAQU_CYLINDER	8

namespace can {

//-----------------------------------------------------------------------------

struct FlightStateAirData
{
	FlightStateAirData();

	float fBodyLongitudinalAcceleration;				//!< [g], forward +
	float fBodyLateralAcceleration;						//!< [g], right +
	float fBodyNormalAcceleration;						//!< [g], up +

	float fBodyPitchRate;									//!< [deg/s], nose up +
	float fBodyRollRate;										//!< [deg/s], roll right +
	float fBodyYawRate;										//!< [deg/s], yaw right +

	float fRudderPosition;									//!< [deg], clockwise +
	float fStabilizerPosition;								//!< [deg], downward +
	float fElevatorPosition;								//!< [deg], downward +
	float fLeftAileronPosition;							//!< [deg], downward +
	float fRightAileronPosition;							//!< [deg], downward +

	float fBodyPitchAngle;									//!< [deg], nose up +
	float fBodyRollAngle;									//!< [deg], roll right +
	float fBodySideSlip;										//!< [deg], yaw right +

	float fAltitudeRate;										//!< [m/s]
	float fIndicatedAirspeed;								//!< [m/s]
	float fTrueAirspeed;										//!< [m/s]
	float fCalibratedAirspeed;								//!< [m/s]
	float fMachNumber;										//!< [Mach]

	float fBaroCorrection;									//!< [hPa]
	float fBaroCorrectedAltitude;							//!< [m]

	float fHeadingAngle;										//!< [deg]

	float fStandardAltitude;								//!< [m]
	float fTotalAirTemperature;							//!< [K]
	float fStaticAirTemperature;							//!< [K]

	float fDifferentialPressure;							//!< [hPa]
	float fStaticPressure;									//!< [hPa]

	float fHeadingRate;										//!< [deg/s]

	float fPortSideAOA;										//!< [deg], nose up + (left)
	float fStarboardSideAOA;								//!< [deg], nose up + (right)

	float fDensityAltitude;									//!< [m]
	float fTurnCoordinationRate;							//!< [deg/s]
	float fTrueAltitude;										//!< [m]
	float fWindSpeed;											//!< [m/s]
	float fWindDirection;									//!< [deg]
	float fOutsideAirTemperature;							//!< [K]

	float fBodyNormalVelocity;								//!< [m/s]
	float fBodyLongitudinalVelocity;						//!< [m/s]
	float fBodyLateralVelocity;							//!< [m/s]
	float fTotalPressure;									//!< [hPa]
};

//-----------------------------------------------------------------------------

struct FlightControlsData
{
	FlightControlsData();

	float fPitchControlPosition;								//!< [Norm] +-1, + aft
	float fRollControlPosition;								//!< [Norm] +-1, + right
	float fLateralStickTrimPosition;							//!< [Norm] +-1, + right
	float fYawControlPosition;									//!< [Norm] +-1, + right
	float fCollectiveControlPosition;						//!< [Norm] +-1, + up
	float fLongitudinalStickTrimPosition;					//!< [Norm] +-1, + aft
	float fDirectionalPedalsTrimPosition;					//!< [Norm] +-1, + right pedal
	float fColectiveLeverTrimPosition;						//!< [Norm] +-1, + up

	uint32_t  uiCyclicControlStickSwitches;							//!< [SWITCH]

	float fLateralTrimSpeed;										//!< [Norm] +-1
	float fLongitudinalTrimSpeed;									//!< [Norm] +-1
	float fPedalTrimSpeed;											//!< [Norm] +-1
	float fCollectiveTrimSpeed;									//!< [Norm] +-1

	float fNoseWheelSteeringHandlePosition;					//!< [Norm] +-1, + right

	float afEngineThrotleLeverPosition[MAX_ENGINES];		//!< [Norm] +-1
	float afEngineConditionLeverPosition[MAX_ENGINES];		//!< [Norm] +-1

	float afEngineThrotleLeverPositionB[MAX_ENGINES];		//!< [Norm] +-1
	float afEngineConditionLeverPositionB[MAX_ENGINES];	//!< [Norm] +-1

	float fFlapsLeverPosition;										//!< [Norm] +-1
	float fSlatsLeverPosition;										//!< [Norm] +-1
	float fParkBrakeLeverPosition;								//!< [Norm] +-1
	float fSpeedBrakeLeverPosition;								//!< [Norm] +-1
	float fThrottleMaxLeverPosition;								//!< [Norm] +-1

	float fPilotLeftBrakePedalPosition;							//!< [Norm] +-1
	float fPilotRightBrakePedalPosition;						//!< [Norm] +-1

	float fCoPilotLeftBrakePedalPosition;						//!< [Norm] +-1
	float fCoPilotRightBrakePedalPosition;						//!< [Norm] +-1

	uint32_t  uiTrimSystemSwitches;									//!< [SWITCH]
	uint32_t  uiTrimSystemLights;										//!< [SWITCH]

	uint32_t  uiCollectiveControlStickSwitches;					//!< [SWITCH]
	uint32_t  uiStickShakerStallWarningDevice;						//!< [SWITCH]
};

//-----------------------------------------------------------------------------

struct EngineFuelSupplyData
{
	EngineFuelSupplyData();

	float afEngineN1[MAX_ENGINES];								//!< [1/min] N1 jet, crankshaft RPM piston
	float afEngineN2[MAX_ENGINES];								//!< [1/min] N2 jet, propeller RPM piston
	float afEngineTorque[MAX_ENGINES];							//!< [Norm -1,+1]
	float afEngineTurbineInletTemperature[MAX_ENGINES];	//!< [K]
	float afEngineInterTurbineTemperature[MAX_ENGINES];	//!< [K]
	float afEngineTurbineOutletTemperature[MAX_ENGINES];	//!< [K]
	float afEngineFuelFlowRate[MAX_ENGINES];					//!< [l/h]
	float afEngineManifoldPressure[MAX_ENGINES];				//!< [hPa]
	float afEngineOilPressure[MAX_ENGINES];					//!< [hPa]
	float afEngineOilTemperature[MAX_ENGINES];				//!< [K]
	float afEngineCylinderHeadTemperature[MAX_ENGINES];	//!< [K]
	float afEngineOilQuantity[MAX_ENGINES];					//!< [l]
	float afEngineCoolantTemperature[MAX_ENGINES];			//!< [K]
	float afEnginePowerRating[MAX_ENGINES];					//!< [Norm -1/+1]
	uint32_t  auiEngineStatus1[MAX_ENGINES];						//!< bit encoded, user defined.
	uint32_t  auiEngineStatus2[MAX_ENGINES];						//!< bit encoded, user defined.

/*	float afEngineN1B[MAX_ENGINES];								//!< [1/min] N1 jet, crankshaft RPM piston
	float afEngineN2B[MAX_ENGINES];								//!< [1/min] N2 jet, propeller RPM piston
	float afEngineTorqueB[MAX_ENGINES];							//!< [Norm -1,+1]
	float afEngineTurbineInletTemperatureB[MAX_ENGINES];	//!< [K]
	float afEngineInterTurbineTemperatureB[MAX_ENGINES];	//!< [K]
	float afEngineTurbineOutletTemperatureB[MAX_ENGINES];	//!< [K]
	float afEngineFuelFlowRateB[MAX_ENGINES];					//!< [l/h]
	float afEngineManifoldPressureB[MAX_ENGINES];			//!< [hPa]
	float afEngineOilPressureB[MAX_ENGINES];					//!< [hPa]
	float afEngineOilTemperatureB[MAX_ENGINES];				//!< [K]
	float afEngineCylinderHeadTemperatureB[MAX_ENGINES];	//!< [K]
	float afEngineOilQuantityB[MAX_ENGINES];					//!< [l]
	float afEngineCoolantTemperatureB[MAX_ENGINES];			//!< [K]
	float afEnginePowerRatingB[MAX_ENGINES];					//!< [Norm -1/+1]
	uint32_t  auiEngineStatus1B[MAX_ENGINES];						//!< bit encoded, user defined.
	uint32_t  auiEngineStatus2B[MAX_ENGINES];						//!< bit encoded, user defined.

	uint32_t  auiReservedForFutureUse[32];							//!< reserved
*/
	float afFuelPumpFlowRate[MAX_FUEL];							//!< [l/h]
	float afFuelTankQuantity[MAX_FUEL];							//!< [l]
	float afFuelTankTemperature[MAX_FUEL];						//!< [K]
	float afFuelSystemPressure[MAX_FUEL];						//!< [hPa]
};

//-----------------------------------------------------------------------------

struct PowerTransmitionData
{
	PowerTransmitionData();

	float afRotorRPM[MAX_ENGINES];								//!< [1/min], helicopters only
	float afGearboxSpeed[MAX_GEARBOXES];						//!< [1/min]
	float afGearboxOilPressure[MAX_GEARBOXES];				//!< [hPa]
	float afGearboxOilTemperature[MAX_GEARBOXES];			//!< [K]
	float afGearboxOilQuantity[MAX_GEARBOXES];				//!< [l]
};

//-----------------------------------------------------------------------------

struct HydraulicData
{
	HydraulicData();

	float afHydraulicSystemPressure[MAX_HYDRAULIC];				//!< [hPa]
	float afHydraulicSystemFluidTemperature[MAX_HYDRAULIC];	//!< [K]
	float afHydraulicSystemFluidQuantity[MAX_HYDRAULIC];		//!< [l]

};

//-----------------------------------------------------------------------------

struct ElectricData {
	ElectricData();

	float afACSystemVoltage[MAX_ELECTRIC];						//!< [V]
	float afACSystemCurrent[MAX_ELECTRIC];						//!< [A]
	float afDCSystemVoltage[MAX_ELECTRIC];						//!< [V]
	float afDCSystemCurrent[MAX_ELECTRIC];						//!< [A]
	float afPropIceguardDCCurrent[MAX_ELECTRIC];				//!< [A]
} ;

//-----------------------------------------------------------------------------

struct NavData
{
	NavData();

	float fGPSAircraftLatitude;									//!< [deg]
	float fGPSAircraftLongitude;									//!< [deg]
	float fGPSAircraftHeightAboveEllipsoid;					//!< [m]
	float fGPSGroundSpeed;											//!< [m/s]
	float fGPSTrueTrack;												//!< [deg]
	float fGPSMagneticTrack;										//!< [deg]
	float fGPSCrossTrackError;										//!< [m]
	float fGPSTrackErrorAngle;										//!< [deg]

	float fGPSGlideslopeDeviation;								//!< [m]
	uint32_t  uiGPSPredictedRAIM;										//!< []
	float fGPSVerticalFigureOfMerit;								//!< [m]
	float fGPSHorizontalFigureOfMerit;							//!< [m]

	uint32_t uiGPSModeOfOperation;								//!< [2xSHORT]

	float fINSAircraftLatitude;									//!< [deg]
	float fINSAircraftLongitude;									//!< [deg]
	float fINSAircraftHeightAboveEllipsoid;					//!< [m]
	float fINSGroundSpeed;											//!< [m/s]
	float fINSTrueTrack;												//!< [deg]
	float fINSMagneticTrack;										//!< [deg]
	float fINSCrossTrackError;										//!< [m]
	float fINSTrackErrorAngle;										//!< [deg]

	float fINSVerticalFigureOfMerit;								//!< [m]
	float fINSHorizontalFigureOfMerit;							//!< [m]

	float fAuxNavSystemAircraftLatitude;						//!< [deg]
	float fAuxNavSystemAircraftLongitude;						//!< [deg]
	float fAuxNavSystemAircraftHeightAboveEllipsoid;		//!< [m]
	float fAuxNavSystemGroundSpeed;								//!< [m/s]
	float fAuxNavSystemTrueTrack;									//!< [deg]
	float fAuxNavSystemMagneticTrack;							//!< [deg]
	float fAuxNavSystemCrossTrackError;							//!< [m]
	float fAuxNavSystemTrackErrorAngle;							//!< [deg]

	float fAuxNavSystemVerticalFigureOfMerit;					//!< [m]
	float fAuxNavSystemHorizontalFigureOfMerit;				//!< [m]

	float fMagneticHeading;											//!< [deg]

	float fRadioHeight;												//!< [m]

	float afDMEDistance[MAX_VOR];									//!< [m]
	int	aiDMETimeToGo[MAX_VOR];									//!< [min]
	float afDMEGroundSpeed[MAX_VOR];								//!< [m/s]

	float afADFBearing[MAX_ADF];									//!< [deg]

	float afILSLocalizeDeviation[MAX_ILS];						//!< [deg]
	float afILSGlideslopeDeviation[MAX_ILS];					//!< [deg]

	float afFlightDirectorPitchDeviation[MAX_FD];			//!< [deg]
	float afFlightDirectorRollDeviation[MAX_FD];				//!< [deg]

	float fDecisionHeight;											//!< [m]

	float afVHFCOMFrequency[MAX_COM];							//!< [MHz]
	float afVORILSFrequency[MAX_VOR];							//!< [MHz]
	float afADFFrequency[MAX_ADF];								//!< [kHz]
	float afDMEChannel[MAX_VOR];

	float afTransponderCode[MAX_TRANSPONDER];

	float fDesiredTrackAngle;										//!< [deg]
	float fMagneticDeclination;									//!< [deg]
	float fSelectedGlidepathAngle;								//!< [deg]
	float fSelectedRunwayHeading;									//!< [deg]

	float fComputedVerticalVelocity;								//!< [m/s]

	float fSelectedCourse;											//!< [deg]

	float afVORRadial[MAX_VOR];									//!< [deg]

	float fTrueEastVelocity;										//!< [m/s]
	float fTrueNorthVelocity;										//!< [m/s]
	float fTrueUpVelocity;											//!< [m/s]

	float fTrueHeading;												//!< [deg] +-180
};

//-----------------------------------------------------------------------------

struct LandingGearData
{
	LandingGearData();

	uint32_t  uiGearLeverSwitches;										//!< bit encoded
	uint32_t	uiGearLeverLights;										//!< bit encoded
	float afLandingGearTirePressure[MAX_GEAR];				//!< [hPa], service code contains tire number.
	float afLandingGearBreakPadThickness[MAX_GEAR];			//!< [mm], service code contains pad number.
};

//-----------------------------------------------------------------------------

struct MiscellaneousData
{
	MiscellaneousData();

	char  acUTC[4];													//!< [hh][mm][ss][00]
	float fCabinPressure;											//!< [hPa]
	float fCabinAltitude;											//!< [m]
	float fCabinTemperature;										//!< [K]
	float fLongitudinalCenterOfGravity;							//!< [% MAC]
	float fLateralCenterOfGravity;								//!< [% MAC]
	char  acDate[4];													//!< [dd][mm][yy][yy]
};

//-----------------------------------------------------------------------------

/** \brief Kanardia specific data that can't be found in the CANaerospace.

	 Here we extend the engine information (for single engine) with temperatures
	 of individual cylinders and exhaust gas temperatures.

	 We also introduce all other material that can't be found in CANaerospace,
	 but it is needed.
 */

struct KanardiaData
{
	KanardiaData();

	float afCHT[MAX_DAQU_CYLINDER]; 			//!< [K] Temperature of individual CHT's
	float afEGT[MAX_DAQU_CYLINDER]; 			//!< [K] Temperature of individual EGT's

	uint32_t uiJulianDayGPS; 				//!< Date in Julian day coding (GPS provided)
	float fTimeSecGPS;					      //!< Time of day - seconds after midnight (GPS provided)

	float fBodyLongitudinalMagneticField;	//!< Magnetic field x component.
	float fBodyLateralMagneticField;			//!< Magnetic field y component.
	float fBodyNormalMagneticField;			//!< Magnetic field z component.

	float fWWMNorthMagneticField;				//!< WMM north component [nT].
	float fWWMEastMagneticField;				//!< WMM east component [nT].
	float fWWMDownMagneticField;				//!< WWM down component [nT].

	float fEngineTotalTime;						//!< Engine total time [h].

	float fAverageFuelFlow;						//!< Engine average fuel flow [l/h].
	
	uint8_t abyBackupBatteryStatus[4];		//!< Backup battery state [mode, V, T, %].
	
	float fGyroBiasRoll;							//!< Gyro bias around x axis [rad/s].
	float fGyroBiasPitch;						//!< Gyro bias around y axis [rad/s].
	float fGyroBiasYaw;							//!< Gyro bias around z axis [rad/s].
	
	float fCarburetorTemp;						//!< Temperature related to carburetor [C].
};


//-----------------------------------------------------------------------------
} // can namespace

#endif
