/*
		Copyright (C) 1992-2003 Trolltech AS. All Rights Reserved.

    This file ("Example Code"), is part of an example program for Qt,
    and is licensed under the Qt Commercial License Agreement,
    Agreement version 2.4 and higher as well as under the GPL.

    The Example Code is licensed to you "as is" without warranty of
    any kind. To the maximum extent permitted by applicable law,
    Trolltech on behalf of itself and its suppliers, disclaims all
    warranties express or implied, including, but not limited to,
    implied warranties of merchantability and fitness for a particular
    purpose with regard to the Example Code. Trolltech does not
    warrant that the Example Code will satisfy your requirements or
    that it is without defect or error or that the operation thereof
    will be uninterrupted. All use of and reliance on the Example Code
    is at the sole risk of and responsibility of you and your
    customers.

    If you are a holder of a Qt Commercial License Agreement, you can
    use the code under that agreement or you can use it under GPL. If
    you are not a holder of a Qt Commercial License Agreement, you can
    use the code under the GPL. Regardless of the applicable license
    agreement, the Example Code may be used, distributed and modified
    without limitation.
*/


#include <QTextCodec>
#include <iterator>

#include "XmlWriter.h"

namespace xml {

// ---------------------------------------------------------------------------

XmlWriter::XmlWriter(
	QIODevice* pDevice, 
	QTextCodec* pCodec
)
: 
	m_iIndentSize(3), 
	m_bAutoNewLine(false), 
   m_bAtBeginningOfLine(true)
{
   m_out.setDevice(pDevice);
	// if there is specific codec ...
   if (pCodec){
		m_out.setCodec(pCodec);
		m_out << "<?xml version=\"1.0\" encoding=\""
	   	   << Protect(pCodec->name() ) << "\"?>\n";
	}
	// no, it is a default codec
	else {
		m_out << "<?xml version=\"1.0\"?>\n";
	}
}

// ---------------------------------------------------------------------------

XmlWriter::~XmlWriter()
{
   if ( m_bAutoNewLine && !m_bAtBeginningOfLine )
		m_out << endl;
}

// ---------------------------------------------------------------------------
/*! Replace special "reserved" xml characters with their escaped 
    equivalents. Special characters are &<>"' 
 */
QString XmlWriter::Protect( const QString& qsString )
{
	QString qs = qsString;
   qs.replace( "&", "&amp;" );
   qs.replace( ">", "&gt;" );
   qs.replace( "<", "&lt;" );
   qs.replace( "\"", "&quot;" );
   qs.replace( "\'", "&apos;" );
   return qs;
}

// ---------------------------------------------------------------------------
/*! Contruct an opening string out of given qsName (tag) and attributes.
	 The form is: <TAG key1="attr" key2="attr">
 */
QString XmlWriter::GetOpening(const QString& qsTag, const AttrMap& attrs )
{
	QString s = "<" + qsTag;
   AttrMap::ConstIterator a = attrs.begin();
   while (a != attrs.end()) {
		s += " " + a.key() + "=\"" + Protect(a->toString()) + "\"";
		++a;
   }
   s += ">";
   return s;
}

// ---------------------------------------------------------------------------

void XmlWriter::WritePendingIndent()
{
	if (m_bAtBeginningOfLine) {
		m_out << m_qsIndentStr;
		m_bAtBeginningOfLine = false;
	}
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteNewLine()
{
    m_out << endl;
    m_bAtBeginningOfLine = true;
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteRaw(const QString& qsXml)
{
    m_out << qsXml;
    m_bAtBeginningOfLine = false;
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteString(const QString& qsString)
{
    m_out << Protect(qsString);
    m_bAtBeginningOfLine = false;
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteOpenTag(const QString& qsName, const AttrMap& attrs)
{
   WritePendingIndent();
   m_out << GetOpening(qsName, attrs);
   m_qsIndentStr += QString().fill( ' ', m_iIndentSize );
	if (m_bAutoNewLine)
		WriteNewLine();
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteCloseTag( const QString& qsName )
{
	m_qsIndentStr = m_qsIndentStr.mid(m_iIndentSize);
   WritePendingIndent();
   m_out << GetOpening( "/" + qsName);
   if (m_bAutoNewLine)
		WriteNewLine();
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteAtomTag(const QString& qsName, const AttrMap& attrs)
{
	WritePendingIndent();
   QString atom = GetOpening(qsName, attrs);
   atom.insert(atom.length() - 1, "/" );
   m_out << atom;
   if (m_bAutoNewLine)
		WriteNewLine();
}

// ---------------------------------------------------------------------------

void XmlWriter::WriteTaggedString(
	const QString& qsName, 
	const QString& qsString,
	const AttrMap& attrs 
)
{
	WritePendingIndent();
   m_out << GetOpening(qsName, attrs);
   WriteString(qsString);
   m_out << GetOpening( "/" + qsName );
   if (m_bAutoNewLine)
		WriteNewLine();
}

// ---------------------------------------------------------------------------

} // namespace
