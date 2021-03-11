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
 * Authors: Ding Jing <dingjing@kylinos.cn>
 *
 */

#include "send-to-device-plugin.h"
#include "file-info.h"

#include <QUrl>
#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QtGlobal>
#include <QProcess>
#include <QTranslator>
#include <QtConcurrent>
#include <QApplication>
#include <file-copy-operation.h>
#include <file-operation-manager.h>

static QString getIconName (GMount* mount);
static void mounted_func (gpointer data, gpointer udata);
static void handle_mount_added  (GVolumeMonitor* monitor, GMount* mount, gpointer data);
static void handle_mount_removed(GVolumeMonitor* monitor, GMount* mount, gpointer data);

using namespace Peony;

SendToPlugin::SendToPlugin(QObject *parent) : QObject(parent), mEnable(true)
{
    // translator
    QTranslator *t = new QTranslator(this);
    t->load(":/translations/peony-send-to-device_"+QLocale::system().name());
    QFile file(":/translations/peony-send-to-device_"+QLocale::system().name()+".ts");
    QApplication::installTranslator(t);

    mVolumeMonitor = g_volume_monitor_get();
    GList* mounts = nullptr;

    g_signal_connect(G_OBJECT(mVolumeMonitor), "mount-added",    G_CALLBACK(handle_mount_added),    (gpointer)this);
    g_signal_connect(G_OBJECT(mVolumeMonitor), "mount-removed",  G_CALLBACK(handle_mount_removed),  (gpointer)this);

    mounts = g_volume_monitor_get_mounts (mVolumeMonitor);
    if (mounts) {
        g_list_foreach (mounts, mounted_func, this);
    }
}

void SendToPlugin::removeDriver(const QString uri)
{
    if (nullptr == uri || "" == uri) return;

    if (mDrivers.contains(uri)) {
        mDrivers.remove(uri);
        Q_EMIT mountDel(uri);
    }
}

void SendToPlugin::addDriver(QString uri, GMount* mount)
{
    if (nullptr == mount || nullptr == uri) {
        return;
    }

    if (nullptr != uri && !mDrivers.contains(uri)) {
        mDrivers[uri] = mount;
        char* name = g_mount_get_name((GMount*)mount);

        Q_EMIT mountAdd (uri, getIconName(mount), name);
    }
}

QList<QAction *> SendToPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> l;

    if (selectionUris.count() <= 0) {
        return l;
    }

    auto info = FileInfo::fromUri(selectionUris.first());

    if (selectionUris.first().startsWith("computer:///")
            || selectionUris.first().startsWith("trash:///")
            || info->isVirtual()) {
        return l;
    }

    auto menu = new QMenu();
    auto action = new QAction(tr("Send to a removable device"));

    action->connect(this, &SendToPlugin::mountAdd, this, [=] (QString uri, QString iconName, QString name) {
        action->setVisible(mDrivers.size() <= 0 ? false : true);
        getOneDriver(selectionUris, menu, uri, iconName, name);

    });

    action->connect(this, &SendToPlugin::mountDel, this, [=] (QString uri) {
        action->setVisible(mDrivers.size() <= 0 ? false : true);

        for (auto c : menu->children()) {
            if (mitems.contains(uri) && c == mitems[uri]) {
                menu->removeAction(mitems[uri]);
            }
        }
        if (mitems.contains(uri)) mitems.remove(uri);
    });

    for (auto item = mDrivers.constBegin(); item != mDrivers.constEnd(); ++item) {
        GMount* mount = item.value();
        char* name = g_mount_get_name((GMount*)mount);

        getOneDriver(selectionUris, menu, item.key(), getIconName(mount), name);
        if (name) g_free (name);
    }

    l << action;
    action->setMenu(menu);
    action->setVisible(mDrivers.size() <= 0 ? false : true);

    Q_UNUSED(uri)
    Q_UNUSED(types)

    return l;
}

void SendToPlugin::getOneDriver(const QStringList &selectionUris, QMenu *menu, QString uri, QString iconName, QString name)
{
    DriverItem* it = new DriverItem(uri, QIcon::fromTheme(iconName), name, menu);
    connect(it, &QAction::triggered, it, [=] () {
        FileCopyOperation* op = new FileCopyOperation(selectionUris, it->uri(), it);
        op->setAutoDelete(true);
        FileOperationManager::getInstance()->startOperation(op);
    });

    if (menu) {
        menu->addAction(it);
    }
    mitems[uri] = it;
}

DriverItem::DriverItem(QString uri, QIcon icon, QString name, QObject* parent) : QAction(parent), mName(name), mIcon(icon), mUri(uri)
{
    setIcon(mIcon);
    setText(name);
}

const QIcon DriverItem::icon()
{
    return mIcon;
}

const QString DriverItem::name()
{
    return mName;
}

const QString DriverItem::uri()
{
    return mUri;
}

static void handle_mount_added(GVolumeMonitor* monitor, GMount* mount, gpointer data)
{
    SendToPlugin* drivers = (SendToPlugin*)data;

    mounted_func (mount, drivers);

    Q_UNUSED(monitor)
}

static void handle_mount_removed(GVolumeMonitor* monitor, GMount* mount, gpointer data)
{
    char*               path = nullptr;
    GFile*              location = nullptr;

    if (!mount || !data) return;

    location = g_mount_get_default_location(mount);
    if (location) {
        path = g_file_get_uri(location);
    }

    if (nullptr != path) {
        ((SendToPlugin*)data)->removeDriver(path);
    }

    if (!path) g_free(path);
    if (!location) g_object_unref(location);

    Q_UNUSED(monitor)
}

static void mounted_func (gpointer data, gpointer udata)
{
    GMount*             m = (GMount*)data;
    GFile*              location = nullptr;
    char*               uri = nullptr;

    if (!data || !udata) return;

    if (m) {
        location = g_mount_get_default_location(m);
        if (location) {
            uri = g_file_get_uri(location);
            if (uri) {
                QString urit = uri;
                ((SendToPlugin*)udata)->addDriver(urit, m);
            }
        }
    }

    if (!uri) g_free(uri);
    if (!location) g_object_unref(location);
}

static QString getIconName (GMount* mount)
{
    if (nullptr == mount) {
        return "";
    }

    QString icon = nullptr;

    GIcon* icons = g_mount_get_icon((GMount*)mount);
    if (G_IS_ICON(icons)) {
        const gchar* const* iconNames = g_themed_icon_get_names(G_THEMED_ICON (icons));
        if (iconNames) {
            auto p = iconNames;
            while (*p) {
                QIcon icont = QIcon::fromTheme(*p);
                if (!icont.isNull()) {
                    icon = QString (*p);
                    break;
                } else {
                    p++;
                }
            }
        }
        g_object_unref(icons);
    }

    return icon;
}
