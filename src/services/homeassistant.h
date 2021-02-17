#ifndef _HOMEASSISTANTMQTTCLIENT_H__
#define _HOMEASSISTANTMQTTCLIENT_H__
#include <AsyncMqttClient.hpp>
// #include <jsmn.h>


class HomeAssistantMqtt {
private:
	String _host;
	uint16_t _port;
	String _username;
	String _password;

	TimerHandle_t mqttReconnectTimer;
	AsyncMqttClient mqttClient;
	boolean reconnect=true;
	void setState(const char* topic, boolean state);

	void onMqttConnect(bool sessionPresent);
	void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
	std::string getToken(std::string s, std::string delimiter, int idx);
	// int dump(const char *js, jsmntok_t *t, size_t count, int indent);
	
public:
	HomeAssistantMqtt(String host, uint16_t port, String username, String password);
	~HomeAssistantMqtt();
	int connect();
	void disconnect();
	void connectToMqtt();
	void setLight(String deviceName, String lightName, boolean state);
	AsyncMqttClient* getMqtt();
};

// extern HomeAssistantMqtt hassio;

#endif
