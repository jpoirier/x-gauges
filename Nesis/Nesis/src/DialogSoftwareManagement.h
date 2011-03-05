#ifndef DIALOG_SOFTWARE_MANAGEMENT_H
#define DIALOG_SOFTWARE_MANAGEMENT_H

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

#include <QtGui>

#include "AbstractNesisDialog.h"
#include "PacketManager.h"

class NesisTableWidget;

// --------------------------------------------------------------------------
//! Update mechanism.

class DialogSoftwareManagement : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogSoftwareManagement(QWidget* pParent);

	//! Destructor
	virtual ~DialogSoftwareManagement();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private slots:
	//! Initalize logbook table.
	void InitializeTable();

	//! Rescan the hardware and populate the table.
	void OnRescan();

	//! Set forced update (downgrade).
	void OnUpgrade();


private slots:
	//! Close the panel and release the focus.
	void OnClose();

	//! Respond on the Nesis input buttons.
	void OnNesisButton(int iBtn);

	//! Respond on the Nesis input buttons.
	void OnPackageChange();

	//! Show all packages.
	void OnShowAll();

	//! Start Firmware update dialog.
	void OnFirmware();

private:
	//! Top selection table - displays some basic data
	NesisTableWidget* m_pwtPackage;

	//! Package manager.
	sup::PacketManager* m_pPM;

	//! Programming progress bar.
	QProgressBar* m_pwProgress;

	//! The text edit control used to output the progress comments.
	QTextEdit* m_pwText;
};

// --------------------------------------------------------------------------

#endif // DIALOG_SOFTWARE_UPDATE_H
