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

#include "NesisDoubleSpinBox.h"

// -----------------------------------------------------------------------

NesisDoubleSpinBox::NesisDoubleSpinBox(int iDecimals, QWidget *pParent)
: QDoubleSpinBox(pParent)
{
	m_eMode = mForwardFocus;
	m_bAuto = false;
	setDecimals(iDecimals);
	
	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setKeyboardTracking(false);
	setAlignment(Qt::AlignHCenter);
	setWrapping(true);
	lineEdit()->setReadOnly(true);

}

// -----------------------------------------------------------------------

NesisDoubleSpinBox::~NesisDoubleSpinBox()
{
}

// -----------------------------------------------------------------------

bool NesisDoubleSpinBox::HandleNesisInputEvent(PanelButton ePB)
{
	switch(ePB) {
		// Knob CW = clockwise
		case pbKnobCW: 
			if(m_eMode==mEdit)
				stepUp();
			else // forward focus 
				emit NesisKnob(true); // true = CW
			break;
		// Knob CCW = counterclockwise
		case pbKnobCCW: 
			if(m_eMode==mEdit)
				stepDown();
			else // forward focus
				emit NesisKnob(false); // false = CCW
			break;
		
		// OK button was pressed
		case pbOk:
			if(m_eMode==mForwardFocus) {
				SetEditMode();
			}
			else {
				// m_eMode = m_eStartMode; 
				// ShowButtons();
				lineEdit()->deselect();
				m_eMode=mForwardFocus;
				emit NesisOk();
			}
			break;
			
		// Cancel button was pressed
		case pbCancel:
			if(m_eMode == mEdit)  {
				m_eMode = mForwardFocus;
				setValue(m_dOldValue);
				lineEdit()->deselect();
				emit NesisEditCancel();
			}
			else {
				//ShowButtons();
				emit NesisCancel();
			}
			break;
			
		// Not handled here - forward this to interested objects.
		default:
			emit NesisButton(ePB-pbBtn1);
			break;
	}
	return true;
}
// -----------------------------------------------------------------------

void NesisDoubleSpinBox::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode=mEdit;
		ShowButtons();
		m_dOldValue = value();
		selectAll();
		lineEdit()->setCursorPosition(-1);
	}
	update();
}

// -----------------------------------------------------------------------

QStringList NesisDoubleSpinBox::GetButtonLabels() const
{
	QStringList sl;
	return sl;
}

// -----------------------------------------------------------------------

void NesisDoubleSpinBox::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QDoubleSpinBox::focusInEvent(pEvent);
	lineEdit()->deselect();
	
	if (m_bAuto)
		SetEditMode();
	
	// Show the buttons according to the mode.
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisDoubleSpinBox::focusOutEvent(QFocusEvent* pEvent)
{
	// TODO Should we restore the mode?
	QDoubleSpinBox::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

void NesisDoubleSpinBox::paintEvent(QPaintEvent* event)
{
	QPalette pl;
	if(m_eMode == mEdit)  {
		pl.setBrush(QPalette::Base, palette().highlight());
	}
	
	setPalette(pl);
	QDoubleSpinBox::paintEvent(event);
}

// -----------------------------------------------------------------------

void NesisDoubleSpinBox::SetAutoEditMode(bool bAuto)
{
	m_bAuto = bAuto;
}

// -----------------------------------------------------------------------
