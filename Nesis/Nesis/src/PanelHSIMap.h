#ifndef __HSI_MAP_PANEL_H
#define __HSI_MAP_PANEL_H

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

class AirspaceVertical;

class PanelHSIMap : public AbstractPanel
{
Q_OBJECT
public:
	//! Constructor
	PanelHSIMap(QSize sz, QWidget *parent = 0);
	//! Destructor
	virtual ~PanelHSIMap();
	
	//! Forward zoom to the map widget.
	void SetZoom(int iZoomFactor);

	//! Draw the panel.
	virtual void Draw(bool bMajor);

	//! This panels is using map. Hence we return radius.
	virtual int GetMapRadius()
	{ return m_pwMap->GetRadius(); }
	
	//! Yes, we are using zoom here.
	virtual bool IsUsingZoom() const
	{ return true; }
	
	//! Process change zoom request.
	virtual bool ChangeZoom(bool bIn);
	
	// Handle input.
	//! Return list of menu actions names
	QStringList GetMenuLabels() const;
	//! Response on the button pressed action.
	void OnMenuButtonPressed(PanelButton pb);
	
private:
	//! Map widget.
	WidgetMap* m_pwMap;
	//! Vertical airspace object.
	AirspaceVertical* m_pAS;
};

#endif
