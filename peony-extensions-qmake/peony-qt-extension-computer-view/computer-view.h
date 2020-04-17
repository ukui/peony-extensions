#ifndef COMPUTERVIEW_H
#define COMPUTERVIEW_H

#include <QWidget>
#include <peony-qt/directory-view-plugin-iface2.h>
#include <peony-qt/controls/directory-view/directory-view-widget.h>

namespace Peony {

class ComputerView : public DirectoryViewWidget
{
    Q_OBJECT
public:
    explicit ComputerView(QWidget *parent = nullptr);

    const virtual QString viewId() {return "Computer View";}

    //location
    const virtual QString getDirectoryUri() {return "computer:///";}

    //selections
    const virtual QStringList getSelections() {return QStringList();}

    //children
    const virtual QStringList getAllFileUris() {return QStringList();}

    virtual int getSortType() {return 0;}
    virtual Qt::SortOrder getSortOrder() {return Qt::AscendingOrder;}

    //zoom
    virtual int currentZoomLevel() {return -1;}
    virtual int minimumZoomLevel() {return -1;}
    virtual int maximumZoomLevel() {return -1;}
    virtual bool supportZoom() {return false;}

protected:
    void paintEvent(QPaintEvent *e);

public Q_SLOTS:
    virtual void bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel);

    //location
    //virtual void open(const QStringList &uris, bool newWindow) {}
    virtual void setDirectoryUri(const QString &uri) {}
    virtual void beginLocationChange();
    virtual void stopLocationChange();

    virtual void closeDirectoryView() {}

    //selections
    virtual void setSelections(const QStringList &uris) {}
    virtual void invertSelections() {}
    virtual void scrollToSelection(const QString &uri) {}

    //clipboard
    //cut items should be drawn differently.
    virtual void setCutFiles(const QStringList &uris) {}

    virtual void setSortType(int sortType) {}
    virtual void setSortOrder(int sortOrder) {}

    virtual void editUri(const QString &uri) {}
    virtual void editUris(const QStringList uris) {}

    virtual void repaintView() {}

    virtual void clearIndexWidget() {}

    //zoom
    virtual void setCurrentZoomLevel(int zoomLevel) {}
};

}

#endif // COMPUTERVIEW_H
