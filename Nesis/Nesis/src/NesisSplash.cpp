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

#include "NesisSplash.h"

NesisSplash* NesisSplash::s_pInstance = NULL;

//-----------------------------------------------------------------------------

NesisSplash::NesisSplash()
{
	m_fileSplash = new QFile("/proc/splash");
	m_fileSplash->open(QIODevice::WriteOnly);

	m_uiProgress = 35000;
}

//-----------------------------------------------------------------------------

NesisSplash* NesisSplash::GetInstance()
{
	if (s_pInstance == NULL) {
		s_pInstance = new NesisSplash();
	}
	return s_pInstance;
}

//-----------------------------------------------------------------------------

void NesisSplash::SetProgress(unsigned int uiProgress)
{
	QByteArray str;
	str.setNum(uiProgress);
	str.prepend("show ");
// 	qDebug() << "proc " << str;
	m_fileSplash->write(str);
	m_fileSplash->flush();
}

//-----------------------------------------------------------------------------

void NesisSplash::Destroy()
{
	if (s_pInstance != NULL) {
		delete(s_pInstance);
	}
}

NesisSplash::~ NesisSplash()
{
	m_fileSplash->close();
}
