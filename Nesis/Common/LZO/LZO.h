#ifndef __LZO_H
#define __LZO_H

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

#include <QByteArray>
#include <QDataStream>

QByteArray CompressLZO999(const QByteArray& baSource);
QByteArray DecompressLZO(const QByteArray& baSource);
QByteArray DecompressLZO(QDataStream& in);

#endif // __LZO_H
