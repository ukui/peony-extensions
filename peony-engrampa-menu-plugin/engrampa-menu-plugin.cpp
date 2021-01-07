/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: Meihong <hemeihong@kylinos.cn>
 *
 */

#include "engrampa-menu-plugin.h"
#include <file-info.h>
#include <gio/gio.h>

#include <QAction>
#include <QFileInfo>
#include <QTranslator>
#include <QLocale>
#include <QApplication>
#include <QProcess>
#include <QDebug>

using namespace Peony;

EngrampaMenuPlugin::EngrampaMenuPlugin(QObject *parent) : QObject (parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"system().name:"<<QLocale::system().name();
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/peony-engrampa-menu_"+QLocale::system().name());
    QApplication::installTranslator(t);
}

QList<QAction*> EngrampaMenuPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction*> actions;
    if (types == MenuPluginInterface::DirectoryView || types == MenuPluginInterface::DesktopWindow)
    {
        if (! selectionUris.isEmpty()) {
            auto info = FileInfo::fromUri(selectionUris.first());
            //special type mountable, return
            qDebug()<<"info isVirtual:"<<info->isVirtual()<<info->mimeType();
            if (selectionUris.first().startsWith("computer:///") ||
                selectionUris.first().startsWith("trash:///") ||
                selectionUris.first().startsWith("recent:///") ||
                info->isVirtual())
                  return actions;

            QFileInfo file(selectionUris.first());
            QAction *compress = new QAction(QIcon::fromTheme("application-zip"), tr("compress..."), nullptr);
            actions<<compress;
            connect(compress, &QAction::triggered, [=](){
                qDebug()<<"compress file:"<<uri<<selectionUris;
                QProcess p;
                p.setProgram("engrampa");
                p.setArguments(QStringList()<<"-d"<<selectionUris);
//                p.startDetached();
                p.startDetached(p.program(), QStringList()<<"-d"<<selectionUris);
                p.waitForFinished(-1);
            });

            //check is compressed file
            //qDebug()<<"file.suffix"<<file.suffix()<<file.isDir()<<file;
            if (!file.isDir() && is_compressed_file(selectionUris.first()))
            {
                QAction *un_compress_default = new QAction(QIcon::fromTheme("application-zip"), tr("uncompress to current path"), nullptr);
                actions<<un_compress_default;
                connect(un_compress_default, &QAction::triggered, [=](){
                    QProcess p;
                    p.setProgram("engrampa");
                    p.setArguments(QStringList()<<"-h"<<selectionUris);
//                    p.startDetached();
                    p.startDetached(p.program(), p.arguments());
                    p.waitForFinished(-1);
                });

                QAction *un_compress_specific = new QAction(QIcon::fromTheme("application-zip"), tr("uncompress to specific path..."), nullptr);
                actions<<un_compress_specific;
                connect(un_compress_specific, &QAction::triggered, [=](){
                    QProcess p;
                    p.setProgram("engrampa");
                    p.setArguments(QStringList()<<"-f"<<selectionUris);
//                    p.startDetached();
                    p.startDetached(p.program(), p.arguments());
                    p.waitForFinished(-1);
                });
            }
        }
    }


    return actions;
}

bool EngrampaMenuPlugin::is_compressed_file(QString file_name)
{
    for(auto name: m_file_type_list)
    {
        auto suffix = "." + name;
        if (file_name.endsWith(suffix))
            return true;
    }

    return false;
}

