#include "frame_main.h"
#include "frame_setting.h"
#include "frame_keyboard.h"
#include "frame_wifiscan.h"
#include "frame_fileindex.h"
#include "frame_home.h"

typedef struct { 
    int id;
    const char *name;
    const uint8_t *image;
    void (*callback)(epdgui_args_vector_t &);
} Setting;



#define KEYS_PER_ROW 4
#define KEY_W 92
#define KEY_H 92

// template<class T, char const  *name>
// void onClick( epdgui_args_vector_t &args ) 
// {
//     Frame_Base *frame = EPDGUI_GetFrame(name);
//     if(frame == NULL)
//     {
//         frame = new T();
//         EPDGUI_AddFrame(name, frame);
//     }
//     EPDGUI_PushFrame(frame);
//     *((int*)(args[0])) = 0;    
// }

void cb_sdfile(epdgui_args_vector_t &args)
{
    Frame_Base *frame = new Frame_FileIndex("/");
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;
}

void cb_settings(epdgui_args_vector_t &args) {
    Frame_Base *frame = EPDGUI_GetFrame("settings");
    if(frame == NULL)
    {
        frame = new Frame_Setting();
        EPDGUI_AddFrame("settings", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;    
}

void cb_keyboard(epdgui_args_vector_t &args) {
    Frame_Base *frame = EPDGUI_GetFrame("key");
    if(frame == NULL)
    {
        frame = new Frame_Keyboard();
        EPDGUI_AddFrame("key", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;  
}

void cb_wifi(epdgui_args_vector_t &args) {
    Frame_Base *frame = EPDGUI_GetFrame("wifi");
    if(frame == NULL)
    {
        frame = new Frame_WifiScan();
        EPDGUI_AddFrame("wifi", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;  
}

void cb_home(epdgui_args_vector_t &args) {
    Frame_Base *frame = EPDGUI_GetFrame("home");
    if(frame == NULL)
    {
        frame = new Frame_Home();
        EPDGUI_AddFrame("home", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;  
}

Setting settings[kKeyCOUNT] = {
    { .id = kKeySetting,    .name = "Settings", .image = ImageResource_main_icon_setting_92x92, .callback = cb_settings },
    { .id = kKeyKeyboard,   .name = "Keyboard", .image = ImageResource_main_icon_setting_92x92, .callback = cb_keyboard },
    { .id = kKeyWifiScan,   .name = "WiFi",     .image = ImageResource_main_icon_setting_92x92, .callback = cb_wifi  },
    { .id = kKeyHome,       .name = "Home",     .image = ImageResource_main_icon_setting_92x92, .callback = cb_home },
    { .id = kKeySDFile,     .name = "SD",       .image = ImageResource_main_icon_setting_92x92, .callback = cb_sdfile },
};



Frame_Main::Frame_Main(void): Frame_Base(false)
{
    _frame_name = "Frame_Main";
    _frame_id = 1;

    _bar = new M5EPD_Canvas(&M5.EPD);
    _bar->createCanvas(540, 44);
    _bar->setTextSize(26);

    _names = new M5EPD_Canvas(&M5.EPD);
    _names->createCanvas(540, 32);
    _names->setTextDatum(CC_DATUM);
    
    for(int i = 0; i < kKeyCOUNT; i++) {
        int y=i / KEYS_PER_ROW;
        int x=i % KEYS_PER_ROW;
        _key[i] = new EPDGUI_Button("x", 20 + x * 136, 90+ y * 150, KEY_W, KEY_H);
         
    //     _key[i]->CanvasNormal()->pushImage(0, 0, KEY_W, KEY_H, settings[i].image);
    //     *(_key[i]->CanvasPressed()) = *(_key[i]->CanvasNormal());
    //     _key[i]->CanvasPressed()->ReverseColor();
    //     _key[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    //     _key[i]->Bind(EPDGUI_Button::EVENT_RELEASED, settings[i].callback);
    }
    
    Frame_AddKey(kKeySetting, ImageResource_main_icon_setting_92x92, cb_settings );
    Frame_AddKey(kKeyKeyboard, ImageResource_main_icon_keyboard_92x92, cb_keyboard );
    Frame_AddKey(kKeyWifiScan, ImageResource_main_icon_wifi_92x92, cb_wifi );
    Frame_AddKey(kKeySDFile, ImageResource_main_icon_sdcard_92x92, cb_sdfile );
    Frame_AddKey(kKeyHome, ImageResource_main_icon_home_92x92, cb_home );

    _time = 0;
    _next_update_time = 0;
}

void Frame_Main::Frame_AddKey(uint16_t idx, const uint8_t *imagePtr, void (*func_cb)(epdgui_args_vector_t &) ) 
{
    _key[idx]->CanvasNormal()->pushImage(0, 0, KEY_W, KEY_H, imagePtr);
    *(_key[idx]->CanvasPressed()) = *(_key[idx]->CanvasNormal());
    _key[idx]->CanvasPressed()->ReverseColor();
    _key[idx]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key[idx]->Bind(EPDGUI_Button::EVENT_RELEASED, func_cb);
}

Frame_Main::~Frame_Main(void)
{
    for(int i = 0; i < kKeyCOUNT; i++) {
        delete _key[i];
    }
}

void Frame_Main::AppName(m5epd_update_mode_t mode)
{
    if(!_names->isRenderExist(20))
    {
        _names->createRender(20, 26);
    }
    _names->setTextSize(20);
    _names->fillCanvas(0);


    for (int i=0; i< kKeyCOUNT; i++) {
        int col = i % KEYS_PER_ROW;
        int row = i / KEYS_PER_ROW;
        _names->drawString(GetMainFrameKeyName(i), 20 + 46 + 136 * col, 16);    
        if (col == KEYS_PER_ROW-1 || i==(kKeyCOUNT-1)) {
            _names->pushCanvas(0, 186 + row * 151, mode);
            _names->fillCanvas(0);

        }
    }

}

void Frame_Main::StatusBar(m5epd_update_mode_t mode)
{
    if((millis() - _time) < _next_update_time)
    {
        return;
    }
    char buf[20];
    _bar->fillCanvas(0);
    _bar->drawFastHLine(0, 43, 540, 15);
    _bar->setTextDatum(CL_DATUM);
    _bar->drawString("M5Paper", 10, 27);

    // Battery
    _bar->setTextDatum(CR_DATUM);
    _bar->pushImage(498, 8, 32, 32, ImageResource_status_bar_battery_32x32);
    uint32_t vol = M5.getBatteryVoltage();

    if(vol < 3300)
    {
        vol = 3300;
    }
    else if(vol > 4350)
    {
        vol = 4350;
    }
    float battery = (float)(vol - 3300) / (float)(4350 - 3300);
    if(battery <= 0.01)
    {
        battery = 0.01;
    }
    if(battery > 1)
    {
        battery = 1;
    }
    uint8_t px = battery * 25;
    sprintf(buf, "%d%%", (int)(battery * 100));
    _bar->drawString(buf, 498 - 10, 27);
    _bar->fillRect(498 + 3, 8 + 10, px, 13, 15);
    // _bar->pushImage(498, 8, 32, 32, 2, ImageResource_status_bar_battery_charging_32x32);

    // Time
    rtc_time_t time_struct;
    rtc_date_t date_struct;
    M5.RTC.getTime(&time_struct);
    M5.RTC.getDate(&date_struct);
    sprintf(buf, "%2d:%02d", time_struct.hour, time_struct.min);
    _bar->setTextDatum(CC_DATUM);
    _bar->drawString(buf, 270, 27);
    _bar->pushCanvas(0, 0, mode);

    _time = millis();
    _next_update_time = (60 - time_struct.sec) * 1000;
}


int Frame_Main::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    for(int i = 0; i < kKeyCOUNT; i++)
    {
        EPDGUI_AddObject(_key[i]);
    }
    _time = 0;
    _next_update_time = 0;
    StatusBar(UPDATE_MODE_NONE);
    AppName(UPDATE_MODE_NONE);
    return 9;
}

int Frame_Main::run()
{
    StatusBar(UPDATE_MODE_GL16);
    return 1;
}