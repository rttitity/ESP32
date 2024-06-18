#include <WiFi.h>
#include <PubSubClient.h>

// 와이파이 SSID/Password
const char* ssid = "U+NetB2E8";
const char* password = "09J2D9#7A9";

// MQTT발행서버 주소
const char* mqtt_server = "192.168.219.150";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Variables to simulate temperature and humidity
float temperature = 25.0;
float humidity = 50.0;
float tempChange = 0.1;
float humChange = 0.1;

// LED Pin
const int ledPin = 4;

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}
/*
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("zinucha_Client")) {              //고유한 클라이언트 ID 설정 필요..
      Serial.println("connected");
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
*/

//MQTT 연결 확인
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 고유한 클라이언트 ID를 설정
    if (client.connect("ESP8266Client12345")) {
      Serial.println("connected");
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == MQTT_CONNECT_FAILED) {
        Serial.println(" (MQTT_CONNECT_FAILED)");
      } else if (client.state() == MQTT_CONNECTION_TIMEOUT) {
        Serial.println(" (MQTT_CONNECTION_TIMEOUT)");
      } else if (client.state() == MQTT_CONNECTION_LOST) {
        Serial.println(" (MQTT_CONNECTION_LOST)");
      } else if (client.state() == MQTT_DISCONNECTED) {
        Serial.println(" (MQTT_DISCONNECTED)");
      } else if (client.state() == MQTT_CONNECT_BAD_PROTOCOL) {
        Serial.println(" (MQTT_CONNECT_BAD_PROTOCOL)");
      } else if (client.state() == MQTT_CONNECT_BAD_CLIENT_ID) {
        Serial.println(" (MQTT_CONNECT_BAD_CLIENT_ID)");
      } else if (client.state() == MQTT_CONNECT_UNAVAILABLE) {
        Serial.println(" (MQTT_CONNECT_UNAVAILABLE)");
      } else if (client.state() == MQTT_CONNECT_BAD_CREDENTIALS) {
        Serial.println(" (MQTT_CONNECT_BAD_CREDENTIALS)");
      } else if (client.state() == MQTT_CONNECT_UNAUTHORIZED) {
        Serial.println(" (MQTT_CONNECT_UNAUTHORIZED)");
      } else {
        Serial.println(" (Unknown Error)");
      }
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Simulate gradual change in temperature and humidity
    temperature += tempChange;
    if (temperature > 30.0 || temperature < 20.0) {
      tempChange = -tempChange;
    }

    humidity += humChange;
    if (humidity > 60.0 || humidity < 40.0) {
      humChange = -humChange;
    }

    // Convert the temperature value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);

    // Convert the humidity value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("esp32/humidity", humString);
  }
}
