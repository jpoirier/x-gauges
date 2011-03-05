#ifndef __PILOT_LIST_H
#define __PILOT_LIST_H

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
#include <QStringList>

namespace logbook {

// -------------------------------------------------------------------------
/** Shared (singletone) pilot list object. It stores the list of pilots
	 that are using the airplane. The list is in the xml format.
 */


class PilotList : public QXmlDefaultHandler
{
private:
	//! Constructor is private in order to prevent direct class creation.
	PilotList();
public:
	//! Destructor.
	~PilotList();

	//! Access point to the class.
	static PilotList* GetInstance()
	{
		static PilotList list; // The only object of this class
		return &list;
	}

	//! Load the XML logbook file.
	int Load(const QString& qsXMLPilotListFile);
	
	//! Add new pilot to the list
	void Add(const QString& qsPilot)
	{ m_slPilot << qsPilot; }

	//! Remove pilot from the list
	void Remove(const QString& qsPilot);

	//! Get const access to the list.
	const QStringList& GetList() const
	{ return m_slPilot; }

	//! Write current content to the XML file.
	void WriteXML();
	
private:
	//! XML simple parser - starting element
	bool startElement(const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName,
							const QXmlAttributes& qaAttributes);
	//! XML simple parser - closing element.
	bool endElement(	const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName);

	
	//! XML simple parser - characters between start and end.
	bool characters(const QString& qs);
	//! XML simple parser - the error handler.
	bool fatalError(const QXmlParseException& exception);
	
private:
	//! The list is stored in QStringList object.
	QStringList m_slPilot;
	//! The XML path (file name).
	QString m_qsFileName;
	
	//! Data string (pilot name) used in XML parsing.
	QString m_qsData;
};

} // namespace
// -------------------------------------------------------------------------
#endif
