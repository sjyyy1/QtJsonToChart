#ifndef CUSTOMELLIPSEITEM_H
#define CUSTOMELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>

class CustomEllipseItem : public QGraphicsEllipseItem {
public:
    CustomEllipseItem(const QRectF &rect, const QString &value, QGraphicsItem *parent = nullptr);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QString value;
    QGraphicsTextItem *hoverText;
    QRectF originalRect;
    qreal scaleFactor; // 放大的比例因子

};

#endif // CUSTOMELLIPSEITEM_H
