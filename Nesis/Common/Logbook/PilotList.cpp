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

#include <QtDebug>
#include <QFile>

#include "XmlWriter.h"

#include "PilotList.h"

#define TAG_PILOT_LIST			"pilot_list"
#define TAG_VERSION				"version"
#define TAG_PILOT					"pilot"


namespace logbook {

// --------------------------------------------------------------------------

PilotList::PilotList()
{
}

PilotList::~PilotList()
{
}

// -----------------------------------------------------------------------

int PilotList::Load(const QString& qsXMLPilotListFile)
{
	m_qsFileName = qsXMLPilotListFile;
	// Load data from the file.
	QFile file(qsXMLPilotListFile);
	QXmlInputSource inputSource(&file);
	
	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	reader.setErrorHandler(this);
	
	// Set reader to the unknown state before parsing.
	reader.parse(inputSource);
	
	// Return number of entries in the pilot list.
	return m_slPilot.count();
}

// -----------------------------------------------------------------------

void PilotList::Remove(const QString& qsPilot)
{
	int i = m_slPilot.indexOf(qsPilot);
	if(i>=0) {
		m_slPilot.erase(m_slPilot.begin()+i);
	}
}

// -----------------------------------------------------------------------

bool PilotList::startElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName,
	const QXmlAttributes& /*qaAttr*/
)
{
	if(qsName == TAG_PILOT) {
		m_qsData.clear();
	}
	return true;
}

// -----------------------------------------------------------------------

bool PilotList::endElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName
)
{
	// Was it pilot?
	if(qsName == TAG_PILOT)
		m_slPilot << m_qsData;
	
// 	qDebug() << "End:" << qsName;
	return true;
}

// -----------------------------------------------------------------------

bool PilotList::characters(const QString& qs)
{
	m_qsData += qs;
	return true;
}

// -----------------------------------------------------------------------

bool PilotList::fatalError(const QXmlParseException& ex)
{
	qDebug() << "PilotList.xml error: Line=" << ex.lineNumber()
			   << " Col=" << ex.columnNumber() 
			   << " Msg=" << ex.message();
	return false;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

void PilotList::WriteXML()
{
	QFile file(m_qsFileName);
	file.open(QIODevice::WriteOnly | QIODevice::Text);

	xml::XmlWriter xw(&file);
	xw.SetAutoNewLine(true);

	// Start with the pilot_list tag and version
	xw.WriteOpenTag(TAG_PILOT_LIST, xml::AttrMap(TAG_VERSION, 1));

	// Write each entry
	foreach(QString qsPilot, m_slPilot) {
		xw.WriteTaggedString(TAG_PILOT, qsPilot);
	}

	xw.WriteCloseTag(TAG_PILOT_LIST);
}

// -----------------------------------------------------------------------

} // namespace logbook
