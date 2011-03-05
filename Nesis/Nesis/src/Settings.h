#ifndef __SETTINGS_H
#define __SETTINGS_H

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
#include <QSettings>
#include <QPoint>
#include <QStringList>
#include <QLocale>
#include "Map/MapDefs.h"

#define HSI_HEADING_UP				0
#define HSI_TRACKING_UP				1
#define HSI_NORTH_UP					2

class Settings
{
private:
	//! Private constructor prevents direct class creation.
	Settings();
public:
	//! Public destructor.
	~Settings();

	//! The only access to the singleton object.
	static Settings* GetInstance();
	
	//! Is a secondary Nesis.
	bool IsSecondary() const;
	//! Is a primary Nesis.
	bool IsPrimary() const
	{ return !IsSecondary(); }
	
	//! Set the nesis home directory. Set this ASAP.
	void SetHome(const QString& qsHome)
	{ m_qsHome = qsHome;	}
	
	//! Return the home directory.
	const QString& GetHome() const
	{ return m_qsHome; }

	//! Save current settings.
	void Save();
	
	//! Where are the Kam files?
	QString GetKamPath() const;
	//! Where are the airspace files?
	QString GetAirspacesPath() const;
	//! Where are the turning points (airfields, navpoints, ...)
	QString GetWaypointsPath() const;
	//! Where are the routes file(s).
	QString GetRoutesPath() const;
	//! Where is the check list xml file.
	QString GetCheckListPath() const;
	//! Where are the instrument images file(s).
	QString GetImagePath() const;
	//! Where are the instrument images file(s).
/*	QString GetInstrumentPath() const;*/
	//! Where are the log files.
	QString GetLogPath() const;
	//! Where are the help files.
	QString GetHelpPath() const;
	//! Where are the airplane/engine xml files.
	QString GetAirplanePath() const;
	//! Where are the uC and program binaries.
	QString GetFirmwarePath() const;
	//! Where is the MD5 file of eveything.
	QString GetMD5File() const;
	
	//! Get instrument gauge round background file.
	QString GetGaugeRoundBackgroundFile() const;
	
	//! Screen resolution - pixels per meter.
	int GetScreenResolution() const;
	
	//! Return last log record id. Returns 0 if no such id is found.
	int GetLastLogRecordId() const;
	//! Increments last log record id for one and stores this. Returns the new number.
	int IncreaseLogRecordId(int iInc=1);

	//! Get default "start" position in WGS84 - decimal degrees.
	QPointF GetMapStartPosition() const;
	//! Set default "start" position in WGS84 - decimal degrees.
	void SetMapStartPosition(const QPointF& ptPosDeg);

	//! Get projection.
	map::Projection GetProjection() const;
	//! Set projection.
	void SetProjection(map::Projection eProj);

	//! Get current pilot.
	QString GetCurrentPilot() const;
	//! Set current pilot.
	void SetCurrentPilot(const QString& qsPilot);

	//! Get airplane registration number.
	QString GetAirplaneRegistration();
	//! Set airplane registration number.
	void SetAirplaneRegistration(const QString& qsRegNumber);

	//! Get user interface language.
	QLocale::Language GetUserInterfaceLanguage() const;
	//! Set user interface language.
	void SetUserInterfaceLanguage(QLocale::Language lan);
	
	//! Get local time vs. UTC time difference in minutes (to be converted into hours).
	int GetLocalTime2UTCDifferenceMinutes() const;
	//! Set local time vs. UTC time difference in minutes.
	void SetLocalTime2UTCDifferenceMinutes(int iMinutes);

	//! Get altitude unit.
	int GetAltitudeUnit() const;
	//! Set altitude unit.
	void SetAltitudeUnit(int iKey);

	//! Get airspace altitude unit.
	int GetAirspaceAltitudeUnit() const;
	//! Set airspace altitude unit.
	void SetAirspaceAltitudeUnit(int iKey);

	//! Get airspeed unit.
	int GetAirspeedUnit() const;
	//! Set airspeed unit.
	void SetAirspeedUnit(int iKey);

	//! Get QNH pressure unit.
	int GetQNHUnit() const;
	//! Set QNH pressure unit.
	void SetQNHUnit(int iKey);

	//! Get vario unit.
	int GetVarioUnit() const;
	//! Set vario unit.
	void SetVarioUnit(int iKey);

	//! Get distance unit.
	int GetDistanceUnit() const;
	//! Set distance unit.
	void SetDistanceUnit(int iKey);

	//! Get engine pressures unit.
	int GetEnginePressureUnit() const;
	//! Set engine pressures unit.
	void SetEnginePressureUnit(int iKey);

	//! Get temperature unit.
	int GetTemperatureUnit() const;
	//! Set engine pressures unit.
	void SetTemperatureUnit(int iKey);

	//! Get mass unit.
	int GetMassUnit() const;
	//! Set mass unit.
	void SetMassUnit(int iKey);

	//! Get fuel volume unit.
	int GetFuelVolumeUnit() const;
	//! Set mass unit.
	void SetFuelVolumeUnit(int iKey);

	//! Get fuel flow unit.
	int GetFuelFlowUnit() const;
	//! Set mass unit.
	void SetFuelFlowUnit(int iKey);
	
	//! Get wind speed unit.
	int GetWindspeedUnit() const;
	//! Set wind speed unit.
	void SetWindspeedUnit(int iKey);

	//! Get cruising speed of flight planning (in m/s)
	float GetCruisingSpeed() const;
	//! Store cruising speed of flight planning (in m/s)
	void SetCruisingSpeed(float fSpeed);
	
	//! Get default waypoint search radius (in km).
	int GetWaypointSearchRadius() const;
	//! Store (Set) default waypoint search radius (in km).
	void SetWaypointSearchRadius(int iRadiusKm);
	
	//! Set when to switch to the next leg radius (meters)
	void SetSwitchToNextLegRadius(float fRadius_m);
	//! Get radius, which tells when to switch to the next leg (meters)
	float GetSwitchToNextLegRadius() const;
	
	//! Set type of the HSI map
	void SetHSIMapType(int iHTN);
	//! Get type of the HSI map (heading up, tracking up, north up)
	int GetHSIMapType() const;
	
	//! Set current airplane/engine xml file name.
	void SetAirplaneEngine(const QString& qsFile);
	//! Get current airplane/engine xml file name.
	QString GetAirplaneEngine() const;
	
	//! Set user alarm selection.
	void EnableUserAlarm(int iAlarm, bool bEnable);
	//! Is user alarm enabled? (default = true)
	bool IsUserAlarmEnabled(int iAlarm);
	
private:
	//! The only static singleton object.
	static Settings* s_pInstance;
	
	//! The home directory. It must be set ASAP - in main().
	QString m_qsHome;
	
	//! The kam files folder (directory).
	QString m_qsKamHome;
	
	//! The qt class which stores the settings.
	QSettings m_settings;
};

#endif
