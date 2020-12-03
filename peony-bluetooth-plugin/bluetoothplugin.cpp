#include "bluetoothplugin.h"

#include <QFileInfo>

using namespace Peony;

BluetoothPlugin::BluetoothPlugin(QObject *parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"system().name:"<<QLocale::system().name();
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/peony-bluetooth-plugin_"+QLocale::system().name());
    QApplication::installTranslator(t);
}

QList<QAction *> BluetoothPlugin::menuActions(Peony::MenuPluginInterface::Types types, const QString &uri, const QStringList &selectionUris)
{
    qDebug() << Q_FUNC_INFO << uri << selectionUris;
    QList<QAction*> actions;
    if (types == MenuPluginInterface::DirectoryView || types == MenuPluginInterface::DesktopWindow)
    {
        if (! selectionUris.isEmpty()) {
            auto info = FileInfo::fromUri(selectionUris.first(), false);
            //special type mountable, return
            qDebug()<<"info isVirtual:"<<info->isVirtual()<<info->mimeType();
            if (!selectionUris.first().startsWith("file:///"))
                  return actions;
            else{
                if(info->mimeType().split("/").at(1) != "directory"){
                    QAction *compress = new QAction(QIcon::fromTheme("blueman-tray"), tr("Send from bluetooth to..."), nullptr);
                    actions << compress;
                    connect(compress, &QAction::triggered, [=](){
                        QString path = selectionUris.at(0);
                        QDBusMessage m = QDBusMessage::createMethodCall("org.ukui.bluetooth","/org/ukui/bluetooth","org.ukui.bluetooth","file_transfer");
                        m << path.split("//").at(1);
                        qDebug() << Q_FUNC_INFO << m.arguments().at(0).value<QString>() <<__LINE__;
                        // 发送Message
                        QDBusConnection::sessionBus().call(m);
                    });
                }
            }
        }
    }

    return actions;
}
