#include "computer-view.h"

#include <peony-qt/file-item-model.h>
#include <peony-qt/file-item-proxy-filter-sort-model.h>

#include <QStylePainter>
#include <QStyleOption>

Peony::ComputerView::ComputerView(QWidget *parent) : DirectoryViewWidget(parent)
{
    Q_EMIT viewDirectoryChanged();
}

void Peony::ComputerView::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.initFrom(this);
    QStylePainter p(this);
    p.drawPrimitive(QStyle::PE_Frame, opt);
    p.drawItemText(this->rect(), 0, this->palette(), true, "this is a computer view");
}

void Peony::ComputerView::bindModel(Peony::FileItemModel *model, Peony::FileItemProxyFilterSortModel *proxyModel)
{
    model->setRootUri("computer:///");
}

void Peony::ComputerView::beginLocationChange()
{
    Q_EMIT viewDirectoryChanged();
}

void Peony::ComputerView::stopLocationChange()
{
    Q_EMIT viewDirectoryChanged();
}
