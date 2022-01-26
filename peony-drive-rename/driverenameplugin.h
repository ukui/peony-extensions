#ifndef DRIVERENAMEPLUGIN_H
#define DRIVERENAMEPLUGIN_H

#include <QObject>
#include <menu-plugin-iface.h>
#include "drive-rename.h"

namespace Peony {

class DriveRenamePlugin : public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid FILE "common.json")
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit DriveRenamePlugin(QObject *parent = nullptr);

    QString testPlugin() override {return "";}
    bool isEnable() override {return mEnable;}
    void setEnable(bool enable) override {mEnable = enable;}
    const QString description() override {return tr("drive rename");}
    const QString name() override {return "Peony Qt drive rename";}
    const QIcon icon() override {return QIcon::fromTheme("system-users-symbolic");}
    PluginInterface::PluginType pluginType() override {return PluginInterface::MenuPlugin;}
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

private:
    DriveRename *m_internalPlugin = nullptr;
    bool mEnable = true;
};

}

#endif // DRIVERENAMEPLUGIN_H
