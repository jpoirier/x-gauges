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

#include <sys/ioctl.h>
#include <signal.h>

#include <QtGui>
#include <QSettings>
#include <QTranslator>
#include "NesisEventFilter.h"
#include "Map/World.h"
#include "Map/MapImageZoom.h"
#include "Map/ProjectionFactory.h"
#include "WidgetMain.h"
#include "Map/AirspaceContainer.h"
#include "Route/WaypointContainerSingleton.h"
#include "Route/RouteContainerSingleton.h"
#include "Settings.h"
#include "Logbook/Logbook.h"
#include "Logbook/PilotList.h"
#include "Unit/Manager.h"
#include "FastUnitKeys.h"
#include "NesisAerospaceDataManager.h"
#include "NesisSplash.h"
#include "SphereNavigation.h"
#include "WidgetMessage.h"
#include "PixmapBattery.h"
#include "PixmapDirection.h"
#include "ParameterContainer.h"
#include "XMLTags.h"

#define NESIS_HOME_TAG				"NESIS_HOME"
#define NESIS_HOME_DEFAULT			"/nesis"
#define _M_								1000000
#define _K_								1000

#include "../Style/qkanardiastyle.h"

#ifdef Q_WS_QWS
	#include "../Style/qdecorationkanardia.h"
#endif

// --------------------------------------------------------------------------

// Some forward function declarations.
void InitializeSettings();
void InitializeUnits();
void InitializeStyleSheet();
void InitializeAirspaces();
void InitializeWaypoints();
void InitializeRoutes();
void InitializeCharts();
void InitializeLogbook();
QDomDocument LoadXmlConfig();
void InitializeInstruments(const QDomDocument& doc);
void InitializePixmaps();


QPair<float, const char*> GetScaleData(int iZoomFactor, int iUnitKey);

// --------------------------------------------------------------------------

void CatchSignal(int i)
{
	std::cerr << "Signal... " << i << std::endl;
	WidgetMessage::GetInstance()->SetText(
		QObject::tr("Device attached/removed"), 5, WidgetMessage::lNormal
	);
}

// --------------------------------------------------------------------------

void CatchSignal_USR2(int /*i*/)
{
	WidgetMessage::GetInstance()->SetText(
		QObject::tr("Updating..."), 35, WidgetMessage::lNormal
	);
}

// --------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	NesisSplash* pSplash = NesisSplash::GetInstance();
	pSplash->SetProgress(41000);

 	// catch SIGUSR1 signal
 	signal(SIGUSR1, CatchSignal);
 	// catch SIGUSR2 signal
 	signal(SIGUSR2, CatchSignal_USR2);

	// Set application style
	QApplication::setStyle(new QKanardiaStyle(true));
	QApplication::setPalette(QApplication::style()->standardPalette());
	#ifdef Q_WS_QWS
	QApplication::qwsSetDecoration(new QDecorationKanardia());
	#endif

	// Install event filter
	NesisEventFilter* pNEF = new NesisEventFilter();
	app.installEventFilter(pNEF);

	// Initialize Settings before anything else!
	InitializeSettings();
	Settings* pSettings = Settings::GetInstance();

	// Set the language translator
	QLocale locale(pSettings->GetUserInterfaceLanguage());
	QTranslator translator;
	QString qsTrPath = pSettings->GetHome() + "/locale/nesis_" + locale.name();
   qDebug() << "Requesting " << qsTrPath << "translation.";
   bool bTranslator = translator.load(qsTrPath);
   if(bTranslator)
   	app.installTranslator(&translator);

	pSplash->SetProgress(43000);

	// We need units
	InitializeUnits();
	// and we need instruments
	QDomDocument doc = LoadXmlConfig();
	InitializeInstruments(doc);

	// Airspaces
	pSplash->SetProgress(47000);
	InitializeAirspaces();

	// Airfields
	pSplash->SetProgress(50000);
	InitializeWaypoints();

	pSplash->SetProgress(56000);
	// Routes
	InitializeRoutes();
	// Logbook (this may take some time if the logbook is full ...)
	InitializeLogbook();
	InitializePixmaps();
	// Charts - Kam files
	qDebug() << "Chart files path" << Settings::GetInstance()->GetKamPath();
	InitializeCharts();

	pSplash->SetProgress(58000);

	//Create the main widget (parent of all other widgets)
	WidgetMain* main = new WidgetMain(doc, QSize(640,480));
	// We do not need the doc any more.
	doc.clear();
	
	pSplash->SetProgress(60000);

	// Set the main widget and enter the application loop
// 	WidgetMain* main = new WidgetMain(QSize(640,480));
	QObject::connect(pNEF, SIGNAL(Shutdown()), main, SLOT(OnShutdown()));

	pSplash->SetProgress(63000);

//  	app.processEvents();

 	qDebug() << "After main";
	main->show();
	qDebug() << "After main.show()";
	pSplash->SetProgress(65535);
	pSplash->Destroy();

	qDebug() << "Before exec()";
	app.exec();

	int iRetVal = main->IsReboot() ? 1 : 0;
	delete main;

	return iRetVal;
}

// --------------------------------------------------------------------------

void InitializeSettings()
{
	Settings* pS = Settings::GetInstance();
	
	// If the ini file does not know the home folder...
	if(pS->GetHome().isEmpty()) {
		// Get the NESIS_HOME environment variable
		// and set the Settings home directory.
		QStringList slEnv = QProcess::systemEnvironment();
		QString qsNesisHome = NESIS_HOME_DEFAULT;
		foreach(QString qs, slEnv) {
			//qDebug() << qs;
			QStringList sl = qs.split('=');
			if(sl[0]==NESIS_HOME_TAG) {
				qsNesisHome = sl[1];
				break;
			}
		}
		pS->SetHome(qsNesisHome);
	}
	qDebug() << "Nesis Home =" << pS->GetHome();

	// Print out the Primary / Secondary message on the screen.
	if(pS->IsSecondary())
		qDebug() << "This is the SECONDARY Nesis!";
	else
		qDebug() << "This is the PRIMARY Nesis!";
}

// --------------------------------------------------------------------------

void InitializeStyleSheet()
{
// 	Settings* pSettings = Settings::GetInstance();
// 	QFile qfSS(pSettings->GetHome() + "/NesisStyleSheet.qss");
// 	qfSS.open(QFile::ReadOnly);
// 	QString sStyleSheet = QLatin1String(qfSS.readAll());
// 	qApp->setStyleSheet(sStyleSheet);
}

// --------------------------------------------------------------------------

void InitializeAirspaces()
{
	map::AirspaceContainer* pAS = map::AirspaceContainer::GetInstance();
	int iCAS;

	qDebug() << "Airspace path =" << Settings::GetInstance()->GetAirspacesPath();
 	iCAS = pAS->Load(Settings::GetInstance()->GetAirspacesPath()+"Airspaces.asp");
 	qDebug() << iCAS << " airspaces loaded in binary format.";
 	pAS->CreateSectorContainer();
}

// --------------------------------------------------------------------------

void InitializeWaypoints()
{
	QPointF ptCurrentCoordinates = NesisAerospaceDataManager::GetInstance()->GetPosition();

	route::WaypointContainerSingleton* pWC;
	pWC = route::WaypointContainerSingleton::GetInstance();
	
	qDebug() << "Waypoint path =" << Settings::GetInstance()->GetWaypointsPath();

	int iC = pWC->Load(
		Settings::GetInstance()->GetWaypointsPath()+"Airfields.wpt",
		ptCurrentCoordinates,
		4000
	);
	qDebug() << "Airfields =" << iC;
	
	iC = pWC->Load(
		Settings::GetInstance()->GetWaypointsPath()+"VFR.wpt",
		ptCurrentCoordinates,
		4000
	);
	qDebug() << "VFR =" << iC;

	iC = pWC->Load(
		Settings::GetInstance()->GetWaypointsPath()+"NavAids.wpt",
		ptCurrentCoordinates,
		4000
	);
	qDebug() << "NavAids =" << iC;

	iC = pWC->Load(
		Settings::GetInstance()->GetWaypointsPath()+"User.wpt",
		ptCurrentCoordinates,
		4000
	);
	qDebug() << "User =" << iC;

	qDebug() << "Total" << pWC->GetCount() << "waypoints were loaded.";
	
	pWC->CreateSectorContainer();
}

// --------------------------------------------------------------------------

void InitializeRoutes()
{
	route::RouteContainerSingleton* pconRoute;
	pconRoute = route::RouteContainerSingleton::GetInstance();
	pconRoute->Load(Settings::GetInstance()->GetRoutesPath()+"Nesis.rts");
}

// --------------------------------------------------------------------------

void InitializeCharts()
{
	Settings* pSettings = Settings::GetInstance();

	// Kam files
	map::World::Lock()->SetKamPath(pSettings->GetKamPath());
	map::World::Unlock();

	// Set the Zoom levels
	map::MapImageZoom* pMI = map::SingletonMapImageZoom::GetInstance();
	const int iScreenRes = pSettings->GetScreenResolution();
	qDebug() << "Screen resolution is" << iScreenRes << "pix/m";

	// Get scale elements
	int iDistanceKey = pSettings->GetDistanceUnit();

	//             scale    button name img size  res     safety margin pix at 200 km/h
	pMI->AddImage( 125*_K_, "1 : 125 k", 1400, iScreenRes, 150, // about 90 sec
					   qMakePair(  30, (const char*)"30\""),
					   qMakePair(  60, (const char*)"1'"),
					   GetScaleData(125*_K_, iDistanceKey)
	);

	pMI->AddImage( 250*_K_, "1 : 250 k", 1344, iScreenRes, 120, // about 144 sec
					   qMakePair(1*60, (const char*)"1'"),
					   qMakePair(2*60, (const char*)"2'"),
					   GetScaleData(250*_K_, iDistanceKey)
	);
	pMI->AddImage( 500*_K_, "1 : 500 k", 1088, iScreenRes,  55, // about 132 sec
					   qMakePair(2*60, (const char*)"2'"),
					   qMakePair(5*60, (const char*)"5'"),
					   GetScaleData(500*_K_, iDistanceKey)
	);
	pMI->AddImage( 1*_M_, "1 : 1 M",   1088, iScreenRes,  30, // about 144 sec
					   qMakePair(5*60,  (const char*)"5'"),
					   qMakePair(10*60, (const char*)"10'"),
					   GetScaleData(1*_M_, iDistanceKey)
	);
	pMI->AddImage( 2*_M_, "1 : 2 M",   1024, iScreenRes,  25, // about 4 min
					   qMakePair(10*60, (const char*)"10'"),
					   qMakePair(20*60, (const char*)"20'"),
					   GetScaleData(2*_M_, iDistanceKey)
	);
	pMI->AddImage( 5*_M_, "1 : 5 M",   1024, iScreenRes,  20, // about 8 min
					   qMakePair(30*60, (const char*)"30'"),
					   qMakePair(60*60, (const char*)"1h"),
					   GetScaleData(5*_M_, iDistanceKey)
	);
	// TODO: There is a blue line across Spain, Italy, ... Obviously a bug.
// 	pMI->AddImage( 50000000, "1 : 50 M",   1024, iScreenRes,  20); // about 6 min

	// Map projection
	QPointF ptWGS = NesisAerospaceDataManager::GetInstance()->GetPosition();
	if(ptWGS.isNull())
		ptWGS = pSettings->GetMapStartPosition();

	map::ProjectionFactory projectionFactory;
	map::AbstractProjection* pProj;
	pProj = projectionFactory.Create(ptWGS, pSettings->GetProjection());
	pMI->SetProjection(pProj);
}

// --------------------------------------------------------------------------

void InitializeLogbook()
{
	Settings* pS = Settings::GetInstance();
	// Log book ...
	using namespace logbook;
	qDebug() << "Log path =" << pS->GetLogPath();
	int iCount = Logbook::GetInstance()->Load(pS->GetLogPath()+"Logbook.xml");
	qDebug() << iCount << " entries found in the Logbook.xml.";

	// Pilots
	iCount = PilotList::GetInstance()->Load(pS->GetLogPath()+"PilotList.xml");
	qDebug() << iCount << " entries found in the PilotList.xml.";
}

// --------------------------------------------------------------------------

void InitializeUnits()
{
//	std::cout << "enter InitializeUnits\n";
	Settings* pS = Settings::GetInstance();
	int iCount = unit::Manager::GetInstance()->LoadXML(
		pS->GetHome() + "/Units.xml"
	);
	qDebug() << iCount << " units loaded from the Units.xml.";
	FastUnitKeys::GetInstance();
//	std::cout << "exit InitializeUnits\n";
}

// --------------------------------------------------------------------------

QDomDocument LoadXmlConfig()
{
	QDomDocument doc;
	Settings* pS = Settings::GetInstance();
	QString qsFile = pS->GetAirplanePath() + pS->GetAirplaneEngine();
	qDebug() << "Requested Xml Config file =" << qsFile;

	// This is due the change of the .xml extension into .conf extension.
	// We need to prevent loading any .xml file.
	if(qsFile.endsWith(".xml"))
		qsFile = pS->GetAirplanePath() + "Standard.conf";

	QFile file(qsFile);
	if(file.open(QIODevice::ReadOnly)==false) {
		qWarning() << "Failed to load the" << qsFile << "config file.";
		qWarning() << "Trying to load default.";
		qsFile = pS->GetAirplanePath() + "Standard.conf";
		file.setFileName(qsFile);
		if(file.open(QIODevice::ReadOnly)==false) {
			qCritical() << "Failed to load the 'default'" << qsFile << "file.";
			return doc;
		}
	}

	// Parse it into QDomDocument object.
	QString qsError;
	int iLine;
	if(doc.setContent(&file, false, &qsError, &iLine)==false) {
		qCritical() << "Failed to parse the XML config content: " << file.fileName() 
						<< qsError << "in line" << iLine;
	}
	qDebug() << "Loaded Xml Config file =" << qsFile;
	return doc;
}

// --------------------------------------------------------------------------
#define CB0(F)		Delegate0<float>::FromConstMethod\
							<NesisAerospaceDataManager, &NesisAerospaceDataManager::F>(pND)
#define CB1(F)		Delegate<float,int>::FromConstMethod\
							<NesisAerospaceDataManager, &NesisAerospaceDataManager::F>(pND)

void InitializeInstruments(const QDomDocument& doc)
{
	using namespace instrument;
	// Load instruments
	ParameterContainer* 			pPC = ParameterContainer::GetInstance();
	Settings* 						pS  = Settings::GetInstance();
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	unit::Manager*					pM	 = unit::Manager::GetInstance();


	if(pPC->Load(doc)) {
		// We need to define user selected units and call-backs
		pPC->SetCallbackUserKey(
			TAG_AIRSPEED,
			CB0(GetIAS),
			pM->GetSignature(pS->GetAirspeedUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_VARIO,
			CB0(GetVario), 
			pM->GetSignature(pS->GetVarioUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_ALTITUDE,
			CB0(GetAltitude),
			pM->GetSignature(pS->GetAltitudeUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_FUEL_FLOW,
			CB0(GetFuelFlow), 
			pM->GetSignature(pS->GetFuelFlowUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_FUEL_LEVEL,
			CB0(GetTotalFuelLevel), 
			pM->GetSignature(pS->GetFuelVolumeUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_MANIFOLD,
			CB0(GetManifoldPressure), 
			"inHg"
		);
		pPC->SetCallbackUserKey(
			TAG_RPM, 
			CB0(GetRPM), 
			"RPM"
		);
		pPC->SetCallbackUserKey(
			TAG_CURRENT, 
			CB0(GetAmperes),
			"A"
		);
		pPC->SetCallbackUserKey(
			TAG_VOLTAGE, 
			CB0(GetVoltage), 
			"V"
		);
		pPC->SetCallbackUserKey(
			TAG_OIL_TEMP,
    		CB0(GetOilTemperature), 
			pM->GetSignature(pS->GetTemperatureUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_WATER_TEMP, 
			CB0(GetWaterTemperature),
			pM->GetSignature(pS->GetTemperatureUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_OIL_PRESS,
			CB0(GetOilPressure), 
			pM->GetSignature(pS->GetEnginePressureUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_FUEL_PRESS,
			CB0(GetFuelPressure),
			pM->GetSignature(pS->GetEnginePressureUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_RANGE,
			CB0(GetRange),
			pM->GetSignature(pS->GetDistanceUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_CARBURETOR,
			CB0(GetCarburretorTemperature), 
			pM->GetSignature(pS->GetTemperatureUnit())
		);

		pPC->SetCallbackUserKey(
			TAG_EGT,
			CB1(GetEGT),
			pM->GetSignature(pS->GetTemperatureUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_CHT,
			CB1(GetCHT), 
			pM->GetSignature(pS->GetTemperatureUnit())
		);
		
		pPC->SetCallbackUserKey(
			TAG_TAS,
			CB0(GetTAS), 
			pM->GetSignature(pS->GetAirspeedUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_ENGINE_TIME, 
			CB0(GetEngineTotalTime), 
			"h"
		);
		pPC->SetCallbackUserKey(
			TAG_QNH,
			CB0(GetQNH), 
			pM->GetSignature(pS->GetQNHUnit())
		);
		pPC->SetCallbackUserKey(
			TAG_ENDURANCE,
			CB0(GetEndurance), 
			"s"
		);
		pPC->SetCallbackUserKey(
			TAG_AVG_FUEL_FLOW,
			CB0(GetAverageFuelFlow), 
			pM->GetSignature(pS->GetFuelFlowUnit())
		);
	}	
}

void InitializePixmaps()
{
	// Load battery pixmaps
	PixmapBattery::GetInstance(); 
	// Load arrow pixmaps
	PixmapDirection::GetInstance();
} 


// --------------------------------------------------------------------------

QPair<float, const char*> GetScaleData(int iZoomFactor, int iUnitKey)
{
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();

	QPair<float, const char*> fcScale;

	if(iZoomFactor >= 10*_M_) {
		if(iUnitKey == pFUK->iKey_km)
			fcScale = qMakePair(200.0f, (const char*)"500 km");
		else if(iUnitKey == pFUK->iKey_SM)
			fcScale = qMakePair(200.0f, (const char*)"200 SM");
		else
			fcScale = qMakePair(200.0f, (const char*)"200 NM");
	}

	else if(iZoomFactor >= 5*_M_) {
		if(iUnitKey == pFUK->iKey_km)
			fcScale = qMakePair(200.0f, (const char*)"200 km");
		else if(iUnitKey == pFUK->iKey_SM)
			fcScale = qMakePair(100.0f, (const char*)"100 SM");
		else
			fcScale = qMakePair(100.0f, (const char*)"100 NM");
	}

	else if(iZoomFactor >= 2*_M_) {
		if(iUnitKey == pFUK->iKey_km)
			fcScale = qMakePair(50.0f, (const char*)"50 km");
		else if(iUnitKey == pFUK->iKey_SM)
			fcScale = qMakePair(20.0f, (const char*)"20 SM");
		else
			fcScale = qMakePair(20.0f, (const char*)"20 NM");
	}

	else if(iZoomFactor >= 1*_M_) {
		if(iUnitKey == pFUK->iKey_km)
			fcScale = qMakePair(20.0f, (const char*)"20 km");
		else if(iUnitKey == pFUK->iKey_SM)
			fcScale = qMakePair(10.0f, (const char*)"10 SM");
		else
			fcScale = qMakePair(10.0f, (const char*)"10 NM");
	}

	else if(iZoomFactor >= 500*_K_) {
		if(iUnitKey == pFUK->iKey_km)
			fcScale = qMakePair(10.0f, (const char*)"10 km");
		else if(iUnitKey == pFUK->iKey_SM)
			fcScale = qMakePair(5.0f, (const char*)"5 SM");
		else
			fcScale = qMakePair(5.0f, (const char*)"5 NM");
	}

	else {
		if(iUnitKey == pFUK->iKey_km)
			fcScale = qMakePair(5.0f, (const char*)"5 km");
		else if(iUnitKey == pFUK->iKey_SM)
			fcScale = qMakePair(2.0f, (const char*)"2 SM");
		else
			fcScale = qMakePair(2.0f, (const char*)"2 NM");
	}

	// Convert to arc
	if(iUnitKey == pFUK->iKey_km)
		fcScale.first *= 1000.0f/EARTH_RADIUS_M;
	else if(iUnitKey == pFUK->iKey_SM)
		fcScale.first *= 1609.344f/EARTH_RADIUS_M;
	else
		fcScale.first *= 1852.0f/EARTH_RADIUS_M;

	return fcScale;
}
