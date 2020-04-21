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

#ifndef COMPUTERVOLUMEITEM_H
#define COMPUTERVOLUMEITEM_H

#include "abstract-computer-item.h"
#include <peony-qt/volume-manager.h>

class ComputerVolumeItem : public AbstractComputerItem
{
    Q_OBJECT
public:
    explicit ComputerVolumeItem(GVolume *volume, ComputerModel *model, AbstractComputerItem *parentNode, QObject *parent = nullptr);
    ~ComputerVolumeItem();

    void updateInfoAsync();

    Type itemType() override {return Volume;}
    const QString uri() override {return m_uri;}
    const QString displayName() override;
    const QIcon icon() override;

    void findChildren() override;

    void updateInfo() override {updateInfoAsync();}


    bool canEject() override;
    void eject() override;
    bool canUnmount() override;
    void unmount() override;

    void mount() override;

    QModelIndex itemIndex() override;

    qint64 usedSpace() override {return m_usedSpace;}
    qint64 totalSpace() override {return m_totalSpace;}

protected:
    //monitor
    static void volume_changed_callback(GVolume *volume, ComputerVolumeItem *p_this);
    static void volume_removed_callback(GVolume *volume, ComputerVolumeItem *p_this);

    //info
    static void qeury_info_async_callback(GFile *file, GAsyncResult *res, ComputerVolumeItem *p_this);
    static void query_root_info_async_callback(GFile *file, GAsyncResult *res, ComputerVolumeItem *p_this);

    //mount op
    static void mount_async_callback(GVolume *volume, GAsyncResult *res, ComputerVolumeItem *p_this);
    static void unmount_async_callback(GMount *mount, GAsyncResult *res, ComputerVolumeItem *p_this);
    static void eject_async_callback(GMount *mount, GAsyncResult *res, ComputerVolumeItem *p_this);

private Q_SLOTS:
    void onVolumeAdded(const std::shared_ptr<Peony::Volume> volume);

private:
    QString m_uri;

    std::shared_ptr<Peony::Volume> m_volume;
    std::shared_ptr<Peony::Mount> m_mount;
    GCancellable *m_cancellable;

    //info
    QString m_displayName;
    QIcon m_icon;
    qint64 m_totalSpace = 0;
    qint64 m_usedSpace = 0;
};

#endif // COMPUTERVOLUMEITEM_H
