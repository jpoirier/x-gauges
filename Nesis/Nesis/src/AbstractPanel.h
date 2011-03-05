#ifndef ABSTRACT_PANEL_H
#define ABSTRACT_PANEL_H

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

//#include "NesisMenu.h"
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------
/** \brief Base class for panels used inside the WidgetMain class.
 */

// TODO Abstract panel should (could) know the WidgetMain class (parent).

class WidgetMain;

class AbstractPanel : public QWidget, public AbstractNesisInput
{
Q_OBJECT
protected:
	enum Mode {
		mDirect,	//! We are in the direct mode - switch screen.
		mMenu		//! We are in the menu mode.
	};
public:
	//! Constructor
	AbstractPanel(QSize sz, QWidget *parent = 0);
	//! Destructor
	virtual ~AbstractPanel();

	//! Apply focus to some control. Does nothing by default.
	virtual void Activate();

	//! Draw the panel (if the panel has dynamic/animated features).
	virtual void Draw(bool bMajor) = 0;

	//! If this panel is showing a map, get the radius.
	virtual int GetMapRadius()
	{ return 0; }
	
	//! Return access to the main widget.
	WidgetMain* GetWidgetMain()
	{ return m_pwMain; }
	
	// AbstractNesisInput
	
	//! We handle the direct mode here.
	bool HandleNesisInputEvent(PanelButton pb);
	//! Get button labels.
	virtual QStringList GetButtonLabels() const;
	
	//! Is menu mode enabled for this panel?
	virtual bool IsMenuAvailable() const 
	{ return true; }
	//! Get labels for the menu mode.
	virtual QStringList GetMenuLabels() const;
	//! Get action for the menu mode.
	virtual void OnMenuButtonPressed(PanelButton pb);
	
	//! Return true if this panel is using zoom. False by default.
	virtual bool IsUsingZoom() const
	{ return false; }
	//! Zoom in/out - nothing by default.
	virtual bool ChangeZoom(bool /*bIn*/)
	{ return false; }

	//! Set the input into direct mode.
	void SetDirectMode();
protected:
	//! Set the new background for the panel.
	void SetBackgroundColor(const QBrush& brush);

// 	void ShowMenu();
	
protected:
	//! Access to the parent, which should be a WidgetMain class.
	WidgetMain* m_pwMain;
	//! Current input mode.
	Mode m_eMode;

	//QMenu* m_pMenu;
};

#endif
