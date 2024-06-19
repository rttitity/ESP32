#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MSG_BUFFER_SIZE  50

const char* ssid = "B02";
const char* password = "sh123456@";
const char* mqtt_server = "192.168.0.12";
//broker.hivemq.com - mqtt 브로커 서비스

const char* outTopic = "201708072/LED";
const char* inTopic = "201708072/LED";
const char* clientName;

int LED_PIN = D4;       //GPIO2
int BUTTON_PIN = 0;     //D3
int isPUSH = 0;
int LED_State = 0;      //LED상태

String sChipID;
char cChipID[20];

WiFiClient espClient;
PubSubClient client(espClient);
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  /*
  IPAddress ip(10, 74, 128, 99);
  IPAddress gateway(10, 74, 0, 1);
  IPAddress subnet(255, 255, 0, 0);
  IPAddress myDns(203, 246, 96, 254);
  WiFi.config(ip, myDns, gateway, subnet);
  */

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientName)) {
      Serial.println("connected");
      client.publish(outTopic, "Reconnected");
      client.subscribe(inTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();

  if ((char)payload[0] == '0') {
    Serial.println("LOW");      digitalWrite(D4, LOW);
  }
  if ((char)payload[0] == '1') {
    Serial.println("HIGH");     digitalWrite(D4, HIGH);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(115200);
  while(!Serial);
  Serial.println();
  setup_wifi();

  sChipID = String(ESP.getChipId(), HEX);             //ESP8266 칩번호 확인
  sChipID.toCharArray(cChipID, sChipID.length()+1);
  clientName = &cChipID[0];
  Serial.println(clientName);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect(); 
  client.loop();

  if((digitalRead(BUTTON_PIN) == LOW) && (!isPUSH)) {
    Serial.println("Switch is pushed");
    if(LED_State == 0) {
      client.publish(outTopic,"1");
      LED_State = 1;
    }
    else if(LED_State == 1) {
      client.publish(outTopic,"0");
      LED_State = 0;
    }
    isPUSH = 1;
  }
  else if((digitalRead(BUTTON_PIN)==HIGH) && isPUSH) {
    Serial.println("Switch is not pushed");

    isPUSH = 0;
  }
  delay(10);
}
