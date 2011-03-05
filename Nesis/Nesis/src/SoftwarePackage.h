#ifndef SOFTWARE_PACKAGE_H
#define SOFTWARE_PACKAGE_H

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
#include <QFile>
#include <QString>
#include <QVector>
#include <QPair>


namespace sup {

class BasicPackageInfo
{
public:

	BasicPackageInfo();
	BasicPackageInfo(QString sName, QString sVersion);
	~BasicPackageInfo();

	//! Return the name of the package.
	QString GetName() const;

	//! Return the version of the package.
	QString GetVersion() const;

	//! Return true if the package is newer.
	bool IsNewer(BasicPackageInfo* pPI) const;

protected:
	//! Package Name
	QString m_qsName;

	//! Package version.
	QString m_qsVersion;
};

// -------------------------------------------------------------------------
/** \brief Base class for packet management
 */

class PackageInfo : public BasicPackageInfo
{
public:
	enum Relation {
		rLessThan,
		rLessEqualThan,
		rEqual,
		rGreaterEqual,
		rGreater,
		rAny,
	};

	//! Short definition of the package relation list, used for dep, req, ...
	typedef QVector< QPair<BasicPackageInfo, Relation> > PackageVec;

public:
	//! Constructor
	PackageInfo();

	//! Destructor.
	virtual ~PackageInfo();

	//! Read package info from opened database.
	bool ReadFromDatabase(QFile* pFile);

	//! Read from .deb file.
	bool ReadFromFile(QString sName);

//	int CompareVersion(Software* pP):

	PackageVec GetDependencies() const
	{ return m_vDependencies; }


protected:
	//! Parse package information.
	bool ParsePackageInfo(QIODevice* pStream);

	PackageVec ParsePackageList(QByteArray baLine);

private:
	//! Architecture for which the package was built.
	QString m_qsArch;

	//! Section in which package fits.
	QString m_qsSection;

	//! Status of the package.
	QString m_qsStatus;

	//! If we are reading from a deb file this is full package filename with path.
	QString m_qsFilename;

	//! Vector with packed filenames and their md5sums.
	QVector< QPair<QString, QString> > m_vFilesMD5;

	//! Unpacked size in bytes.
	uint32_t m_uiSize;

	//! Package depends on.
	PackageVec m_vDependencies;

	//! Package is required for.
	PackageVec m_vRequirements;

};

// -------------------------------------------------------------------------

} // namespace

#endif
