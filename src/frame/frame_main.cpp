#include "frame_main.h"
#include "frame_setting.h"
#include "frame_keyboard.h"
#include "frame_wifiscan.h"
#include "frame_home.h"
#include "frame_main_settings.h"
#include "WiFi.h"

void cb_settings(epdgui_args_vector_t &args);
void cb_keyboard(epdgui_args_vector_t &args);
void cb_wifi(epdgui_args_vector_t &args);
void cb_home(epdgui_args_vector_t &args);

Setting settings[] = {
    { .text = "Settings", .image = ImageResource_main_icon_setting_92x92,   .callback = cb_settings },
    { .text = "Keyboard", .image = ImageResource_main_icon_keyboard_92x92,  .callback = cb_keyboard },
    { .text = "WiFi",     .image = ImageResource_main_icon_wifi_92x92,      .callback = cb_wifi  },
    { .text = "Home",     .image = ImageResource_main_icon_home_92x92,      .callback = cb_home },
    // { .id = kKeySDFile,     .name = "SD",       .image = ImageResource_main_icon_sdcard_92x92, .callback = cb_sdfile },
};

// void cb_sdfile(epdgui_args_vector_t &args)
// {
//     Frame_Base *frame = new Frame_FileIndex("/");
//     EPDGUI_PushFrame(frame);
//     *((int*)(args[0])) = 0;
// }

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

int getSettingCount() {
    return sizeof(settings)/sizeof(Setting);
}

Frame_Main::Frame_Main(void): Frame_Base(false)
{
    _frame_name = "Frame_Main";
    _frame_id = 1;

    _bar = new M5EPD_Canvas(&M5.EPD);
    _bar->createCanvas(540, 44);
    _bar->setTextSize(26);

    for(int i = 0; i < getSettingCount(); i++) {
        int x=i % KEYS_PER_ROW;
        int y=i / KEYS_PER_ROW;

        EPDGUI_Button *btn = new EPDGUI_Button(MARGIN_LEFT + x * KEY_OFFSET_X, MARGIN_TOP+ y * KEY_OFFSET_Y, KEY_W, KEY_H, settings[i].image, settings[i].text, 20);
        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
        btn->Bind(EPDGUI_Button::EVENT_RELEASED, settings[i].callback);
        _key.push_back(btn);
    }
    
    _time = 0;
    _next_update_time = 0;
}

Frame_Main::~Frame_Main(void)
{
    for(int i = 0; i < _key.size(); i++) {
        delete _key[i];
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

    // String ssid = WiFi.SSID(0);
    // int32_t rssi = WiFi.RSSI(0);
    
    // _bar->drawString(ssid+" "+String(rssi), 10, 27);
    _bar->drawString(GetWifiSSID(), 10, 27);

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
    for(int i = 0; i < _key.size(); i++)
    {
        EPDGUI_AddObject(_key[i]);
    }
    _time = 0;
    _next_update_time = 0;
    StatusBar(UPDATE_MODE_NONE);
    return 9;
}

int Frame_Main::run()
{
    StatusBar(UPDATE_MODE_GL16);
    return 1;
}