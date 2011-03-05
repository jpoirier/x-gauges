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
#include "GraphicsSingleton.h"

#include "DialogYesNo.h"

// --------------------------------------------------------------------
// TODO Larger fonts?

DialogYesNo::DialogYesNo(
	const QString& qsLblText,
//	const QStringList& slButtons,
	QWidget *pParent
)
 : AbstractNesisDialog(pParent)
{
	setFocusPolicy(Qt::StrongFocus);

	m_bYes = false;

	// Buttons to display
//	m_slButtons = slButtons;
//	if(m_slButtons.isEmpty()) {
		m_slButtons << QString();
		m_slButtons << tr("Yes");
		m_slButtons << QString();
		m_slButtons << tr("No");
//	}
	
	// vertical layout
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);

	// font
	QFont font = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialog);
	
	// label
	QLabel* pLbl = new QLabel(qsLblText);
	pLbl->setFont(font);
	pMainLayout->addWidget(pLbl);
	
	// horizontal layout - buttons
	QHBoxLayout* pBtnLayout = new QHBoxLayout;
	pMainLayout->addLayout(pBtnLayout);

	for(int i=0; i<m_slButtons.size(); i++) {
		if(m_slButtons[i].isEmpty() == false) {
			QPushButton* pb = new QPushButton(m_slButtons[i]);
			pb->setFont(font);
			pb->setFocusPolicy(Qt::NoFocus);
			pBtnLayout->addWidget(pb);
		}
		else {
			pBtnLayout->addStretch(1);
		}
	} 
	
	// FIXME This call may have no effect in some enviroments.
	setFocus(Qt::OtherFocusReason);
}

DialogYesNo::~DialogYesNo()
{
}
// ------------------------------------------------------------------------

QStringList DialogYesNo::GetButtonLabels() const
{
	return m_slButtons;
}

// ------------------------------------------------------------------------

bool DialogYesNo::HandleNesisInputEvent(PanelButton pb)
{
	switch(pb) {
		case pbBtn2:
		case pbOk:
			m_bYes = true;
			accept();
			break;
		case pbBtn4:
		case pbCancel:
			m_bYes = false;
			reject();
			break;
		default:
			break;
	}
	return true;
}

// ------------------------------------------------------------------------

void DialogYesNo::focusInEvent(QFocusEvent* pEvent)
{
	AbstractNesisDialog::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void DialogYesNo::focusOutEvent(QFocusEvent* pEvent)
{
	AbstractNesisDialog::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------
