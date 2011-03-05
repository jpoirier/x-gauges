#ifndef DIALOG_PILOTS_H
#define DIALOG_PILOTS_H

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

class NesisListWidget;
class NesisEditBox2;
class QLineEdit;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogPilots : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogPilots(QWidget* pParent);

	//! Destructor
	virtual ~DialogPilots();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize list of pilots.
	void InitializeList();
	
private slots:
	//! Respond on external command.
	void OnExternalCmd(int iCmd);
	//! Close the panel and release the focus.
	void OnClose();
	//! Select command form the list of pilots.
	void OnPilotSelect();
	//! One of edit commands from the list of pilots.
	void OnNesisEditList(int iCmd);

private:
	//! List of pilots for this airplane.
	NesisListWidget* m_plwPilots;
	//! Currently selected pilot.
	QLineEdit* m_pleSelected;
	//! Internal flag which tells if the pilot list was changed and needs to be saved.
	bool m_bDirty;
};

// --------------------------------------------------------------------------

#endif // DIALOG_PILOTS_H
