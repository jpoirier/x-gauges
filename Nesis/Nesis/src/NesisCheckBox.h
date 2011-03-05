#ifndef __NESIS_CHECKBOX_H
#define __NESIS_CHECKBOX_H

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

#include <QCheckBox>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class NesisCheckBox : public QCheckBox, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Constructor
	NesisCheckBox(const QString& qsText, QWidget* pParent = 0);
	//! Destructor
	~NesisCheckBox();
	
	//! Handle the Nesis input here.
	bool HandleNesisInputEvent(PanelButton ePB);
	//! Get button labels according to current mode.
	QStringList GetButtonLabels() const;

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);
				
signals:
	//! Nesis knob was pressed.
	void NesisKnob(bool bCW);
	//! Nesis Ok button was pressed.
	void NesisOk();
	//! Nesis Cancel was pressed.
	void NesisCancel();
	//! Nesis Button was pressed.
	void NesisButton(int iBtn);
};

// -------------------------------------------------------------------------

#endif	// __NESIS_CHECKBOX_H
