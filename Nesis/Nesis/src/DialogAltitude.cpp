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

#include <QtGui>
#include "CommonDefs.h"
#include "GraphicsSingleton.h"
#include "NesisSpinBox.h"
#include "NesisCheckBox.h"
#include "NesisEditBox.h"
#include "Settings.h"

#include "DialogAltitude.h"

#define SPIN_DIGIT_WIDTH		21

// ------------------------------------------------------------------------

DialogAltitude::DialogAltitude(unit::Value uvAlt, QWidget* pParent)
 : AbstractNesisDialog(pParent)
{
	Q_ASSERT(uvAlt.GetValue() > 0);
	m_uvAlt = uvAlt;

	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	QFont font = pGS->GetFont(GraphicsSingleton::fDialog);
	QFont fontSm = pGS->GetFont(GraphicsSingleton::fDialogSmall);

	// Main vertical layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	// Label
	QLabel* pLbl = new QLabel(tr("Set altitude:"));
	pLbl->setFont(font);
	pLayout->addWidget(pLbl, 0, Qt::AlignHCenter);

	// Units for altitude (only these are allowed.)
	Q_ASSERT(m_uvAlt.GetUnit()->GetSignature()=="feet" ||
	         m_uvAlt.GetUnit()->GetSignature()=="m");

	bool bFeet = (m_uvAlt.GetUnit()->GetSignature()=="feet");

	// Set initial selection (snap to nearest value)
	int iAltBug = common::Snap(m_uvAlt.GetValue(), 50);

	// Input in another layout
	QHBoxLayout* pAltitudeLayout = new QHBoxLayout;
	pLayout->addLayout(pAltitudeLayout);
	pAltitudeLayout->addStretch(1);

	m_pwThousands = new NesisSpinBox(2); // Two digits
	m_pwThousands->setFont(font);
	m_pwThousands->setRange(0, (bFeet ? 30 : 10));
	m_pwThousands->setValue(iAltBug/1000);
	m_pwThousands->setAlignment(Qt::AlignHCenter);
	m_pwThousands->selectAll();
	pAltitudeLayout->addWidget(m_pwThousands);

	m_pwHundreds = new NesisSpinBox(3); // Three digits
	m_pwHundreds->setFont(font);
	m_pwHundreds->setRange(0,950);
	m_pwHundreds->setSingleStep(50);
	m_pwHundreds->setValue(iAltBug % 1000);
	m_pwHundreds->setAlignment(Qt::AlignHCenter);
	pAltitudeLayout->addWidget(m_pwHundreds);

	pLbl = new QLabel(m_uvAlt.GetUnit()->GetSignature());
	pLbl->setFont(font);
	pAltitudeLayout->addWidget(pLbl);
	pAltitudeLayout->addStretch(1);

	// Set width
	m_pwThousands->setMaximumWidth(SPIN_DIGIT_WIDTH*2+1);
	m_pwHundreds->setMaximumWidth(SPIN_DIGIT_WIDTH*3+1);

	// connections

	// Ok button on the thousands moves to the hundreds.
	connect(m_pwThousands, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_pwThousands, SIGNAL(NesisOk()), m_pwHundreds, SLOT(SetEditMode()));
	// Ok button on the hundreds completes the input.
	connect(m_pwHundreds, SIGNAL(NesisOk()), this, SLOT(accept()));

	// Cancel button rejects the input if selected on thousands.
	connect(m_pwThousands, SIGNAL(NesisCancel()), this, SLOT(reject()));
	// Cancel button moves to the previous on hundreds.
	connect(m_pwHundreds, SIGNAL(NesisCancel()), this, SLOT(FocusPreviousChild()));
//	connect(m_pwHundreds, SIGNAL(NesisCancel()), m_pwThousands, SLOT(SetEditMode()));

	// Knob moves the focus
	connect(m_pwThousands, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pwHundreds, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
}

DialogAltitude::~DialogAltitude()
{
}

// ------------------------------------------------------------------------

const unit::Value& DialogAltitude::GetAltitude() const
{
	// We need altitude in meters.
	int iAlt = m_pwThousands->value()*1000 + m_pwHundreds->value();
	m_uvAlt.SetValue(iAlt);
	return m_uvAlt;
}

// ------------------------------------------------------------------------
