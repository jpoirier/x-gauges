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

#include "NesisGroupBox.h"

// -----------------------------------------------------------------------

NesisGroupBox::NesisGroupBox(QWidget *pParent)
: QGroupBox(pParent)
{
// 	setKeyboardTracking(false);
}

// -----------------------------------------------------------------------

NesisGroupBox::~NesisGroupBox()
{
}

// -----------------------------------------------------------------------

bool NesisGroupBox::HandleNesisInputEvent(PanelButton ePB)
{
	switch(ePB) {
		case pbBtn1:
		case pbBtn2:
		case pbBtn3:

		// Knob CW = clockwise
		case pbKnobCW:
			break;
		// Knob CCW = counterclockwise
		case pbKnobCCW:
			break;

		// OK button was pressed
		case pbOk:
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

QStringList NesisGroupBox::GetButtonLabels() const
{
	QStringList sl;
	sl << tr("Down");
	sl << tr("Up");

	return sl;
}

// -----------------------------------------------------------------------

void NesisGroupBox::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QGroupBox::focusInEvent(pEvent);
	// Show the buttons according to the mode.
// 	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisGroupBox::focusOutEvent(QFocusEvent* pEvent)
{
	// TODO Should we restore the mode?
	QGroupBox::focusOutEvent(pEvent);
// 	HideButtons();
}

// -----------------------------------------------------------------------
