#ifndef DIALOG_BROWSER_H
#define DIALOG_BROWSER_H

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

class QAction;
class NesisBrowser;
class QUrl;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogBrowser : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogBrowser(const QUrl& urlHome, QWidget *pParent);

	//! Destructor
	virtual ~DialogBrowser();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private slots:
	//! Nesis buttons on the browser command.
	void OnBrowserCmd(int iCmd);
	//! Close the panel and release the focus.
 	void OnClose();

private:
	//! The manual text browser (using a limited set of HTML commands).
	NesisBrowser* m_pwBrowser;
};

// --------------------------------------------------------------------------

#endif // DIALOG_BROWSER_H
