/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
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

#include "ISA/Altitude.h"
#include "NesisDoubleSpinBox.h"
#include "GraphicsSingleton.h"
#include "FastUnitKeys.h"
#include "Settings.h"
#include "NesisAerospaceDataManager.h"
#include "ParameterContainer.h"
#include "XMLTags.h"
#include "Unit/Manager.h"

#include "DialogFuelLevel.h"

// -----------------------------------------------------------------------

DialogFuelLevel::DialogFuelLevel(
	const unit::Value& uvFuel,	//! Fuel in tanks given in user units
	QWidget *pParent
)
	: AbstractNesisDialog(pParent)
{
	m_uvFuel = uvFuel;

	QHBoxLayout* pLayout = new QHBoxLayout;
	setLayout(pLayout);

	const QFont& font = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialog);

	// Debug check
	instrument::Parameter* pFuelLevel;
	pFuelLevel = instrument::ParameterContainer::GetInstance()->GetParameter(TAG_FUEL_LEVEL);
	unit::Manager* pM = unit::Manager::GetInstance();
	Q_ASSERT(m_uvFuel.GetUnit()->GetSignature()=="l" ||
				m_uvFuel.GetUnit()->GetSignature()=="gal");
	Q_ASSERT(m_uvFuel.GetUnit()->GetKey() == pFuelLevel->GetUnitKeyUser());

	// Vertical layput
	QVBoxLayout* pVerLayout = new QVBoxLayout;
	pLayout->addLayout(pVerLayout);

	// Label
	QLabel* pLbl = new QLabel(tr("Set Fuel:"));
	pLbl->setFont(font);
	pVerLayout->addWidget(pLbl);

	// Unit conversion
	bool bGal = m_uvFuel.GetUnit()->GetSignature()=="gal";
	int iDec    = bGal ?    1 : 0;
	float fStep = bGal ? 1.0f : 5.0f;
	float fFuel = common::Snap(m_uvFuel.GetValue(), fStep);
	float fMin  = 0.0;
	float fMax  = pM->Convert(
		pFuelLevel->GetRange().second, pFuelLevel->GetUnitKeySystem(), pFuelLevel->GetUnitKeyUser()
	);
	fMax  = common::Snap(fMax, fStep);

	// Spin box
	m_pwSpin = new NesisDoubleSpinBox(iDec);
	m_pwSpin->setFont(font);
	m_pwSpin->setSuffix(QString(" ") + m_uvFuel.GetUnit()->GetSignature());
	m_pwSpin->setMinimum(fMin);
	m_pwSpin->setMaximum(fMax);
	m_pwSpin->setValue(fFuel);
	m_pwSpin->setSingleStep(fStep);
	m_pwSpin->setWrapping(false);

	pVerLayout->addWidget(m_pwSpin);

	// Vertical bar

	m_pwBar = new QProgressBar;
	m_pwBar->setOrientation(Qt::Vertical);
	m_pwBar->setRange(0, 100);
	pLayout->addWidget(m_pwBar);

	connect(m_pwSpin, SIGNAL(NesisOk()), this, SLOT(accept()));
	connect(m_pwSpin, SIGNAL(NesisEditCancel()), this, SLOT(reject()));
	connect(m_pwSpin, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));

	// Set the label
	OnValueChanged(fFuel);
	// This is the only control. Set the edit mode directly.
	m_pwSpin->SetEditMode();
}

// -----------------------------------------------------------------------

DialogFuelLevel::~DialogFuelLevel()
{
}

// -----------------------------------------------------------------------

unit::Value DialogFuelLevel::GetFuel() const
{
	m_uvFuel.SetValue(m_pwSpin->value());
	return m_uvFuel;
}

// -----------------------------------------------------------------------

void DialogFuelLevel::OnValueChanged(double dFuel)
{
	m_uvFuel.SetValue(dFuel);
	int iVal = qBound(0,qRound(100 * dFuel / m_pwSpin->maximum()), 100);
	m_pwBar->setValue(iVal);
}

// -----------------------------------------------------------------------
