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

static QString getIconName (GIcon* icons);
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

    QAction* action = new DriverAction(selectionUris);
    if (action) {
        l << action;
    }

    Q_UNUSED(uri)
    Q_UNUSED(types)

    return l;
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
    DriverAction* drivers = (DriverAction*)data;

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
        ((DriverAction*)data)->driverRemove(path);
    }

    qDebug() << "remove uri:" << path;

    if (!path) g_free(path);
    if (!location) g_object_unref(location);

    Q_UNUSED(monitor)
}

static void mounted_func (gpointer data, gpointer udata)
{
    GMount*             mount = (GMount*)data;
    GFile*              location = nullptr;
    char*               uri = nullptr;
    char*               name = nullptr;
    GIcon*              icons = nullptr;
    QString             icon = nullptr;

    if (!data || !udata) return;

    if (mount) {
        name = g_mount_get_name(mount);
        icons = g_mount_get_icon(mount);
        location = g_mount_get_default_location(mount);
        if (location) {
            uri = g_file_get_uri(location);
        }
    }

    icon = getIconName (icons);

    if (uri && name) {
        ((DriverAction*)udata)->driverAdded(uri, name, icon);
    }

    qDebug() << "name:" << name << " uri:" << uri << " icons:" << icons << " icon:" << icon;

    if (!uri) g_free(uri);
    if (!name) g_free(name);
    // FIXME://
//    if (icons) g_object_unref(icons);
    if (!location) g_object_unref(location);
}

static QString getIconName (GIcon* icons)
{
    if (nullptr == icons) {
        return "";
    }

    QString icon = nullptr;

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

DriverAction::DriverAction(const QStringList& uris, QObject *parent) : QAction(parent)
{
    mMenu = new QMenu();
    mVolumeMonitor = g_volume_monitor_get();

    mDeviceAdd = g_signal_connect(G_OBJECT(mVolumeMonitor), "mount-added", G_CALLBACK(handle_mount_added), (gpointer)this);
    mDeviceRemove = g_signal_connect(G_OBJECT(mVolumeMonitor), "mount-removed", G_CALLBACK(handle_mount_removed), (gpointer)this);

    connect(this, &DriverAction::driverAdded, this, [=] (QString uri, QString name, QString icon) {
        if (!mDrivers.contains(uri)) {
            auto it = new DriverItem (uri, QIcon::fromTheme(icon), name);
            it->connect(it, &QAction::triggered, it, [=] () {
                FileCopyOperation* op = new FileCopyOperation(uris, it->uri(), it);
                op->setAutoDelete(true);
                FileOperationManager::getInstance()->startOperation(op);
            });
            mMenu->addAction(it);
            mDrivers[uri] = it;
        }
        showAction();
    });

    connect(this, &DriverAction::driverRemove, this, [=] (QString uri) {
        if (mDrivers.contains(uri)) {
            auto it = mDrivers[uri];
            mMenu->removeAction(it);
            it->deleteLater();
            mDrivers.remove(uri);
        }
        showAction();
    });

    GList* mounts = g_volume_monitor_get_mounts (mVolumeMonitor);
    if (mounts) {
        g_list_foreach (mounts, mounted_func, this);
        g_list_free_full(mounts, g_object_unref);
    }

    setMenu(mMenu);
    setText(tr("Send to a removable device"));
}

DriverAction::~DriverAction()
{
    if (mMenu) delete mMenu;
    if (mVolumeMonitor) {
        g_signal_handler_disconnect(G_OBJECT(mVolumeMonitor), mDeviceAdd);
        g_signal_handler_disconnect(G_OBJECT(mVolumeMonitor), mDeviceRemove);
        g_object_unref(mVolumeMonitor);
    }
    for (auto it = mDrivers.begin(); it != mDrivers.end(); ++it) {
        it.value()->deleteLater();
    }
}

void DriverAction::showAction()
{
    setVisible(mDrivers.size() > 0 ? true : false);
}
