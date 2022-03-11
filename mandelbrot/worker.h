#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include "palette.h"

#define MAX_ITERATIONS 50
#define X_START 1.5
#define Y_START 1.5

#define NUM_OF_CALC_THREADS 2

typedef struct COMPUTATION_DATA{
    int width;
    int height;
    float xOffset;
    float yOffset;
    float xStep;
    float yStep;
    float zoom;
    QImage im;
    palettes p;
}compData;

class CalcThread: public QThread
{
    Q_OBJECT
    void run() override;
public:
    CalcThread(compData * dataStruct, QMutex * mutex, int threadID);

public slots:
    void startRecalculation();

signals:
    void resultReady();

private:
    QMutex * m;
    compData * data;
    bool recalculate;
    int width, height;
    int threadID;
    float xOffset, yOffset;
    float zoom;
    palettes p;
    void calculateFract(float xStep, float yStep, float xOrigin, float yOrigin);
};

#endif // WORKER_H
