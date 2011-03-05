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

#include "NesisSpinBox.h"

// -----------------------------------------------------------------------

NesisSpinBox::NesisSpinBox(int iFixDigits, QWidget *pParent)
: QSpinBox(pParent)
{
	m_eMode = mForwardFocus;
	m_iFixDigits = iFixDigits; // zero means default.
	
	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setKeyboardTracking(false);
	setAlignment(Qt::AlignHCenter);
	setWrapping(true);
}

// -----------------------------------------------------------------------

NesisSpinBox::~NesisSpinBox()
{
}

// -----------------------------------------------------------------------

bool NesisSpinBox::HandleNesisInputEvent(PanelButton ePB)
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
				m_eMode = mForwardFocus;
				emit NesisOk();
			}
			break;
			
		// Cancel button was pressed
		case pbCancel:
			if(m_eMode == mEdit)  {
				m_eMode = mForwardFocus;
				setValue(m_iOldValue);
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

void NesisSpinBox::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode=mEdit;
		ShowButtons();
		m_iOldValue = value();
	}	
}

// -----------------------------------------------------------------------

QStringList NesisSpinBox::GetButtonLabels() const
{
	QStringList sl;
// 	sl << tr("Down");
// 	sl << tr("Up");
// 	if(m_eMode == mDefault)
// 		sl << tr("Use Knob");
// 	else
// 		sl << QString();

	return sl;
}

// -----------------------------------------------------------------------

void NesisSpinBox::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QSpinBox::focusInEvent(pEvent);
	// Show the buttons according to the mode.
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisSpinBox::focusOutEvent(QFocusEvent* pEvent)
{
	// TODO Should we restore the mode?
	QSpinBox::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

QString NesisSpinBox::textFromValue (int iValue) const
{
	if(m_iFixDigits==0)
		return QSpinBox::textFromValue(iValue);
	
	return QString("%1").arg(iValue, m_iFixDigits, 10, QChar('0'));
}

// -----------------------------------------------------------------------

void NesisSpinBox::paintEvent(QPaintEvent* event)
{
// 	QPainter p;
// 	p.drawRect(1,1,10,10);
// 	event->ignore();
	QSpinBox::paintEvent(event);
}

