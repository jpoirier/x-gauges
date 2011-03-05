/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QProcess>
#include <QStringList>
#include <QDebug>

#include <math.h>
#include <float.h>
#include <Tolerance.h>

#include "CommonDefsQt.h"

namespace common {
// --------------------------------------------------------------------

QString Capitalize(const QString& qs)
{
	QString qsCap = qs;
	bool bPrevWS = true;
	for(int i=0; i<qsCap.size(); i++) {
		if(qsCap[i].isLetter()) {
			if(bPrevWS)
				qsCap[i] = qsCap[i].toUpper();
			else
				qsCap[i] = qsCap[i].toLower();
			bPrevWS = false;
		}
		else if(qsCap[i].isSpace()) {
			bPrevWS = true;
		}
		else {
			bPrevWS = true;
		}
	}
	return qsCap;
}
// --------------------------------------------------------------------

QPolygon GetChamferPolygon(
	QRect rc,
	int iCx,
	int iCy
)
{
	if(iCy==0)
		iCy = iCx;

	QPolygon pg;
	pg << QPoint(rc.left()  + iCx, rc.top());
	pg << QPoint(rc.right() - iCx, rc.top());
	pg << QPoint(rc.right(), rc.top()    + iCy);
	pg << QPoint(rc.right(), rc.bottom() - iCy);
	pg << QPoint(rc.right() - iCx, rc.bottom());
	pg << QPoint(rc.left()  + iCx, rc.bottom());
	pg << QPoint(rc.left(), rc.bottom() - iCy);
	pg << QPoint(rc.left(), rc.top() + iCy);

	return pg;
}


// --------------------------------------------------------------------

void Serialize(QDataStream& ds, std::string& ss)
{
   if(IsStoring(ds)) {
      QString qs(ss.c_str());
      ds << qs;
   }
   else {
      QString qs;
      ds >> qs;
      ss = qs.toStdString();
//       ss = qs.toLocal8Bit().data();
   }
}

// --------------------------------------------------------------------

void Serialize(QDataStream& ds, QString& cs)
{
   if(IsStoring(ds)) {
      ds << cs;
   }
   else {
      ds >> cs;
   }
}

// --------------------------------------------------------------------

QString Float2String(float fV, unsigned int uiDec)
{
	int iV;

	if(uiDec > 0) {
		for(unsigned int i=0; i<uiDec; i++)
			fV *= 10;
		iV = qRound(fV);
		QString qs = QString::number(iV);
		qs.insert(qs.count()-uiDec, '.');
		return qs;
	}
	else {
		iV = qRound(fV);
		return QString::number(iV);
	}
}

// --------------------------------------------------------------------

bool CompareFiles(const QString& qsFile1, const QString& qsFile2)
{
	QProcess P;
	QStringList sl;
	sl << "-s" << qsFile1 << qsFile2;
	P.start("cmp", sl);
	if (!P.waitForStarted(3000)) {
		qDebug() << "cmp failed to start.";
		return false;
	}
	if(!P.waitForFinished(2000)) {
		qDebug() << "cmp failed to finish.";
		return false;
	}
	qDebug() << "Exit code=" << P.exitCode();
	return P.exitCode()==0;
}

// --------------------------------------------------------------------

QByteArray GetMD5(const QString qsFile)
{
	// Empty array
	QByteArray ba;
	
	// Get the Md5 of bin file using the md5sum linux command.
	QProcess P;
	P.start("md5sum", QStringList(qsFile));
		
	if (!P.waitForStarted(3000)) {
		qDebug() << "md5sum failed to start.";
		return ba;
	}
	if(!P.waitForFinished(2000)) {
		qDebug() << "md5sum failed to finish.";
		return ba;
	}
	
	// Read the output from the process into byte array
	// and split into the strings.
	QList<QByteArray> lsBa = P.readAll().simplified().split(' ');
	if(lsBa.count() > 0)
		ba = lsBa[0];
	
	return ba;
}
// --------------------------------------------------------------------

bool CopyFile(const QString& qsSource, const QString& qsDest)
{
	QProcess P;
	QStringList sl;
	sl << "-f" << qsSource << qsDest;
	P.start("cp", sl);
	if (!P.waitForStarted(3000)) {
		qDebug() << "cp failed to start.";
		return false;
	}
	if(!P.waitForFinished(2000)) {
		qDebug() << "cp failed to finish.";
		return false;
	}
	qDebug() << "Exit code=" << P.exitCode();
	return P.exitCode()==0;
}

// --------------------------------------------------------------------
} // namespace common
