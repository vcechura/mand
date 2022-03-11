#ifndef PALETTE_H
#define PALETTE_H

#include <fstream>
#include <QString>

#define PALETTE_RANGE 256

typedef struct RGB{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb;

class palettes {
public:
    palettes();
    int currentPalette;
    enum paletteNames {SEPIA, BLACKRED, BWRGB}pal;
    rgb colorPixel(int count);

private:
    bool loadPalette(QString file_name, rgb (&paletteArray)[PALETTE_RANGE]);

    rgb bwrgb[PALETTE_RANGE];
    rgb blackRed[PALETTE_RANGE];
    rgb sepia[PALETTE_RANGE];
    rgb test[PALETTE_RANGE];
};

#endif // PALETTE_H
