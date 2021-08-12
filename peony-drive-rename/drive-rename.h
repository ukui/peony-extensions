/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: deng hao <denghao@kylinos.cn>
 *
 */

#ifndef DRIVERENAME_H
#define DRIVERENAME_H

#include <QObject>
#include <gio/gio.h>
#include <menu-plugin-iface.h>

namespace Peony {
class DriveRename : public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid FILE "common.json")
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit DriveRename(QObject *parent = nullptr);

    QString testPlugin() override {return "";}
    bool isEnable() override {return mEnable;}
    void setEnable(bool enable) override {mEnable = enable;}
    const QString description() override {return tr("drive rename");}
    const QString name() override {return "Peony Qt drive rename";}
    const QIcon icon() override {return QIcon::fromTheme("system-users-symbolic");}
    PluginInterface::PluginType pluginType() override {return PluginInterface::MenuPlugin;}
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

private:
    bool                                mEnable;
    QString                             mDevName;

};
};

#endif // DRIVERENAME_H
