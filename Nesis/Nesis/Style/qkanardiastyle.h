/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
**
** Trolltech ASA (c) 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


#ifndef QKANARDIASTYLE_H
#define QKANARDIASTYLE_H

#include <QtGui/qmotifstyle.h>
#include <QtGui/qplastiquestyle.h>

QT_BEGIN_HEADER

QT_MODULE(Gui)


class Q_GUI_EXPORT QKanardiaStyle : public QMotifStyle
{
    Q_OBJECT
public:
    explicit QKanardiaStyle(bool useHighlightCols = false);
    virtual ~QKanardiaStyle();

    int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                    const QWidget *widget = 0) const;

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = 0) const;

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = 0) const;

   void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                     const QWidget *widget) const;

// 	QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const;

	int styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                           QStyleHintReturn *returnData) const;

	QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                          const QSize &size, const QWidget *widget) const;


	QPalette standardPalette() const;

	void polish(QPalette& pal);


protected Q_SLOTS:
    QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *opt = 0,
                                     const QWidget *widget = 0) const;
};


QT_END_HEADER

#endif // QKanardiaStyle_H
