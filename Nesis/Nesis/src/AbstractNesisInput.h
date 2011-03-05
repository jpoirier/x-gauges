#ifndef ABSTRACT_NESIS_INPUT_H
#define ABSTRACT_NESIS_INPUT_H

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

#include <QObject>
#include <QVector>
#include <QStringList>

class QPushButton;

// -------------------------------------------------------------------------
/** \brief Abstract base class for the hardware specific Nesis input.
    
    Derived classes will build several solutions upon this base.
    Widgets and controls will be derive from this class. They will
    typically implement HandleNesisInputEvent() method. 
    They will also access QPushButton objects which visually simulate 
    Nesis push button keys.
 */


class AbstractNesisInput {
public:
	//! Panel buttons - Btn1 ... Btn5 = BtnMenu, BtnOK, BtnCancel, ...
	enum PanelButton {
		pbBtn1,				//!< Button one on the panel.
		pbBtn2,				//!< Button two on the panel.
		pbBtn3,				//!< Button three on the panel.
		pbBtn4,				//!< Button four on the panel.
		pbBtn5,				//!< Button five on the panel.
		pbOk,					//!< Ok button on the panel.
		pbCancel,			//!< Cancel button on the panel.
		pbKnobCCW,			//!< Rotating knob on the panel counterclockwise.
		pbKnobCW,			//!< Rotating knob on the panel clockwise.
		pbInvalid 			//!< Invalid condition marker.
	};
	
public:
	//! Constructor
	AbstractNesisInput();
	//! Destructor
	virtual ~AbstractNesisInput();
	
	//! Set buttons. This is a shared resource between all objects.
	static void SetButtons(const QVector<QPushButton*>& conBtn) 
	{ s_conBtn = conBtn; }
	
	//! Show/hide the buttons.
	virtual void ShowButtons(bool bShow=true);
	//! Hide the buttons.
	void HideButtons()
	{ ShowButtons(false); }
	
	//! Set external labels.
	void SetExternalButtonLabels(const QStringList& sl)
	{ m_slExternalLabels = sl; }
	
	//! Handle Nesis input event - pure virtual.
	virtual bool HandleNesisInputEvent(PanelButton pb) = 0;
	//! Get button labels - pure virtual.
	virtual QStringList GetButtonLabels() const
	{ return QStringList(); }
	
protected:
	//! List of QButton objects. We don't own them. They must be created elsewhere.
	static QVector<QPushButton*> s_conBtn;
	//! List of external buttons labels to be displayed.
	QStringList m_slExternalLabels;
};

// --------------------------------------------------------------------

#endif
