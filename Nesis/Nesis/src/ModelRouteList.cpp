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

#include "CommonDefs.h"
#include "ModelRouteList.h"

// ------------------------------------------------------------------------

ModelRouteList::ModelRouteList(
	route::RouteContainer* pConRoute, 
	QObject *parent
)
 : QAbstractListModel(parent)
{
	m_pConRoute = pConRoute;
}


ModelRouteList::~ModelRouteList()
{
}

// ------------------------------------------------------------------------

int ModelRouteList::rowCount(
	const QModelIndex& /*miParent*/
) const
{
	ASSERT(m_pConRoute);
	return m_pConRoute->GetCount();
}

// ------------------------------------------------------------------------

QVariant ModelRouteList::data(
	const QModelIndex& mi, 
	int iRole
) const
{
	ASSERT(m_pConRoute);
	if(!mi.isValid())
		return QVariant();

	if(iRole == Qt::DisplayRole) {
		int iR = mi.row();
		const route::Route* pR = m_pConRoute->GetRoute(iR);
		return pR->GetName();
	}
	else if(iRole == Qt::SizeHintRole) {
		return QVariant(QSize(150, 22));
	}
	return QVariant();
}

// ------------------------------------------------------------------------

QModelIndex ModelRouteList::Append(route::Route* pR)
{
	m_pConRoute->Append(pR);
	reset();
	return createIndex(m_pConRoute->GetCount()-1,0);
}

// ------------------------------------------------------------------------

QModelIndex ModelRouteList::Erase(const QModelIndex& mi)
{
	m_pConRoute->Erase(mi.row());
	reset();
	return CreateModelIndex(mi.row());
}

// ------------------------------------------------------------------------

route::Route* ModelRouteList::GetRoute(const QModelIndex& mi)
{
	if(!mi.isValid())
		return NULL;
	return m_pConRoute->GetRoute(mi.row());
}

// ------------------------------------------------------------------------

void ModelRouteList::Save()
{
	m_pConRoute->RemoveInvalidRoutes();
	m_pConRoute->Save();
}

// ------------------------------------------------------------------------

QModelIndex ModelRouteList::CreateModelIndex(int iRow)
{
	if(m_pConRoute->GetCount()==0)
		iRow = -1;
	else if(iRow >= m_pConRoute->GetCount())
		iRow = m_pConRoute->GetCount()-1;
	return createIndex(iRow,0);
}

// ------------------------------------------------------------------------

void ModelRouteList::Rename(
	const QModelIndex& mi, 
	const QString& qsNewName
) 
{
	route::Route* pR = GetRoute(mi);
	if(pR) {
		pR->SetName(qsNewName);
		m_pConRoute->SetDirty();
		reset();
	}
}
