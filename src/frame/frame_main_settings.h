#ifndef _FRAME_MAIN_SETTINGS_H_
#define _FRAME_MAIN_SETTINGS_H_

#include "frame_base.h"

#define SCREENWIDTH  540
#define MARGIN_LEFT  20
#define MARGIN_TOP   90

#define KEYS_PER_ROW 4
#define KEY_W 92
#define KEY_H 92

#define ROWWIDTH ( SCREENWIDTH - 2 * MARGIN_LEFT )
#define KEY_OFFSET_X 136 // ( ROWWIDTH - ( KEYS_PER_ROW * KEY_W ) / (KEYS_PER_ROW - 1) )
#define KEY_OFFSET_Y 150


typedef struct { 
    const char *text;
    const uint8_t *image;
    void (*callback)(epdgui_args_vector_t &);
} Setting;

#endif