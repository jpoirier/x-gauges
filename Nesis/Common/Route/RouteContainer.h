#ifndef __ROUTE_ROUTE_CONTAINER_H
#define __ROUTE_ROUTE_CONTAINER_H

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

#include <QVector>
#include <QRect>
#include "Route.h"

namespace route {

class RouteContainer
{
	typedef QVector<Route*> Container;
public:
	//! Constructor
	RouteContainer();
	//! Destructor
	~RouteContainer();
	
	//! Clear - make the container empty.
	void Clear();
	
	//! Add new route into the container.
	void Append(Route* pRoute);
	
	//! Erase route from container.
	void Erase(int index);
	
	//! Return number of items in the container.
	int GetCount() const
	{ return m_conRoute.size(); }

	//! Get the route at given index. 
	const Route* GetRoute(unsigned int index) const
	{ return m_conRoute[index]; }
	
	//! Get the route at given index - full access. 
	Route* GetRoute(unsigned int index) 
	{ return m_conRoute[index]; }

	//! Save the container into given file.
	void Save(const QString& qsFile = QString(), bool bSaveIfDirty=true) const;
	//! Load the container from given file in native format.
	void Load(const QString& qsFile);

	//! Return true if the container of routes need to be saved.
	bool IsDirty() const;
	//! Force dirty flag.
	void SetDirty()
	{ m_bDirty = true; }
	
	//! Remove any invalid routes - routes that don't have at least two points.
	int RemoveInvalidRoutes();
	
private:
	//! Dirty flag.
	mutable bool m_bDirty;
	//! Container of routes.
	Container m_conRoute;	
	//! File name for the Save with no parameters.
	QString m_qsFileName;
};

// --------------------------------------------------------------------

}

#endif
