#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include "palette.h"
#include "worker.h"
#include <QFile>
#include <QKeyEvent>

#define WIDTH_MIN 200
#define WIDTH_MAX 1800
#define HEIGHT_MIN 200
#define HEIGHT_MAX 900
#define BASIC_WIDTH 800
#define BASIC_HEIGHT 800

#define MAX_ZOOM 1.5

#define NUM_OF_BUTTONS 5

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleResults(); // when threads signal that they are done computing, handle the image

signals:
    void recalculateImage(); // used to signal to calc. threads to recalculate the image

private:
    /* GUI variables */
    QLabel * image;
    QGridLayout * layout;
    QPixmap currentPix;
    QImage currentImage;
    QLineEdit * widthEdit;
    QLineEdit * heightEdit;
    QValidator * widthV;
    QValidator * heightV;
    QPushButton * buttons[NUM_OF_BUTTONS];

    /* Local calculation data */
    uint32_t width, height;
    float xOffset, yOffset;
    float xStep, yStep;
    float zoom;
    palettes p;

    /* Variables connected to threads */
    CalcThread * t[NUM_OF_CALC_THREADS];
    compData * data; // computation data
    QMutex * m;
    uint16_t numOfThreadsFinished;
    bool threadsReady;

    /* Init functions */
    void initLayout();
    void initVariables();
    void initcompData();
    void initImage();
    void initEditFields();
    void initButtons();
    void startWorkInAThread();

    /* Functions for interacting with GUI */
    void changeOrigin(int key);
    void changeZoom(int key);
    void changePalette();
    void savePicture();
    void setDimensions();
    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
