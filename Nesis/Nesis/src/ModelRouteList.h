#ifndef __MODEL_ROUTE_LIST_H
#define __MODEL_ROUTE_LIST_H

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

#include <QAbstractListModel>
#include "Route/RouteContainer.h"

class ModelRouteList : public QAbstractListModel
{
Q_OBJECT
public:
	//! Constructor - needs the route container.
	ModelRouteList(route::RouteContainer* pConRoute, QObject *pParent = 0);
	//! Destructor.
	~ModelRouteList();

	//! Number of rows.
	virtual int rowCount(const QModelIndex& miParent = QModelIndex()) const;

	//! Basically it provides the name for the mi item.
	virtual QVariant data(const QModelIndex& mi, int iRole = Qt::DisplayRole) const;
	
	//! Append new route to the model.
	QModelIndex Append(route::Route* pR);
	//! Delete given route - return index to the next.
	QModelIndex Erase(const QModelIndex& mi);

	//! Save the route.
	void Save();
	
	//! Get Route at given index. 
	route::Route* GetRoute(const QModelIndex& mi);

	//! Rename route at given index.
	void Rename(const QModelIndex& mi, const QString& qsNewName);

	//! Get model index for required row.
	QModelIndex CreateModelIndex(int iRow);
	
protected:
	//! Container that we are working on.
	route::RouteContainer* m_pConRoute;
};

#endif
