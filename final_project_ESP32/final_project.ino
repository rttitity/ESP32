#include <WiFi.h>
#include <PubSubClient.h>

// 와이파이 SSID/Password
const char* ssid = "U+NetB2E8";
const char* password = "09J2D9#7A9";

// MQTT발행서버 주소
const char* mqtt_server = "192.168.219.105";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

// Variables to simulate temperature, humidity, and light level
float temperature = 25.0;
float humidity = 50.0;
float lightLevel = 500.0;
float tempChange = 0.5;
float humChange = 0.5;
float lightChange = 20.0;

// Direction of changes
bool tempIncrease = true;
bool humIncrease = true;
bool lightIncrease = true;

// LED Pin
const int ledPin = 4;

void setup() {
  Serial.begin(115200);

  // WiFi 설정
  setup_wifi();

  // MQTT 서버 설정
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // 와이파이 연결 시작
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // 설정한 와이파이 SSID, 비밀번호 일치하는지 확인
  WiFi.begin(ssid, password);

  // 연결될 때까지 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 연결 성공 시 while문 빠져나옴
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// callback 함수, MQTT 발행자로부터 온 메시지를 수신 받는다.
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

  // node-red에서 MQTT 발행자를 구독함, LED 상태를 MQTT로부터 수신받는다.
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    } else if (messageTemp == "off") {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

// MQTT 연결 확인 및 재연결 시도
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 고유한 클라이언트 ID 설정
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

// 온습도 및 조도 값을 loop하여 MQTT에 정보 전달
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();

  // 온습도 및 조도 값을 서서히 변화시킴
  if (now - lastMsg > 300) {
    lastMsg = now;

    // Simulate gradual change in temperature
    if (tempIncrease) {
      temperature += tempChange;
      if (temperature > 30.0) tempIncrease = false;
    } else {
      temperature -= tempChange;
      if (temperature < 0.0) tempIncrease = true;
    }

    // Simulate gradual change in humidity
    if (humIncrease) {
      humidity += humChange;
      if (humidity > 60.0) humIncrease = false;
    } else {
      humidity -= humChange;
      if (humidity < 40.0) humIncrease = true;
    }

    // Simulate gradual change in light level
    if (lightIncrease) {
      lightLevel += lightChange;
      if (lightLevel > 800) lightIncrease = false;
    } else {
      lightLevel -= lightChange;
      if (lightLevel < 200) lightIncrease = true;
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

    // Convert the light level value to a char array
    char lightString[8];
    dtostrf(lightLevel, 1, 0, lightString); // No decimal places needed for light level
    Serial.print("Light Level: ");
    Serial.println(lightString);
    client.publish("esp32/light", lightString);

    Serial.println("-------------------------");

  }
}
