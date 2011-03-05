#ifndef CAN_MESSAGE_AEROSPACE_H
#define CAN_MESSAGE_AEROSPACE_H

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

/** \brief CAN message structure according to the CANaerospace specification.

	 Our CAN aerospace structures follow the specification of CAN identifiers
	 proposed by Stock Flight Systems (version 1.7).
	 See http://www.stockflightsystems.com/ for more details.
 */

#include "Message.h"

namespace can {

// Responce Response
//-----------------------------------------------------------------------------

//! Identifiers in the CANaerospace specification.
enum Id {
	idEE_BootloadError =										127,

	idNS_C00_Request  =										128,
	idNS_C00_Response =										129,

	// -------------------------------------------------

	idFirstFlightStateAirData =							300, //!< Marker.
	idFirstValidFlightStateAirData =						300,

	idBodyLongitudinalAcceleration =						300,
	idBodyLateralAcceleration,
	idBodyNormalAcceleration,

	idBodyPitchRate,
	idBodyRollRate,
	idBodyYawRate,

	idRudderPosition,
	idStabilizerPosition,
	idElevatorPosition,
	idLeftAileronPosition,
	idRightAileronPosition,

	idBodyPitchAngle =										311,	//!< Pitch
	idBodyRollAngle,													//!< Roll

	idBodySideslip,

	idAltitudeRate =											314,
	idIndicatedAirspeed,
	idTrueAirspeed,
	idCalibratedAirspeed,
	idMachNumber,
	idBaroCorrection,
	idBaroCorrectedAltitude,

	idHeadingAngle = 											321,	//!< Yaw

	idStandardAltitude,
	idTotalAirTemperature,
	idStaticAirTemperature,
	idDifferentialPressure,
	idStaticPressure,

	idHeadingRate = 											327,
	idPortSideAOA,
	idStarboardSideAOA,

	idDensityAltitude =										330,
	idTurnCoordinationRate,
	idTrueAltitude =											332,

	idWindSpeed = 												333,
	idWindDirection,

	idOutsideAirTemperature =								335,

	idBodyNormalVelocity,
	idBodyLongitudinalVelocity,
	idBodyLateralVelocity,

	idTotalPressure =											339,

	idLastValidFlightStateAirData =						339,
	idLastFlightStateAirData =								399, //!< Marker.

	// -------------------------------------------------

	idFirstFlightControlsData =							400, //!< Marker.
	idFirstValidFlightControlsData = 					400,

	idPitchControlPosition = 								400,
	idRollControlPosition,
	idLateralStickTrimPosition,
	idYawControlPosition,
	idCollectiveControlPosition,
	idLongitudinalStickTrimPosition,
	idDirectionalPedalsTrimPosition,
	idColectiveLeverTrimPosition,

	idCyclicControlStickSwitches,

	idLateralTrimSpeed,
	idLongitudinalTrimSpeed,
	idPedalTrimSpeed,
	idCollectiveTrimSpeed,

	idNoseWheelSteeringHandlePosition,

	idEngineThrotleLeverPosition_0,
	idEngineThrotleLeverPosition_1,
	idEngineThrotleLeverPosition_2,
	idEngineThrotleLeverPosition_3,
	idEngineConditionLeverPosition_0,
	idEngineConditionLeverPosition_1,
	idEngineConditionLeverPosition_2,
	idEngineConditionLeverPosition_3,

	idEngineThrotleLeverPositionB_0,
	idEngineThrotleLeverPositionB_1,
	idEngineThrotleLeverPositionB_2,
	idEngineThrotleLeverPositionB_3,
	idEngineConditionLeverPositionB_0,
	idEngineConditionLeverPositionB_1,
	idEngineConditionLeverPositionB_2,
	idEngineConditionLeverPositionB_3,

	idFlapsLeverPosition,
	idSlatsLeverPosition,
	idParkBrakeLeverPosition,
	idSpeedBrakeLeverPosition,
	idThrottleMaxLeverPosition,

	idPilotLeftBrakePedalPosition,
	idPilotRightBrakePedalPosition,

	idCoPilotLeftBrakePedalPosition,
	idCoPilotRightBrakePedalPosition,

	idTrimSystemSwitches,
	idTrimSystemLights,

	idCollectiveControlStickSwitches,
	idStickShakerStallWarningDevice = 					442,

	idLastValidFlightControlsData = 						442,
	idLastFlightControlsData =								499, //!< Marker.

	// -------------------------------------------------

	idFirstEngineFuelSupplyData =							500, //!< Marker.
	idFirstValidEngineFuelSupplyData = 					500,

	idEngineN1_0 =												500,
	idEngineN1_1,
	idEngineN1_2,
	idEngineN1_3,
	idEngineN2_0,
	idEngineN2_1,
	idEngineN2_2,
	idEngineN2_3,
	idEngineTorque_0,
	idEngineTorque_1,
	idEngineTorque_2,
	idEngineTorque_3,
	idEngineTurbineInletTemperature_0,
	idEngineTurbineInletTemperature_1,
	idEngineTurbineInletTemperature_2,
	idEngineTurbineInletTemperature_3,
	idEngineInterTurbineTemperature_0,
	idEngineInterTurbineTemperature_1,
	idEngineInterTurbineTemperature_2,
	idEngineInterTurbineTemperature_3,
	idEngineTurbineOutletTemperature_0,
	idEngineTurbineOutletTemperature_1,
	idEngineTurbineOutletTemperature_2,
	idEngineTurbineOutletTemperature_3,
	idEngineFuelFlowRate_0,
	idEngineFuelFlowRate_1,
	idEngineFuelFlowRate_2,
	idEngineFuelFlowRate_3,
	idEngineManifoldPressure_0,
	idEngineManifoldPressure_1,
	idEngineManifoldPressure_2,
	idEngineManifoldPressure_3,
	idEngineOilPressure_0,
	idEngineOilPressure_1,
	idEngineOilPressure_2,
	idEngineOilPressure_3,
	idEngineOilTemperature_0,
	idEngineOilTemperature_1,
	idEngineOilTemperature_2,
	idEngineOilTemperature_3,
	idEngineCylinderHeadTemperature_0,
	idEngineCylinderHeadTemperature_1,
	idEngineCylinderHeadTemperature_2,
	idEngineCylinderHeadTemperature_3,
	idEngineOilQuantity_0,
	idEngineOilQuantity_1,
	idEngineOilQuantity_2,
	idEngineOilQuantity_3,
	idEngineCoolantTemperature_0,
	idEngineCoolantTemperature_1,
	idEngineCoolantTemperature_2,
	idEngineCoolantTemperature_3,
	idEnginePowerRating_0,
	idEnginePowerRating_1,
	idEnginePowerRating_2,
	idEnginePowerRating_3,
	idEngineStatus1_0,
	idEngineStatus1_1,
	idEngineStatus1_2,
	idEngineStatus1_3,
	idEngineStatus2_0,
	idEngineStatus2_1,
	idEngineStatus2_2,
	idEngineStatus2_3,


	idEngineN1B_0 =											564,
	idEngineN1B_1,
	idEngineN1B_2,
	idEngineN1B_3,
	idEngineN2B_0,
	idEngineN2B_1,
	idEngineN2B_2,
	idEngineN2B_3,
	idEngineTorqueB_0,
	idEngineTorqueB_1,
	idEngineTorqueB_2,
	idEngineTorqueB_3,
	idEngineTurbineInletTemperatureB_0,
	idEngineTurbineInletTemperatureB_1,
	idEngineTurbineInletTemperatureB_2,
	idEngineTurbineInletTemperatureB_3,
	idEngineInterTurbineTemperatureB_0,
	idEngineInterTurbineTemperatureB_1,
	idEngineInterTurbineTemperatureB_2,
	idEngineInterTurbineTemperatureB_3,
	idEngineTurbineOutletTemperatureB_0,
	idEngineTurbineOutletTemperatureB_1,
	idEngineTurbineOutletTemperatureB_2,
	idEngineTurbineOutletTemperatureB_3,
	idEngineFuelFlowRateB_0,
	idEngineFuelFlowRateB_1,
	idEngineFuelFlowRateB_2,
	idEngineFuelFlowRateB_3,
	idEngineManifoldPressureB_0,
	idEngineManifoldPressureB_1,
	idEngineManifoldPressureB_2,
	idEngineManifoldPressureB_3,
	idEngineOilPressureB_0,
	idEngineOilPressureB_1,
	idEngineOilPressureB_2,
	idEngineOilPressureB_3,
	idEngineOilTemperatureB_0,
	idEngineOilTemperatureB_1,
	idEngineOilTemperatureB_2,
	idEngineOilTemperatureB_3,
	idEngineCylinderHeadTemperatureB_0,
	idEngineCylinderHeadTemperatureB_1,
	idEngineCylinderHeadTemperatureB_2,
	idEngineCylinderHeadTemperatureB_3,
	idEngineOilQuantityB_0,
	idEngineOilQuantityB_1,
	idEngineOilQuantityB_2,
	idEngineOilQuantityB_3,
	idEngineCoolantTemperatureB_0,
	idEngineCoolantTemperatureB_1,
	idEngineCoolantTemperatureB_2,
	idEngineCoolantTemperatureB_3,
	idEnginePowerRatingB_0,
	idEnginePowerRatingB_1,
	idEnginePowerRatingB_2,
	idEnginePowerRatingB_3,
	idEngineStatus1B_0,
	idEngineStatus1B_1,
	idEngineStatus1B_2,
	idEngineStatus1B_3,
	idEngineStatus2B_0,
	idEngineStatus2B_1,
	idEngineStatus2B_2,
	idEngineStatus2B_3,

	idEngineReservedForFutureUseFirst =					628,
	idEngineReservedForFutureUseLast =					659,

	idFuelPumpFlowRate_0 =									660,
	idFuelPumpFlowRate_1,
	idFuelPumpFlowRate_2,
	idFuelPumpFlowRate_3,
	idFuelPumpFlowRate_4,
	idFuelPumpFlowRate_5,
	idFuelPumpFlowRate_6,
	idFuelPumpFlowRate_7,
	idFuelTankQuantity_0 =									668,
	idFuelTankQuantity_1,
	idFuelTankQuantity_2,
	idFuelTankQuantity_3,
	idFuelTankQuantity_4,
	idFuelTankQuantity_5,
	idFuelTankQuantity_6,
	idFuelTankQuantity_7,
	idFuelTankTemperature_0 = 								676,
	idFuelTankTemperature_1,
	idFuelTankTemperature_2,
	idFuelTankTemperature_3,
	idFuelTankTemperature_4,
	idFuelTankTemperature_5,
	idFuelTankTemperature_6,
	idFuelTankTemperature_7,
	idFuelSystemPressure_0 =								684,
	idFuelSystemPressure_1,
	idFuelSystemPressure_2,
	idFuelSystemPressure_3,
	idFuelSystemPressure_4,
	idFuelSystemPressure_5,
	idFuelSystemPressure_6,
	idFuelSystemPressure_7,

	idLastValidEngineFuelSupplyData =					691,
	idLastEngineFuelSupplyData =							699, //!< Marker.

	// -------------------------------------------------

	idFirstPowerTransmitionData =							700, //!< Marker.
	idFirstValidPowerTransmitionData = 					700,

	idRotorRPM_0 = 											700,
	idRotorRPM_1,
	idRotorRPM_2,
	idRotorRPM_3,
	idGearboxSpeed_0,
	idGearboxSpeed_1,
	idGearboxSpeed_2,
	idGearboxSpeed_3,
	idGearboxSpeed_4,
	idGearboxSpeed_5,
	idGearboxSpeed_6,
	idGearboxSpeed_7,
	idGearboxOilPressure_0,
	idGearboxOilPressure_1,
	idGearboxOilPressure_2,
	idGearboxOilPressure_3,
	idGearboxOilPressure_4,
	idGearboxOilPressure_5,
	idGearboxOilPressure_6,
	idGearboxOilPressure_7,
	idGearboxOilTemperature_0,
	idGearboxOilTemperature_1,
	idGearboxOilTemperature_2,
	idGearboxOilTemperature_3,
	idGearboxOilTemperature_4,
	idGearboxOilTemperature_5,
	idGearboxOilTemperature_6,
	idGearboxOilTemperature_7,
	idGearboxOilQuantity_0,
	idGearboxOilQuantity_1,
	idGearboxOilQuantity_2,
	idGearboxOilQuantity_3,
	idGearboxOilQuantity_4,
	idGearboxOilQuantity_5,
	idGearboxOilQuantity_6,
	idGearboxOilQuantity_7  = 								735,

	idLastValidPowerTransmitionData =					735,
	idLastPowerTransmitionData =							799, //!< Marker.

	// -------------------------------------------------

	idFirstHydraulicData =									800, //!< Marker.
	idFirstValidHydraulicData = 							800,

	idHydraulicSystemPressure_0 = 						800,
	idHydraulicSystemPressure_1,
	idHydraulicSystemPressure_2,
	idHydraulicSystemPressure_3,
	idHydraulicSystemPressure_4,
	idHydraulicSystemPressure_5,
	idHydraulicSystemPressure_6,
	idHydraulicSystemPressure_7,
	idHydraulicSystemFluidTemperature_0,
	idHydraulicSystemFluidTemperature_1,
	idHydraulicSystemFluidTemperature_2,
	idHydraulicSystemFluidTemperature_3,
	idHydraulicSystemFluidTemperature_4,
	idHydraulicSystemFluidTemperature_5,
	idHydraulicSystemFluidTemperature_6,
	idHydraulicSystemFluidTemperature_7,
	idHydraulicSystemFluidQuantity_0,
	idHydraulicSystemFluidQuantity_1,
	idHydraulicSystemFluidQuantity_2,
	idHydraulicSystemFluidQuantity_3,
	idHydraulicSystemFluidQuantity_4,
	idHydraulicSystemFluidQuantity_5,
	idHydraulicSystemFluidQuantity_6,
	idHydraulicSystemFluidQuantity_7 =					823,

	idLastValidHydraulicData = 							823,
	idLastHydraulicData =									899, //!< Marker.

	// -------------------------------------------------

	idFirstElectricData =									900, //!< Marker.
	idFirstValidElectricData = 							900,

	idACSystemVoltage_0 =									900,
	idACSystemVoltage_1,
	idACSystemVoltage_2,
	idACSystemVoltage_3,
	idACSystemVoltage_4,
	idACSystemVoltage_5,
	idACSystemVoltage_6,
	idACSystemVoltage_7,
	idACSystemVoltage_8,
	idACSystemVoltage_9,
	idACSystemCurrent_0,
	idACSystemCurrent_1,
	idACSystemCurrent_2,
	idACSystemCurrent_3,
	idACSystemCurrent_4,
	idACSystemCurrent_5,
	idACSystemCurrent_6,
	idACSystemCurrent_7,
	idACSystemCurrent_8,
	idACSystemCurrent_9,
	idDCSystemVoltage_0,
	idDCSystemVoltage_1,
	idDCSystemVoltage_2,
	idDCSystemVoltage_3,
	idDCSystemVoltage_4,
	idDCSystemVoltage_5,
	idDCSystemVoltage_6,
	idDCSystemVoltage_7,
	idDCSystemVoltage_8,
	idDCSystemVoltage_9,
	idDCSystemCurrent_0,
	idDCSystemCurrent_1,
	idDCSystemCurrent_2,
	idDCSystemCurrent_3,
	idDCSystemCurrent_4,
	idDCSystemCurrent_5,
	idDCSystemCurrent_6,
	idDCSystemCurrent_7,
	idDCSystemCurrent_8,
	idDCSystemCurrent_9,
	idPropIceguardDCCurrent_0,
	idPropIceguardDCCurrent_1,
	idPropIceguardDCCurrent_2,
	idPropIceguardDCCurrent_3,
	idPropIceguardDCCurrent_4,
	idPropIceguardDCCurrent_5,
	idPropIceguardDCCurrent_6,
	idPropIceguardDCCurrent_7,
	idPropIceguardDCCurrent_8,
	idPropIceguardDCCurrent_9 = 							949,

	idLastValidElectricData = 								949,
	idLastElectricData =										999, //!< Marker.

	// -------------------------------------------------

	idFirstNavData =										  1000, //!< Marker.
	idFirstValidNavData = 								  1036,
	// Missing ...
	idGPSAircraftLatitude = 							  1036,
	idGPSAircraftLongitude,
	idGPSAircraftHAE,

	idGPSGroundSpeed =									  1039,
	idGPSTrueTrack,
	idGPSMagneticTrack,
	idGPSCrossTrackError,
	idGPSTrackErrorAngle,

	idGPSGlideslopeDeviation = 						  1044,
	idGPSPredictedRAIM,
	idGPSVerticalFigureOfMerit,
	idGPSHorizontalFigureOfMerit,

	// Modified - returns GPSFix and Satelite count using two shorts
	idGPSModeOfOperation = 								  1048,

	idINSAircraftLatitude = 							  1049,
	idINSAircraftLongitude,
	idINSAircraftHAE,
	idINSGroundSpeed,
	idINSTrueTrack,
	idINSMagneticTrack,
	idINSCrossTrackError,
	idINSTrackErrorAngle,

	idINSVerticalFigureOfMerit = 						  1057,
	idINSHorizontalFigureOfMerit,

	idAuxNavSystemAircraftLatitude =					  1059,
	idAuxNavSystemAircraftLongitude,
	idAuxNavSystemAircraftHAE,
	idAuxNavSystemGroundSpeed,
	idAuxNavSystemTrueTrack,
	idAuxNavSystemMagneticTrack,
	idAuxNavSystemCrossTrackError,
	idAuxNavSystemTrackErrorAngle,

	idAuxNavSystemVerticalFigureOfMerit =			  1067,
	idAuxNavSystemHorizontalFigureOfMerit,

	idMagneticHeading = 									  1069,

	idRadioHeight = 										  1070,

	idDMEDistance_0 =										  1071,
	idDMEDistance_1,
	idDMEDistance_2,
	idDMEDistance_3,

	idDMETimeToGo_0 =										  1075,
	idDMETimeToGo_1,
	idDMETimeToGo_2,
	idDMETimeToGo_3,

	idDMEGroundSpeed_0 =									  1079,
	idDMEGroundSpeed_1,
	idDMEGroundSpeed_2,
	idDMEGroundSpeed_3,

	idADFBearing_0 = 										  1083,
	idADFBearing_1,
	idADFBearing_2,
	idADFBearing_3,

	idILSLocalizeDeviation_0 = 						  1087,
	idILSLocalizeDeviation_1,
	idILSLocalizeDeviation_2,
	idILSLocalizeDeviation_3,

	idILSGlideslopeDeviation_0 = 						  1091,
	idILSGlideslopeDeviation_1,
	idILSGlideslopeDeviation_2,
	idILSGlideslopeDeviation_3,

	idFlightDirectorPitchDeviation_0 =				  1095,
	idFlightDirectorPitchDeviation_1,

	idFlightDirectorRollDeviation_0,
	idFlightDirectorRollDeviation_1,

	idDecisionHeight =									  1099,

	idVHFCOMFrequency_0 =								  1100,
	idVHFCOMFrequency_1,
	idVHFCOMFrequency_2,
	idVHFCOMFrequency_3,

	idVORILSFrequency_0 =								  1104,
	idVORILSFrequency_1,
	idVORILSFrequency_2,
	idVORILSFrequency_3,

	idADFFrequency_0 =									  1108,
	idADFFrequency_1,
	idADFFrequency_2,
	idADFFrequency_3,

	idDMEChannel_0 =										  1112,
	idDMEChannel_1,
	idDMEChannel_2,
	idDMEChannel_3,

	idTransponderCode_0 =								  1116,
	idTransponderCode_1,
	idTransponderCode_2,
	idTransponderCode_3,

	idDesiredTrackAngle = 								  1120,
	idMagneticDeclination,
	idSelectedGlidepathAngle,
	idSelectedRunwayHeading,

	idComputedVerticalVelocity = 						  1124,

	idSelectedCourse = 									  1125,

	idVORRadial_0 =										  1126,
	idVORRadial_1,
	idVORRadial_2,
	idVORRadial_3,

	idTrueEastVelocity =									  1130,
	idTrueNorthVelocity,
	idTrueUpVelocity,

	idTrueHeading =										  1133,

	idLastValidNavData =									  1133,
	idLastNavData =										  1174, //!< Marker.

	// -------------------------------------------------

	idFirstLandingGearData =							  1175, //!< Marker.
	idFirstValidLandingGearData = 					  1175,

	idGearLeverSwitches,
	idGearLeverLights,
	idLandingGearTirePressure_0,
	idLandingGearTirePressure_1,
	idLandingGearTirePressure_2,
	idLandingGearTirePressure_3,
	idLandingGearBreakPadThickness_0,
	idLandingGearBreakPadThickness_1,
	idLandingGearBreakPadThickness_2,
	idLandingGearBreakPadThickness_3 =				  1184,

	idLastValidLandingGearData = 						  1184,
	idLastLandingGearData =								  1199, //!< Marker.

	// -------------------------------------------------

	idFirstMiscellaneousData =							  1200, //!< Marker.
	idFirstValidMiscellaneousData =					  1200,

	idUTC = 													  1200,
	idCabinPressure,
	idCabinAltitude,
	idCabinTemperature,
	idLongitudinalCenterOfGravity,
	idLateralCenterOfGravity,
	idDate =													  1206, //!< Sent by INU (origin from GPS)

	idLastValidMiscellaneousData = 					  1206,
	idLastMiscellaneousData =							  1299, //!< Marker.

	// -------------------------------------------------

	idFirstReservedData =								  1300, //!< Marker.
	idLastReservedData =									  1499, //!< Marker.

	// -------------------------------------------------
	// Kanardia specific NOD
	idFirstKanardiaNOD =									  1500, //!< Marker
	idFirstValidKanardiaNOD = 							  1500,

	idKanDaquCHT =											  1500, //!< CHT (cyl # in service code)
	idKanDaquEGT,													  //!< EGT (cyl # in service code)

	idKanDateJulianDay,											  //!< Date in Julian day coding.
	idKanTimeSec,													  //!< UTC Time of day - seconds after midnight.

	idKanBodyLongitudinalMagneticField =			  1504, //!< Magnetic field x component [nT, raw].
	idKanBodyLateralMagneticField,							  //!< Magnetic field y component [nT, raw].
	idKanBodyNormalMagneticField,								  //!< Magnetic field z component [nT, raw].

	idKanWWMNorthMagneticField,						  		  //!< WMM north component [nT].
	idKanWWMEastMagneticField,							  		  //!< WMM east component [nT].
	idKanWWMDownMagneticField,								     //!< WWM down component [nT].

	idKanEngineTotalTime = 								  1510, //!< Total engine time [decimal h].

	idKanDaquAverageFuelFlow,									  //!< Average fuel flow [l/h]

	idKanBackupBatteryStatus,								     //!< Backup battery status composite.

	idKanGyroBiasRoll =									  1513, //!< Gyro bias around x axis [rad/s]
	idKanGyroBiasPitch,											  //!< Gyro bias around y axis [rad/s]
	idKanGyroBiasYaw,												  //!< Gyro bias around z axis [rad/s]
	
	idKanCarburetorTemp = 								  1516, //!< Carburetor temperature

	idLastValidKanardiaNOD = 							  1516,
	idLastKanardiaNOD =									  1799, //!< Marker
	// End of Kanardia specific NOD
	// -------------------------------------------------

	// Kanardia specific User-Defined Low priority data
	idFirstKanardiaUDL =									  1800, //! Marker
	idFirstValidKanardiaUDL =							  1800,

	idKanMabuData =									  	  1800, //!< Used for Mabu specific communication

	idLastValidKanardiaUDL = 							  1800,
	idLastKanardiaUDL =									  1899, //!< Marker
	// End of Kanardia specific User-Defined Low priority data
	// -------------------------------------------------

	// Kanardia specific Debug Service Data
	idFirstKanardiaDSD =									  1900, //! Marker
	idFirstValidKanardiaDSD =							  1900,

	idConsolePrint	=										  1900, //!< Not quite to CANaerospace specification.

	idLastValidKanardiaDSD = 							  1900,
	idLastKanardiaDSD =									  1999, //!< Marker
	// End of Kanardia specific User-Defined Low priority data
	// -------------------------------------------------

	// Low priority Kanardia specific node services
	idNS_APS_Request  =									  2000, //!< Channel 100 used for Kanardia APS
	idNS_APS_Response =									  2001,

	// End of Low priority Kanardia specific node services
	// -------------------------------------------------
};

//-----------------------------------------------------------------------------
//! Data types used in the CANaerospace specification.

enum DataType {
	dtNoData = 0,
	dtError  = 1,
	dtFloat  = 2,
	dtLong   = 3,
	dtULong  = 4,
	dtBLong  = 5,
	dtShort  = 6,
	dtUShort = 7,
	dtBShort = 8,
	dtChar   = 9,
	dtUChar  = 10,
	dtBChar  = 11,
	dtShort2 = 12,
	dtUShort2 = 13,
	dtBShort2 = 14,
	dtChar4   = 15,
	dtUChar4  = 16,
	dtBChar4  = 17,
	dtChar2   = 18,
	dtUChar2  = 19,
	dtBChar2  = 20,
	dtMemId   = 21,
	dtChksum  = 22,
	dtAChar   = 23,
	dtAChar2  = 24,
	dtAChar4  = 25,
	dtChar3   = 26,
	dtUChar3  = 27,
	dtBChar3  = 28,
	dtAChar3  = 29,
	dtDoubleH = 30,
	dtDoubleL = 31
};

//-----------------------------------------------------------------------------
//! CANaerospace services.

enum Service {
	sIdentification = 								0,		//! "Sign of Life"
	sNodeSynchronisation,
	sDataDownload,
	sDataUpload,
	sSimulationControl,
	sTransmitionInterval,
	sFlashProgramming,
	sStateTransmission,
	sFilterSetting,
	sTestControl,
	sBaudrateSetting,
	sNodeIdSetting,
	sModuleInformation,
	sModuleConfiguration,
	sCANIdSetting,
	sCANIdDistributionSetting,

	sFirstUserService = 							100,		//! User's defined services start here.
	sApplicationProgramming = sFirstUserService,
	sSignOfLife
};

//-----------------------------------------------------------------------------
/*! \brief Extends the Message class with CANaerospace specific data and register access.
 */
class MessageAerospace : public Message
{
public:
	//! Default constructor
	MessageAerospace();

	//! Constructor from the base class
	MessageAerospace(const Message& msg);

	//! Convenience CANaerospace constructor.
	MessageAerospace(Identifier I,
						  DataType edt, uint8_t byServiceCode, uint8_t byMessageCode,
						  Register rData);

	//! Convenience CANaerospace constructor.
	MessageAerospace(Identifier I, Register rHead, Register rData);

	//! Set message from given paramaters.
	void Set(Identifier I,
	         DataType edt, uint8_t byServiceCode, uint8_t byMessageCode,
				Register rData);

	void Set(Identifier I, Register rHeader, Register rData);

	//! Return current counter.
	uint8_t GetCounter() const
	{ return m_rA.by[NODE_ID_COUNTER]; }

	//! Return the CANaerospace DataType enum.
	DataType GetDataType() const
	{ return static_cast<DataType>(m_rA.by[DATA_TYPE_INDEX]); }
	//! Set data type enum.
	void SetDataType(DataType edt)
	{ m_rA.by[DATA_TYPE_INDEX] = (uint8_t)edt; }

	//! Return the CANaerospace service code.
	uint8_t GetServiceCode() const
	{ return m_rA.by[SERVICE_CODE_INDEX]; }
	//! Return the CANaerospace message code.
	uint8_t GetMessageCode() const
	{ return m_rA.by[MESSAGE_CODE_INDEX]; }

	//! Set header register.
	void SetHeader(const Register& rH)
	{ m_rA = rH; }

	//! Return the data register
	const Register& GetData() const
	{ return m_rB; }
	//! Set data register.
	void SetData(const Register& rD)
	{ m_rB = rD; }
};

//-----------------------------------------------------------------------------

} // can namespace

#endif
