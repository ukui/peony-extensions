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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#ifndef SHAREPAGE_H
#define SHAREPAGE_H

#include "SwitchButton/switchbutton.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <properties-window-tab-iface.h>
#include <memory>
#include <file-info.h>
#include <QCheckBox>
#include <QFuture>

#if LIBPEONY_USERSHARE_MANAGER
#include <peony-qt/usershare-manager.h>
#else
#include "net-usershare-helper.h"
#endif

using namespace Peony;

class SharePage : public Peony::PropertiesWindowTabIface
{
Q_OBJECT
public:
    explicit SharePage(const QString &uri, QWidget *parent = nullptr);

    void addSeparate()
    {
        QFrame * separate = new QFrame(this);
        separate->setFrameShape(QFrame::HLine);
        QPalette palette = separate->palette();
        palette.setColor(QPalette::WindowText,QColor("#F0F0F0FF"));
        separate->setPalette(palette);
        m_layout->addWidget(separate);
    }

    void init();

    void initFloorOne();

    void initFloorTwo();

    void initFloorThree();

    /*!
     * 响应确定按钮
     * \brief
     */
    void saveAllChange() override;

private:
    QFutureWatcher<void> *m_theFutureWatcher = nullptr;

#if LIBPEONY_USERSHARE_MANAGER
    Peony::ShareInfo       m_shareInfo;
#else
    ShareInfo              m_shareInfo;
#endif
    QVBoxLayout*    m_layout = nullptr;

    std::shared_ptr<Peony::FileInfo> m_fileInfo = nullptr;

    //floor1
    QPushButton *m_iconButton  = nullptr;
    QLabel      *m_folderName  = nullptr;
    QLabel      *m_sharedState = nullptr;

    //floor2
    SwitchButton *m_switchButton = nullptr;
    //floor3
    QFrame    *m_floor3        = nullptr;
    QLineEdit *m_shareNameEdit = nullptr;
    QLineEdit *m_commentEdit   = nullptr;

    QCheckBox *m_allowGuestModify   = nullptr;
    QCheckBox *m_shareAllowGuestCheckBox = nullptr;

};

#endif // SHAREPAGE_H
