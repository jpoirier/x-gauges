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

#include "NesisBrowser.h"

// -----------------------------------------------------------------------

NesisBrowser::NesisBrowser(QWidget *pParent)
: QTextBrowser(pParent)
{
}

// -----------------------------------------------------------------------

NesisBrowser::~NesisBrowser()
{
}

// -----------------------------------------------------------------------

bool NesisBrowser::HandleNesisInputEvent(PanelButton ePB)
{
	switch(ePB) {
		// Back button
		case pbBtn1: backward(); break;
		// Forward button
		case pbBtn2: forward(); break;
		// Page Down button
		case pbBtn3: 
			qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
			break;
		// Page Up button
		case pbBtn4: 
			qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
			break;
		
		// Go to next link Knob CW = clockwise
		case pbKnobCW: 
			qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
			break;
		// Knob CCW = counterclockwise
		case pbKnobCCW: 
			qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
			break;
		
		// OK button was pressed
		case pbOk: 
			qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
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

QStringList NesisBrowser::GetButtonLabels() const
{
	QStringList sl;
	sl << tr("Back");
	sl << tr("Forward");
	sl << tr("Pg Down");
	sl << tr("Pg Up");

	return sl;
}

// -----------------------------------------------------------------------

void NesisBrowser::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QTextBrowser::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisBrowser::focusOutEvent(QFocusEvent* pEvent)
{
	if(pEvent->reason() == Qt::TabFocusReason || pEvent->reason() == Qt::BacktabFocusReason) {
		// FIXME The following line may be causing problems.
		setFocus(Qt::OtherFocusReason);
		return;
	}

	QTextBrowser::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------
