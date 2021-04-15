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

#ifndef NETUSERSHAREHELPER_H
#define NETUSERSHAREHELPER_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QThread>

class ShareInfo
{
public:
    ShareInfo& operator= (const ShareInfo*);

    QString name;
    QString comment;
    QString originalPath;

    bool readOnly   = true;
    bool allowGuest = false;
    bool isShared   = false;
};


class UserShareInfoManager : public QObject/*, public QThread*/
{
    Q_OBJECT
public:
    static UserShareInfoManager& getInstance ();
    void run (); // fixme:// not use

    bool hasSharedInfo (QString& name);
    void removeShareInfo (QString& name);
    bool addShareInfo (ShareInfo* shareInfo);
    bool updateShareInfo (ShareInfo& shareInfo);
    const ShareInfo* getShareInfo (QString& name);

private:
    explicit UserShareInfoManager (QObject* parent = nullptr) : QObject(parent) {};

private:
    bool                            mIsInit = false;
    QMutex                          mLock;
    QMap <QString, ShareInfo*>      mSharedInfo;
    static UserShareInfoManager*    gShareInfo;
};

#endif // NETUSERSHAREHELPER_H
