#include "CustomEllipseItem.h"

CustomEllipseItem::CustomEllipseItem(const QRectF &rect, const QString &value, QGraphicsItem *parent)
    : QGraphicsEllipseItem(rect, parent), value(value), hoverText(nullptr)
//调用基类 QGraphicsEllipseItem 的构造函数，并将 rect 和 parent 传递给它
{
    setAcceptHoverEvents(true);//启用悬停事件
    originalRect = rect;//保存原始矩形
    scaleFactor = 1.2; // 放大的比例因子
}

void CustomEllipseItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    // 创建文本项
    hoverText = new QGraphicsTextItem(value, this);
    double x=event->pos().x() - hoverText->boundingRect().width() / 2;
    double y=event->pos().y() - hoverText->boundingRect().height() - 5;
    hoverText->setPos(x, y);
    hoverText->setDefaultTextColor(Qt::black);

    // 记录原始大小
    originalRect = rect();

    // 放大圆形
    QRectF newRect = rect();
    qreal newRadius = newRect.width() / 2 * scaleFactor; //新的半径
    newRect.setSize(QSizeF(newRadius * 2, newRadius * 2));
    setRect(newRect);

    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void CustomEllipseItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    // 删除文本项
    delete hoverText;
    hoverText = nullptr;

    // 恢复原始大小
    setRect(originalRect);

    QGraphicsEllipseItem::hoverLeaveEvent(event);
}
