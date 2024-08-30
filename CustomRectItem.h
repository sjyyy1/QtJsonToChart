#ifndef CUSTOMRECTITEM_H
#define CUSTOMRECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QPen>
#include <QBrush>

class CustomRectItem : public QGraphicsRectItem
{
public:
    CustomRectItem(const QRectF &rect, const QString &value);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QString value;
    QGraphicsTextItem *hoverText;
    QPen originalPen;
    QBrush originalBrush;
};

#endif // CUSTOMRECTITEM_H
