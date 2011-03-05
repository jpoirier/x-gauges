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

#include "NesisCheckBox.h"

// -----------------------------------------------------------------------

NesisCheckBox::NesisCheckBox(const QString& qsText, QWidget *pParent)
: QCheckBox(qsText, pParent)
{
}

// -----------------------------------------------------------------------

NesisCheckBox::~NesisCheckBox()
{
}

// -----------------------------------------------------------------------

bool NesisCheckBox::HandleNesisInputEvent(PanelButton ePB)
{
	switch(ePB) {
/*		// Check button
		case pbBtn1: setCheckState(Qt::Checked); break;
		// Up button
		case pbBtn2: setCheckState(Qt::Unchecked); break;*/
		
		// Knob CW = clockwise
		case pbKnobCW: 
			emit NesisKnob(true); // true = CW
			break;
		// Knob CCW = counterclockwise
		case pbKnobCCW: 
			emit NesisKnob(false); // false = CCW
			break;
		
		// OK button was pressed
		case pbOk: 
			toggle();
			emit NesisOk();
			break;
			
		// Cancel button was pressed
		case pbCancel:
			emit NesisCancel();
			break;
			
		// Not handled here - forward this to interested objects.
		default:
			emit NesisButton(ePB-pbBtn1);
			break;
	}
	return true;
}

// -----------------------------------------------------------------------

QStringList NesisCheckBox::GetButtonLabels() const
{
	QStringList sl;
	return sl;
}

// -----------------------------------------------------------------------

void NesisCheckBox::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QCheckBox::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisCheckBox::focusOutEvent(QFocusEvent* pEvent)
{
	QCheckBox::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------
