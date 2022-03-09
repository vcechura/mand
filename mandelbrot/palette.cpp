#include "palette.h"
#include <QFile>
#include <QTextStream>
#include <QString>

palettes::palettes(){


    if(!loadPalette("Sepia.plt", sepia)){
        exit(100);
    }
    if(!loadPalette("BlackRed.plt", blackRed)){
        exit(100);
    }
    if(!loadPalette("BlueRed.plt", blueRed)){
        exit(100);
    }

    currentPalette = SEPIA;
}

bool palettes::loadPalette(QString filename, rgb (&paletteArray)[PALETTE_RANGE]){
    QFile palette(filename);
    if (!palette.open(QIODevice::ReadOnly)){
        return false;
    }
    int count=0;
    QTextStream input(&palette);
    while(!input.atEnd()){
        if(count > PALETTE_RANGE){
            return false;
        }
        QString line = input.readLine();
        QString tempNum;
        uint8_t colorCount = 0;
        int tempArray[3];
        for(int i = 0; i < line.size(); i++){
            QChar ch = line[i];
            if(ch >= 48 && ch <= 57){
                tempNum.append(ch);
            }else if(ch == ';'){
                tempArray[colorCount] = tempNum.toInt();
                colorCount++;
                tempNum.clear();
            }
        }
        if(colorCount == 3){
            paletteArray[count].r = tempArray[0];
            paletteArray[count].g = tempArray[1];
            paletteArray[count].b = tempArray[2];
        }else{
            return false;
        }
        count++;
    }
    palette.close();
    return true;
}

rgb palettes::colorPixel(int count){
    if(currentPalette == SEPIA){
        return sepia[count];
    }else if(currentPalette == BLUERED){
        return blueRed[count];
    }else{
        return blackRed[count];
    }
}

