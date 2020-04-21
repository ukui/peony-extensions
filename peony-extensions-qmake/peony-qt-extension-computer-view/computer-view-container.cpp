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

#include "computer-view-container.h"
#include "computer-view.h"

#include <peony-qt/file-item-model.h>
#include <peony-qt/file-item-proxy-filter-sort-model.h>

#include <QStylePainter>
#include <QStyleOption>

#include <QHBoxLayout>

Peony::ComputerViewContainer::ComputerViewContainer(QWidget *parent) : DirectoryViewWidget(parent)
{
    m_view = new ComputerView(this);
    auto layout = new QHBoxLayout;
    layout->addWidget(m_view);
    setLayout(layout);
    Q_EMIT viewDirectoryChanged();
}

void Peony::ComputerViewContainer::paintEvent(QPaintEvent *e)
{
    DirectoryViewWidget::paintEvent(e);
}

void Peony::ComputerViewContainer::bindModel(Peony::FileItemModel *model, Peony::FileItemProxyFilterSortModel *proxyModel)
{
    m_model = model;
    m_proxyModel = proxyModel;
    model->setRootUri("computer:///");
    connect(model, &Peony::FileItemModel::findChildrenFinished, this, &Peony::DirectoryViewWidget::viewDirectoryChanged);
}

void Peony::ComputerViewContainer::beginLocationChange()
{
    Q_EMIT viewDirectoryChanged();
}

void Peony::ComputerViewContainer::stopLocationChange()
{
    Q_EMIT viewDirectoryChanged();
}
