#include "palette.h"
#include <QFile>
#include <QTextStream>
#include <QString>

/* Load the palettes into arrays and set current pelette to BWRGB */
palettes::palettes(){
    if(!loadPalette("Sepia.plt", sepia)){
        exit(100); // Exit if loading the palette failed
    }
    if(!loadPalette("BlackRed.plt", blackRed)){
        exit(100);
    }
    if(!loadPalette("BWRGB.plt", bwrgb)){
        exit(100);
    }
    currentPalette = BWRGB;
}

/* Reads the file with a palette and loads it into array. Returns false if the palette file is corrupted. */
bool palettes::loadPalette(QString filename, rgb (&paletteArray)[PALETTE_RANGE]){
    QFile palette(filename); //open the file
    if (!palette.open(QIODevice::ReadOnly)){
        return false;
    }
    int count=0;
    QTextStream input(&palette);

    while(!input.atEnd()){//Read all lines
        if(count > PALETTE_RANGE){//palette should have 256 lines
            palette.close();
            return false;
        }
        QString line = input.readLine();//read the file line by line
        QString tempNum;
        uint8_t colorCount = 0;
        uint8_t tempArray[3];//temporaryArray
        /*Check each line char by char, if there are 3 rgb values */
        for(int i = 0; i < line.size(); i++){
            QChar ch = line[i];
            if(ch >= '0' && ch <= '9'){
                tempNum.append(ch);
            }else if(ch == ';'){//Colors are separated by ";"
                tempArray[colorCount] = tempNum.toUInt();
                colorCount++;
                tempNum.clear();
            }
        }
        if(colorCount == 3){
            paletteArray[count].r = tempArray[0];
            paletteArray[count].g = tempArray[1];
            paletteArray[count].b = tempArray[2];
        }else{ // return false if there are not 3 rgb colors on one line
            palette.close();
            return false;
        }
        count++;
    }
    palette.close();
    if(count != PALETTE_RANGE){ // check that the program read 256 lines
        return false;
    }
    return true;
}

/* Returns palette values of the currently selected palette at given index */
rgb palettes::colorPixel(int count){
    if(currentPalette == SEPIA){
        return sepia[count];
    }else if(currentPalette == BWRGB){
        return bwrgb[count];
    }else{
        return blackRed[count];
    }
}

