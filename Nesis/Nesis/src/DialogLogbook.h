#ifndef DIALOG_LOGBOOK_H
#define DIALOG_LOGBOOK_H

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
#include "Logbook/Logbook.h"

class QAction;
class QLineEdit;
class NesisTableWidget;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogLogbook : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogLogbook(QWidget *pParent);

	//! Destructor
	virtual ~DialogLogbook();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize logbook table.
	void InitializeTable();

private slots:
	//! Current cell has changed.
	void OnTableRowChanged(int icr, int icc, int ipr, int ipc);

	//! Close the panel and release the focus.
	void OnClose();
	//! Clear all entries.
	void OnClear();

	//! Respond on the Nesis input buttons.
	void OnNesisButton(int iBtn);

	//! Update table
	void OnUpdateTable();

private:
	//! Top selection table - displays some basic data
	NesisTableWidget* m_ptwEntry;
	//! Engine on/off widget.
	QLineEdit* m_pleEngineOnOff;
	//! Engine cycles/time widget.
	QLineEdit* m_pleEngineCycleTime;
	//! Distance widget.
	QLineEdit* m_pleDistance;
	//! Speed widget.
	QLineEdit* m_pleSpeed;
	//! Fuel widget.
	QLineEdit* m_pleFuel;
	//! Altitude widget.
	QLineEdit* m_pleAltitude;

	//! Current logbook container
	logbook::Logbook::Container m_conBook;
};

// --------------------------------------------------------------------------

#endif // DIALOG_LOGBOOK_H
