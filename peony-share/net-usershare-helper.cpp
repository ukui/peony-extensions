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

#include "net-usershare-helper.h"
#include "share-properties-page-plugin.h"
#include <glib.h>

#include <QDebug>
#include <QProcess>

UserShareInfoManager* UserShareInfoManager::gShareInfo = nullptr;

static void         parseShareInfo (ShareInfo& shareInfo, QString& content);
static QString      exectueCommand (QStringList& args, bool* ret /* out */);

ShareInfo& ShareInfo::operator =(const ShareInfo* oth)
{
    if (oth == this) {
        return *this;
    }

    this->name = oth->name;
    this->comment = oth->comment;
    this->isShared = oth->isShared;
    this->readOnly = oth->readOnly;
    this->allowGuest = oth->allowGuest;
    this->originalPath = oth->originalPath;

    return *this;
}

UserShareInfoManager& UserShareInfoManager::getInstance()
{
    if (!gShareInfo) {
        gShareInfo = new UserShareInfoManager;
        //gShareInfo->run();
    }

    return *gShareInfo;
}

// fixme:// not use
void UserShareInfoManager::run()
{
    bool            ret;
    QStringList     args;
    QStringList     items;
    QString         result;

    // get all shared file
    args << "usershare" << "list";
    result = exectueCommand (args, &ret);
    if (!ret) {
        return;
    }

    // get all shared fileInfo
    items = result.split('\n');
    for (auto item : items) {
        args.clear();
        args << "usershare" << "info" << item;
        result = exectueCommand (args, &ret);
        if (!ret) {
            qDebug() << "exectueCommand: error!";
            continue;
        }

        // parse UserShared
        ShareInfo* shareInfo = new ShareInfo;
        shareInfo->name = item;
        parseShareInfo(*shareInfo, result);
        if (!addShareInfo(shareInfo)) {
            delete shareInfo;
        }
    }

    mIsInit = true;
}

bool UserShareInfoManager::hasSharedInfo(QString& name)
{
    if (!mIsInit) {
        bool ret = false;
        QStringList args;
        args << "usershare" << "list";
        QString result = exectueCommand (args, &ret);
        return ret && !result.isEmpty();
    }

    mLock.lock();
    if (mSharedInfo.contains(name) && nullptr != mSharedInfo[name]) {
        mLock.unlock();
        return true;
    }
    mLock.unlock();

    return false;
}


static void parseShareInfo (ShareInfo& shareInfo, QString& content)
{
    auto lines = content.split('\n');

    for (auto line : lines) {
        if (line.startsWith("path")) {
            shareInfo.originalPath = line.split('=').last();
        } else if (line.startsWith("comment")) {
            shareInfo.comment = line.split('=').last();
        } else if (line.startsWith("usershare_acl")) {
            shareInfo.readOnly = line.contains("Everyone:R");
        } else if (line.startsWith("guest_ok")) {
            shareInfo.allowGuest = line.split('=').last() == "y";
        }
    }
}

static QString exectueCommand (QStringList& args, bool* retb /* out */)
{
    QProcess proc;
    proc.open();

    args.prepend("/usr/bin/peony-share.sh");
    args.prepend("pkexec");
    proc.start("bash");
    proc.waitForStarted();
    QString cmd = args.join(" ");
    proc.write(cmd.toUtf8() + "\n");
    proc.waitForFinished(500);
//    qDebug() << "command: " << args.join(" ");

    if (retb) {
        if (proc.readAllStandardError().isEmpty()) {
            *retb = true;
        } else {
            *retb = false;
        }
    }

    QString all = proc.readAllStandardOutput();
    proc.close();

    return all;
}

bool UserShareInfoManager::updateShareInfo(ShareInfo &shareInfo)
{
    if ("" == shareInfo.name
            || shareInfo.name.isEmpty()
            || shareInfo.originalPath.isEmpty()) {
        return false;
    }

    bool ret = false;
    QStringList args;
    ShareInfo* sharedInfo = new ShareInfo;
    sharedInfo->name = shareInfo.name;
    sharedInfo->comment = shareInfo.comment;
    sharedInfo->isShared = shareInfo.isShared;
    sharedInfo->readOnly = shareInfo.readOnly;
    sharedInfo->allowGuest = shareInfo.allowGuest;
    sharedInfo->originalPath = shareInfo.originalPath;

    mLock.lock();
    if (mSharedInfo.contains(sharedInfo->name)
            && nullptr != mSharedInfo[sharedInfo->name]) {
        delete mSharedInfo[sharedInfo->name];
    }
    mSharedInfo[sharedInfo->name] = sharedInfo;
    mLock.unlock();

    args << "usershare" << "add";
    args << sharedInfo->name;
    args << sharedInfo->originalPath;
    args << (sharedInfo->comment.isNull() ? "Peony-Qt-Share-Extension" : sharedInfo->comment);
    args << (sharedInfo->readOnly ? "Everyone:R" : "Everyone:F");
    args << (sharedInfo->allowGuest ? "guest_ok=y" : "guest_ok=n");

    exectueCommand (args, &ret);

    return ret;
}

const ShareInfo* UserShareInfoManager::getShareInfo(QString &name)
{
    if (nullptr == name || name.isEmpty()) {
        qDebug() << "invalid param";
        return nullptr;
    }

    if (!mIsInit) {
        bool            ret;
        QStringList     args;
        args << "usershare" << "info" << name;
        QString result = exectueCommand (args, &ret);
        if (!ret && result.isEmpty()) {
            return nullptr;
        }

        // parse UserShared
        ShareInfo* shareInfo = new ShareInfo;
        shareInfo->name = name;
        parseShareInfo(*shareInfo, result);
        if (!addShareInfo(shareInfo)) {
            delete shareInfo;
        }
    }

    mLock.lock();
    if (!mSharedInfo.contains(name)) {
        mLock.unlock();
        return nullptr;
    }

    mLock.unlock();

    return mSharedInfo[name];
}

bool UserShareInfoManager::addShareInfo(ShareInfo* shareInfo)
{
    if (nullptr == shareInfo
            || shareInfo->name.isEmpty()
            || shareInfo->originalPath.isEmpty()) {
        return false;
    }

    mLock.lock();
    if (mSharedInfo.contains(shareInfo->name)) {
        mLock.unlock();
        return false;
    }

    mSharedInfo[shareInfo->name] = shareInfo;
    mLock.unlock();

    return true;
}

void UserShareInfoManager::removeShareInfo(QString &name)
{
    mLock.lock();
    if (mSharedInfo.contains(name)) {
        if (nullptr != mSharedInfo[name]) delete mSharedInfo[name];
        mSharedInfo.remove(name);
    }
    mLock.unlock();

    QStringList args;
    args << "usershare" << "delete" << name;

    exectueCommand (args, nullptr);
}
