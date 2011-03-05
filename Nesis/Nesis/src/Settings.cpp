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
#include <QPoint>
#include <QApplication>
#include "Settings.h"
#include "Unit/Manager.h"

#define SCREEN_RESOLUTION						"Screen_Resolution"

#define MAP_START_POSITION						"Map_Start_Position"
#define MAP_PROJECTION							"Map_Projection"
#define HOME_FOLDER								"Home_Folder"
#define MAP_FOLDER								"Map_Folder"

#define LOG_RECORD_ID							"Log_Record_Id"

#define CURRENT_PILOT							"Current_Pilot"
#define AIRPLANE_REGISTRATION					"Airplane_Registration"
#define CRUISING_SPEED							"Cruising_Speed"
#define WAYPOINT_SEARCH_RADIUS				"Waypoint_Search_Radius"

#define LANGUAGE									"Language"
#define UTC_DIFFERENCE							"UTC_Difference"
#define UNIT_ALTITUDE							"Unit_Altitude"
#define UNIT_AIRSPACE_ALT						"Unit_Airspace_Alt"
#define UNIT_AIRSPEED							"Unit_Airspeed"
#define UNIT_WINDSPEED							"Unit_Windspeed"
#define UNIT_QNH									"Unit_QNH"
#define UNIT_VARIO								"Unit_Vario"
#define UNIT_DISTANCE							"Unit_Distance"
#define UNIT_ENGINE_PRESSURE					"Unit_Engine_Pressure"
#define UNIT_TEMPERATURE						"Unit_Temperature"
#define UNIT_MASS									"Unit_Mass"
#define UNIT_FUEL_VOLUME						"Unit_Fuel_Volume"
#define UNIT_FUEL_FLOW							"Unit_Fuel_Flow"

#define SWITCH_TO_NEXT_LEG_RADIUS			"Switch_to_Next_Leg_Radius"

#define HSI_MAP_TYPE								"HSI_Map_Type"
#define AIRPLANE_ENGINE_TYPE					"Airplane_Engine_Type"
#define OIL_CARBURRETOR_TEMP					"Oil_Carburretor_Temp"

#define SECONDARY									"Secondary"

#define USER_ALARM_ENABLED						"User_Alarm_Enabled_"

Settings* Settings::s_pInstance = NULL;
using namespace unit;

// -------------------------------------------------------------------------

Settings::Settings()
	: m_settings(
		QCoreApplication::applicationDirPath() + "/Nesis.ini",
		QSettings::IniFormat
	)
{
	// Maybe the ini file defines the Nesis home folder.
	m_qsHome = m_settings.value(HOME_FOLDER).toString();
	if(m_qsHome.endsWith('/'))
		m_qsHome.chop(1);
	m_qsKamHome = m_settings.value(MAP_FOLDER, "/media/mmc/map/").toString();
	qDebug() << "Nesis application path =" << QCoreApplication::applicationDirPath();
	qDebug() << "Nesis Kam files path =" << m_qsKamHome;
}

Settings::~Settings()
{
}

// -------------------------------------------------------------------------

Settings* Settings::GetInstance()
{
	if(s_pInstance==NULL)
		s_pInstance = new Settings();
	return s_pInstance;
}

// -------------------------------------------------------------------------

void Settings::Save()
{
	m_settings.sync();
}
// -------------------------------------------------------------------------

QString Settings::GetKamPath() const
{
	return m_qsKamHome;
}
// -------------------------------------------------------------------------

QString Settings::GetAirspacesPath() const
{
	return m_qsHome + "/airspace/";
}

// -------------------------------------------------------------------------

QString Settings::GetWaypointsPath() const
{
	return m_qsHome + "/waypoint/";
}
// -------------------------------------------------------------------------

QString Settings::GetRoutesPath() const
{
	return m_qsHome + "/routes/";
}

// -------------------------------------------------------------------------

QString Settings::GetCheckListPath() const
{
	return m_qsHome + "/checklist/";
}

// -------------------------------------------------------------------------

// QString Settings::GetInstrumentPath() const
// {
// 	return m_qsHome + "/Images/Instruments/";
// }

// -------------------------------------------------------------------------

QString Settings::GetImagePath() const
{
	return m_qsHome + "/Images/";
}

// -------------------------------------------------------------------------

QString Settings::GetLogPath() const
{
	return m_qsHome + "/log/";
}

// -------------------------------------------------------------------------

QString Settings::GetHelpPath() const
{
	return m_qsHome + "/help/";
}
// -------------------------------------------------------------------------

QString Settings::GetAirplanePath() const
{
	return m_qsHome + "/airplane/";
}
// -------------------------------------------------------------------------

QString Settings::GetFirmwarePath() const
{
	return m_qsHome + "/firmware/";
}
// -------------------------------------------------------------------------

QString Settings::GetMD5File() const
{
	return m_qsHome + "/updatelist";
}

// -------------------------------------------------------------------------

int Settings::GetScreenResolution() const
{
	// Correct value for 8.4" and Lambert projection is 3742 pix/m

	// Mercator projection changes distance factor as we move from equator.
	// hence the nominal zoom factor is true only near equator.

	// Since we are using Marcator projection and most of our maket is +/- 45
	// degrees from equator, a factor modification is made.
	return m_settings.value(
		SCREEN_RESOLUTION,
		3742 / 1.4
	).toInt();
}

// -------------------------------------------------------------------------

QPointF Settings::GetMapStartPosition() const
{
	return m_settings.value(
		MAP_START_POSITION,
//		QPointF( 9.5114, 47.6714)		// FH
		QPointF(13.88, 45.88)			// Ajdovščina
// 		QPointF( -7.897222, 38.525 )   // EVORA - Pt
// 		QPointF( -20, 64 )   // Island
	).toPointF();
}

void Settings::SetMapStartPosition(const QPointF& ptPosDeg)
{
	m_settings.setValue(MAP_START_POSITION, ptPosDeg);
}

// -------------------------------------------------------------------------

map::Projection Settings::GetProjection() const
{
	return (map::Projection) m_settings.value(
		MAP_PROJECTION,
		//map::projLambert40N
		map::projMercator
	).toInt();
}

// -------------------------------------------------------------------------

void Settings::SetProjection(map::Projection eProj)
{
	m_settings.setValue(MAP_PROJECTION, eProj);
}

// -------------------------------------------------------------------------

int Settings::GetLastLogRecordId() const
{
	return m_settings.value(LOG_RECORD_ID, 0).toInt();
}

// -------------------------------------------------------------------------

int Settings::IncreaseLogRecordId(int iInc) {
	int id = GetLastLogRecordId();
	id = qMax(id+iInc, 0);

	m_settings.setValue(LOG_RECORD_ID, id);
	return id;
}
// -------------------------------------------------------------------------

QString Settings::GetCurrentPilot() const
{
	QString qs = m_settings.value(CURRENT_PILOT, QString()).toString();
	if(qs.isEmpty())
		qs = QObject::tr("Unknown");
	return qs;
}

// -------------------------------------------------------------------------

void Settings::SetCurrentPilot(const QString& qsPilot)
{
	m_settings.setValue(CURRENT_PILOT, qsPilot);
}

// -------------------------------------------------------------------------

QString Settings::GetAirplaneRegistration()
{
	return m_settings.value(AIRPLANE_REGISTRATION, QString()).toString();
}

// -------------------------------------------------------------------------

void Settings::SetAirplaneRegistration(const QString& qsRegNumber)
{
	m_settings.setValue(AIRPLANE_REGISTRATION, qsRegNumber);
}

// -------------------------------------------------------------------------

QLocale::Language Settings::GetUserInterfaceLanguage() const
{
	return (QLocale::Language)m_settings.value(LANGUAGE, QLocale::English).toInt();
}

// -------------------------------------------------------------------------

void Settings::SetUserInterfaceLanguage(QLocale::Language lan)
{
	m_settings.setValue(LANGUAGE, lan);
}

// -------------------------------------------------------------------------

int Settings::GetLocalTime2UTCDifferenceMinutes() const
{
	return m_settings.value(UTC_DIFFERENCE, 0).toInt();
}

// -------------------------------------------------------------------------

void Settings::SetLocalTime2UTCDifferenceMinutes(int iMin)
{
	m_settings.setValue(UTC_DIFFERENCE, iMin);
}

// -------------------------------------------------------------------------

int Settings::GetAltitudeUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_ALTITUDE, "m").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetAltitudeUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_ALTITUDE,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetAirspaceAltitudeUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_AIRSPACE_ALT, "feet").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetAirspaceAltitudeUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_AIRSPACE_ALT,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetAirspeedUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_AIRSPEED, "km/h").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetAirspeedUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_AIRSPEED,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetQNHUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_QNH, "hPa").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetQNHUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_QNH,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetVarioUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_VARIO, "m/s").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetVarioUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_VARIO,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetDistanceUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_DISTANCE, "km").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetDistanceUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_DISTANCE,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetEnginePressureUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_ENGINE_PRESSURE, "bar").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetEnginePressureUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_ENGINE_PRESSURE,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetTemperatureUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_TEMPERATURE, "C").toString()
	);
}
// -------------------------------------------------------------------------

void Settings::SetTemperatureUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_TEMPERATURE,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetMassUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_MASS, "kg").toString()
	);
}
// -------------------------------------------------------------------------

void Settings::SetMassUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_MASS,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetFuelVolumeUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_FUEL_VOLUME, "l").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetFuelVolumeUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_FUEL_VOLUME,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

int Settings::GetFuelFlowUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_FUEL_FLOW, "l/h").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetFuelFlowUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_FUEL_FLOW,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}
// -------------------------------------------------------------------------

int Settings::GetWindspeedUnit() const
{
	return Manager::GetInstance()->GetKey(
		m_settings.value(UNIT_WINDSPEED, "kts").toString()
	);
}

// -------------------------------------------------------------------------

void Settings::SetWindspeedUnit(int iUnitKey)
{
	m_settings.setValue(
		UNIT_WINDSPEED,
		Manager::GetInstance()->GetUnit(iUnitKey)->GetSignature()
	);
}

// -------------------------------------------------------------------------

float Settings::GetCruisingSpeed() const
{
	return m_settings.value(CRUISING_SPEED, 45.0).toDouble();
}

// -------------------------------------------------------------------------

void Settings::SetCruisingSpeed(float fSpeed)
{
	m_settings.setValue(CRUISING_SPEED, (double)fSpeed);
}

// -------------------------------------------------------------------------

int Settings::GetWaypointSearchRadius() const
{
	return m_settings.value(WAYPOINT_SEARCH_RADIUS, 250).toInt();
}

// -------------------------------------------------------------------------

void Settings::SetWaypointSearchRadius(int iRadiusKm)
{
	m_settings.setValue(WAYPOINT_SEARCH_RADIUS, iRadiusKm);
}

// -------------------------------------------------------------------------

void Settings::SetSwitchToNextLegRadius(float fRadius_m)
{
	m_settings.setValue(SWITCH_TO_NEXT_LEG_RADIUS, fRadius_m);
}

// -------------------------------------------------------------------------

float Settings::GetSwitchToNextLegRadius() const
{
	return m_settings.value(SWITCH_TO_NEXT_LEG_RADIUS, 1000.0f).toDouble();
}

// -------------------------------------------------------------------------

void Settings::SetHSIMapType(int iHTN)
{
	m_settings.setValue(HSI_MAP_TYPE, iHTN);
}

// -------------------------------------------------------------------------

int Settings::GetHSIMapType() const
{
	return m_settings.value(HSI_MAP_TYPE, HSI_TRACKING_UP).toInt();
}

// -------------------------------------------------------------------------

void Settings::SetAirplaneEngine(const QString& qsFile)
{
	m_settings.setValue(AIRPLANE_ENGINE_TYPE, qsFile);
}

// -------------------------------------------------------------------------

QString Settings::GetAirplaneEngine() const
{
	return m_settings.value(AIRPLANE_ENGINE_TYPE, "Standard.conf").toString();
/*	QString qs = m_settings.value(AIRPLANE_ENGINE_TYPE, "Instruments").toString();
	if(qs.endsWith(QLatin1String(".xml"))==false)
		qs += ".xml";
	return qs;*/
}

// -------------------------------------------------------------------------

// bool Settings::IsOilCarburretorTemp() const
// {
// 	return m_settings.value(OIL_CARBURRETOR_TEMP, false).toBool();
// }

// -------------------------------------------------------------------------

bool Settings::IsSecondary() const
{
	return m_settings.value(SECONDARY, false).toBool();
}

// -------------------------------------------------------------------------

void Settings::EnableUserAlarm(int iAlarm, bool bEnable)
{
	m_settings.setValue(USER_ALARM_ENABLED+QString::number(iAlarm), bEnable);
}

// -------------------------------------------------------------------------

bool Settings::IsUserAlarmEnabled(int iAlarm)
{
	return m_settings.value(USER_ALARM_ENABLED+QString::number(iAlarm), true).toBool();
}

// -------------------------------------------------------------------------

QString Settings::GetGaugeRoundBackgroundFile() const
{
	return GetImagePath() + "InstrumentBaseRound.png";
}
