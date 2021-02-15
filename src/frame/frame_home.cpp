#include "frame_home.h"
#include "WiFi.h"

#include <AsyncMqttClient.h>

TimerHandle_t mqttReconnectTimer;
AsyncMqttClient mqttClient;

std::string getToken(std::string s, std::string delimiter, int idx) 
{
    size_t pos = 0;
    size_t cnt=0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        if (cnt == idx) {
            return token;
        }
        // std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
        cnt++;
    }
    // std::cout << s << std::endl;
    return s;
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  // Serial.println("Publish received.");
//   Serial.print("  topic: ");
  Serial.println(topic);
  const char* x = getToken(topic,"/",2).c_str();
  Serial.println(x);
  
//   Serial.print("  qos: ");
//   Serial.println(properties.qos);
//   Serial.print("  dup: ");
//   Serial.println(properties.dup);
//   Serial.print("  retain: ");
//   Serial.println(properties.retain);
//   Serial.print("  len: ");
//   Serial.println(len);
//   Serial.print("  index: ");
//   Serial.println(index);
//   Serial.print("  total: ");
//   Serial.println(total);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("homeassistant/light/+/+/config", 0);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
//   mqttClient.publish("test/lol", 0, true, "test 1");
//   Serial.println("Publishing at QoS 0");
  const char* config ="";

  uint16_t packetIdPub1 = mqttClient.publish("homeassistant/switch/m5paper/config", 2, true, config);
  Serial.print("Publishing at QoS 1, packetId: ");
  Serial.println(packetIdPub1);
//   uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
//   Serial.print("Publishing at QoS 2, packetId: ");
//   Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void Frame_Home::InitSwitch(EPDGUI_Switch* sw, String title, String subtitle, const uint8_t *img1, const uint8_t *img2)
{
    memcpy(sw->Canvas(0)->frameBuffer(), ImageResource_home_button_background_228x228, 228 * 228 / 2);
    sw->Canvas(0)->setTextSize(36);
    sw->Canvas(0)->setTextDatum(TC_DATUM);
    sw->Canvas(0)->drawString(title, 114, 136);
    sw->Canvas(0)->setTextSize(26);
    sw->Canvas(0)->drawString(subtitle, 114, 183);
    memcpy(sw->Canvas(1)->frameBuffer(), sw->Canvas(0)->frameBuffer(), 228 * 228 / 2);
    sw->Canvas(0)->pushImage(68, 20, 92, 92, img1);
    sw->Canvas(1)->pushImage(68, 20, 92, 92, img2);
}

#ifdef SHOW_AIR

void key_home_air_adjust_cb(epdgui_args_vector_t &args)
{
    int operation = ((EPDGUI_Button*)(args[0]))->GetCustomString().toInt();
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[1]));
    if(sw->getState() == 0)
    {
        return;
    }
    int temp = sw->GetCustomString().toInt();
    char buf[10];
    if(operation == 1)
    {
        temp++;
    }
    else
    {
        temp--;
    }
    sprintf(buf, "%d", temp);
    sw->SetCustomString(buf);
    sprintf(buf, "%d℃", temp);
    sw->Canvas(1)->setTextSize(36);
    sw->Canvas(1)->setTextDatum(TC_DATUM);
    sw->Canvas(1)->fillRect(114 - 100, 108, 200, 38, 0);
    sw->Canvas(1)->drawString(buf, 114, 108);
    sw->Canvas(1)->pushCanvas(sw->getX(), sw->getY(), UPDATE_MODE_A2);
}

void key_home_air_state0_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *b1 = ((EPDGUI_Button*)(args[0]));
    EPDGUI_Button *b2 = ((EPDGUI_Button*)(args[1]));
    b1->SetEnable(false);
    b2->SetEnable(false);
}

void key_home_air_state1_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *b1 = ((EPDGUI_Button*)(args[0]));
    EPDGUI_Button *b2 = ((EPDGUI_Button*)(args[1]));
    b1->SetEnable(true);
    b2->SetEnable(true);
}
#endif

void sw_light_cb(epdgui_args_vector_t &args) 
{
    EPDGUI_Switch *sw = ((EPDGUI_Switch*)(args[0]));
    String topic = "m5paper/" + sw->GetUID() + "/set";
    char payload[10];
    if (sw->getState() == 1)
        sprintf(payload, "ON");
    else
        sprintf(payload, "OFF");
    
    mqttClient.publish(topic.c_str(), 0, false, (const char*)payload );
}


Frame_Home::Frame_Home(void)
{
    _frame_name = "Frame_Home";

    
    _sw_light1       = new EPDGUI_Switch(2, 20, 44 + 72, 228, 228);
    _sw_light1->SetUID("light1");
    _sw_light2       = new EPDGUI_Switch(2, 288, 44 + 72, 228, 228);
    _sw_light2->SetUID("light2");

#ifdef SHOW_SOCKET    
    _sw_socket1      = new EPDGUI_Switch(2, 20, 324 + 72, 228, 228);
    _sw_socket2      = new EPDGUI_Switch(2, 288, 324 + 72, 228, 228);
#endif

#ifdef SHOW_AIR
    _sw_air_1        = new EPDGUI_Switch(2, 20, 604 + 72, 228, 184);
    _key_air_1_plus  = new EPDGUI_Button(20 + 116, 604 + 72 + 184, 112, 44);
    _key_air_1_minus = new EPDGUI_Button(20, 604 + 72 + 184, 116, 44);
    _key_air_1_plus ->SetCustomString("1");
    _key_air_1_minus->SetCustomString("0");
    _key_air_1_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_1_plus);
    _key_air_1_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_1);
    _key_air_1_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_1_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_1_minus);
    _key_air_1_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_1);
    _key_air_1_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);

    _sw_air_2        = new EPDGUI_Switch(2, 288, 604 + 72, 228, 184);
    _key_air_2_plus  = new EPDGUI_Button(288 + 116, 604 + 72 + 184, 112, 44);
    _key_air_2_minus = new EPDGUI_Button(288, 604 + 72 + 184, 116, 44);
    _key_air_2_plus ->SetCustomString("1");
    _key_air_2_minus->SetCustomString("0");
    _key_air_2_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_2_plus);
    _key_air_2_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_2);
    _key_air_2_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
    _key_air_2_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_air_2_minus);
    _key_air_2_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _sw_air_2);
    _key_air_2_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_home_air_adjust_cb);
#endif
    M5EPD_Canvas canvas_temp(&M5.EPD);
    canvas_temp.createRender(36);



    InitSwitch(_sw_light1, "Ceiling Light", "Living Room", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
    InitSwitch(_sw_light2, "Table Lamp", "Bedroom", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
#ifdef SHOW_SOCKET    
    InitSwitch(_sw_socket1, "Rice Cooker", "Kitchen", ImageResource_home_icon_socket_off_92x92, ImageResource_home_icon_socket_on_92x92);
    InitSwitch(_sw_socket2, "Computer", "Bedroom", ImageResource_home_icon_socket_off_92x92, ImageResource_home_icon_socket_on_92x92);
#endif    
  
    _sw_light1->AddArgs(0, 0, _sw_light1);
    _sw_light1->Bind(0, sw_light_cb);

    _sw_light1->AddArgs(1, 0, _sw_light1);
    _sw_light1->Bind(1, sw_light_cb);

    _sw_light2->AddArgs(0, 0, _sw_light2);
    _sw_light2->Bind(0, sw_light_cb);
    _sw_light2->AddArgs(1, 0, _sw_light2);
    _sw_light2->Bind(1, sw_light_cb);
#ifdef SHOW_AIR
    memcpy(_sw_air_1->Canvas(0)->frameBuffer(), ImageResource_home_air_background_228x184, 228 * 184 / 2);
    _sw_air_1->Canvas(0)->setTextDatum(TC_DATUM);
    _sw_air_1->Canvas(0)->setTextSize(26);
    _sw_air_1->Canvas(0)->drawString("Bedroom", 114, 152);
    memcpy(_sw_air_1->Canvas(1)->frameBuffer(), _sw_air_1->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_1->Canvas(0)->setTextSize(36);
    _sw_air_1->Canvas(0)->drawString("OFF", 114, 108);
    _sw_air_1->Canvas(1)->setTextSize(36);
    _sw_air_1->Canvas(1)->setTextDatum(TC_DATUM);
    _sw_air_1->Canvas(1)->drawString("26℃", 114, 108);
    _sw_air_1->SetCustomString("26");

    memcpy(_sw_air_2->Canvas(0)->frameBuffer(), ImageResource_home_air_background_228x184, 228 * 184 / 2);
    _sw_air_2->Canvas(0)->setTextDatum(TC_DATUM);
    _sw_air_2->Canvas(0)->setTextSize(26);
    _sw_air_2->Canvas(0)->drawString("Living Room", 114, 152);
    memcpy(_sw_air_2->Canvas(1)->frameBuffer(), _sw_air_2->Canvas(0)->frameBuffer(), 228 * 184 / 2);
    _sw_air_2->Canvas(0)->setTextSize(36);
    _sw_air_2->Canvas(0)->drawString("OFF", 114, 108);
    _sw_air_2->Canvas(1)->setTextSize(36);
    _sw_air_2->Canvas(1)->setTextDatum(TC_DATUM);
    _sw_air_2->Canvas(1)->drawString("26℃", 114, 108);
    _sw_air_2->SetCustomString("26");

    memcpy(_key_air_1_plus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_r_112x44, 112 * 44 / 2);
    memcpy(_key_air_1_plus->CanvasPressed()->frameBuffer(), _key_air_1_plus->CanvasNormal()->frameBuffer(), 112 * 44 / 2);
    _key_air_1_plus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_2_plus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_r_112x44, 112 * 44 / 2);
    memcpy(_key_air_2_plus->CanvasPressed()->frameBuffer(), _key_air_2_plus->CanvasNormal()->frameBuffer(), 112 * 44 / 2);
    _key_air_2_plus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_1_minus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_l_116x44, 116 * 44 / 2);
    memcpy(_key_air_1_minus->CanvasPressed()->frameBuffer(), _key_air_1_minus->CanvasNormal()->frameBuffer(), 116 * 44 / 2);
    _key_air_1_minus->CanvasPressed()->ReverseColor();
    memcpy(_key_air_2_minus->CanvasNormal()->frameBuffer(), ImageResource_home_air_background_l_116x44, 116 * 44 / 2);
    memcpy(_key_air_2_minus->CanvasPressed()->frameBuffer(), _key_air_2_minus->CanvasNormal()->frameBuffer(), 116 * 44 / 2);
    _key_air_2_minus->CanvasPressed()->ReverseColor();

    _key_air_1_plus->SetEnable(false);
    _key_air_2_plus->SetEnable(false);
    _key_air_1_minus->SetEnable(false);
    _key_air_2_minus->SetEnable(false);

    _sw_air_1->Canvas(0)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_off_92x92);
    _sw_air_1->Canvas(1)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);
    _sw_air_2->Canvas(0)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_off_92x92);
    _sw_air_2->Canvas(1)->pushImage(68, 12, 92, 92, ImageResource_home_icon_conditioner_on_92x92);

    _sw_air_1->AddArgs(0, 0, _key_air_1_plus);
    _sw_air_1->AddArgs(0, 1, _key_air_1_minus);
    _sw_air_1->AddArgs(0, 2, _sw_air_1);
    _sw_air_1->Bind(0, key_home_air_state0_cb);
    _sw_air_1->AddArgs(1, 0, _key_air_1_plus);
    _sw_air_1->AddArgs(1, 1, _key_air_1_minus);
    _sw_air_1->AddArgs(1, 2, _sw_air_1);
    _sw_air_1->Bind(1, key_home_air_state1_cb);

    _sw_air_2->AddArgs(0, 0, _key_air_2_plus);
    _sw_air_2->AddArgs(0, 1, _key_air_2_minus);
    _sw_air_2->AddArgs(0, 2, _sw_air_2);
    _sw_air_2->Bind(0, key_home_air_state0_cb);
    _sw_air_2->AddArgs(1, 0, _key_air_2_plus);
    _sw_air_2->AddArgs(1, 1, _key_air_2_minus);
    _sw_air_2->AddArgs(1, 2, _sw_air_2);
    _sw_air_2->Bind(1, key_home_air_state1_cb);
#endif
    
    exitbtn("Home");
    _canvas_title->drawString("Control Panel", 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}


Frame_Home::~Frame_Home(void)
{
    delete _sw_light1;
    delete _sw_light2;
#ifdef SHOW_SOCKET    
    delete _sw_socket1;
    delete _sw_socket2;
#endif    
#ifdef SHOW_AIR    
    delete _sw_air_1;
    delete _sw_air_2;
    delete _key_air_1_plus;
    delete _key_air_1_minus;
    delete _key_air_2_plus;
    delete _key_air_2_minus;
#endif    
}

int Frame_Home::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_sw_light1);
    EPDGUI_AddObject(_sw_light2);
#ifdef SHOW_SOCKET    
    EPDGUI_AddObject(_sw_socket1);
    EPDGUI_AddObject(_sw_socket2);
#endif    
#ifdef SHOW_AIR    
    EPDGUI_AddObject(_sw_air_1);
    EPDGUI_AddObject(_sw_air_2);
    EPDGUI_AddObject(_key_air_1_plus);
    EPDGUI_AddObject(_key_air_1_minus);
    EPDGUI_AddObject(_key_air_2_plus);
    EPDGUI_AddObject(_key_air_2_minus);
#endif
    for(int i = 0; i < buttons.size(); i++)
    {
        EPDGUI_AddObject(buttons[i]);
    }

    EPDGUI_AddObject(_key_exit);

    if (WiFi.status() == WL_CONNECTED) {
        mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
        mqttClient.setServer("192.168.99.100",1883);
        mqttClient.setCredentials("module-user","module-pass");
        mqttClient.setClientId("test1");
        mqttClient.onConnect(onMqttConnect);
        mqttClient.onDisconnect(onMqttDisconnect);
        mqttClient.onSubscribe(onMqttSubscribe);
        mqttClient.onUnsubscribe(onMqttUnsubscribe);
        mqttClient.onMessage(onMqttMessage);
        mqttClient.onPublish(onMqttPublish);
        mqttClient.connect();
    }

    return 3;
}

void Frame_Home::addSwitch(String uid, uint8_t col, uint8_t row, String caption, String subcaption, const uint8_t* img1, const uint8_t* img2) {

    EPDGUI_Switch* btn = new EPDGUI_Switch(2, col?288:20, 44 + 72 + row*280, 228, 228);
    InitSwitch(btn, caption, subcaption, img1, img2);    
    btn->SetUID(uid);
    // EPDGUI_AddObject(btn);
    // _sw_light1       = new EPDGUI_Switch(2, 20, 44 + 72, 228, 228);
    // _sw_socket1      = new EPDGUI_Switch(2, 20, 324 + 72, 228, 228);
    // _sw_socket2      = new EPDGUI_Switch(2, 288, 324 + 72, 228, 228);
    // _sw_light1->SetUID("light1");
    // InitSwitch(_sw_light1, "Ceiling Light", "Living Room", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);
    // InitSwitch(_sw_light2, "Table Lamp", "Bedroom", ImageResource_home_icon_light_off_92x92, ImageResource_home_icon_light_on_92x92);

}

