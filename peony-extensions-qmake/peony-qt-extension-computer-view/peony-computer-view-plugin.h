#ifndef PEONYCOMPUTERVIEWPLUGIN_H
#define PEONYCOMPUTERVIEWPLUGIN_H

#include "peony-qt-extension-computer-view_global.h"
#include <peony-qt/directory-view-plugin-iface2.h>

namespace Peony {

class PEONYQTEXTENSIONCOMPUTERVIEW_EXPORT PeonyComputerViewPlugin : public QObject, public Peony::DirectoryViewPluginIface2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DirectoryViewPluginIface2_iid)
    Q_INTERFACES(Peony::DirectoryViewPluginIface2)
public:
    explicit PeonyComputerViewPlugin(QObject *parent = nullptr);

    //common
    PluginType pluginType() {return PluginType::DirectoryViewPlugin2;}

    const QString name() {return QObject::tr("Computer View");}
    const QString description() {return QObject::tr("Show drives, network and personal directories.");}
    const QIcon icon() {return QIcon::fromTheme("computer");}
    void setEnable(bool enable) {}
    bool isEnable() {return true;}

    //view
    QString viewIdentity() {return "Computer View";}
    QString viewName() {return name();}
    QIcon viewIcon() {return icon();}
    bool supportUri(const QString &uri) {return uri == "computer:///";}

    int zoom_level_hint() {return -1;}
    int minimumSupportedZoomLevel() {return -1;}
    int maximumSupportedZoomLevel() {return -1;}

    int priority(const QString &directoryUri);
    bool supportZoom() {return false;}

    DirectoryViewWidget *create();
};

}

#endif // PEONYCOMPUTERVIEWPLUGIN_H
