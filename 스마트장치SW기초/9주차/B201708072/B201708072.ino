#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MSG_BUFFER_SIZE 50

const char* ssid = "chajinwoo";
const char* password = "20051677";
//const char* mqtt_server = "192.168.0.17";
const char* mqtt_server = "broker.hivemq.com";

const char* outTopic = "201708072/adcValue";
const char* inTopic = "201708072/adcValue";
const char* clientName;


unsigned long lastMsg = 0;
int LED_PIN = D4;         //GPIO2
int BUTTON_PIN = 0;       //D3
int isPUSH = 0;

String sChipID;
char cChipID[20];

WiFiClient espClient;
PubSubClient client(espClient);
char msg[MSG_BUFFER_SIZE];

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  //정적 IP 설정
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
//  randomSeed(micros());
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
      client.publish(outTopic, "reconnected");
      client.subscribe(inTopic);
      client.subscribe("201708072/LED");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  if ((char)payload[0] == '0') {
    Serial.println("LOW");    digitalWrite(LED_PIN, LOW);
  } 
  if ((char)payload[0] == '1') {
    Serial.println("HIGH");    digitalWrite(LED_PIN, HIGH);
  }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(115200);
  while(!Serial);
  Serial.println();
  setup_wifi();
  
  sChipID = String(ESP.getChipId(), HEX);
  sChipID.toCharArray(cChipID, sChipID.length()+1);
  clientName = &cChipID[0];
  Serial.println(clientName);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if(!!client.connected()) reconnect();
  client.loop();

  if((digitalRead(BUTTON_PIN) == LOW) && (!isPUSH)) {
    Serial.println("Switch is pushed");
    client.publish("201708072/LED","1");
    isPUSH = 1;
  }
  else if((digitalRead(BUTTON_PIN)==HIGH) && isPUSH) {
    Serial.println("Switch is not pushed");
    client.publish("201708072/LED", "1");
    isPUSH = 0;
  }

  unsigned long now = millis();

  if (now - lastMsg > 500) {
    lastMsg = now;

    snprintf (msg, MSG_BUFFER_SIZE, "%ld", analogRead(A0));
    Serial.print("Piblish message: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
  }

  delay(1000);
}
