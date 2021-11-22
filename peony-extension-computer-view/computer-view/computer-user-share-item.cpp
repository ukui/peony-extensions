#include "computer-user-share-item.h"
#include "computer-model.h"

#include <file-utils.h>

#include <gio/gio.h>

void query_file_info_async_callback(GFile *file, GAsyncResult *res, ComputerUserShareItem* p_this);

ComputerUserShareItem::ComputerUserShareItem(GVolume *volume, ComputerModel *model, AbstractComputerItem *parentNode, QObject *parent)
    : AbstractComputerItem(model, parentNode, parent)
{
    m_cancellable = g_cancellable_new();

    m_model->beginInsertItem(parentNode->itemIndex(), parentNode->m_children.count());
    parentNode->m_children<<this;

    m_icon = QIcon::fromTheme("drive-harddisk");
    m_uri = "file:///data";
    m_displayName = tr("Data");
    m_model->endInsterItem();

    m_file = g_file_new_for_uri("file:///data");
    GFile* file = g_file_new_for_uri("file:///data");
    GFileInfo* fileInfo = g_file_query_info(file, G_FILE_ATTRIBUTE_UNIX_IS_MOUNTPOINT, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
    if (g_file_info_get_attribute_boolean(fileInfo, G_FILE_ATTRIBUTE_UNIX_IS_MOUNTPOINT)) {
        updateInfoAsync();
    }

    if (file)       g_object_unref(file);
    if (fileInfo)   g_object_unref(fileInfo);

    Q_UNUSED(volume);
}

ComputerUserShareItem::~ComputerUserShareItem()
{
    g_cancellable_cancel(m_cancellable);
    g_object_unref(m_cancellable);
    if (m_file) g_object_unref(m_file);
}

void ComputerUserShareItem::updateInfoAsync()
{
    g_file_query_filesystem_info_async(m_file, "filesystem::*", 0, m_cancellable, GAsyncReadyCallback(query_file_info_async_callback), this);
}

void ComputerUserShareItem::check()
{

}

QModelIndex ComputerUserShareItem::itemIndex()
{
    return m_model->createItemIndex(m_parentNode->m_children.indexOf(this), this);
}

void query_file_info_async_callback(GFile *file, GAsyncResult *res, ComputerUserShareItem* p_this)
{
    GError *err = nullptr;
    GFileInfo *info = g_file_query_info_finish(file, res, &err);
    if (err) {
        g_error_free(err);
        return;
    }
    if (info) {
        quint64 total = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
        quint64 used = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_USED);
        p_this->m_totalSpace = total;
        p_this->m_usedSpace = used;
        auto index = p_this->itemIndex();
        p_this->m_model->dataChanged(index, index);

        g_object_unref(info);
    }
}
