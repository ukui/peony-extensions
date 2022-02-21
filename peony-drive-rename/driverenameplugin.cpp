#include "driverenameplugin.h"
#include "drive-rename.h"

using namespace Peony;

DriveRenamePlugin::DriveRenamePlugin(QObject *parent) : QObject(parent)
{
    m_internalPlugin = new DriveRename(this);
}

QList<QAction *> DriveRenamePlugin::menuActions(Peony::MenuPluginInterface::Types types, const QString &uri, const QStringList &selectionUris)
{
    return m_internalPlugin->menuActions(types, uri, selectionUris);
}
