#ifndef _FRAME_HOME_H_
#define _FRAME_HOME_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"
#include <vector>

// #define SHOW_AIR
// #define SHOW_SOCKET
class Frame_Home : public Frame_Base
{
public:
    Frame_Home();
    ~Frame_Home();
    int init(epdgui_args_vector_t &args);
    void InitSwitch(EPDGUI_Switch* sw, String title, String subtitle, const uint8_t *img1, const uint8_t *img2);
    void addSwitch(String uid, uint8_t col, uint8_t row, String caption, String subcaption, const uint8_t* img1, const uint8_t* img2);
private:
    std::vector<EPDGUI_Switch*> buttons;
    // std::vector<EPDGUI_Switch*> lights;
    // std::vector<EPDGUI_Switch*> sockets;
    // std::vector<EPDGUI_Switch*> climates;
    EPDGUI_Switch *_sw_light1;
    EPDGUI_Switch *_sw_light2;
#ifdef SHOW_SOCKET    
    EPDGUI_Switch *_sw_socket1;
    EPDGUI_Switch *_sw_socket2;
#endif    
#ifdef SHOW_AIR
    EPDGUI_Switch *_sw_air_1;
    EPDGUI_Switch *_sw_air_2;
    EPDGUI_Button *_key_air_1_plus;
    EPDGUI_Button *_key_air_1_minus;
    EPDGUI_Button *_key_air_2_plus;
    EPDGUI_Button *_key_air_2_minus;
#endif
};

#endif //_FRAME_HOME_H_