#include "CustomGraphicsView.h"

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent), currentScale(1.0) // 初始化缩放比例
{
    setRenderHint(QPainter::Antialiasing, true);//启用抗锯齿，用于平滑图形的边缘，减少锯齿状边缘，使得图形更加平滑和美观
    setTransformationAnchor(AnchorUnderMouse); //设置变换锚点为鼠标下方，当用户缩放或旋转视图时，变换将以鼠标光标所在位置为中心进行
    setResizeAnchor(AnchorUnderMouse);//设置调整大小的锚点为鼠标下方，当用户调整视图窗口的大小时，视图将以鼠标光标所在位置为中心进行重新布局
}

//处理鼠标滚轮事件
void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15;//缩放因子，表示每次滚轮滚动时的缩放比例
    if (event->angleDelta().y() > 0) { //检测滚轮滚动方向，如果 y 轴上的增量大于 0，表示向上滚动（放大视图）
        scale(scaleFactor, scaleFactor); //调用 scale 方法将视图按 scaleFactor 缩放
        currentScale *= scaleFactor;//更新当前缩放比例
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);//否则向下滚动,调用 scale 方法将视图按 1.0 / scaleFactor 缩放
        currentScale /= scaleFactor;//更新当前缩放比例
    }
    emit scaleChanged(currentScale * 100);//通过发射 scaleChanged 信号，任何连接到这个信号的槽函数（即响应函数）都会被调用
}

void CustomGraphicsView::setScale(int scalePercentage) {
    QTransform t;//QTransform 用于定义 2D 图形变换
    t.scale(scalePercentage / 100.0, scalePercentage / 100.0);//调用 QTransform 的 scale 方法来设置缩放比例
    setTransform(t);//使用 QGraphicsView 的 setTransform 方法将刚刚创建的变换 t 应用到视图中,更新视图的缩放比例
    currentScale = scalePercentage / 100.0;//currentScale 用于保存当前的缩放比例，以便在其他地方使用。
    emit scaleChanged(scalePercentage);
}
