#ifndef _FRAME_MAIN_H_
#define _FRAME_MAIN_H_

#include "frame_base.h"


enum
{
    kKeySetting = 0,
    kKeyKeyboard,
    kKeyWifiScan,
    kKeySDFile,
    kKeyHome,
    kKeyCOUNT,
};
class Frame_Main : public Frame_Base
{
public:
    Frame_Main();
    ~Frame_Main();
    int run();
    int init(epdgui_args_vector_t &args);
    void StatusBar(m5epd_update_mode_t mode);
    void AppName(m5epd_update_mode_t mode);

private:
    EPDGUI_Button *_key[kKeyCOUNT];
    M5EPD_Canvas *_bar;
    M5EPD_Canvas *_names;
    uint32_t _next_update_time;
    uint32_t _time;
    void Frame_AddKey(uint16_t idx, const uint8_t *imagePtr, void (*func_cb)(epdgui_args_vector_t &) );
};

#endif //_FRAME_MAIN_H_