#include <WiFi.h>
#include <PubSubClient.h>

// 와이파이 SSID/Password
const char* ssid = "U+NetB2E8";
const char* password = "09J2D9#7A9";

// MQTT발행서버 주소 = localhost 주소
const char* mqtt_server = "192.168.219.105";

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

//wifi setup
  setup_wifi();

  // mqtt 서버 기본 포트 1883
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network 와이파이 연결 시작
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

// 설정한 와이파이 SSID, passwd 일치하는지 확인
  WiFi.begin(ssid, password);

//연결될 때 까지 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

// 연결 성공시 while문 빠져나옴
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// callback 함수, mqtt발행자로 부터 온 메세지를 수신 받는다.
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

// node-red에서 mqtt 발행자를 구독함, LED 상태를 mqtt로부터 수신받는다.
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


//MQTT 연결 확인
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 고유한 클라이언트 ID를 설정
    if (client.connect("ESP8266Client12345")) {
      Serial.println("connected");
      client.subscribe("esp32/output");

      // MQTT 연결 실패시, rc값에 따라 다른 오류 상황
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == MQTT_CONNECT_FAILED) {
        Serial.println(" (MQTT_CONNECT_FAILED)");
      } 
      
      else if (client.state() == MQTT_CONNECTION_TIMEOUT) {
        Serial.println(" (MQTT_CONNECTION_TIMEOUT)");
      } 
      
      else if (client.state() == MQTT_CONNECTION_LOST) {
        Serial.println(" (MQTT_CONNECTION_LOST)");
      } 
      
      else if (client.state() == MQTT_DISCONNECTED) {
        Serial.println(" (MQTT_DISCONNECTED)");
      } 
      
      else if (client.state() == MQTT_CONNECT_BAD_PROTOCOL) {
        Serial.println(" (MQTT_CONNECT_BAD_PROTOCOL)");
      } 
      
      else if (client.state() == MQTT_CONNECT_BAD_CLIENT_ID) {
        Serial.println(" (MQTT_CONNECT_BAD_CLIENT_ID)");
      } 
      
      else if (client.state() == MQTT_CONNECT_UNAVAILABLE) {
        Serial.println(" (MQTT_CONNECT_UNAVAILABLE)");
      } 
      
      else if (client.state() == MQTT_CONNECT_BAD_CREDENTIALS) {
        Serial.println(" (MQTT_CONNECT_BAD_CREDENTIALS)");
      } 
      
      else if (client.state() == MQTT_CONNECT_UNAUTHORIZED) {
        Serial.println(" (MQTT_CONNECT_UNAUTHORIZED)");
      } 
      
      else {
        Serial.println(" (Unknown Error)");
      }
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// 온습도 값을 loop하여 mqtt에 정보 전달
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();

  // 온습도 값을 서서히 늘이거나 줄임
  if (now - lastMsg > 2000) {
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
