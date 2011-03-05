#ifndef PACKET_MANAGER_H
#define PACKET_MANAGER_H

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

#include <QList>
#include <QObject>

#include "SoftwarePackage.h"

namespace sup {

// -------------------------------------------------------------------------
/** \brief Base class for packet management
 */



class PacketManager : public QObject
{
Q_OBJECT

public:
	enum Task {
		tInstall,
		tUpgrade,
		tRemove,
	};

public:
	//! Constructor
	PacketManager();
	//! Destructor.
	virtual ~PacketManager();

	//! Return installed package at index.
	PackageInfo* GetInstalledPackage(int iIndex) const
	{ return m_lInstalled.at(iIndex); }

	//! Return how many installed packages are on the list.
	int GetInstalledPackageCount() const
	{ return m_lInstalled.size(); }

	//! Return installed package at index.
	PackageInfo* GetChangedPackage(int iIndex) const
	{ return m_lTask.at(iIndex).first; }

	//! Return how many installed packages are on the list.
	int GetChangedPackageCount() const
	{ return m_lTask.size(); }

	//! Return installed package at index.
	PackageInfo* GetAvailablePackage(int iIndex) const
	{ return m_lAvailable.at(iIndex); }

	//! Return how many installed packages are on the list.
	int GetAvailablePackageCount() const
	{ return m_lAvailable.size(); }

	//! Select package and try to install it.
	bool SelectPackage(QString qsName, QString qsVersion);

public slots:
	//! Rescan packages, database and compile data.
	void Refresh();

	//! Apply all changes.
	void Apply();

signals:
	//! Emit on progress.
	void Progress(int i);

	//! Emit message.
	void Message(QString s);

protected:
	//! Lock the dpkg database.
	void LockDatabase();

	//! Unlock the dpkg database.
	void UnlockDatabase();

	//! Read and load package database.
	void ReadDatabase();

	//! Find and parse all available packages.
	void SearchSoftwareSources();

	//! Compile updates, dependencies,...
	void Compile();

	//! check dependancies, required for upgrade.
	bool CheckDependecies(PackageInfo* pPI);

private:
	//! List of installed software.
	QList<PackageInfo*> m_lInstalled;

	//! List of available software.
	QList<PackageInfo*> m_lAvailable;

	//! List of tasks to be done.
	QList< QPair<PackageInfo*, Task> > m_lTask;

};

// -------------------------------------------------------------------------

} // namespace

#endif
