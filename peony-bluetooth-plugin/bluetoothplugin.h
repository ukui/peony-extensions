#ifndef BLUETOOTHPLUGIN_H
#define BLUETOOTHPLUGIN_H

#include <file-info.h>

#include "peony-bluetooth-plugin_global.h"
#include <menu-plugin-iface.h>

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QString>
#include <QStringList>
#include <QAction>
#include <QTranslator>
#include <QLocale>
#include <QApplication>
#include <QDebug>

namespace Peony {

class PEONYQTENGRAMPAMENUPLUGINSHARED_EXPORT BluetoothPlugin: public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid)
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit BluetoothPlugin(QObject *parent = nullptr);

    PluginInterface::PluginType pluginType() override {return PluginInterface::MenuPlugin;}
    const QString name() override {return tr("Peony-Qt bluetooth Extension");}
    const QString description() override {return tr("bluetooth Menu Extension.");}
    const QIcon icon() override {return QIcon::fromTheme("blueman-tray");}
    void setEnable(bool enable) override {m_enable = enable;}
    bool isEnable() override {return m_enable;}

    QString testPlugin() override {return "test compress";}
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

private:
    bool m_enable;
};

}
#endif // BLUETOOTHPLUGIN_H
