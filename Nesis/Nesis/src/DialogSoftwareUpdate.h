#ifndef DIALOG_SOFTWARE_UPDATE_H
#define DIALOG_SOFTWARE_UPDATE_H

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
#include "CanAerospace/UnitProgrammer.h"

class QAction;
class NesisTableWidget;
class QProgressBar;
class QTextEdit;

// --------------------------------------------------------------------------
//! Update mechanism.

class DialogSoftwareUpdate : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogSoftwareUpdate(QWidget* pParent);

	//! Destructor
	virtual ~DialogSoftwareUpdate();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize logbook table.
	void InitializeTable();
	
	//! Rescan the hardware and populate the table.
	void OnRescan();
	
	//! Set forced update (downgrade).
	void OnDowngrade();
	
	//! Update the program in selected hardware.
	void OnProgram();
	
private slots:
	//! Close the panel and release the focus.
	void OnClose();
	
	//! Respond on the Nesis input buttons.
	void OnNesisButton(int iBtn);
	
	//! Selection has changed. We may write something on the text area.
	void OnTableRowChanged(int iCurrentRow, int iCurrentColumn, 
		int iPreviousRow, int iPreviousColumn);
	
	//! On APS programming started.
	void OnAPSStartReport(quint8 byHardwareId, quint32 uiPages);
	//! On APS page request.
	void OnAPSPageRequestReport(quint32 uiPage);
	//! On APS Finish.
	void OnAPSFinishReport(qint32 iState);
	
	//! On Mabu (SPI) programming
	void OnProgramMabu(bool bStart);

private:
	//! Top selection table - displays some basic data
	NesisTableWidget* m_pwtUnit;
	//! Programming progress bar.
	QProgressBar* m_pwProgress;
	//! The text edit control used to output the progress comments.
	QTextEdit* m_pwText;
};

// --------------------------------------------------------------------------

#endif // DIALOG_SOFTWARE_UPDATE_H
