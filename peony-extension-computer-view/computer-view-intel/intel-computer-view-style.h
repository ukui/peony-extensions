#ifndef INTEL_COMPUTERVIEWSTYLE_H
#define INTEL_COMPUTERVIEWSTYLE_H
#include <QProxyStyle>
#include <peony-core_global.h>

namespace Intel {

class ComputerViewStyle : public QProxyStyle
{
public:
    static ComputerViewStyle *getStyle();

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

    void drawControl(QStyle::ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget = nullptr) const override;

    void drawItemPixmap(QPainter *painter,
                        const QRect &rect,
                        int alignment,
                        const QPixmap &pixmap) const override;

    void drawItemText(QPainter *painter,
                      const QRect &rect,
                      int flags,
                      const QPalette &pal,
                      bool enabled,
                      const QString &text,
                      QPalette::ColorRole textRole = QPalette::NoRole) const override;

private:
    explicit ComputerViewStyle(QStyle *style = nullptr);
    ~ComputerViewStyle() override {}
};

}

#endif // COMPUTERVIEWSTYLE_H
