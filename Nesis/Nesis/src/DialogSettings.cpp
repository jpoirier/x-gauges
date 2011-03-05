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
#include <QtGui>

#include "PanelMiscellaneous.h"
#include "Settings.h"
#include "FastUnitKeys.h"
#include "GraphicsSingleton.h"
#include "NesisComboBox.h"
#include "NesisSpinBox.h"
#include "NesisDoubleSpinBox.h"
#include "NesisEditBox2.h"
#include "WidgetMessage.h"
#include "NesisUnitCAN.h"


#include "DialogSettings.h"


// -----------------------------------------------------------------------

DialogSettings::DialogSettings(QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	setGeometry(0,0, 600,390);
	m_bDirty = false;

	const QFont& fontBuddy = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fKnobBuddy);
	const QFont& fontSm    = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	QFrame* pFrameMisc = new QFrame();
	pFrameMisc->setFrameShape(QFrame::StyledPanel);
	pLayout->addWidget(pFrameMisc);
	
	// Top row - grid layout
	QGridLayout* pTopLayout = new QGridLayout;
	pFrameMisc->setLayout(pTopLayout);
	pTopLayout->setHorizontalSpacing(30);

	// Language
	QLabel* pLbl = new QLabel(tr("Language"));
	pLbl->setFont(fontSm);
	pTopLayout->addWidget(pLbl, 0,0);
	m_pcbLanguage = new NesisComboBox;
	m_pcbLanguage->setFont(fontSm);
	pTopLayout->addWidget(m_pcbLanguage, 1,0);
	InitializeLanguage();

	// HSI Map type
	pLbl = new QLabel(tr("HSI Map type"));
	pLbl->setFont(fontSm);
	pTopLayout->addWidget(pLbl, 0,1);
	m_pcbHSIMapType = new NesisComboBox;
	m_pcbHSIMapType->setFont(fontSm);
	pTopLayout->addWidget(m_pcbHSIMapType, 1,1);
	InitializeHSIMapType();
	
	// Registration
	pLbl = new QLabel(tr("Registration"));
	pLbl->setFont(fontSm);
	pTopLayout->addWidget(pLbl, 0,2);
	QHBoxLayout* pHLayout = new QHBoxLayout;
	m_pleRegistration = new NesisEditBox2(QString());
	pHLayout->addWidget(m_pleRegistration);
 	QLabel* plBuddy = new QLabel;
 	plBuddy->setFont(fontBuddy);
	plBuddy->hide();
 	pHLayout->addWidget(plBuddy);
 	m_pleRegistration->SetBuddy(plBuddy);
	m_pleRegistration->setFont(fontSm);
	pTopLayout->addLayout(pHLayout, 1,2);

/*	// units frame
	QFrame* pFrameUnits = new QFrame();
	pFrameUnits->setFrameShape(QFrame::StyledPanel);
	pLayout->addWidget(pFrameUnits);*/
	
	// Add custom units frame
	pLbl = new QLabel(tr("Selected units"));
	pLbl->setFont(fontSm);
	pLayout->addSpacing(10);
	pLayout->addWidget(pLbl);
	QFrame* pFrame = new QFrame;
	pFrame->setFrameShape(QFrame::StyledPanel);
	pLayout->addWidget(pFrame);
	QGridLayout* pUnitLayout = new QGridLayout;
	pFrame->setLayout(pUnitLayout);
	InitializeUnits(pUnitLayout, &fontSm);

	pLbl = new QLabel(tr("Note: Any change may result in the program reboot."));
	pLbl->setFont(fontSm);
	pLayout->addWidget(pLbl);
	pLayout->addStretch(5);

	// List of external commands
	QStringList slExternalCmd;
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << QString();
	slExternalCmd << tr("Close");

	// List of all combos ...
	QVector<NesisComboBox*> cbList;
	cbList << m_pcbLanguage;
	cbList << m_pcbHSIMapType;
	cbList << m_pcbAltitude;
	cbList << m_pcbAirspace;
	cbList << m_pcbSpeed;
	cbList << m_pcbWindspeed;
	cbList << m_pcbQNH;
	cbList << m_pcbVario;
	cbList << m_pcbDistance;
	cbList << m_pcbPressure;
	cbList << m_pcbTemperature;
	cbList << m_pcbMass;
	cbList << m_pcbFuel;
	cbList << m_pcbFuelFlow;

	// Connections
	// All 
	for(int i=0; i<cbList.count(); i++)
		connect(cbList[i], SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
		
	connect(m_pleRegistration, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pleRegistration, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_pleRegistration, SIGNAL(NesisEditCancel()), this, SLOT(FocusNextChild()));

	// External commands
	foreach(NesisComboBox* pcb, cbList) {
		pcb->SetExternalButtonLabels(slExternalCmd);
		connect(pcb, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));
	}
	m_pleRegistration->SetExternalButtonLabels(slExternalCmd);
	connect(m_pleRegistration, SIGNAL(NesisButton(int)), this, SLOT(OnExternalCmd(int)));

	// Any change in comboboxes should set the dirty flag.
	for(int i=0; i<cbList.count(); i++)
		connect(cbList[i], SIGNAL(activated(int)), this, SLOT(OnSetDirty()));
	 
	// Get registration
	Settings* pS = Settings::GetInstance();
	m_pleRegistration->setText(pS->GetAirplaneRegistration());
	
	Activate();
}

DialogSettings::~DialogSettings()
{
}
// -----------------------------------------------------------------------

void DialogSettings::Activate()
{
	// This will also show buttons.
	m_pcbLanguage->setFocus();
}

// -----------------------------------------------------------------------

void DialogSettings::OnClose()
{
	// Check if the fuel flow factor has changed?
	Settings* pS = Settings::GetInstance();
//	NesisUnitInfoContainer& uibc    = NesisUnitCAN::GetInstance()->GetUnitInfoContainer();
//	can::AbstractAerospaceDataManager* pADM = uibc.GetAerospaceDataManager();
//	const can::UnitInfoDaqu* pDaqu = uibc.GetDaqu();
	
	pS->SetAirplaneRegistration(m_pleRegistration->text());
	
	if(m_bDirty) {
		pS->SetAltitudeUnit(GetCurrentData(m_pcbAltitude));
// 		qDebug() << "Alt";
		pS->SetAirspaceAltitudeUnit(GetCurrentData(m_pcbAirspace));
// 		qDebug() << "Airspace";
		pS->SetAirspeedUnit(GetCurrentData(m_pcbSpeed));
// 		qDebug() << "Airspeed";
		pS->SetWindspeedUnit(GetCurrentData(m_pcbWindspeed));
// 		qDebug() << "Windspeed";
		pS->SetQNHUnit(GetCurrentData(m_pcbQNH));
// 		qDebug() << "QNH";
		pS->SetVarioUnit(GetCurrentData(m_pcbVario));
// 		qDebug() << "Vario";
		pS->SetDistanceUnit(GetCurrentData(m_pcbDistance));
// 		qDebug() << "Distance";
		pS->SetEnginePressureUnit(GetCurrentData(m_pcbPressure));
// 		qDebug() << "Engine pressure";
		pS->SetTemperatureUnit(GetCurrentData(m_pcbTemperature));
// 		qDebug() << "Temperature";
		pS->SetMassUnit(GetCurrentData(m_pcbMass));
// 		qDebug() << "Mass";
		pS->SetFuelVolumeUnit(GetCurrentData(m_pcbFuel));
// 		qDebug() << "Volume";
		pS->SetFuelFlowUnit(GetCurrentData(m_pcbFuelFlow));
// 		qDebug() << "Fuel";

		pS->SetUserInterfaceLanguage((QLocale::Language)GetCurrentData(m_pcbLanguage));
//  		qDebug() << "Language";
		pS->SetHSIMapType(m_pcbHSIMapType->currentIndex());

		m_bDirty = false;
		pS->Save(); 
		sync();
		
		accept();
	}
	else {
		reject();
	}
}

// -----------------------------------------------------------------------

void DialogSettings::InitializeLanguage()
{
	Settings* pS = Settings::GetInstance();
	const QString& qsPath = pS->GetImagePath();

	m_pcbLanguage->setMinimumWidth(160);

	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-gb.png"), "English", 	QLocale::English);
	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-si.png"), "Slovenian", QLocale::Slovenian);
	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-de.png"), "German", 	QLocale::German);
	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-pt.png"), "Portuguese", QLocale::Portuguese);
	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-es.png"), "Spanish", 	QLocale::Spanish);
	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-it.png"), "Italian", 	QLocale::Italian);
	m_pcbLanguage->addItem(QIcon(qsPath+"Flag-fi.png"), "Finnish", 	QLocale::Finnish);

	SelectItem(m_pcbLanguage, pS->GetUserInterfaceLanguage());
}
// -----------------------------------------------------------------------

void DialogSettings::InitializeHSIMapType()
{
	Settings* pS = Settings::GetInstance();

	m_pcbHSIMapType->setMinimumWidth(160);

	m_pcbHSIMapType->addItem(tr("Heading up"));
	m_pcbHSIMapType->addItem(tr("Tracking up"));
//	m_pcbHSIMapType->addItem(tr("North up"));

	m_pcbHSIMapType->setCurrentIndex(pS->GetHSIMapType());
}

// -----------------------------------------------------------------------

void DialogSettings::InitializeUnits(
	QGridLayout* pUnitLayout,
	const QFont* pFont
)
{
	Settings* pS = Settings::GetInstance();
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();

	// Left column
	QLabel* pLbl = new QLabel(tr("Altitude:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 0,0, Qt::AlignRight);
	m_pcbAltitude = new NesisComboBox;
	m_pcbAltitude->setFont(*pFont);
   m_pcbAltitude->addItem("feet", 	pFUK->iKey_feet);
   m_pcbAltitude->addItem("meters", pFUK->iKey_m);
	SelectItem(m_pcbAltitude, pS->GetAltitudeUnit());
	pUnitLayout->addWidget(m_pcbAltitude, 0,1);

	pLbl = new QLabel(tr("Airspace:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 1,0, Qt::AlignRight);
	m_pcbAirspace = new NesisComboBox;
	m_pcbAirspace->setFont(*pFont);
   m_pcbAirspace->addItem("feet", 	pFUK->iKey_feet);
//   m_pcbAirspace->addItem("meters", pFUK->iKey_m);
	SelectItem(m_pcbAirspace, pS->GetAirspaceAltitudeUnit());
	pUnitLayout->addWidget(m_pcbAirspace, 1,1);

	pLbl = new QLabel(tr("Speed:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 2,0, Qt::AlignRight);
	m_pcbSpeed = new NesisComboBox;
	m_pcbSpeed->setFont(*pFont);
   m_pcbSpeed->addItem("kts", pFUK->iKey_kts);
   m_pcbSpeed->addItem("km/h",  pFUK->iKey_km_h);
   m_pcbSpeed->addItem("mph",   pFUK->iKey_mph);
	SelectItem(m_pcbSpeed, pS->GetAirspeedUnit());
	pUnitLayout->addWidget(m_pcbSpeed, 2,1);

	pLbl = new QLabel(tr("QNH:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 3,0, Qt::AlignRight);
	m_pcbQNH = new NesisComboBox;
	m_pcbQNH->setFont(*pFont);
   m_pcbQNH->addItem("hPa",  pFUK->iKey_hPa);
   m_pcbQNH->addItem("inHg", pFUK->iKey_inHg);
	SelectItem(m_pcbQNH, pS->GetQNHUnit());
   pUnitLayout->addWidget(m_pcbQNH, 3,1);

	pLbl = new QLabel(tr("Vario:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 4,0, Qt::AlignRight);
	m_pcbVario = new NesisComboBox;
	m_pcbVario->setFont(*pFont);
   m_pcbVario->addItem("ft/min", pFUK->iKey_ft_min);
   m_pcbVario->addItem("m/s",    pFUK->iKey_m_s);
   m_pcbVario->addItem("m/min",  pFUK->iKey_m_min);
	SelectItem(m_pcbVario, pS->GetVarioUnit());
	pUnitLayout->addWidget(m_pcbVario, 4,1);

	pLbl = new QLabel(tr("Wind speed:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 5,0, Qt::AlignRight);
	m_pcbWindspeed = new NesisComboBox;
	m_pcbWindspeed->setFont(*pFont);
   m_pcbWindspeed->addItem("m/s", pFUK->iKey_m_s);
   m_pcbWindspeed->addItem("kts", pFUK->iKey_kts);
   m_pcbWindspeed->addItem("km/h", pFUK->iKey_km_h);
	SelectItem(m_pcbWindspeed, pS->GetWindspeedUnit());
	pUnitLayout->addWidget(m_pcbWindspeed, 5,1);

	// Right column

	pLbl = new QLabel(tr("Distance:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 0,2, Qt::AlignRight);
	m_pcbDistance = new NesisComboBox;
	m_pcbDistance->setFont(*pFont);
   m_pcbDistance->addItem("NM", pFUK->iKey_NM);
   m_pcbDistance->addItem("km", pFUK->iKey_km);
   m_pcbDistance->addItem("SM", pFUK->iKey_SM);
	SelectItem(m_pcbDistance, pS->GetDistanceUnit());
	pUnitLayout->addWidget(m_pcbDistance, 0,3);

	pLbl = new QLabel(tr("Pressure:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 1,2, Qt::AlignRight);
	m_pcbPressure = new NesisComboBox;
	m_pcbPressure->setFont(*pFont);
   m_pcbPressure->addItem("psi", pFUK->iKey_psi);
   m_pcbPressure->addItem("bar", pFUK->iKey_bar);
	SelectItem(m_pcbPressure, pS->GetEnginePressureUnit());
	pUnitLayout->addWidget(m_pcbPressure, 1,3);

	pLbl = new QLabel(tr("Temperature:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 2,2, Qt::AlignRight);
	m_pcbTemperature = new NesisComboBox;
	m_pcbTemperature->setFont(*pFont);
   m_pcbTemperature->addItem("C", pFUK->iKey_C);
   m_pcbTemperature->addItem("F", pFUK->iKey_F);
	SelectItem(m_pcbTemperature, pS->GetTemperatureUnit());
	pUnitLayout->addWidget(m_pcbTemperature, 2,3);

	pLbl = new QLabel(tr("Mass:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 3,2, Qt::AlignRight);
	m_pcbMass = new NesisComboBox;
	m_pcbMass->setFont(*pFont);
   m_pcbMass->addItem("kg",  pFUK->iKey_kg);
   m_pcbMass->addItem("lbs", pFUK->iKey_lbs);
	SelectItem(m_pcbMass, pS->GetMassUnit());
	pUnitLayout->addWidget(m_pcbMass, 3,3);

	pLbl = new QLabel(tr("Fuel:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl, 4,2, Qt::AlignRight);
	m_pcbFuel = new NesisComboBox;
	m_pcbFuel->setFont(*pFont);
   m_pcbFuel->addItem("l",   pFUK->iKey_l);
   m_pcbFuel->addItem("gal", pFUK->iKey_gal);
	SelectItem(m_pcbFuel, pS->GetFuelVolumeUnit());
	pUnitLayout->addWidget(m_pcbFuel, 4,3);

	pLbl = new QLabel(tr("Fuel flow:"));
	pLbl->setFont(*pFont);
	pUnitLayout->addWidget(pLbl ,5,2, Qt::AlignRight);
	m_pcbFuelFlow = new NesisComboBox;
	m_pcbFuelFlow->setFont(*pFont);
   m_pcbFuelFlow->addItem("l/h",   pFUK->iKey_l_h);
   m_pcbFuelFlow->addItem("gal/h", pFUK->iKey_gal_h);
	SelectItem(m_pcbFuelFlow, pS->GetFuelFlowUnit());
	pUnitLayout->addWidget(m_pcbFuelFlow, 5,3);
}

// -----------------------------------------------------------------------

void DialogSettings::OnExternalCmd(int iCmd)
{
	if(iCmd == AbstractNesisInput::pbBtn5)
		OnClose();
}

// -----------------------------------------------------------------------

void DialogSettings::OnSetDirty()
{
	m_bDirty = true;
}

// -----------------------------------------------------------------------

int DialogSettings::GetCurrentData(const NesisComboBox* pcb)
{
	QVariant v = pcb->itemData(pcb->currentIndex());
	if(v.type()!=QVariant::Invalid)
		return v.toInt();
	return -1;
}

// -----------------------------------------------------------------------

void DialogSettings::SelectItem(NesisComboBox* pcb, int iData)
{
	int i = pcb->findData(iData);
	pcb->setCurrentIndex(i);
}

// -----------------------------------------------------------------------
