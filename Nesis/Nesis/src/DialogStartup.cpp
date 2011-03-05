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

#include "NesisComboBox.h"
#include "NesisDoubleSpinBox.h"
#include "GraphicsSingleton.h"
#include "Logbook/PilotList.h"
#include "Settings.h"
#include "ApplicationDefines.h"
#include "ParameterContainer.h"
#include "XMLTags.h"
#include "Unit/Manager.h"
#include "svn.h"

#include "DialogStartup.h"

// -----------------------------------------------------------------------

DialogStartup::DialogStartup(const unit::Value& uvFuel, QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
// 	m_bMoveToTop = true;
// 	setGeometry(0,0,380, 280);
	Settings* pS = Settings::GetInstance();

	// Main layout
	m_pLayout = new QStackedLayout;
	setLayout(m_pLayout);

	const QFont& fontS = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialogSmall);

	// First screen

	// Warning widget
//	QHBoxLayout* pLayout = new QHBoxLayout;
	QString qsAE = pS->GetAirplaneEngine();
	if(qsAE.endsWith(".conf"))
		qsAE.chop(5);
	
	m_pwlWarning = new QLabel(
		tr(
			"<qt>"
			"<head><style type=\"text/css\"> body { color: black; background: white } </style></head>"
			"<body>"
			"<center>"
			"<h2>Warning</h2>"
			"<p>This instrument may be used under<br>"
			"VFR (visual flying rules) only!<br><br>"
			"Any navigational information is provided for<br>"
			"reference only. The pilot takes all responsibility<br>"
			"and risk associated with the use of this device."
			"<p>Press OK to agree and continue!</p>"
			"</center>"
			"<small><em><p align=\"right\">Version %1.%2, SVN %3 : %4</p></em></small>"
			"</body>"
			"</qt>"
		)
		.arg(SOFTWARE_VERSION/16)
		.arg(SOFTWARE_VERSION%16)
		.arg(SVN_REVISION)
		.arg(qsAE)
	);
	m_pwlWarning->setFont(fontS);
	m_pwlWarning->setBackgroundRole(QPalette::Base);
	m_pwlWarning->setFrameStyle(QFrame::StyledPanel);
	m_pwlWarning->setFrameRect(m_pwlWarning->rect().adjusted(9,9,-9,-9));
	m_pLayout->addWidget(m_pwlWarning);

	// Second screen
	QWidget* pSecond = new QWidget;
	QVBoxLayout* pLayout2 = new QVBoxLayout;
	pSecond->setLayout(pLayout2);
	m_pLayout->addWidget(pSecond);

	// Pilot

	// TODO: Add "Guest" to the list
	QFrame* pFrame = new QFrame;
	pFrame->setFrameShape(QFrame::StyledPanel);
	QHBoxLayout* pHLayout = new QHBoxLayout;
	pFrame->setLayout(pHLayout);
	pLayout2->addWidget(pFrame);

	QLabel* pLabel = new QLabel(tr("Select Pilot:"));
	pLabel->setFont(fontS);
	pHLayout->addWidget(pLabel);
	m_pwcPilots = new NesisComboBox;
	m_pwcPilots->setFont(fontS);
	pHLayout->addWidget(m_pwcPilots);

	InitializePilotList();

	// FUEL Level

	pFrame = new QFrame;
	pFrame->setFrameShape(QFrame::StyledPanel);
	pHLayout = new QHBoxLayout;
	pFrame->setLayout(pHLayout);
	pLayout2->addWidget(pFrame);

	pLabel = new QLabel(tr("Set fuel level:"));
	pLabel->setFont(fontS);
	pHLayout->addWidget(pLabel);

	m_uvFuel = uvFuel; //unit::Value("l",30);
	instrument::Parameter* pFuelLevel;
	pFuelLevel = instrument::ParameterContainer::GetInstance()->GetParameter(TAG_FUEL_LEVEL);
	unit::Manager* pM = unit::Manager::GetInstance();
	Q_ASSERT(m_uvFuel.GetUnit()->GetSignature()=="l" ||
				m_uvFuel.GetUnit()->GetSignature()=="gal");
	Q_ASSERT(m_uvFuel.GetUnit()->GetKey() == pFuelLevel->GetUnitKeyUser());

	// Unit conversion
	bool bGal = m_uvFuel.GetUnit()->GetSignature()=="gal";
	int iDec    = bGal ?    1 : 0;
	float fStep = bGal ? 1.0f : 2.0f;
	float fFuel = common::Snap(m_uvFuel.GetValue(), fStep);
	float fMin  = 0.0f;
	float fMax  = pM->Convert(
		pFuelLevel->GetRange().second, pFuelLevel->GetUnitKeySystem(), pFuelLevel->GetUnitKeyUser()
	);

	// Spin box
	m_pwsFuel = new NesisDoubleSpinBox(iDec);
	m_pwsFuel->setFont(fontS);
	m_pwsFuel->setSuffix(QString(" ") + m_uvFuel.GetUnit()->GetSignature());
	m_pwsFuel->setMinimum(fMin);
	m_pwsFuel->setMaximum(fMax);
	m_pwsFuel->setSingleStep(fStep);
	m_pwsFuel->setWrapping(false);
	m_pwsFuel->setValue(fFuel);
	pHLayout->addWidget(m_pwsFuel);

	// Vertical bar
	m_pwBar = new QProgressBar;
	m_pwBar->setOrientation(Qt::Vertical);
	m_pwBar->setRange(0, 100);
	pHLayout->addWidget(m_pwBar);

	// Connections
	connect(m_pwcPilots, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pwcPilots, SIGNAL(NesisOk()), this, SLOT(SetFuelFocusEdit()));
	connect(m_pwsFuel, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pwsFuel, SIGNAL(NesisOk()), this, SLOT(accept()));
	connect(m_pwsFuel, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));

	OnValueChanged(fFuel);

	// Show the first screen

	m_pLayout->setCurrentIndex(0);
	// Set focust to this dialog.
	setFocus();

}

DialogStartup::~DialogStartup()
{
}
// -----------------------------------------------------------------------

void DialogStartup::SetFuelFocusEdit()
{
	m_pwsFuel->SetEditMode();
	m_pwsFuel->setFocus();
}

// -----------------------------------------------------------------------

bool DialogStartup::HandleNesisInputEvent(PanelButton pb)
{
	if(m_pLayout->currentIndex()==0) {
		if(pb == pbOk) {
			qDebug() << "To next screen";
			m_pLayout->setCurrentIndex(1);
			m_pwcPilots->setFocus();
			m_pwcPilots->SetEditMode();
		}
	}

	qDebug() << "Key" << (int)pb;
	return true;
}

// -----------------------------------------------------------------------

void DialogStartup::OnValueChanged(double dFuel)
{
	m_uvFuel.SetValue(dFuel);
	int iVal = qBound(0, qRound(100 * dFuel / m_pwsFuel->maximum()), 100);
	m_pwBar->setValue(iVal);
}

// -----------------------------------------------------------------------

void DialogStartup::InitializePilotList()
{
	logbook::PilotList* pPL = logbook::PilotList::GetInstance();
	Settings* pS = Settings::GetInstance();

	// Set list of pilots
	QStringList sl = pPL->GetList();
	sl.sort();
	m_pwcPilots->addItems(sl);

	// Get current pilot from settings
	QString qs = pS->GetCurrentPilot();

	// Select correct pilot from the list
	int iFound = m_pwcPilots->findText(qs);
	if(iFound != -1) {
		m_pwcPilots->setCurrentIndex(iFound);
	}
	// If not found, select the first one.
	else {
		m_pwcPilots->setCurrentIndex(0);
	}
}

// -----------------------------------------------------------------------

unit::Value DialogStartup::GetFuel() const
{
	m_uvFuel.SetValue(m_pwsFuel->value());
	return m_uvFuel;
}

// -----------------------------------------------------------------------

QString DialogStartup::GetPilot() const
{
	int iC = m_pwcPilots->currentIndex();
	if(iC >=0 )
		return m_pwcPilots->itemText(iC);
	return QString();
}
