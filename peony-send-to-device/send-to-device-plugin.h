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

#ifndef ADMINMENUPLUGIN_H
#define ADMINMENUPLUGIN_H

#include <QObject>
#include <gio/gio.h>
#include <menu-plugin-iface.h>

#undef signals

namespace Peony {

//class Drive
class DriverItem;
class RemoveableDriver;

class SendToPlugin : public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid FILE "common.json")
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit SendToPlugin(QObject *parent = nullptr);

    void addDriver (QString uri, GMount* file);
    void removeDriver (const QString uri);
    QString testPlugin() override {return "";}
    bool isEnable() override {return mEnable;}
    void setEnable(bool enable) override {mEnable = enable;}
    const QString description() override {return tr("Send to a removable device");}
    const QString name() override {return "Peony Qt Send to a removable device";}
    const QIcon icon() override {return QIcon::fromTheme("system-users-symbolic");}
    PluginInterface::PluginType pluginType() override {return PluginInterface::MenuPlugin;}
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

public Q_SLOTS:

Q_SIGNALS:
    void mountDel (QString uri);
    void mountAdd (QString uri, QString iconName, QString name);

private:
    void getOneDriver(const QStringList &selectionUris, QMenu *menu, QString uri, QString iconName, QString name);

private:
    bool                                mEnable;
    QMap<QString, DriverItem*>          mitems;
    QMap<QString, GMount*>              mDrivers;
    GVolumeMonitor*                     mVolumeMonitor;
};

class DriverItem : public QAction
{
    friend class RemoveableDriverModel;
    Q_OBJECT
public:
    explicit DriverItem (QString uri, QIcon icon, QString name, QObject* parent = nullptr);

    const QIcon icon ();
    const QString uri ();
    const QString name ();

private:
    bool                                mConnect;
    QString                             mName;
    QIcon                               mIcon;
    QString                             mUri;
};
}

#endif // ADMINMENUPLUGIN_H
