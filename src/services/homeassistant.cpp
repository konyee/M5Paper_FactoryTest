#include "homeassistant.h"
#include <jsmn.h>
#include <WiFi.h>


// HomeAssistantMqtt hassio;

HomeAssistantMqtt::HomeAssistantMqtt(String host, uint16_t port, String username, String password) 
{
	_host=host;
	_port=port;
	_username=username;
	_password=password;

	mqttClient.onConnect([this](bool sessionPresent) { onMqttConnect(sessionPresent); });
	mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) { onMqttDisconnect(reason);});
	mqttClient.onMessage([this](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) { onMqttMessage(topic,payload,properties,len,index,total);});
}

HomeAssistantMqtt::~HomeAssistantMqtt()
{
	mqttClient.disconnect();
}

int HomeAssistantMqtt::connect()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("wifi not connected");
		return 1;
	}

	mqttClient.setServer(_host.c_str(), _port);
	mqttClient.setCredentials(_username.c_str(), _password.c_str());
	// mqttClient.setClientId("test1");

	// mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connect));
	// mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, [this]() { connect(); } );
	Serial.println("Connecting to MQTT...");
	mqttClient.connect();
	return 0;
}

void HomeAssistantMqtt::disconnect()
{
	mqttClient.disconnect();
}

std::string HomeAssistantMqtt::getToken(std::string s, std::string delimiter, int idx)
{
	size_t pos = 0;
	size_t cnt = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		if (cnt == idx)
		{
			return token;
		}
		s.erase(0, pos + delimiter.length());
		cnt++;
	}
	return s;
}

int dump(const char *js, jsmntok_t *t, size_t count, int indent) 
{
  int i, j, k;
  jsmntok_t *key;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    Serial.printf("%.*s", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_STRING) {
    Serial.printf("'%.*s'", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_OBJECT) {
    Serial.printf("\n");
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
        Serial.printf("  ");
      }
      key = t + 1 + j;
      j += dump(js, key, count - j, indent + 1);
      if (key->size > 0) {
        Serial.printf(": ");
        j += dump(js, t + 1 + j, count - j, indent + 1);
      }
      Serial.printf("\n");
    }
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    Serial.printf("\n");
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
        Serial.printf("  ");
      }
      Serial.printf("   - ");
      j += dump(js, t + 1 + j, count - j, indent + 1);
      Serial.printf("\n");
    }
    return j + 1;
  }
  return 0;
}

void HomeAssistantMqtt::onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
	// const char *top = getToken(topic, "/", 2).c_str();
	const char *x= topic;
	Serial.println(x);

	jsmn_parser p;
	jsmntok_t t[128];
	jsmn_init(&p);
	jsmn_parse(&p, payload, strlen(payload), t, 128);
	dump(payload,(jsmntok_t*)t, p.toknext, 0);

	//   Serial.println(properties.qos);
	//   Serial.println(properties.dup);
	//   Serial.println(properties.retain);
	//   Serial.println(len);
	//   Serial.println(index);
	//   Serial.println(total);
}

void HomeAssistantMqtt::onMqttConnect(bool sessionPresent)
{
	Serial.println("Connected to MQTT.");
	mqttClient.subscribe("homeassistant/light/+/+/config", 0);
// 	uint16_t packetIdPub1 = mqttClient.publish("homeassistant/switch/m5paper/config", 2, true, config);
}

void HomeAssistantMqtt::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
	Serial.println("Disconnected from MQTT.");
	if (reconnect) {
		if (WiFi.isConnected())
		{
			xTimerStart(mqttReconnectTimer, 0);
		}
	}
}
void HomeAssistantMqtt::setLight(String deviceName, String lightName, boolean state)
{
	String topic = deviceName+"/light/"+lightName+"/command";
	setState(topic.c_str(), state);
}

void HomeAssistantMqtt::setState(const char* topic, boolean state)
{
	char payload[20]={0};
	sprintf(payload, "{\"state\":\"%s\"}", state? "ON":"OFF");
	mqttClient.publish(topic, 0, false, (const char *)payload);
}

AsyncMqttClient* HomeAssistantMqtt::getMqtt() {
	return &mqttClient;
}