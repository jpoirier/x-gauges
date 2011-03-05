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

#include "NesisEventFilter.h"

// --------------------------------------------------------------------

NesisEventFilter::NesisEventFilter(QObject *pParent)
: QObject(pParent)
{
}


NesisEventFilter::~NesisEventFilter()
{
}

// --------------------------------------------------------------------

/*! Here we catch the panel actions and transfer them into appropriate
    handlers. Menu buttons 1-5 below the screen, Ok and Cancel buttons
    are forwarded to the HandleButtonEvent virtual function. The rotating
    knob actions are forwarded to the HandleWheelEvent virtual function.
 */
bool NesisEventFilter::eventFilter(QObject* /*pObj*/, QEvent* pEvent)
{
	// We pass all keyboard keys here.
	if(pEvent->type()==QEvent::KeyPress) {
		// Get the widget with the focus
		QWidget* pwFocus = qApp->focusWidget();
		// Can we cross-cast the focus widget into AbstractNesisInput?
		AbstractNesisInput* pANI = dynamic_cast<AbstractNesisInput*>(pwFocus);

		// What do we have here?
		QKeyEvent* pKey = static_cast<QKeyEvent*>(pEvent);
		const int iKey = pKey->key();
// 		qDebug() << "Key " << iKey;
		AbstractNesisInput::PanelButton epb = AbstractNesisInput::pbInvalid;
		if(iKey >= KEYCODE_BTN_1 && iKey <= KEYCODE_BTN_POWER) {
//  			qDebug() << "Key " << pKey->key();
			switch (iKey) {
				case KEYCODE_BTN_1		: epb = AbstractNesisInput::pbBtn1; break;
				case KEYCODE_BTN_2		: epb = AbstractNesisInput::pbBtn2; break;
				case KEYCODE_BTN_3		: epb = AbstractNesisInput::pbBtn3; break;
				case KEYCODE_BTN_4		: epb = AbstractNesisInput::pbBtn4; break;
				case KEYCODE_BTN_5		: epb = AbstractNesisInput::pbBtn5; break;
				case KEYCODE_BTN_OK		: epb = AbstractNesisInput::pbOk;   break;
				case KEYCODE_BTN_CANCEL	: epb = AbstractNesisInput::pbCancel; break;
				case KEYCODE_KNOB_CW	   : epb = AbstractNesisInput::pbKnobCW; break;
				case KEYCODE_KNOB_CCW	: epb = AbstractNesisInput::pbKnobCCW; break;
				case KEYCODE_BTN_POWER	: emit Shutdown(); return true;
				default						: epb = AbstractNesisInput::pbInvalid;
			}
			if(epb != AbstractNesisInput::pbInvalid) {
				if(pANI)
					return pANI->HandleNesisInputEvent(epb);
				else {
					qDebug() << "Call the main widget handler";
					return false;
				}
				return true;
			}
		}
	}
	// Let other classes handle unknown evetns.
	return false;
}

// --------------------------------------------------------------------
