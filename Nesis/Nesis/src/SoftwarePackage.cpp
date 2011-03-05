/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
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
#include <QProcess>

#include "SoftwarePackage.h"

namespace sup {

// --------------------------------------------------------------------


BasicPackageInfo::BasicPackageInfo()
{

}

// --------------------------------------------------------------------

BasicPackageInfo::BasicPackageInfo(QString sName, QString sVersion)
{
	m_qsName = sName;
	m_qsVersion = sVersion;
}

// --------------------------------------------------------------------
BasicPackageInfo::~BasicPackageInfo()
{

}

// --------------------------------------------------------------------

QString BasicPackageInfo::GetName() const
{
	return m_qsName;
}

// --------------------------------------------------------------------

QString BasicPackageInfo::GetVersion() const
{
	return m_qsVersion;
}

bool BasicPackageInfo::IsNewer(BasicPackageInfo* pPI) const
{
	return (pPI->GetVersion() > m_qsVersion);
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------

PackageInfo::PackageInfo()
 : BasicPackageInfo()
{
}

// --------------------------------------------------------------------

PackageInfo::~PackageInfo()
{
}

// --------------------------------------------------------------------

PackageInfo::PackageVec PackageInfo::ParsePackageList(QByteArray baLine)
{
	QStringList slList = QString(baLine).split(',');
	PackageVec vPack;
	QString sSingle;
	QString sName;
	QString sVersion;
	Relation rRel;

	vPack.clear();

// 	qDebug() << "Package:" << m_sName << "depends on:";

	if (slList.size() != 0) {
		for (int i = 0; i < slList.size(); i++) {
			sSingle = slList.at(i).simplified();
			// check if we have version string (...).
			if (sSingle.contains('(')) {
				// remove version ()
				sSingle = sSingle.remove('(').remove(')');
				QStringList slPart = sSingle.simplified().split(' ');

				sName = slPart.at(0);
				sVersion = slPart.at(2);

				if (slPart.at(1) == ">>")
					rRel = rLessThan;
				else if (slPart.at(1) == ">=")
					rRel = rLessEqualThan;
				else if (slPart.at(1) == "=")
					rRel = rEqual;
				else if (slPart.at(1) == "<=")
					rRel = rGreaterEqual;
				else if (slPart.at(1) == "<<")
					rRel = rGreater;
				else
					qDebug() << "Unknown relation" << slPart.at(1);
			}
			else {
				sName = sSingle.left(sSingle.indexOf(' '));
				sVersion = "";
				rRel = rAny;
			}

// 			qDebug() << "     " << sName << sVersion;

			vPack.append(qMakePair(BasicPackageInfo(sName,sVersion), rRel));
		}
	}
	return vPack;
}

// --------------------------------------------------------------------

bool PackageInfo::ReadFromDatabase(QFile* pFile)
{
	return ParsePackageInfo(pFile);
}

// --------------------------------------------------------------------

bool PackageInfo::ReadFromFile(QString sName)
{
	QProcess procInfo;

	procInfo.setReadChannel(QProcess::StandardOutput);
	procInfo.start("dpkg-deb", QStringList() << "-f" << sName);
	procInfo.waitForFinished(3000);

	if (procInfo.exitStatus() == QProcess::NormalExit) {
		m_qsFilename = sName;
		return ParsePackageInfo(&procInfo);
	}
	else {
		qDebug() << procInfo.readAllStandardError();
	}

	return false;
}

// --------------------------------------------------------------------

bool PackageInfo::ParsePackageInfo(QIODevice* pStream)
{
	QByteArray baLine;
	bool finish = false;

	do {
		baLine = pStream->readLine();

		// few checks
		if (pStream->atEnd())
			break;

		if (baLine.startsWith(' '))
			continue;

		baLine = baLine.simplified();

		if (baLine.size() == 0)
			continue;

		// if we have multiline, handle it.
		while (baLine.endsWith('\\')) {
			baLine.chop(1);
			baLine += pStream->readLine();
			baLine = baLine.simplified();
		}

		// for start we are expecting Name: xxxx
		if (baLine.startsWith("Package:")) {
			m_qsName = baLine.remove(0,8).simplified();
//			qDebug() << "Found package " << m_sName;
		}
		else if (baLine.startsWith("Status:")) {
			m_qsStatus = baLine.remove(0,7).simplified();
		}
		else if (baLine.startsWith("Depends:")) {
			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Architecture:")) {
			m_qsArch = baLine.remove(0,13).simplified();
		}
		else if (baLine.startsWith("Priority:")) {
//			m_sName = baLine.remove(0,5).simplified();
		}
		else if (baLine.startsWith("Source:")) {
//			m_sName = baLine.remove(0,5).simplified();
		}
		else if (baLine.startsWith("Version:")) {
			m_qsVersion = baLine.remove(0,8).simplified();
		}
		else if (baLine.startsWith("Section:")) {
			m_qsSection = baLine.remove(0,8).simplified();
		}
		else if (baLine.startsWith("Replaces:")) {
// 			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Provides:")) {
// 			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Conflicts:")) {
// 			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Maintainer:")) {
//			m_sName = baLine.remove(0,5).simplified();
		}
		else if (baLine.startsWith("Installed-Size:")) {
//			m_sName = baLine.remove(0,5).simplified();
		}
		else if (baLine.startsWith("Pre-Depends:")) {
// 			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Breaks:")) {
// 			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Recommends:")) {
// 			m_vDependencies = ParsePackageList(baLine.remove(0,8).simplified());
		}
		else if (baLine.startsWith("Config-Version:")) {
		}
		else if (baLine.startsWith("Suggests:")) {
		}
		else if (baLine.startsWith("Conffiles:")) {
		}
		else if (baLine.startsWith("Bugs:")) {
		}
		else if (baLine.startsWith("Enhances:")) {
		}
		else if (baLine.startsWith("Essential:")) {
//			m_sName = baLine.remove(0,5).simplified();
		}
		else if (baLine.startsWith("Description:")) {
			finish = true;
			// last entry ... read until next free line
			while ( (pStream->readLine().simplified().isEmpty()) == false) ;
		}
		else {
			qDebug() << "Unknown entry: " << baLine;
		}

	} while(finish==false);

	if (finish==false && pStream->atEnd() == false)
		qDebug("Read database error.");

	return finish;
}

// --------------------------------------------------------------------



}
