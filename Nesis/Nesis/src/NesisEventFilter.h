#ifndef __NESIS_EVENT_FILTER_H
#define __NESIS_EVENT_FILTER_H

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

#include <QObject>
#include <QEvent>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

#define KEYCODE_BTN_1			Qt::Key_F1
#define KEYCODE_BTN_2			Qt::Key_F2
#define KEYCODE_BTN_3			Qt::Key_F3
#define KEYCODE_BTN_4			Qt::Key_F4
#define KEYCODE_BTN_5			Qt::Key_F5
#define KEYCODE_BTN_OK			Qt::Key_F6
#define KEYCODE_BTN_CANCEL		Qt::Key_F7

#define KEYCODE_BTN_POWER		Qt::Key_F12

#define KEYCODE_KNOB_CW			Qt::Key_F9
#define KEYCODE_KNOB_CCW		Qt::Key_F10

// -------------------------------------------------------------------------
/** \brief Nesis event filter inserted into QApplication.

    This class catches events as soon as possible (in QApplication's
    event loop) and detects Qt events that are used to control the Nesis.
    These events are translated into more specific form and
    and transmited to the active control using virtual functions.
    The controls must be derived from the \a AbstractNesisInput class.
 */

class NesisEventFilter : public QObject
{
Q_OBJECT
public:
	//! Constructor
	NesisEventFilter(QObject *pParent=0);
	//! Destructor
	virtual ~NesisEventFilter();

	//! The event filter function. We need to catch panel actions here.
	virtual bool eventFilter(QObject *pObj, QEvent *pEvent);

signals:
	//! Shutdown signal.
	void Shutdown();
};
// -------------------------------------------------------------------------

#endif
