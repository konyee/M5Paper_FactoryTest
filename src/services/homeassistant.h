#ifndef _HOMEASSISTANTMQTTCLIENT_H__
#define _HOMEASSISTANTMQTTCLIENT_H__

#include <AsyncMqttClient.hpp>

class HomeAssistantMqtt {
private:
	TimerHandle_t mqttReconnectTimer;
	AsyncMqttClient mqttClient;
	boolean reconnect=true;
	void setState(const char* topic, boolean state);

	void onMqttConnect(bool sessionPresent);
	void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
	
public:
	HomeAssistantMqtt();
	~HomeAssistantMqtt();
	int connect(const char *host, uint16_t port = 1883);
	void disconnect();
	void connectToMqtt();
	void setLight(String deviceName, String lightName, boolean state);
	AsyncMqttClient* getMqtt();
};

extern HomeAssistantMqtt hassio;

#endif
