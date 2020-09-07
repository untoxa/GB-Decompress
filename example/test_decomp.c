#include <gb/gb.h>

#include "rle_decompress.h"

#include "graphics.h"

unsigned char buffer[3000];

void main(void)
{    
    DISPLAY_OFF;
    LCDC_REG = 0xd1u;

    gb_decompress(picture_tiles, buffer); 
    set_bkg_data(0, (11 * 16), buffer);
    
    gb_decompress(picture_map, buffer);    
    set_bkg_tiles(0, 0, 32, 32, buffer);

    SCX_REG = SCY_REG = 48;
            
    SHOW_BKG;
    DISPLAY_ON; 
}