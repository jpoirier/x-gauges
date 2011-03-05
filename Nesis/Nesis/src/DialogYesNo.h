#ifndef __DIALOG_YES_NO_H
#define __DIALOG_YES_NO_H

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

#include "AbstractNesisDialog.h"
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class QRadioButton;

class DialogYesNo : public AbstractNesisDialog, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Constructor
	DialogYesNo(
		const QString& qsLblText,
//		const QStringList& slButtons = QStringList(),
		QWidget *pParent = 0
	);
	//! Destructor
	~DialogYesNo();
	
	bool HandleNesisInputEvent(PanelButton pb);
	QStringList GetButtonLabels() const;

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);

	//! Return true if yes button was selected.
	bool IsYes() const
	{ return m_bYes; }
private:
	QStringList m_slButtons;
	bool m_bYes;
};

// -------------------------------------------------------------------------

#endif
