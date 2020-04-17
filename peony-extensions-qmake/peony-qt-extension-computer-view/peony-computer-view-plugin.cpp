#include "peony-computer-view-plugin.h"
#include "computer-view.h"

Peony::PeonyComputerViewPlugin::PeonyComputerViewPlugin(QObject *parent) : QObject(parent)
{
}

int Peony::PeonyComputerViewPlugin::priority(const QString &directoryUri)
{
    if (directoryUri == "computer:///")
        return 1;
    return -1;
}

Peony::DirectoryViewWidget *Peony::PeonyComputerViewPlugin::create()
{
    return new ComputerView;
}
