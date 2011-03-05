#ifndef __PANEL_CHECK_LIST_H
#define __PANEL_CHECK_LIST_H

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

#include <QXmlDefaultHandler>
#include <QFrame>
#include "AbstractPanel.h"

// --------------------------------------------------------------------------

class NesisListWidget;
class QListWidgetItem;
class QModelIndex;

// -----------------------------------------------------------------------
// Special widget class for checkslist

class WidgetChecklist : public QFrame
{
Q_OBJECT
public:
	WidgetChecklist(QWidget* pParent);
	void SetItems(const QStringList& slItems) {
		m_slItems = slItems;
		update();
	}
protected:
   void paintEvent(QPaintEvent *event);
private:
	QStringList m_slItems;
};

// --------------------------------------------------------------------------

class PanelChecklist : public AbstractPanel, public QXmlDefaultHandler
{
Q_OBJECT
private:
	//! Content of checklist.
	typedef QVector<QStringList> ChecklistContainer;
public:
	//! Constructor
	PanelChecklist(QSize sz, QWidget *pParent = 0);

	//! Destructor
	virtual ~PanelChecklist();

	//! Draw the panel.
	virtual void Draw(bool bMajor);

	//! Take care for the focus on the activation.
	void Activate();
	

	bool startElement(const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName,
							const QXmlAttributes& qaAttributes);
	bool endElement(	const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName);
	bool characters(const QString& qs);
	bool fatalError(const QXmlParseException& exception);

public slots:
	//! A checklist selection has changed.
	void OnCurrentRowChanged(const QModelIndex& miNew, const QModelIndex& miOld);

private:
	//! Check lists are listed here.
	NesisListWidget* m_plwCheck;
	//! Item of the check list.
	QListWidgetItem* m_pCheckItem;

	//! This is content of the selected checklist.
	WidgetChecklist* m_pwContent;

	//! All checklists are here.
	ChecklistContainer m_conChecklist;
	//! Working checklist content used by XML parser.
	QStringList	m_slContent;
};

// --------------------------------------------------------------------------

#endif // __PANEL_CHECK_LIST_H
