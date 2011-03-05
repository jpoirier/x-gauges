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

#include "NesisEditBox2.h"


#define CHARS_ALPHA 		"ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
// #define CHARS_NUMERIC 	"0123456789"
// #define CHARS_SYMBOL 	".,;:!?()<>[]*@#%&_"

// -----------------------------------------------------------------------

NesisEditBox2::NesisEditBox2(
		const QString& qsText,
		QWidget *pParent
)
: QLineEdit(qsText, pParent)
{
	m_eMode = mForwardFocus;
	m_plBuddy = NULL;
	m_iKnob = 0;
	m_qsSubset = CHARS_ALPHA;
}

// -----------------------------------------------------------------------

NesisEditBox2::~NesisEditBox2()
{
}

// -----------------------------------------------------------------------

bool NesisEditBox2::HandleNesisInputEvent(PanelButton ePB)
{
	// Start mode
	if(m_eMode == mForwardFocus) {
		switch(ePB) {
			case pbKnobCW:
				emit NesisKnob(true);
				break;
			case pbKnobCCW:
				emit NesisKnob(false);
				break;
			case pbOk:
				SetEditMode();
				break;
			case pbCancel:
				emit NesisCancel();
				break;
			default:
				emit NesisButton(ePB-pbBtn1);
				break;
		}
	}

	// Edit mode
	else {
		Q_ASSERT(m_eMode == mEdit);
		QPalette pl;
		switch(ePB) {
			// Cancel puts you back into the cell selection mode.
			case pbCancel:
				m_eMode = mForwardFocus;
				HideButtons();
				emit NesisEditCancel();
				pl.setColor(QPalette::Base, QColor("white"));
				setPalette(pl);
				break;
			// Ok inserts buddy character at cursor position.
			case pbOk:
				Insert(m_qsSubset.at(m_iKnob));
				break;
			// Knob changes the buddy character.
			case pbKnobCW:
				m_iKnob++;
				if(m_iKnob >= m_qsSubset.size())
					m_iKnob = 0;
				ShowBuddy();
				break;
			case pbKnobCCW:
				m_iKnob--;
				if(m_iKnob < 0)
					m_iKnob = m_qsSubset.size()-1;
				ShowBuddy();
				break;
			// Delete
			case pbBtn1:
				DeleteChar();
				break;
			// Finish
			case pbBtn2:
				m_eMode = mForwardFocus;
				HideButtons();
				emit NesisOk();
				pl.setColor(QPalette::Base, QColor("white"));
				setPalette(pl);
				break;
				
			default:
				emit NesisButton(ePB-pbBtn1);
				break;
		}
	}

	return true;
}

// -----------------------------------------------------------------------

QStringList NesisEditBox2::GetButtonLabels() const
{
	QStringList sl;
	if(m_eMode == mEdit) {
		sl << tr("Delete");
		sl << tr("Accept");
	}
	return sl;
}

// -----------------------------------------------------------------------

void NesisEditBox2::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QLineEdit::focusInEvent(pEvent);
	deselect();
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisEditBox2::focusOutEvent(QFocusEvent* pEvent)
{
	QLineEdit::focusOutEvent(pEvent);
	HideButtons();
}

// -------------------------------------------------------------------------

void NesisEditBox2::DeleteChar()
{
	backspace();
}

// -------------------------------------------------------------------------

void NesisEditBox2::ShowButtons(bool bShow)
{
	AbstractNesisInput::ShowButtons(bShow);
	ShowBuddy(bShow);
}

// -------------------------------------------------------------------------

void NesisEditBox2::ShowBuddy(bool bShow)
{
	if(m_plBuddy) {
		if(bShow == false)
			m_plBuddy->hide();
		else if(m_eMode==mEdit && m_iKnob < m_qsSubset.size()) {
			m_plBuddy->setText(m_qsSubset.at(m_iKnob));
			m_plBuddy->show();
		}
	}
}

// -------------------------------------------------------------------------

void NesisEditBox2::SetEditMode()
{
	m_eMode = mEdit;
	m_iKnob = 0;
	ShowButtons(true);
	QPalette pl;
	pl.setBrush(QPalette::Base, palette().highlight());
	setPalette(pl);
}

// -------------------------------------------------------------------------

void NesisEditBox2::Insert(const QString& qs)
{
	if(qs == "_")
		insert(" ");
	else
		insert(qs);
}

// -------------------------------------------------------------------------
