#ifndef DIALOG_ALARMS_H
#define DIALOG_ALARMS_H

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

class NesisCheckBox;

// --------------------------------------------------------------------------
//! Enable, disable user configurable alarms.

class DialogAlarms : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogAlarms(QWidget* pParent);

	//! Destructor
	virtual ~DialogAlarms();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize list of pilots.
//	void InitializeList();
	
private slots:
	//! Respond on external command.
	void OnExternalCmd(int iCmd);
	//! Close the panel and release the focus.
	void OnClose();

private:
	QVector<QPair<NesisCheckBox*, int> > m_vpcbUserAlarms;	
};

// --------------------------------------------------------------------------

#endif // DIALOG_PILOTS_H
