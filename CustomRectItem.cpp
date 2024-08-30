#include "CustomRectItem.h"

CustomRectItem::CustomRectItem(const QRectF &rect, const QString &value)
    : QGraphicsRectItem(rect), value(value), hoverText(nullptr)
//初始化：调用基类 QGraphicsRectItem 的构造函数，用 rect 初始化矩形边界。
//将参数 value 的值赋给成员变量 value。

{
    setAcceptHoverEvents(true);//使得这个图形项可以接收鼠标悬停事件
}

//当鼠标悬停进入 CustomRectItem 对象时，这个函数会被调用
void CustomRectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // 保存原始的画笔和填充颜色
    originalPen = pen();
    originalBrush = brush();

    // 修改填充颜色使其变浅
    QColor lighterColor = originalBrush.color().lighter(120); // 使用 lighter 方法使颜色变浅
    setBrush(QBrush(lighterColor));

    hoverText = new QGraphicsTextItem(value, this);//创建一个新的 QGraphicsTextItem 对象用于显示文本
    QRectF rect = this->rect(); // 获取矩形的边界
    double x = rect.center().x() - hoverText->boundingRect().width() / 2;
    double y = rect.top() - hoverText->boundingRect().height() - 5;
    hoverText->setPos( x , y );
    hoverText->setDefaultTextColor(Qt::black); // 设置文本颜色
    hoverText->setZValue(1); // 设置一个较高的Z值，确保文本在图层中处于顶端
    QGraphicsRectItem::hoverEnterEvent(event); // 调用基类 QGraphicsRectItem 的 hoverEnterEvent 函数，确保基类的处理逻辑也被执行
}

void CustomRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    // 恢复原始的画笔和填充颜色
    setPen(originalPen);
    setBrush(originalBrush);

    delete hoverText;
    hoverText = nullptr;
    QGraphicsRectItem::hoverLeaveEvent(event);
}
