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
#include "computer-proxy-model.h"
#include "abstract-computer-item.h"
#include "login-remote-filesystem.h"

#include <peony-qt/file-utils.h>
#include <peony-qt/file-item-model.h>
#include <peony-qt/connect-to-server-dialog.h>
#include <peony-qt/file-item-proxy-filter-sort-model.h>

#include <QMenu>
#include <QProcess>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStyleOption>
#include <QInputDialog>
#include <QStylePainter>

static void ask_question_cb(GMountOperation *op, char *message, char **choices, Peony::ComputerViewContainer *p_this);
static void ask_password_cb(GMountOperation *op, const char *message, const char *default_user, const char *default_domain, GAskPasswordFlags flags, Peony::ComputerViewContainer *p_this);


static GAsyncReadyCallback mount_enclosing_volume_callback(GFile *volume, GAsyncResult *res, Peony::ComputerViewContainer *p_this)
{
    GError *err = nullptr;
    g_file_mount_enclosing_volume_finish (volume, res, &err);
    if ((nullptr == err) || (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_ALREADY_MOUNTED))) {
        qDebug() << "login successful!";
        Q_EMIT p_this->updateWindowLocationRequest(p_this->m_remote_uri);
    } else {
        qDebug() << "login remote error: " <<err->code<<err->message<<err->domain;
        QMessageBox::warning(nullptr, "log remote error", err->message, QMessageBox::Ok);
    }

    if (nullptr != err) {
        g_error_free(err);
    }

    return nullptr;
}

void aborted_cb(GMountOperation *op, Peony::ComputerViewContainer *p_this)
{
    g_mount_operation_reply(op, G_MOUNT_OPERATION_ABORTED);
    p_this->disconnect();
}

Peony::ComputerViewContainer::ComputerViewContainer(QWidget *parent) : DirectoryViewWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_op = g_mount_operation_new();
    g_signal_connect (m_op, "aborted", G_CALLBACK (aborted_cb), this);

    connect(this, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos){
        auto selectedIndexes = m_view->selectionModel()->selectedIndexes();
        auto index = m_view->indexAt(pos);
        if (!selectedIndexes.contains(index))
            m_view->clearSelection();

        if (index.isValid()) {
            m_view->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        }

        QMenu menu;
        auto model = static_cast<ComputerProxyModel *>(m_view->model());
        QStringList uris;
        QList<AbstractComputerItem *> items;
        for (auto index : m_view->selectionModel()->selectedIndexes()) {
            auto item = model->itemFromIndex(index);
            uris<<item->uri();
            if (item->uri() == "network:///")
                return;
            items<<item;
        }

        if (items.count() == 0) {
            menu.addAction(tr("Connect a server"), [=](){
                QString uri;
                ConnectServerDialog* dlg = new ConnectServerDialog;
                dlg->deleteLater();
                auto code = dlg->exec();
                if (code == QDialog::Rejected) {
                    return;
                }

                QUrl url = dlg->uri();

                ConnectServerLogin* dlgLogin = new ConnectServerLogin(url.host());
                dlgLogin->deleteLater();
                code = dlgLogin->exec();
                if (code == QDialog::Rejected) {
                    return;
                }

                g_mount_operation_set_username(m_op, dlgLogin->user().toUtf8().constData());
                g_mount_operation_set_password(m_op, dlgLogin->password().toUtf8().constData());
//                g_mount_operation_set_domain(m_op, dlg->domain().toUtf8().constData());
                g_mount_operation_set_anonymous(m_op, dlgLogin->anonymous());
                g_mount_operation_set_password_save(m_op, dlgLogin->savePassword()? G_PASSWORD_SAVE_NEVER: G_PASSWORD_SAVE_FOR_SESSION);

                GFile* m_volume = g_file_new_for_uri(dlg->uri().toUtf8().constData());
                m_remote_uri = dlg->uri();
                g_file_mount_enclosing_volume(m_volume, G_MOUNT_MOUNT_NONE, m_op, nullptr, GAsyncReadyCallback(mount_enclosing_volume_callback), this);
                g_signal_connect (m_op, "ask-question", G_CALLBACK(ask_question_cb), this);
                g_signal_connect (m_op, "ask-password", G_CALLBACK (ask_password_cb), this);
            });
        } else if (items.count() == 1 && items.first()->uri() != "" && items.first()->uri() != "network:///") {
            auto item = items.first();
            bool unmountable = item->canUnmount();
            menu.addAction(tr("Unmount"), [=](){
                item->unmount(G_MOUNT_UNMOUNT_NONE);
            });
            menu.actions().first()->setEnabled(unmountable);

            /*eject function for volume. fix #18216*/
            auto ejectAction = menu.addAction(tr("Eject"), [=](){
                item->eject(G_MOUNT_UNMOUNT_NONE);
            });
            ejectAction->setEnabled(item->canEject());

            auto uri = item->uri();
            auto realUri = m_view->tryGetVolumeUriFromMountTarget(uri);
            if (!realUri.isEmpty()) {
                uri = realUri;
            }
            auto a = menu.addAction(tr("Property"), [=](){
                if (uri.isNull()) {
                    QMessageBox::warning(0, 0, tr("You have to mount this volume first"));
                } else {
                    QProcess p;
                    p.setProgram("peony");
                    QStringList args;
                    args<<"-p"<<uri;
                    p.setArguments(args);
//                    p.startDetached();
                    p.startDetached(p.program(), p.arguments());
                }
            });
            a->setEnabled(!uri.isNull());
        } else if(items.first()->uri() != "" && items.first()->uri() != "network:///"){
            qDebug() << "unable Property uri:" <<items.first()->uri();
            menu.addAction(tr("Property"));
            menu.actions().first()->setEnabled(false);
        }

        menu.exec(QCursor::pos());
    });
}

static void ask_question_cb(GMountOperation *op, char *message, char **choices, Peony::ComputerViewContainer *p_this)
{
    g_mount_operation_reply (op, G_MOUNT_OPERATION_HANDLED);
}

static void ask_password_cb(GMountOperation *op, const char *message, const char *default_user, const char *default_domain, GAskPasswordFlags flags, Peony::ComputerViewContainer *p_this)
{
    Q_UNUSED(message);
    Q_UNUSED(default_user);
    Q_UNUSED(default_domain);
    Q_UNUSED(flags);
    Q_UNUSED(p_this);

    g_mount_operation_reply (op, G_MOUNT_OPERATION_HANDLED);
}


Peony::ComputerViewContainer::~ComputerViewContainer()
{
    if (nullptr != m_op) {
        g_object_unref(m_op);
    }
}

const QStringList Peony::ComputerViewContainer::getSelections()
{
    QStringList uris;
    auto model = static_cast<ComputerProxyModel *>(m_view->model());
    for (auto index : m_view->selectionModel()->selectedIndexes()) {
        auto item = model->itemFromIndex(index);
        uris<<item->uri();
    }
    return uris;
}

void Peony::ComputerViewContainer::paintEvent(QPaintEvent *e)
{
    DirectoryViewWidget::paintEvent(e);
}

void Peony::ComputerViewContainer::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        if (m_enterAction)
            m_enterAction->triggered();
        e->accept();
        return;
    }

    QWidget::keyPressEvent(e);
}

void Peony::ComputerViewContainer::bindModel(Peony::FileItemModel *model, Peony::FileItemProxyFilterSortModel *proxyModel)
{
    m_model = model;
    m_proxyModel = proxyModel;
    model->setRootUri("computer:///");
    connect(model, &Peony::FileItemModel::findChildrenFinished, this, &Peony::DirectoryViewWidget::viewDirectoryChanged);

    if (m_view)
        m_view->deleteLater();

    m_view = new ComputerView(this);
    auto layout = new QHBoxLayout;
    layout->addWidget(m_view);
    setLayout(layout);
    Q_EMIT viewDirectoryChanged();

    auto selectionModel = m_view->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &DirectoryViewWidget::viewSelectionChanged);;

    connect(m_view, &QAbstractItemView::doubleClicked, this, [=](const QModelIndex &index){
        if (!index.parent().isValid())
            return;

        auto model = static_cast<ComputerProxyModel *>(m_view->model());
        auto item = model->itemFromIndex(index);
        if (!item->uri().isEmpty()) {
            item->check();
            this->updateWindowLocationRequest(item->uri());
        } else {
            item->mount();
        }
    });

    m_enterAction = new QAction(this);
    m_enterAction->setShortcut(Qt::Key_Enter);
    addAction(m_enterAction);

    connect(m_enterAction, &QAction::triggered, this, [=](){
        if (m_view->selectionModel()->selectedIndexes().count() == 1) {
            Q_EMIT m_view->doubleClicked(m_view->selectionModel()->selectedIndexes().first());
        }
    });
}

void Peony::ComputerViewContainer::beginLocationChange()
{
    Q_EMIT viewDirectoryChanged();

    m_view->refresh();
}

void Peony::ComputerViewContainer::stopLocationChange()
{
    Q_EMIT viewDirectoryChanged();
}
