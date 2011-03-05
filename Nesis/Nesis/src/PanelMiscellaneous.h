#ifndef __PANEL_MISCELLANEOUS_H
#define __PANEL_MISCELLANEOUS_H

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

#include "AbstractPanel.h"
#include "WidgetMap.h"

// --------------------------------------------------------------------------

class NesisListWidget;
class QStackedWidget;

// --------------------------------------------------------------------------

class PanelMiscellaneous : public AbstractPanel
{
Q_OBJECT
public:
	//! Constructor
	PanelMiscellaneous(QSize sz, QWidget *pParent = 0);

	//! Destructor
	virtual ~PanelMiscellaneous();

	//! Draw the panel - we are not using it here;
	virtual void Draw(bool /*bMajor*/) {;}

	//! Apply focus to the selector control.
	virtual void Activate(); 
public slots:
	//! Activate subpanel.
	void ActivateSubpanel();

private:
	//! List of icons.
	NesisListWidget* m_pwIcon;
};


// --------------------------------------------------------------------------

#endif // __PANEL_MISCELLANEOUS_H
