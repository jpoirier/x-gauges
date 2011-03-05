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
#include <QDataStream>
#include "CommonDefs.h"
#include "Airfield.h"
#include "RouteContainer.h"

namespace route {

// -------------------------------------------------------------------------
	
RouteContainer::RouteContainer()
{
	m_bDirty = false;
}


RouteContainer::~RouteContainer()
{
	Clear();
}
// -------------------------------------------------------------------------

void RouteContainer::Clear()
{
	foreach(Route* pR, m_conRoute) {
		delete pR;
	}
	m_conRoute.clear();
	m_qsFileName.clear();
	m_bDirty = false;
}

// -------------------------------------------------------------------------

void RouteContainer::Save(const QString& qsFile, bool bSaveIfDirty) const
{
	// Save only if the container is dirty.
	if(bSaveIfDirty && IsDirty() == false)
		return;
			
	// If the file name is not given, use the 
	// loading file name.
	QString qsFileName = m_qsFileName;
	if(!qsFile.isEmpty())
		qsFileName = qsFile;
	
	// Open file
	QFile file(qsFileName);
	
	if(file.open(QIODevice::WriteOnly)==false) {
		qDebug() << "RouteContainer::Save failed to open the file: " 
				   << qsFileName; 
		return;
	}
	QDataStream out(&file);
	
	// version first
	out << (qint8)1;
	
	// container size
	out << (qint16) m_conRoute.size();
	
	// item by item ...
	foreach(Route* pR, m_conRoute) {
		pR->Write(out);
	}
	m_bDirty = false;
}

// -------------------------------------------------------------------------

void RouteContainer::Load(
	const QString& qsFile
)
{
	Clear();

	// Remember the filename. Use the same name for saving
	// if the Save is called without argument.
	m_qsFileName = qsFile;

	// Open file
	QFile file(qsFile);
	if(file.open(QIODevice::ReadOnly)==false) {
		qDebug() << "RouteContainer::Load failed to open the file: " 
				<< qsFile; 
		return;
	}

	// Attach stream to the file.
	QDataStream in(&file);
	
	// version first
	qint8 iVer;
	in >> iVer;
	
	// container size
	qint16 iCount;
	in >> iCount;
	
	Route* pR;
	while(iCount-- > 0) {
		pR = new Route();
			// Load data
		pR->Read(in);
		m_conRoute << pR;
	}
	m_bDirty = false;
}

// -------------------------------------------------------------------------

void RouteContainer::Append(Route* pRoute)
{ 
	m_conRoute << pRoute; 
	m_bDirty = true;
}

// -------------------------------------------------------------------------

void RouteContainer::Erase(int index)
{
	if(index < m_conRoute.size()) {
		delete m_conRoute[index];
		m_conRoute.erase(m_conRoute.begin()+index);
	}
	m_bDirty = true;
}

// -------------------------------------------------------------------------

bool RouteContainer::IsDirty() const
{
	if(m_bDirty)
		return true;

	foreach(const Route* pR, m_conRoute) {
		if(pR->IsDirty())
			return true;
	}
	return false;
}

// -------------------------------------------------------------------------
	
int RouteContainer::RemoveInvalidRoutes()
{
	int i=0;
	int iCount = 0;
	while(i < m_conRoute.count()) {
		if(m_conRoute[i]->IsValid()==false) {
			Erase(i);
			iCount++;
		}
		else 
			i++;
	}
	// return number of removed routes.
	return iCount;
}

// -------------------------------------------------------------------------

}
