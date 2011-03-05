#ifndef __XMLWRITER_H
#define __XMLWRITER_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *                                                                         *
 *   Note: This code is slightly modified Trolltech "Exmaple Code",        *
 *         which was published in Qt Quarterly. Follow the link below      *
 *         for more details:                                               *
 *         http://doc.trolltech.com/qq/qq05-generating-xml.html            *
 *                                                                         *
 ***************************************************************************/

/*! \file 
    \brief 

    \author  Trolltech AS, Ales Krajnc 
    \date   
 */

// ------------------------------------------------------------------------

#include <QMap>
#include <QString>
#include <QVariant>
#include <QTextStream>

// ------------------------------------------------------------------------

namespace xml {

//! Helper class for the xml attributes. It stores "name = value" pairs.
  
class AttrMap : public QMap<QString, QVariant>
{
public:
	//! Constructor.
	AttrMap() { }
	//! Constructor taking the first pair.
	AttrMap(const QString& qsName, const QVariant& vValue) {
		insert(qsName, vValue );
   }
};

// ------------------------------------------------------------------------
//! XmlWriter is used to generate XML code.

class XmlWriter
{
public:
	//! Constructor needs output device and optinal codec.
	XmlWriter(QIODevice *pDevice, QTextCodec *pCodec = 0);
	//! Destructor.
	~XmlWriter();

	/** Write raw XML string (or any string) to the stream object.
		 No changes are made to qsXml string. 
    */ 
   void WriteRaw(const QString& qsXml);
	//! Write string to XML. The string is protected before output.
   void WriteString(const QString& qsString);

	//! Writes the open tag for given name and attributes.
   void WriteOpenTag(const QString& name, const AttrMap& attrs = AttrMap());
	//! Writes closing tag for given name.
   void WriteCloseTag(const QString& name);

	//! Writes atomic tag - adds closing /> at the end.
   void WriteAtomTag(const QString& name, const AttrMap& attrs = AttrMap());

	//! Writes a tag + attributes, string and closes a tag. 
   void WriteTaggedString(const QString& name, const QString& string,
			    const AttrMap& attrs = AttrMap());

	//! Writes new line to the output.
   void WriteNewLine();

	//! Set a new indent size. Default is 3.
   void SetIndentSize( int iSize ) 
	{ m_iIndentSize = iSize; }

	//! Turn auto new line on/off.
   void SetAutoNewLine( bool bOn ) 
	{ m_bAutoNewLine = bOn; }

private:
	//! Protect string with the escape characters. Eg.: '<' is replaced by '&lt'. 
   QString Protect(const QString& qsString);
 	//! Construct an opening tag from the name of the tag and the "name = value" attributes  
	QString GetOpening(const QString& tag, const AttrMap& attrs = AttrMap());
	//! Write indent at the beginning of a new line.
   void WritePendingIndent();

private:
	//! Output stream object.
   QTextStream m_out;
	//! Current indent string for well formated xml
   QString m_qsIndentStr;
	//! Current indents size.
   int  m_iIndentSize;
	//! A flag for new lines.
   bool m_bAutoNewLine;
	//! A flag which tells if we are at the begining of a new line.
   bool m_bAtBeginningOfLine;
};

} // namespace xml

#endif // __XMLWRITER_H
