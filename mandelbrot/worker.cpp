#include "worker.h"

CalcThread::CalcThread(compData * dataStruct, QMutex * mutex, int thread){
    data = dataStruct;
    m = mutex;
    threadID = thread;
    recalculate = false;
}

void CalcThread::run(){
    /* While loop runs until threads are requested to close*/
    while(!QThread::currentThread()->isInterruptionRequested()){

        if(recalculate){
            recalculate = 0;

            m->lock(); // update calculation data
            width = data->width;
            height = data->height;
            xOffset = data->xOffset;
            yOffset = data->yOffset;
            zoom = data->zoom;
            p = data->p;
            m->unlock();

            /* Calculate the step and image origin based on width, zoom and origin offset */
            float xScaled, yScaled; // We have to scale the size of the protion of complex plane
            float xOrigin, yOrigin; // Origin of the calculation
            float xStep, yStep; // steps between pixels
            if(width > height){
                float scale = width/height;
                xScaled = X_START * zoom * scale; // scale the plane
                yScaled = Y_START * zoom;
            }else{
                float scale = height/width;
                xScaled = X_START* zoom;
                yScaled = Y_START * zoom * scale;
            }
            xStep = 2*xScaled/width;
            yStep = 2*yScaled/height;
            xOrigin = -(width*xStep)*1/2 + xOffset; // Left of the complex plane shown
            yOrigin = (height*yStep)*1/2 + yOffset; // Top of the complex plane shown

            if(threadID == 0){ // only need to update this once
                m->lock();
                data->xStep = xStep;
                data->yStep = yStep;
                m->unlock();
            }

            calculateFract(xStep, yStep, xOrigin, yOrigin); // calculates the fractal values
        }
    }
}

/* Calculates how many iterations are needed to decide if the pixel belongs to mandel. and colors the pixels */
void CalcThread::calculateFract(float xStep, float yStep, float xOrigin, float yOrigin){

    float zx, zy, cx, cy, tempx;
    int count;
    int part = width/NUM_OF_CALC_THREADS;
    for(int i = threadID*part; i < threadID*part + part; i++){ // divide the work among work threads
        for(int j = 0; j < height; j++){
            zx = 0;
            zy = 0;
            count = 0;
            cx = xOrigin + i * xStep;
            cy = yOrigin - j * yStep;
            while ((zx * zx + zy * zy < 4) && (count < MAX_ITERATIONS)) {

                // tempx = z_real*_real - z_imaginary*z_imaginary + c_real
                tempx = zx * zx - zy * zy + cx;

                // 2*z_real*z_imaginary + c_imaginary
                zy = 2 * zx * zy + cy;

                // Updating z_real = tempx
                zx = tempx;

                // Increment count
                count += 1;
            }
            float temp = (float)count/MAX_ITERATIONS; // scale it to be between 0-256
            int index = temp * PALETTE_RANGE;
            rgb colorRGB = p.colorPixel(index); // Read the color from palette
            QRgb color = qRgb(colorRGB.r, colorRGB.g, colorRGB.b);
            data->im.setPixel(i,j,color); // Update the pixel in image
        }
    }
    emit resultReady();
}

void CalcThread::startRecalculation(){
    recalculate = true;
}
