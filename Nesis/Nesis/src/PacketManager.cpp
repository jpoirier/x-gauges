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
#include <QDirIterator>
#include <QProcess>

#include "PacketManager.h"

namespace sup {

// --------------------------------------------------------------------

PacketManager::PacketManager()
{

}

// --------------------------------------------------------------------

PacketManager::~PacketManager()
{
}

// --------------------------------------------------------------------


void PacketManager::ReadDatabase()
{
	QFile fDatabase("/var/lib/dpkg/status");
	fDatabase.open(QIODevice::ReadOnly);

	PackageInfo* pPI;
	bool ret = false;

	do {
		pPI = new PackageInfo();
		ret = pPI->ReadFromDatabase(&fDatabase);
		if (ret == true)
			m_lInstalled.append(pPI);
	} while (ret == true);

	fDatabase.close();

	qDebug("Successfuly read %d packages from database.", m_lInstalled.size());
}

// --------------------------------------------------------------------

void PacketManager::SearchSoftwareSources()
{
	PackageInfo* pPI;
	bool ret = false;

	QStringList slAvailable;
	QDirIterator it("/media", QDirIterator::Subdirectories);

	while (it.hasNext()) {
		if (it.fileInfo().isFile()) {
			if (it.fileName().endsWith(".deb")) {
//				qDebug() << it.filePath();
				slAvailable.append(it.filePath());
			}
		}
		it.next();
	}

	emit Progress(30);

	float fStep = 0.0f;
	float fValue = 0.0f;

	fStep = 50.0 / slAvailable.size();

	foreach(QString sName, slAvailable) {
		pPI = new PackageInfo();
		ret = pPI->ReadFromFile(sName);
		if (ret == true)
			m_lAvailable.append(pPI);
		fValue += fStep;
		emit Progress(30 + (int)fValue);
	}

	qDebug("Found %d available packages.", m_lAvailable.size());
}

// --------------------------------------------------------------------

void PacketManager::Refresh()
{
	emit Progress(0);
	emit Message(QString(tr("Scaning database")));

	m_lAvailable.clear();
	m_lInstalled.clear();
	m_lTask.clear();

	ReadDatabase();

	emit Message(QString(tr("Scaning software sources")));
	emit Progress(20);
	SearchSoftwareSources();

	emit Message(QString(tr("Checking for update")));
	emit Progress(80);
	Compile();

	emit Progress(100);
	emit Message(QString(tr("Finished.")));
}

// --------------------------------------------------------------------

void PacketManager::Compile()
{
	foreach(PackageInfo* pPI, m_lInstalled) {
		// search for available packages with the same name
		for (int i = 0; i < m_lAvailable.count(); i++) {
			PackageInfo* pAvail = m_lAvailable.at(i);
			if (pAvail->GetName() == pPI->GetName()) {
				// we have the same package
				if (pPI->IsNewer(pAvail)) {
					if (CheckDependecies(pAvail)) {
						m_lTask.append(qMakePair(pAvail, tUpgrade));
					}
				}
			}
		}
	}
}

// --------------------------------------------------------------------

void PacketManager::Apply()
{
	for (int i = 0; i < m_lTask.count(); i++) {
		PackageInfo* pPI = m_lTask.at(i).first;
		Task eTask = m_lTask.at(i).second;

		qDebug() << "Installing" << pPI->GetName();
	}
}

// --------------------------------------------------------------------

bool PacketManager::CheckDependecies(PackageInfo* pPI)
{
	bool bMeet;
	QPair<BasicPackageInfo, PackageInfo::Relation> bpiDep;
	PackageInfo::PackageVec vDep = pPI->GetDependencies();

	if (vDep.isEmpty())
		return true;

	qDebug() << "Testing dep for" << pPI->GetName();

	foreach (bpiDep, vDep) {
		bMeet = false;
		for (int i=0; i<m_lInstalled.size(); i++) {
			// find candidate.
			if (bpiDep.first.GetName() == m_lInstalled.at(i)->GetName()) {
				// check relation
				switch (bpiDep.second) {
					case PackageInfo::rLessThan:
						if (m_lInstalled.at(i)->GetVersion() < bpiDep.first.GetVersion())
							bMeet = true;
						break;
					case PackageInfo::rLessEqualThan:
						if (m_lInstalled.at(i)->GetVersion() <= bpiDep.first.GetVersion())
							bMeet = true;
						break;
					case PackageInfo::rEqual:
						if (m_lInstalled.at(i)->GetVersion() == bpiDep.first.GetVersion())
							bMeet = true;
						break;
					case PackageInfo::rGreaterEqual:
						if (m_lInstalled.at(i)->GetVersion() >= bpiDep.first.GetVersion())
							bMeet = true;
						break;
					case PackageInfo::rGreater:
						if (m_lInstalled.at(i)->GetVersion() > bpiDep.first.GetVersion())
							bMeet = true;
						break;
					case PackageInfo::rAny:
						bMeet = true;
						break;

				}
			}
		}

		qDebug() << "Dependency was" << bMeet;

		if (bMeet == false) {
			emit Message(QString("Dependancy for package %1 was not met")
						.arg(pPI->GetName())
			);
			return false;
		}
	}
	return true;
}

// --------------------------------------------------------------------

bool PacketManager::SelectPackage(QString qsName, QString qsVersion)
{
	// find package, handle dependencies, install it
	PackageInfo* pAvail;
	for (int i = 0; i<m_lAvailable.size(); i++) {
		pAvail = m_lAvailable.at(i);
		if (pAvail->GetName() == qsName &&
 			 pAvail->GetVersion() == qsVersion) {
			if (CheckDependecies(pAvail)) {
				m_lTask.append(qMakePair(pAvail, tInstall));
			}
			return true;
		}
	}
	return false;
}

// --------------------------------------------------------------------

}

