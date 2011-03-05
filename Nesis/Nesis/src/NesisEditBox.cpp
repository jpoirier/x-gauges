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

#include "NesisEditBox.h"


#define CHARS_ALPHA 		"ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define CHARS_NUMERIC 	"0123456789"
#define CHARS_SYMBOL 	".,;:!?()<>[]*@#%&_"

// -----------------------------------------------------------------------

NesisEditBox::NesisEditBox(
		const QString& qsText,
		int iAllowedInput,
		Mode eMode,
		QWidget *pParent
)
: QLineEdit(qsText, pParent)
{
	m_eMode = eMode;
	m_eDefaultMode = m_eMode;
	m_uiAllowedInput = iAllowedInput;
	m_plBuddy = NULL;
	m_iKnob = 0;
}

// -----------------------------------------------------------------------

NesisEditBox::~NesisEditBox()
{
}

// -----------------------------------------------------------------------

bool NesisEditBox::HandleNesisInputEvent(PanelButton ePB)
{
	// Start mode
	if(m_eMode == mStart) {
		switch(ePB) {
			case pbKnobCW:
				emit NesisKnob(true);
				break;
			case pbKnobCCW:
				emit NesisKnob(false);
				break;
			case pbOk:
			case pbBtn1: // Edit
				m_eMode = mCell;
				ShowButtons();
				break;
			case pbCancel:
				emit NesisCancel();
				break;
			default:
				emit NesisButton(ePB-pbBtn1);
				break;
		}
	}

	// Cell selection mode
	else if(m_eMode == mCell) {
		switch(ePB) {
			case pbBtn1:	// Alpha
				if(m_uiAllowedInput & aiAlpha) {
					/*m_eMode = mChar;
					m_qsSubset = CHARS_ALPHA;
					m_iKnob = 0;
					ShowButtons();*/
					SetCharSelectionMode(CHARS_ALPHA);
				}
				break;
			case pbBtn2:	// Numeric
				if(m_uiAllowedInput & aiNumeric) {
					/*m_eMode = mChar;
					m_qsSubset = CHARS_NUMERIC;
					m_iKnob = 0;
					ShowButtons();*/
					SetCharSelectionMode(CHARS_NUMERIC);
				}
				break;
			case pbBtn3:	// Symbol
				if(m_uiAllowedInput & aiSymbol) {
					/*m_eMode = mChar;
					m_qsSubset = CHARS_SYMBOL;
					m_iKnob = 0;
					ShowButtons();*/
					SetCharSelectionMode(CHARS_SYMBOL);
				}
				break;

			case pbBtn4:   // Delete
				DeleteChar();
				break;

			case pbOk:
			case pbBtn5:   // Finish
				m_eMode = m_eDefaultMode;
				emit NesisOk();
				break;

			case pbCancel: // Cancel button
				m_eMode = m_eDefaultMode;
				emit NesisCancel();
				break;

			case pbKnobCW: // Next char
				setCursorPosition(cursorPosition()+1);
				break;
			case pbKnobCCW: // Previous char
				setCursorPosition(cursorPosition()-1);
				break;
			default:
				break;
		}
	}

	// Char selection mode
	else {
		Q_ASSERT(m_eMode == mChar);
		// Handle bottom buttons selection directly
		if(ePB <= pbBtn5) {
			int i = ePB - pbBtn1;
			const QString& qs = m_slLabels[i];
			// ignore if empty string
			if(qs.isEmpty())
				return true;
			// Is it final selection?
			if(qs.size()==1) {
				Insert(qs);
				SetCellSelectionMode();
				/*m_eMode = mCell;
				ShowButtons();
				m_iKnob = 0;
				ShowBuddy(false);*/
			}
			// No it is not. Display another level.
			else {
				/*m_qsSubset = qs;
				m_iKnob = 0;
				ShowButtons();*/
				SetCharSelectionMode(qs);
			}
			return true;
		}

		switch(ePB) {
			// Cancel puts you back into the cell selection mode.
			case pbCancel:
				SetCellSelectionMode();
				/*m_eMode = mCell;
				ShowButtons();
				m_iKnob = 0;
				ShowBuddy(false);*/
				break;
			// Ok inserts buddy character at cursor position.
			case pbOk:
				Insert(m_qsSubset.at(m_iKnob));
				SetCellSelectionMode();
				/*m_eMode = mCell;
				ShowButtons();
				m_iKnob = 0;
				ShowBuddy(false);*/
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
			default:
				break;
		}
	}

	return true;
}

// -----------------------------------------------------------------------

QStringList NesisEditBox::GetButtonLabels() const
{
	QStringList sl;
	if(m_eMode == mStart) {
		sl << tr("Edit");
	}
	else if(m_eMode == mCell) {
		sl << (m_uiAllowedInput & aiAlpha   ? tr("A-Z") : QString());
		sl << (m_uiAllowedInput & aiNumeric ? tr("0-9") : QString());
		sl << (m_uiAllowedInput & aiSymbol  ? tr("Symbols") : QString());
		sl << tr("Delete");
		sl << tr("Finish");
	}
	else {
		m_slLabels = sl = SplitCharacters(m_qsSubset);
	}

	return sl;
}

// -----------------------------------------------------------------------

void NesisEditBox::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QLineEdit::focusInEvent(pEvent);
	deselect();
// 	m_eMode = mCell;
//	setCursorPosition(text().count());
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisEditBox::focusOutEvent(QFocusEvent* pEvent)
{
	QLineEdit::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

QStringList NesisEditBox::SplitCharacters(const QString& qsSource) const
{
	const int nChars = qsSource.size();
	const int nBtn   = s_conBtn.size();
	Q_ASSERT(nChars > 0 && nBtn > 0);

	QStringList slSplit;

	// characters per button ...
	const int iI = nChars/nBtn;
	const int iR = nChars%nBtn;
	const int nActiveBtn = iI > 0 ? nBtn : iR;

	int iPos = 0;
	for(int i=0; i<nActiveBtn; i++) {
		int n=iI;		// default
		// Put the reminders on the back (optimised for letters)
		if((nActiveBtn-i-1)<iR) n++;  // remainder
//		if(i<iR) n++;  // remainder
		slSplit << qsSource.mid(iPos,n);
		iPos+=n;
	}

	// pad with empty strings
	while(slSplit.size() < nBtn)
		slSplit << QString();

	return slSplit;
}

// -------------------------------------------------------------------------

void NesisEditBox::DeleteChar()
{
	backspace();
}

// -------------------------------------------------------------------------

void NesisEditBox::ShowButtons(bool bShow)
{
	AbstractNesisInput::ShowButtons(bShow);
	ShowBuddy(bShow);
}

// -------------------------------------------------------------------------

void NesisEditBox::ShowBuddy(bool bShow)
{
	if(m_plBuddy) {
		if(bShow == false)
			m_plBuddy->hide();
		else if(m_eMode==mChar && m_iKnob < m_qsSubset.size()) {
			m_plBuddy->setText(m_qsSubset.at(m_iKnob));
			m_plBuddy->show();
		}
	}
}

// -------------------------------------------------------------------------

void NesisEditBox::SetCharSelectionMode(const QString& qsSelection)
{
	m_eMode = mChar;
	m_qsSubset = qsSelection;
	m_iKnob = 0;
	ShowButtons();
}

// -------------------------------------------------------------------------

void NesisEditBox::SetCellSelectionMode()
{
	m_eMode = mCell;
	ShowButtons();
	m_iKnob = 0;
	ShowBuddy(false);
}

// -------------------------------------------------------------------------

void NesisEditBox::Insert(const QString& qs)
{
	if(qs == "_")
		insert(" ");
	else
		insert(qs);
}

// -------------------------------------------------------------------------
