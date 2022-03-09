#include "worker.h"

CalcThread::CalcThread(compData * dataStruct, QMutex * mutex){
    data = dataStruct;
    m = mutex;
    recalculate = false;
}

void CalcThread::run(){
    while(!QThread::currentThread()->isInterruptionRequested()){

        if(recalculate){
            m->lock();
            width = data->width;
            height = data->height;
            xOrigin = data->xOrigin;
            yOrigin = data->yOrigin;
            p = data->p;
            m->unlock();
            float xScale;
            float yScale;
            if(width > height){
                float scale = width/height;
                xScale = X_BASIC * scale;
                yScale = Y_BASIC;
            }else{
                float scale = height/width;
                xScale = X_BASIC;
                yScale = Y_BASIC * scale;
            }
            recalculate = 0;
            float xStep = 1.5*xScale/width;
            float yStep = 2*yScale/height;
            calculateFract(xStep, yStep, xOrigin, yOrigin);
        }

    }
}

void CalcThread::calculateFract(float xStep,float yStep, float xStart, float yStart){

    float zx, zy, cx, cy, tempx;
    int count;
    QImage image = QImage(width, height, QImage::Format_RGB888);
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            zx = 0;
            zy = 0;
            count = 0;
            cx = -xStart + i * xStep;
            cy = yStart - j * yStep;
            while ((zx * zx + zy * zy < 4) && (count < MAX_INTERATIONS)) {

                // tempx = z_real*_real - z_imaginary*z_imaginary + c_real
                tempx = zx * zx - zy * zy + cx;

                // 2*z_real*z_imaginary + c_imaginary
                zy = 2 * zx * zy + cy;

                // Updating z_real = tempx
                zx = tempx;

                // Increment count
                count += 1;
            }
            float temp = (float)count/MAX_INTERATIONS;
            int index = temp * PALETTE_RANGE;
            rgb colorRGB = p.colorPixel(index);
            QRgb color = qRgb(colorRGB.r, colorRGB.g, colorRGB.b);
            image.setPixel(i,j,color);
        }
        if(recalculate){
            return;
        }
    }
    m->lock();
    data->im = image;
    m->unlock();
    emit resultReady();
}

void CalcThread::startRecalculation(){
    recalculate = true;
}
