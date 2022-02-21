/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "peony-computer-view-plugin.h"
#include "computer-view-container.h"
#include "intel-computer-view-container.h"

#ifdef KYLIN_COMMON
#include <ukuisdk/kylin-com4cxx.h>
#endif

#define V10_SP1_EDU "V10SP1-edu"

#include <QTranslator>
#include <QLocale>
#include <QApplication>

Peony::PeonyComputerViewPlugin::PeonyComputerViewPlugin(QObject *parent) : QObject(parent)
{
    QTranslator *t = new QTranslator(this);
    t->load(":/peony-extension-computer-view_"+QLocale::system().name());
    QApplication::installTranslator(t);
}

int Peony::PeonyComputerViewPlugin::priority(const QString &directoryUri)
{
    if (directoryUri == "computer:///")
        return 1;
    return -1;
}

Peony::DirectoryViewWidget *Peony::PeonyComputerViewPlugin::create()
{
#ifdef KYLIN_COMMON
    if (QString::fromStdString(KDKGetPrjCodeName()) == V10_SP1_EDU) {
        return new Intel::ComputerViewContainer;
    } else {
        return new ComputerViewContainer;
    }
#else
    return new ComputerViewContainer;
#endif
}
