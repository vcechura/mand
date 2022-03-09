#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include "palette.h"

#define MAX_INTERATIONS 70
#define X_BASIC 1.5
#define Y_BASIC 1.5

typedef struct COMPUTATION_DATA{
    int width;
    int height;
    float xOrigin;
    float yOrigin;
    QImage im;
    palettes p;
}compData;

class CalcThread: public QThread
{
    Q_OBJECT
    void run() override;
public:
    CalcThread(compData * dataStruct, QMutex * mutex);

public slots:
    void startRecalculation();

signals:
    void resultReady();

private:
    QMutex * m;
    compData * data;
    bool recalculate;
    int width, height;
    float xOrigin, yOrigin;
    palettes p;
    void calculateFract(float xStep,float yStep, float xStart, float yStart);
};

#endif // WORKER_H
