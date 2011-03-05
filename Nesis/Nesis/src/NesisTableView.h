#ifndef __NESIS_TABLE_VIEW_H
#define __NESIS_TABLE_VIEW_H

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

#include <QTableView>
#include <QTableWidget>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class NesisTableView : public QTableView, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Forward knob event and try to move focus to next.
		mEdit				//!< Knob mode is active.
	};

public:
	//! Constructor
	NesisTableView(QWidget* pParent = 0);
	//! Destructor
	~NesisTableView();
	
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
private:
	//! One line up helper.
	void GoLineUp();
	//! One line down helper.
	void GoLineDown();
	
protected:
	//! Current mode
	Mode m_eMode;	
	//! Remember initial selection.
	QModelIndex m_miOld;
};

// -------------------------------------------------------------------------

class NesisTableWidget : public QTableWidget, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Forward knob event and try to move focus to next.
		mEdit				//!< Knob mode is active.
	};

public:
	//! Constructor
	NesisTableWidget(QWidget* pParent = 0);
	//! Destructor
	~NesisTableWidget();
	
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
	//! Nesis Cancel was pressed in edit mode.
	void NesisEditCancel();
	//! Nesis Button was pressed.
	void NesisButton(int iBtn);

public slots:
	//! Set edit mode.
	void SetEditMode();
	
private:
	//! One line up helper.
	void GoLineUp();
	//! One line down helper.
	void GoLineDown();

protected:
	//! Current mode
	Mode m_eMode;	
	//! Start mode
	Mode m_eStartMode;
	//! Remember initial selection.
	QModelIndex m_miOld;
};

// -------------------------------------------------------------------------

#endif	// __NESIS_TABLE_VIEW_H
