#ifndef PANEL_CLASSIC_H
#define PANEL_CLASSIC_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Aleš Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QDomDocument>
#include "AbstractPanel.h"
#include "WidgetHSIMiniMap.h"

// --------------------------------------------------------------------------

// forward declaration
namespace instrument
{
	class XMLGaugeRound;
	class AttitudeGaugeRound;
	class EngineMiniGaugeRect;
	class FuelCompGaugeRect;
}

// --------------------------------------------------------------------------

class PanelClassic : public AbstractPanel
{
Q_OBJECT
public:
	//! Constructor
	PanelClassic(const QDomDocument& doc, QSize sz, QWidget *pParent = 0);

	//! Destructor
	virtual ~PanelClassic();

	//! Draw the panel.
	virtual void Draw(bool bMajor);

	//! Tell the base class: "Yes, we are using zoom here."
	bool IsUsingZoom() const { return true; }
	//! Forward zoom to the map widget.
	void SetZoom(int iZoomFactor);
	//! Change zoom command. 
	bool ChangeZoom(bool bIn);
	//! This panels is using map. Hence we return the radius.
	virtual int GetMapRadius()
	{ return m_pwMap->GetRadius(); }

	// Handle input.
	//! Return list of menu actions names
	QStringList GetMenuLabels() const;
	//! Response on the button pressed action.
	void OnMenuButtonPressed(PanelButton pb);
private:
	//! Get full access to widget object according to positon.
	instrument::XMLGaugeRound*& GetWidget(QString& qsPos);

private:
	// The following four widgets are configurable
	instrument::XMLGaugeRound* m_pwTopLeft;		//!< Top-left widget.
	instrument::XMLGaugeRound* m_pwTopRight;		//!< Top-right widget.
	instrument::XMLGaugeRound* m_pwBottomLeft;	//!< Top-left widget.
	instrument::XMLGaugeRound* m_pwBottomRight;	//!< Top-right widget.

	//! Attitude indicator widget.
	instrument::AttitudeGaugeRound* m_pwAtt;
	//! Warning lights widget.
	instrument::EngineMiniGaugeRect* m_pwEngine;
	//! Fuel and fuel computer widget.
	instrument::FuelCompGaugeRect* m_pwFuel;
	//! Map widget.
	WidgetHSIMiniMap* m_pwMap;
};

// --------------------------------------------------------------------------

#endif // __PANEL_CLASSIC_H
