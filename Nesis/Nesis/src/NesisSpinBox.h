#ifndef __NESIS_SPINBOX_H
#define __NESIS_SPINBOX_H

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

#include <QSpinBox>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class NesisSpinBox : public QSpinBox, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Knob will forward focus
		mEdit				//!< Knob will edit internal spin.
	};

public:
	//! Constructor
	NesisSpinBox(int iFixDigits = 0, QWidget* pParent = 0);
	//! Destructor
	~NesisSpinBox();
	
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
	
protected:
	//! String formating - display leading zeroes if m_iFixDigits is set.
	QString textFromValue (int iValue) const;

	//! Paint event.
	virtual void paintEvent ( QPaintEvent * event );

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

private:
	//! Current controling mode.
	Mode m_eMode;
	//! Number of fix digits formating (0 means default)
	int m_iFixDigits;
	//! Initial value
	int m_iOldValue;
};

// -------------------------------------------------------------------------

#endif	// __NESIS_SPINBOX_H
