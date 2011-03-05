#ifndef DIALOG_SHUTDOWN_H
#define DIALOG_SHUTDOWN_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
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
#include "Unit/Value.h"

class QLabel;
class QProgressBar;

// -------------------------------------------------------------------------

class DialogShutDown : public AbstractNesisDialog, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Constructor (takes iQNH in hPa)
	DialogShutDown(QWidget *pParent = 0);
	//! Destructor
	~DialogShutDown();
	
	//! Handle Nesis input event - pure virtual.
	bool HandleNesisInputEvent(PanelButton pb);
	//! Get button labels - pure virtual.
	QStringList GetButtonLabels() const;
	
private slots:
	//! On timer update.
	void OnUpdate();
	
private:
	QLabel* m_pLbl;
	QProgressBar* m_pProgress;
	
	unsigned int m_uiCounter;
};

// -------------------------------------------------------------------------

#endif	// __DIALOG_SHUTDOWN_H
