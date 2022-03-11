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
#define BASIC_WIDTH 500
#define BASIC_HEIGHT 500

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
    void initLayout(); // Inits the GUI layout
    void initVariables(); // Inits class variables to starting values
    void initcompData(); // Inits comp. data structure
    void initImage(); // Inits QLabel which is a widget used to show the image
    void initEditFields(); // Edit fiels used to get user input for width and height
    void initButtons(); // Inits push buttons
    void startWorkInAThread(); // Starts the worker threads

    /* Functions for interacting with GUI */
    void changePalette();//Changes the current palette
    void savePicture();//Saves a picture of the current image in .png format
    void setDimensions();//Sets width and height according to the edit field input
    void keyPressEvent(QKeyEvent *event);//Handles key presses
};
#endif // MAINWINDOW_H
