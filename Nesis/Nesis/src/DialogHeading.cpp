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
#include "CommonDefs.h"
#include "GraphicsSingleton.h"
#include "NesisSpinBox.h"

#include "DialogHeading.h"

#define SPIN_DIGIT_WIDTH		30

// ----------------------------------------------------------------------

DialogHeading::DialogHeading(int iHeading, QWidget* pParent)
: AbstractNesisDialog(pParent)
{
	ASSERT(iHeading >= -1);
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	const QFont& font = pGS->GetFont(GraphicsSingleton::fDialog);

	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	// Label
	QLabel* pLbl = new QLabel(tr("Set Heading:"));
	pLbl->setFont(font);
	pLayout->addWidget(pLbl, 0, Qt::AlignHCenter);

	// Heading as three spin boxes
	QHBoxLayout* pHeadingLayout = new QHBoxLayout;
	pLayout->addLayout(pHeadingLayout);
	pHeadingLayout->addStretch(10);

	// Hundreds
	m_pwHundreds = new NesisSpinBox(0);
	m_pwHundreds->setFont(font);
	m_pwHundreds->setRange(0,3);
	m_pwHundreds->setValue(iHeading/100);
	m_pwHundreds->selectAll();
	pHeadingLayout->addWidget(m_pwHundreds);

	// Tens
	m_pwTens = new NesisSpinBox(0);
	m_pwTens->setFont(font);
	m_pwTens->setRange(0,9);
	m_pwTens->setValue((iHeading%100)/10);
	pHeadingLayout->addWidget(m_pwTens);

	// Ones
	m_pwOnes = new NesisSpinBox(0);
	m_pwOnes->setFont(font);
	m_pwOnes->setRange(0,9);
	m_pwOnes->setValue(iHeading%10);
	pHeadingLayout->addWidget(m_pwOnes);
	pHeadingLayout->addStretch(10);

	// Set width
	m_pwHundreds->setMaximumWidth(SPIN_DIGIT_WIDTH);
	m_pwTens->setMaximumWidth(SPIN_DIGIT_WIDTH);
	m_pwOnes->setMaximumWidth(SPIN_DIGIT_WIDTH);

	// connections

	// Ok button on the hundreds, moves to the tens.
	connect(m_pwHundreds, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_pwHundreds, SIGNAL(NesisOk()), m_pwTens, SLOT(SetEditMode()));
	// Ok button on the tens, moves to the ones.
	connect(m_pwTens, SIGNAL(NesisOk()), this, SLOT(FocusNextChild()));
	connect(m_pwTens, SIGNAL(NesisOk()), m_pwOnes, SLOT(SetEditMode()));
	// Ok button on the last digit (ones) completes the input.
	connect(m_pwOnes, SIGNAL(NesisOk()), this, SLOT(accept()));

	// Cancel button rejects the input if selected on hundreds.
	connect(m_pwHundreds, SIGNAL(NesisCancel()), this, SLOT(reject()));
	// Cancel button moves to the previous on tens.
	connect(m_pwTens, SIGNAL(NesisCancel()), this, SLOT(FocusPreviousChild()));
	// Cancel button moves to the previous on ones.
	connect(m_pwOnes, SIGNAL(NesisCancel()), this, SLOT(FocusPreviousChild()));

	// Knob moves the focus
	connect(m_pwHundreds, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pwTens, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
	connect(m_pwOnes, SIGNAL(NesisKnob(bool)), this, SLOT(FocusNextPrevChild(bool)));
}

DialogHeading::~DialogHeading()
{
}

// --------------------------------------------------------------------

int DialogHeading::GetHeading() const
{
	return m_pwHundreds->value() * 100 +
          m_pwTens->value()     * 10  +
          m_pwOnes->value();
}

