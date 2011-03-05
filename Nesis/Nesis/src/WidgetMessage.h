#ifndef MESSAGE_WIDGET_H
#define MESSAGE_WIDGET_H

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

#include <memory>
#include <QWidget>
#include <QLabel>
#include <QTimer>

class WidgetMessage : public QWidget
{
Q_OBJECT
public:
	enum Level {
		lNormal,		//! Normal message (e.g. "All zoom levels are ready.");
		lWarning,	//! Warning	message (e.g. "Low altitude." or "Airspeed approaching VNE");
		lCritical	//! Critical message (e.g. "Airspeed above VNE." );
	};
public:
	//! Constructor
	WidgetMessage(QWidget *pParent = 0);
public:
	//! Destructor
	~WidgetMessage();

	//! Create an instance of the class. Only one instance can exist.
	static void CreateInstance(QWidget* pParent);
	//! Get the only instance of the object. Returns null, if instance does not exist.
	static WidgetMessage* GetInstance()
	{ return s_pInstance; }

	//! Set message and duration in seconds
	void SetText(const QString& csText, int iSeconds, Level eLevel=lWarning);

protected:
	void paintEvent(QPaintEvent *pEvent);
	void resizeEvent(QResizeEvent *pEvent);
private:
	//! The only object of this class.
	static WidgetMessage* s_pInstance;

	//! Timer for message hidding.
	QTimer* m_pTimer;

	//! Label used to display the message.
	QLabel* m_pLblMessage;
	//! Message level.
	Level m_eLevel;
};

#endif
