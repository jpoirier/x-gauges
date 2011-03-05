#ifndef __NESIS_COMBO_BOX_H
#define __NESIS_COMBO_BOX_H

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

#include <QComboBox>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class NesisComboBox : public QComboBox, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Focus is forwarded to the next control.
		mEdit				//!< Knob is used to select items.
	};

public:
	//! Constructor
	NesisComboBox(QWidget* pParent = 0);
	//! Destructor
	~NesisComboBox();
	
	//! Handle the Nesis input here.
	bool HandleNesisInputEvent(PanelButton ePB);
	//! Get button labels according to current mode.
	QStringList GetButtonLabels() const;

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);
public slots:
	//! Set edit mode.
	void SetEditMode();
				
signals:
	//! Nesis knob was pressed.
	void NesisKnob(bool bCW);
	//! Nesis Ok button was pressed.
	void NesisOk();
	//! Nesis Cancel was pressed.
	void NesisCancel();
	//! Nesis Cancel was pressed in edit mode.
	void NesisEditCancel();
	//! Nesis Button was pressed.
	void NesisButton(int iBtn);
	
	//! Nesis knob was pressed.
	void PopupMove(bool bCW);

protected:
	//! One line up helper.
	void GoLineUp();
	//! One line down helper.
	void GoLineDown();

protected:
	//! Current mode
	Mode m_eMode;	
	//! Old selection.
	int m_iOldIndex;
};

#endif	// __NESIS_COMBO_BOX_H
