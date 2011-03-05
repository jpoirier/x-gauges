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

#include "ISA/Altitude.h"
#include "NesisDoubleSpinBox.h"
#include "GraphicsSingleton.h"
#include "FastUnitKeys.h"
#include "Settings.h"
#include "NesisAerospaceDataManager.h"

#include "DialogQNH.h"

// -----------------------------------------------------------------------

DialogQNH::DialogQNH(
	const unit::Value& uvQNH,	//! QNH pressure given in user units
	QWidget *pParent
)
	: AbstractNesisDialog(pParent)
{
	m_uvQNH = uvQNH;
	
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	const QFont& font = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialog);

	// Debug check
	Q_ASSERT(m_uvQNH.GetUnit()->GetSignature()=="hPa" || 
				m_uvQNH.GetUnit()->GetSignature()=="inHg");

	// Label
	QLabel* pLbl = new QLabel(tr("Set QNH:"));
	pLbl->setFont(font);
	pLayout->addWidget(pLbl);
	
	// Unit conversion
	bool bmmHg = m_uvQNH.GetUnit()->GetSignature()=="inHg";
	int iDec    = bmmHg ? 2 : 0;
	float fStep = bmmHg ? 0.02f : 1.0f; 
	float fQNH = common::Snap(m_uvQNH.GetValue(), fStep);
	float fMin = common::Snap(unit::Value(pFUK->iKey_hPa,  650).GetAs(m_uvQNH.GetUnit()), fStep);
	float fMax = common::Snap(unit::Value(pFUK->iKey_hPa, 1080).GetAs(m_uvQNH.GetUnit()), fStep);

	// Spin box
	m_pwSpinQNH = new NesisDoubleSpinBox(iDec);
	// TODO Read units from settings.
	m_pwSpinQNH->setFont(font);
	m_pwSpinQNH->setSuffix(QString(" ") + m_uvQNH.GetUnit()->GetSignature());
	m_pwSpinQNH->setMinimum(fMin);
	m_pwSpinQNH->setMaximum(fMax);
	m_pwSpinQNH->setValue(fQNH);
	m_pwSpinQNH->setSingleStep(fStep);
	pLayout->addWidget(m_pwSpinQNH);
	
	m_pLblQFE = new QLabel();
	m_pLblQFE->setFont(font);
	pLayout->addWidget(m_pLblQFE);
	
	connect(m_pwSpinQNH, SIGNAL(NesisOk()), this, SLOT(accept()));
	connect(m_pwSpinQNH, SIGNAL(NesisEditCancel()), this, SLOT(reject()));
	connect(m_pwSpinQNH, SIGNAL(valueChanged(double)), this, SLOT(OnQNHValueChanged(double)));
	
	// Set the label
	OnQNHValueChanged(fQNH);
	
	// This is the only control. Set the edit mode directly.
	m_pwSpinQNH->SetEditMode();
}

// -----------------------------------------------------------------------

DialogQNH::~DialogQNH()
{
}

// -----------------------------------------------------------------------

unit::Value DialogQNH::GetQNH() const
{
	m_uvQNH.SetValue(m_pwSpinQNH->value());
	return m_uvQNH;
}

// -----------------------------------------------------------------------

void DialogQNH::OnQNHValueChanged(double dQNH)
{
	FastUnitKeys* pFUK = FastUnitKeys::GetInstance();
	// Convert into hPa
	m_uvQNH.SetValue(dQNH);
	isa::Altitude alt(m_uvQNH.GetAs(pFUK->iKey_hPa));
	// Altitude in meters
	unit::Value uvAlt(
		pFUK->iKey_m, 
		alt.GetAltitude(NesisAerospaceDataManager::GetInstance()->GetStaticPressure())
	); 
	// Convert it into user units
	uvAlt.ConvertTo(Settings::GetInstance()->GetAltitudeUnit());
	
	m_pLblQFE->setText(QString(tr("QFE altitude %1.")).arg(uvAlt.Format(0)));
}

// -----------------------------------------------------------------------
