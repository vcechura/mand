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

#define NUM_OF_BUTTONS 5

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void changePalette();
    void savePicture();
    void setDimensions();
    void handleResults();
    void keyPressEvent(QKeyEvent *event);

signals:
    void recalculateImage();

private:
    palettes p;
    CalcThread * t;
    QLabel * image;
    QGridLayout * layout;
    QPixmap currentPix;
    QImage currentImage;
    QLineEdit * widthEdit;
    QLineEdit * heightEdit;
    QValidator * widthV;
    QValidator * heightV;
    QPushButton * buttons[NUM_OF_BUTTONS];

    int width, height;
    float xOrigin, yOrigin;

    compData * data;
    QMutex * m;

    void initLayout();
    void initImage();
    void initEditFields();
    void initButtons();
    void startWorkInAThread();

    void changeOrigin(int key);

};
#endif // MAINWINDOW_H
