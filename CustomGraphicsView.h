#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H


#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    CustomGraphicsView(QWidget *parent = nullptr);
    void wheelEvent(QWheelEvent *event) override;
    double currentScale; // 跟踪当前缩放比例

signals:
    void scaleChanged(int scalePercentage);

public slots:
    void setScale(int scalePercentage);
};

#endif // CUSTOMGRAPHICSVIEW_H
