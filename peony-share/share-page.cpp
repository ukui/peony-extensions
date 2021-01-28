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

#include "share-page.h"
#include "share-properties-page-plugin.h"

#include <PeonyFileInfoJob>
#include <QUrl>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QButtonGroup>
#include <PeonyFileInfo>
#include <QtConcurrent>

#include <QDebug>

SharePage::SharePage(const QString &uri, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    QFuture<void> initFileInfoJob = QtConcurrent::run([=]() {
        m_fileInfo = Peony::FileInfo::fromUri(uri);
        Peony::FileInfoJob *fileInfoJob = new Peony::FileInfoJob(m_fileInfo);
        fileInfoJob->setAutoDelete(true);
        fileInfoJob->querySync();
    });

    m_theFutureWatcher = new QFutureWatcher<void>();
    m_theFutureWatcher->setFuture(initFileInfoJob);

    connect(m_theFutureWatcher, &QFutureWatcher<void>::finished, this, &SharePage::init);
}

void SharePage::init()
{
    if (m_theFutureWatcher)
        delete m_theFutureWatcher;

    m_shareInfo = ShareInfo(m_fileInfo->displayName(), false);
    m_shareInfo.originalPath = m_fileInfo->filePath();
    auto manager = Peony::SharePropertiesPagePlugin::getInstance();
    auto infos = manager->getCurrentShareInfos();
    for (auto info : infos) {
        qDebug() << "compare" << m_shareInfo.name << info.name << m_shareInfo.originalPath << info.originalPath;
        if (m_shareInfo.name == info.name && m_shareInfo.originalPath == info.originalPath) {
            m_shareInfo.comment = info.comment;
            m_shareInfo.readOnly = info.readOnly;
            m_shareInfo.allowGuest = info.allowGuest;
            m_shareInfo.originalPath = info.originalPath;
            m_shareInfo.isShared = true;
            break;
        }
    }

    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);

    this->initFloorOne();

    this->addSeparate();

    this->initFloorTwo();

    this->initFloorThree();

    m_layout->addStretch(1);
}

void SharePage::saveAllChange()
{
    qDebug() << __FILE__ << __FUNCTION__ << "page changed?" << m_thisPageChanged;

    if (!m_thisPageChanged)
        return;

    bool checked = m_shareCheckBox->isChecked();

    if (checked) {
        //FIXME:暂时不支持自定义共享名 - Currently does not support custom shared name
        m_shareInfo.name = m_fileInfo->displayName();
        m_shareInfo.readOnly = m_shareReadOnlyCheckBox->isChecked();
        m_shareInfo.allowGuest = m_shareAllowGuestCheckBox->isChecked();
        m_shareInfo.isShared = true;

        NetUsershareHelper::updateShareInfo(m_shareInfo);
    } else {
        //FIXME:需要先判断存在共享吗 - Need to determine whether there is sharing first
        NetUsershareHelper::removeShared(m_shareInfo.name);
    }
}

void SharePage::initFloorOne()
{
    QFrame * floor1 = new QFrame(this);
    floor1->setMinimumHeight(90);

    QHBoxLayout * layout1 = new QHBoxLayout(floor1);
    layout1->setContentsMargins(22, 14, 22, 0);
    layout1->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    floor1->setLayout(layout1);

    m_iconButton = new QPushButton(floor1);
    m_iconButton->setIcon(QIcon::fromTheme(m_fileInfo.get()->iconName()));
    m_iconButton->setIconSize(QSize(48, 48));

    layout1->addWidget(m_iconButton);

    QVBoxLayout * vBoxLayout = new QVBoxLayout(floor1);

    m_folderName = new QLabel(floor1);
    m_sharedState = new QLabel(floor1);

    m_folderName->setText(m_fileInfo.get()->displayName());
    m_sharedState->setText(m_shareInfo.isShared ? tr("share this folder") : tr("don`t share this folder"));

    vBoxLayout->addStretch(1);
    vBoxLayout->addWidget(m_folderName);
    vBoxLayout->addWidget(m_sharedState);
    vBoxLayout->addStretch(1);

    layout1->addLayout(vBoxLayout);
    layout1->addStretch(1);

    m_layout->addWidget(floor1);
}

void SharePage::initFloorTwo()
{
    QFrame * floor2 = new QFrame(this);
    floor2->setMinimumHeight(64);

    QVBoxLayout * layout2 = new QVBoxLayout(floor2);
    layout2->setContentsMargins(22, 0, 22, 0);

    floor2->setLayout(layout2);

    bool shared = m_shareInfo.isShared;

    m_shareCheckBox = new QCheckBox(tr("Share folder"), floor2);
    m_shareCheckBox->setChecked(shared);

    layout2->addWidget(m_shareCheckBox);

    m_layout->addWidget(floor2);

    connect(m_shareCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        this->thisPageChanged();
        if (checked) {
            //FIXME:暂时不支持自定义共享名 - Currently does not support custom shared name
            m_shareInfo.name = m_fileInfo->displayName();
            m_shareInfo.readOnly = m_shareReadOnlyCheckBox->isChecked();
            m_shareInfo.allowGuest = m_shareAllowGuestCheckBox->isChecked();

            if (m_floor3)
                m_floor3->setVisible(true);

            m_sharedState->setText(tr("share this folder"));
            m_shareInfo.isShared = true;

        } else {
            if (m_floor3)
                m_floor3->setVisible(false);

            m_sharedState->setText(tr("don`t share this folder"));
            m_shareInfo.isShared = false;
        }
    });
}

void SharePage::initFloorThree()
{
    m_floor3 = new QFrame(this);

    QVBoxLayout * layout3 = new QVBoxLayout(m_floor3);
    layout3->setContentsMargins(22, 0, 22, 0);

    m_floor3->setLayout(layout3);

    QFormLayout *formLayout = new QFormLayout(m_floor3);

    m_shareNameEdit = new QLineEdit(m_floor3);
    m_shareNameEdit->setMaxLength(32);
    m_shareNameEdit->setEnabled(false);
    m_shareNameEdit->setText(m_shareInfo.name);

    m_commentEdit = new QLineEdit(m_floor3);
    m_commentEdit->setMaxLength(64);
    m_commentEdit->setText(m_shareInfo.comment);

    formLayout->addRow(tr("Share name:"), m_shareNameEdit);
    formLayout->addRow(tr("Comment:"), m_commentEdit);

    layout3->addLayout(formLayout);

    m_shareReadOnlyCheckBox = new QCheckBox(tr("Read Only"), this);
    m_shareReadOnlyCheckBox->setChecked(m_shareInfo.readOnly);

    m_shareAllowGuestCheckBox = new QCheckBox(tr("Allow Anonymous"));
    m_shareAllowGuestCheckBox->setChecked(m_shareInfo.allowGuest);

    layout3->addWidget(m_shareReadOnlyCheckBox);
    layout3->addWidget(m_shareAllowGuestCheckBox);

    m_floor3->setVisible(m_shareInfo.isShared);

    m_layout->addWidget(m_floor3);

    connect(m_commentEdit, &QLineEdit::textChanged, this, [=]() {
        m_shareInfo.comment = m_commentEdit->text();
        this->thisPageChanged();
    });

    connect(m_shareReadOnlyCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        m_shareInfo.readOnly = checked;
        this->thisPageChanged();
    });

    connect(m_shareAllowGuestCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        m_shareInfo.allowGuest = checked;
        this->thisPageChanged();
    });
}
