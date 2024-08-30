#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CustomGraphicsView.h"
#include "CustomEllipseItem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainterPath>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),  drawButton(nullptr), exportButton(nullptr), jsonEditor(nullptr), graphicsView(nullptr),
    scene(nullptr),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1000, 600);
    createLayout();
    applyStyles();
}

//界面色彩美化
void MainWindow::applyStyles() {
    setStyleSheet(R"(
        QWidget {
            background-color: #e0f7fa;
        }
        QTextEdit {
            background-color: #ffffff;
            border: 1px solid #b0bec5;
            border-radius: 4px;
            padding: 4px;
        }
        QPushButton {
            background-color: #26a69a;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #80cbc4;
        }
        QPushButton:pressed {
            background-color: #004d40;
        }
        QSlider::groove:horizontal {
            border: 1px solid #999999;
            height: 8px;
            background: #b0bec5;
            margin: 2px 0;
        }
        QSlider::handle:horizontal {
            background: #26a69a;
            border: 1px solid #5c5c5c;
            width: 18px;
            margin: -2px 0;
            border-radius: 3px;
        }
    )");
}

//更新缩放比例标签
void MainWindow::updateScaleLabel(int scalePercentage) {  //参数：缩放比例的百分比
    scaleLabel->setText(QString::number(scalePercentage) + "%");
}

//设置窗口
void MainWindow::createLayout() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget); //创建中心部件并设置为主窗口的中心部件

    jsonEditor = new QTextEdit(centralWidget); //创建一个文本编辑器
    graphicsView = new CustomGraphicsView(centralWidget); //创建一个自定义的图形视图
    scene = new QGraphicsScene(this); //创建一个新的 QGraphicsScene 对象，一个用于管理和显示 2D 图形对象的容器
    graphicsView->setScene(scene); //将场景设置为 QGraphicsView 的场景

    drawButton = new QPushButton("绘制图像", centralWidget); //在中心部件创建四个按钮
    exportButton = new QPushButton("导出图像", centralWidget);
    clearButton = new QPushButton("界面清空", centralWidget);
    formatButton = new QPushButton("文本格式化", centralWidget);

    connect(drawButton, &QPushButton::clicked, this, &MainWindow::drawImage); //当按钮被点击时，执行相应的槽函数。
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportImage);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearText);
    connect(formatButton, &QPushButton::clicked, this, &MainWindow::formatText);

    QVBoxLayout *leftLayout = new QVBoxLayout(); //设置左侧布局
    leftLayout->addWidget(jsonEditor);

    QHBoxLayout *leftButtonLayout = new QHBoxLayout();  //文本编辑器和三个按钮（界面清空、绘制图像、文本格式化）排布在一起
    leftButtonLayout->addWidget(clearButton);
    leftButtonLayout->addWidget(drawButton);
    leftButtonLayout->addWidget(formatButton);
    leftLayout->addLayout(leftButtonLayout);

    QVBoxLayout *rightLayout = new QVBoxLayout(); //设置右侧布局
    rightLayout->addWidget(graphicsView);

    //页面缩放设置
    QHBoxLayout *zoomLayout = new QHBoxLayout();
    zoomSlider = new QSlider(Qt::Horizontal, centralWidget);//创建一个新的水平滑动条
    zoomSlider->setRange(10, 400); // 缩放范围 10% to 400%
    zoomSlider->setValue(100);     // 默认为100%
    zoomLabel = new QLabel("-", centralWidget);//创建两个 QLabel 对象，用于显示文本
    scaleLabel = new QLabel("100%", centralWidget);
    zoomLayout->addWidget(zoomLabel);//水平排列控件
    zoomLayout->addWidget(zoomSlider);
    zoomLayout->addWidget(scaleLabel);

    QHBoxLayout *rightButtonLayout = new QHBoxLayout();
    rightButtonLayout->addLayout(zoomLayout);
    rightButtonLayout->addWidget(exportButton);
    rightLayout->addLayout(rightButtonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout(); //设置主布局
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    centralWidget->setLayout(mainLayout);

    //连接滑块信号到自定义图形视图的缩放功能
    connect(zoomSlider, &QSlider::valueChanged, graphicsView, &CustomGraphicsView::setScale);//更新 CustomGraphicsView 的缩放比例
    connect(graphicsView, &CustomGraphicsView::scaleChanged, this, &MainWindow::updateScaleLabel);//更新显示的缩放百分比标签
}

//格式化
void MainWindow::formatText() {
    QString text = jsonEditor->toPlainText();

    text.remove(QRegularExpression("^option\\s*=\\s*"));
    text.remove(QRegularExpression(";$"));
    text.replace(QRegularExpression("([a-zA-Z0-9_]+)\\s*:"), "\"\\1\":");
    text.replace("'", "\"");

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(text.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "格式化错误", "无效的JSON文本，无法格式化");
        return;
    }

    // 格式化JSON文本
    QString formattedText = jsonDoc.toJson(QJsonDocument::Indented);
    jsonEditor->setPlainText(formattedText);
}

//提取文本，绘图
void MainWindow::drawImage() {
    QString jsonText = jsonEditor->toPlainText();
    jsonText.remove(QRegularExpression("^option\\s*=\\s*"));
    jsonText.remove(QRegularExpression(";$"));
    jsonText.replace(QRegularExpression("([a-zA-Z0-9_]+)\\s*:"), "\"\\1\":");
    jsonText.replace("'", "\"");
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        QMessageBox::warning(this, "错误", "JSON 格式错误");
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray xAxisData = jsonObj["xAxis"].toObject()["data"].toArray();
    QJsonArray series = jsonObj["series"].toArray();
    QString seriesType = series[0].toObject()["type"].toString();

    // 清空场景
    scene->clear();

    xAxisLength = graphicsView->width() - 50; // 动态计算 X 轴长度
    yAxisLength = graphicsView->height() - 200; // 动态计算 Y 轴长度
    xOffset = graphicsView->width()-20;
    yOffset = graphicsView->height() - 100; // 动态计算 Y 轴起点位置

    drawAxes();
    if(seriesType == "bar"){
        drawStackedBar(series,xAxisData);
    }else if(seriesType == "line"){
        bool smooth = series[0].toObject()["smooth"].toBool(false);//如果 smooth 字段不存在或其值无法转换为布尔值，则默认返回 false
        if (smooth) {
            drawSmoothLine(series, xAxisData);
        } else {
            drawLine(series, xAxisData);
        }
    }
}

//绘制坐标轴
void MainWindow::drawAxes() {
    int xOffset1 = xOffset - 5;

    // 绘制 X 轴
    scene->addLine(xOffset1, yOffset, xOffset1 + xAxisLength, yOffset, QPen(Qt::black));

    // 绘制 Y 轴
    scene->addLine(xOffset1, yOffset, xOffset1, yOffset - yAxisLength, QPen(Qt::black));

    // 绘制 X 轴箭头
    scene->addLine(xOffset1 + xAxisLength, yOffset, xOffset1 + xAxisLength - 10, yOffset - 5, QPen(Qt::black));
    scene->addLine(xOffset1 + xAxisLength, yOffset, xOffset1 + xAxisLength - 10, yOffset + 5, QPen(Qt::black));

    // 绘制 Y 轴箭头
    scene->addLine(xOffset1, yOffset - yAxisLength, xOffset1 - 5, yOffset - yAxisLength + 10, QPen(Qt::black));
    scene->addLine(xOffset1, yOffset - yAxisLength, xOffset1 + 5, yOffset - yAxisLength + 10, QPen(Qt::black));
}

//添加文本
void MainWindow::addTextToGraphicsView(const QString &text, const QPointF &position, int fontSize) {
    QGraphicsTextItem *textItem = new QGraphicsTextItem(text);
    QFont font = textItem->font();
    font.setPointSize(fontSize);
    textItem->setFont(font);
    textItem->setPos(position);
    scene->addItem(textItem);
}

//绘制柱状图
void MainWindow::drawStackedBar(const QJsonArray &series, const QJsonArray &xAxisData) {
    double wid = xAxisLength / xAxisData.size(); //
    double barWidth = wid * 5 / 6; //柱状图宽度

    // 计算堆叠柱状图的最大值
    QVector<int> stackedValues(xAxisData.size(), 0);
    for (const auto &serie : series) {
        QJsonArray data = serie.toObject()["data"].toArray();
        for (int i = 0; i < data.size(); ++i) {
            stackedValues[i] += data[i].toInt();
        }
    }
    int maxValue = *std::max_element(stackedValues.begin(), stackedValues.end());
    maxValue += 4;

    // 标 y 轴坐标
    int yTicks = 6; // 设置 Y 轴上要显示的刻度数目
    double valueStep = maxValue / yTicks; //计算每个刻度对应的值
    double yStep = yAxisLength / yTicks; //计算每个刻度之间的距离
    for (int i = 0; i <= yTicks; ++i) {
        int y = yOffset - i * yStep;
        scene->addLine(xOffset - 5, y, xOffset, y, QPen(Qt::black));
        QGraphicsTextItem *textItem = scene->addText(QString::number(i * valueStep));
        textItem->setPos(xOffset - 35, y - 10);
    }

    // 绘制堆叠柱状图
    double heightRatio = static_cast<double>(yAxisLength) / maxValue;
    QVector<double> accumulatedHeights(xAxisData.size(), 0);
    colors = { QColor(173, 216, 230), QColor(144, 238, 144), QColor(255, 182, 193), QColor(221, 160, 221), QColor(211, 211, 211) };

    for (int seriesIndex = 0; seriesIndex < series.size(); ++seriesIndex) {
        QJsonArray data = series[seriesIndex].toObject()["data"].toArray();
        QColor color = colors[seriesIndex % colors.size()];

        for (int i = 0; i < data.size(); ++i) {
            double height = data[i].toDouble() * heightRatio;
            double yPos = yOffset - (series.size() > 1 ? accumulatedHeights[i] : 0);
            QRectF rect(xOffset + i * wid, yPos - height, barWidth, height);
            QString tooltipText = series.size() > 1 ? QString::number(stackedValues[i]) : QString::number(data[i].toInt());
            CustomRectItem *barItem = new CustomRectItem(rect, tooltipText);
            barItem->setPen(QPen(Qt::black));
            barItem->setBrush(QBrush(color));
            scene->addItem(barItem);

            if (series.size() > 1) {
                accumulatedHeights[i] += height;
            }

            // 绘制x轴
            if (seriesIndex == 0) {
                QGraphicsTextItem *textItem = scene->addText(xAxisData[i].toString());
                double x = xOffset + i * wid + barWidth / 2 - textItem -> boundingRect().width() / 2 ;
                double y = yOffset + 10 ;
                textItem->setPos( x , y );
            }
        }
    }
    QString text = "柱状图";
    QPointF position(xOffset + xAxisLength * 7 / 16, yOffset + 35); // 指定文本位置
    int fontSize = 12; // 指定字体大小
    addTextToGraphicsView(text, position, fontSize);
}

//绘制折线图
void MainWindow::drawLine(const QJsonArray &series, const QJsonArray &xAxisData) {
    double wid = xAxisLength / xAxisData.size(); //每一块的宽度，点在块中间画即可
    int diameter = 7;
    //计算最大值
    int maxValue = 0;
    QJsonArray dataArray = series[0].toObject()["data"].toArray();
    for (const QJsonValue& value : dataArray){
        int intValue = value.toInt();
        maxValue = intValue > maxValue ? intValue : maxValue;
    }
    maxValue += 4;


    //标y轴坐标
    int yTicks = 6; //设置 Y 轴上要显示的刻度数目
    double valueStep = maxValue / yTicks; //计算每个刻度对应的值
    double yStep = yAxisLength / yTicks; //计算每个刻度之间的距离
    for (int i = 0; i <= yTicks; ++i) {
        int y = yOffset - i * yStep;
        scene->addLine(xOffset - 5, y, xOffset, y, QPen(Qt::black));
        QGraphicsTextItem *textItem = scene->addText(QString::number(i * valueStep));
        textItem->setPos(xOffset - 35, y - 10);
    }

    double heightRatio = static_cast<double>(yAxisLength) / maxValue;//求比例
    //绘制折线
    for (int i = 0; i < dataArray.size(); ++i) {
        double height = dataArray[i].toDouble()* heightRatio;
        if(i!=0){
            double height0 = dataArray[i-1].toDouble()* heightRatio;
            double x1 = xOffset + (i-1) * wid + wid / 2 +  diameter / 2 ;
            double y1 = yOffset - height0 +  diameter / 2;
            double x2 = xOffset + i * wid + wid / 2 +  diameter / 2;
            double y2 = yOffset - height +  diameter / 2 ;
            scene->addLine(x1 , y1 , x2 , y2  , QPen(QColor(50,100,150),2));
        }
    }
    //绘制圆圈
    for (int i = 0; i < dataArray.size(); ++i) {
        double height = dataArray[i].toDouble()* heightRatio;
        QRectF rect(xOffset + i * wid + wid / 2 , yOffset - height, diameter , diameter);
        CustomEllipseItem *ellipseItem = new CustomEllipseItem(rect, QString::number(dataArray[i].toInt()));
        ellipseItem->setPen(QPen(QColor(50,100,150),2));
        ellipseItem->setBrush(QBrush(Qt::white));
        scene->addItem(ellipseItem);
        //标x值
        QGraphicsTextItem *textItem = scene->addText(xAxisData[i].toString());
        textItem->setPos(xOffset + i * wid + wid / 2, yOffset + 10);
        // 绘制小竖线
        scene->addLine(xOffset + i * wid , yOffset -5 , xOffset + i * wid , yOffset, QPen(Qt::black));
    }
    QString text = "折线图";
    QPointF position(xOffset + xAxisLength * 7 / 16, yOffset + 35); // 指定文本位置
    int fontSize = 12; // 指定字体大小
    addTextToGraphicsView(text, position, fontSize);
}


//绘制平滑折线图
void MainWindow::drawSmoothLine(const QJsonArray &series, const QJsonArray &xAxisData) {
    double wid = xAxisLength / xAxisData.size(); // 每一块的宽度，点在块中间画即可
    int diameter = 7;
    //计算最大值
    int maxValue = 0;
    QJsonArray dataArray = series[0].toObject()["data"].toArray();
    for (const QJsonValue &value : dataArray) {
        int intValue = value.toInt();
        maxValue = intValue > maxValue ? intValue : maxValue;
    }
    maxValue += 4;

    // 标 y 轴坐标
    int yTicks = 6; // 设置 Y 轴上要显示的刻度数目
    double valueStep = maxValue / yTicks; // 计算每个刻度对应的值
    double yStep = yAxisLength / yTicks; // 计算每个刻度之间的距离
    for (int i = 0; i <= yTicks; ++i) {
        int y = yOffset - i * yStep;
        scene->addLine(xOffset - 5, y, xOffset, y, QPen(Qt::black));
        QGraphicsTextItem *textItem = scene->addText(QString::number(i * valueStep));
        textItem->setPos(xOffset - 35, y - 10);
    }

    // 绘制折线图
    double heightRatio = static_cast<double>(yAxisLength) / maxValue; // 求比例

    QPainterPath path;
    for (int i = 0; i < dataArray.size(); ++i) {
        double height = dataArray[i].toDouble() * heightRatio;
        double x = xOffset + i * wid + wid / 2;
        double y = yOffset - height;
        if (i == 0) {
            path.moveTo(x, y);
        } else {
            double previousX = xOffset + (i - 1) * wid + wid / 2; //目标点的 x 和 y 坐标，即曲线的终点
            double previousY = yOffset - dataArray[i - 1].toDouble() * heightRatio;
            double control1X = previousX + wid / 2; //第一个控制点的 x 和 y 坐标，控制曲线的起始方向和弯曲程度
            double control1Y = previousY;
            double control2X = x - wid / 2; //第二个控制点的 x 和 y 坐标，控制曲线的结束方向和弯曲程度
            double control2Y = y;
            path.cubicTo(control1X, control1Y, control2X, control2Y, x, y); //用于绘制三次贝塞尔曲线的函数
        }
    }

    scene->addPath(path, QPen(QColor(50,100,150),2));

    // 绘制数据点和标 x 值
    for (int i = 0; i < dataArray.size(); ++i) {
        double height = dataArray[i].toDouble() * heightRatio;
        QRectF rect(xOffset + i * wid + wid / 2 - diameter / 2, yOffset - height - diameter / 2, diameter, diameter);
        CustomEllipseItem *ellipseItem = new CustomEllipseItem(rect, QString::number(dataArray[i].toInt()));
        ellipseItem->setPen(QPen(QColor(50,100,150),2));
        ellipseItem->setBrush(QBrush(Qt::white));
        scene->addItem(ellipseItem);

        QGraphicsTextItem *textItem = scene->addText(xAxisData[i].toString());
        textItem->setPos(xOffset + i * wid + wid / 2, yOffset + 10);

        // 绘制小竖线
        scene->addLine(xOffset + i * wid, yOffset - 5, xOffset + i * wid, yOffset, QPen(Qt::black));
    }

    QString text = "平滑折线图";
    QPointF position(xOffset + xAxisLength * 3 / 8, yOffset + 35); // 指定文本位置
    int fontSize = 12; // 指定字体大小
    addTextToGraphicsView(text, position, fontSize);
}


// 导出图像
void MainWindow::exportImage() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.xpm *.jpg)"));
    if (fileName.isEmpty())
        return;
    else {
        QPixmap pixMap = graphicsView->grab();
        pixMap.save(fileName);
    }
}

//界面清空
void MainWindow::clearText() {
    jsonEditor->clear();
    scene->clear(); // 清空图像框
}

MainWindow::~MainWindow()
{
    delete ui;
}
