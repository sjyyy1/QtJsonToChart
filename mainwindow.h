#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "CustomRectItem.h"
#include "CustomGraphicsView.h"
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QJsonArray>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QDebug>
#include <QSlider>
#include <QLabel>
#include <QWheelEvent>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void drawImage();
    void exportImage();
    void clearText();
    void updateScaleLabel(int scalePercentage);

private:
    Ui::MainWindow *ui;

    void createLayout();
    void drawAxes();
    void drawBar(const QJsonArray &seriesData, const QJsonArray &xAxisData);
    void drawStackedBar(const QJsonArray &seriesData, const QJsonArray &xAxisData);
    void drawLine(const QJsonArray &series, const QJsonArray &xAxisData);
    void drawSmoothLine(const QJsonArray &series, const QJsonArray &xAxisData);
    QString convertJsToJson(const QString &jsText);
    void applyStyles();
    void formatText();
    void addTextToGraphicsView(const QString &text, const QPointF &position, int fontSize);

    QPushButton *drawButton;
    QPushButton *exportButton;
    QPushButton *clearButton;
    QPushButton *formatButton;

    QTextEdit *jsonEditor;
    CustomGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QSlider *zoomSlider;
    QLabel *scaleLabel;
    QLabel *zoomLabel;
    QVector<QColor> colors;

    int xAxisLength;
    int yAxisLength;
    int xOffset;
    int yOffset;
};
#endif // MAINWINDOW_H
