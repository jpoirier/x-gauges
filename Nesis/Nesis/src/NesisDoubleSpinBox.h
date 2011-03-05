#ifndef __NESIS_DOUBLE_SPINBOX_H
#define __NESIS_DOUBLE_SPINBOX_H

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

#include <QDoubleSpinBox>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class NesisDoubleSpinBox : public QDoubleSpinBox, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Knob signal is forwarded.
		mEdit				//!< Knob is editing.
	};

public:
	//! Constructor
	NesisDoubleSpinBox(int iDecimals, QWidget* pParent = 0);
	//! Destructor
	~NesisDoubleSpinBox();
	
	//! Handle the Nesis input here.
	bool HandleNesisInputEvent(PanelButton ePB);
	//! Get button labels according to current mode.
	QStringList GetButtonLabels() const;

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);

	//! Enter edit mode on focus In.
	void SetAutoEditMode(bool bAuto);

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

protected:
	//! Paint event.
	virtual void paintEvent ( QPaintEvent * event );


private:
	//! Current controling mode.
	Mode m_eMode;
	//! Flag that represent auto mode.
	bool m_bAuto;
	//! Initialily selected value.
	double m_dOldValue;
};

// -------------------------------------------------------------------------

#endif	// __NESIS_DOUBLE_SPINBOX_H
