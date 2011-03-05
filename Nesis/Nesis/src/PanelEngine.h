#ifndef PANEL_ENGINE_H
#define PANEL_ENGINE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                 Dimona-100                                                        *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QDomDocument>
#include "AbstractPanel.h"

// --------------------------------------------------------------------------

// forward declaration
namespace instrument
{
class XMLGaugeRound;
}
class PixmapBattery;

// --------------------------------------------------------------------------

class WidgetBottom : public QWidget
{
Q_OBJECT
public:
	//! Constructor
	WidgetBottom(QWidget *pParent=0);
	//! Destructor
	virtual ~WidgetBottom();
	
	//! Set battery parameters.
	void SetBattery(int iMode, float fVoltage, int iTemp, int iCapacity);
	//! Set OAT
	void SetOAT(int iOAT);
	
	//! Draw the widget.
	virtual void paintEvent(QPaintEvent *pEvent);
private:
	//! The battery pixmap object (we do not own it).
	PixmapBattery* m_pPixBat;
	//! The capacity in percentage.
	int m_iCapacity;
	//! The cell voltage.
	float m_fVoltage;
	//! The battery mode.
	int m_iMode;
	//! The cell temperature.
	int m_iTemp;
	
	//! The OAT temperature.
	int m_iOAT;
};

// --------------------------------------------------------------------------

class PanelEngine : public AbstractPanel
{
Q_OBJECT
public:
	//! Constructor
	PanelEngine(const QDomDocument& doc, QSize sz, QWidget *pParent = 0);
	//! Destructor
	virtual ~PanelEngine();

	//! Draw the panel.
	virtual void Draw(bool bMajor);

	//! Give the list of menu button labels.	
	QStringList GetMenuLabels() const;
	
	//! Respond on a menu button.
	void OnMenuButtonPressed(PanelButton ePB);

private:
	//! Get full access to widget object according to positon.
	instrument::XMLGaugeRound*& GetWidget(QString& qsPos);
private:
	// The following six widgets are configurable
	instrument::XMLGaugeRound* m_pwTopLeft;		//!< Top-left widget.
	instrument::XMLGaugeRound* m_pwTopMiddle;		//!< Top-middle widget.
	instrument::XMLGaugeRound* m_pwTopRight;		//!< Top-right widget.
	instrument::XMLGaugeRound* m_pwBottomLeft;	//!< Top-left widget.
	instrument::XMLGaugeRound* m_pwBottomMiddle;	//!< Top-left widget.
	instrument::XMLGaugeRound* m_pwBottomRight;	//!< Top-right widget.

	//! The bottom widget holds battery, OAT.
	WidgetBottom* m_pwBottom;
};

#endif
