#ifndef PALETTE_H
#define PALETTE_H

#include <fstream>
#include <QString>

#define PALETTE_RANGE 256

typedef struct RGB{
    int r;
    int g;
    int b;
}rgb;

class palettes {
public:
    palettes();
    int currentPalette;
    enum paletteNames {SEPIA, BLACKRED, BLUERED}pal;
    rgb colorPixel(int count);

private:
    rgb blackRed[PALETTE_RANGE];
    rgb sepia[PALETTE_RANGE];
    rgb blueRed[PALETTE_RANGE];
    bool loadPalette(QString file_name, rgb (&paletteArray)[PALETTE_RANGE]);
};

#endif // PALETTE_H
