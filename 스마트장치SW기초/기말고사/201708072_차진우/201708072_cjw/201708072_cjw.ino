#include "DHTesp.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MSG_BUFFER_SIZE  50

const char* ssid = "ce404";
const char* password = "ce404lab";
const char* mqtt_server = "10.74.122.223";

const char* LED1 = "201708072/LED1";
const char* LED2 = "201708072/LED2";
const char* LED3 = "201708072/LED3";
const char* LED4 = "201708072/LED4";
const char* LED5 = "201708072/LED5";

const char* LED1Status ="201708072/LED1Status";
const char* LED2Status ="201708072/LED2Status";
const char* LED3Status ="201708072/LED3Status";
const char* LED4Status ="201708072/LED4Status";
const char* LED5Status ="201708072/LED5Status";

const char* DHTStatus = "201708072/DHTStatus";
const char* TEMPERATURE = "201708072/temperature";
const char* TEMPSET = "201708072/TempSet";
const char* HUMIDITY = "201708072/humidity";
const char* HUMIDSET = "201708072/HumidSet";

int DHTpin = D1;
int LED1_PIN = D4;    //GPIO2
int LED2_PIN = D5;
int LED6_PIN = D6;
int LED7_PIN = D7;
int LED8_PIN = D8;
int BUTTON_PIN = 0;   //D3
int isPUSH = 0;
unsigned long lastMsg1 = 0, lastMsg2=0;

const char* clientName = "";
String sChipID;
char cChipID[20];
char cValue[10];

DHTesp dht;
//TM1637Display display(CLK, DIO);    //TM1637
WiFiClient espClient;
PubSubClient client(espClient);
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
/* 정적 IP 설정 */

  IPAddress ip(10, 74, 122, 223); 
  IPAddress gateway(10, 74, 0, 1);
  IPAddress subnet(255, 255, 0, 0);
  IPAddress myDns(203, 246, 96, 254);
  WiFi.config(ip, myDns, gateway, subnet); 
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if(!strcmp(LED1,topic))
  {
    if ((char)payload[0] == '0') {
      digitalWrite(LED1_PIN, LOW);
    } 
    else if ((char)payload[0] == '1') {
      digitalWrite(LED1_PIN, HIGH);
    }
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", digitalRead(LED1_PIN));
    Serial.print("Publish message: ");
    Serial.print(LED1Status);
    Serial.println(msg);
    client.publish(LED1Status, msg);
  }
  else if(!strcmp(LED2,topic))
  {
    if ((char)payload[0] == '0') {
      digitalWrite(LED2_PIN, LOW);
    } 
    else if ((char)payload[0] == '1') {
      digitalWrite(LED2_PIN, HIGH);
    }
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", digitalRead(LED2_PIN));
    Serial.print("Publish message: ");
    Serial.print(LED2Status);
    Serial.println(msg);
    client.publish(LED2Status, msg);
  }
  else if(!strcmp(TEMPSET,topic))
  {
    int i;
    for (i = 0; i < length; i++) {
      cValue[i] = (char)payload[i];
    }
    cValue[i] = 0;
    
    int value = atoi(cValue);
    Serial.print("온도 설정 message: ");
    Serial.println(value+100);
  }
  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(clientName)) {
      Serial.println("connected");

      client.publish(DHTStatus, "START");
      client.subscribe(LED1);
      client.subscribe(LED2);
      client.subscribe(LED1Status);
      client.subscribe(LED2Status);
      client.subscribe(TEMPSET);
      client.subscribe(HUMIDSET);


    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup()
{
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  Serial.begin(115200);
  Serial.println();
 
  String thisBoard = ARDUINO_BOARD;
  Serial.print("Board Type : ");
  Serial.println(thisBoard);
  
  dht.setup(DHTpin, DHTesp::DHT11);

  setup_wifi();
  
  sChipID = String(ESP.getChipId(), HEX);
  sChipID.toCharArray(cChipID, sChipID.length() + 1);
  clientName = &cChipID[0];
  Serial.println(clientName);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  unsigned long now = millis();

  
  if (now - lastMsg1 > 5000) {
    lastMsg1 = now;
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    Serial.println("\n==================================");
    Serial.print("humidity : "); Serial.println(humidity, 1);
    Serial.print("temperature : "); Serial.println(temperature, 1);
  
    char tmp[30];
    sprintf(tmp, "%s",dht.getStatusString());
    client.publish(DHTStatus, tmp);
    
    sprintf(tmp, "%.2f", temperature);
    client.publish(TEMPERATURE, tmp);
    
    sprintf(tmp, "%.2f", humidity);
    client.publish(HUMIDITY, tmp);
  }

  if((digitalRead(BUTTON_PIN) == LOW) && (!isPUSH)) {
    Serial.println("Switch is pushed");
    client.publish(LED1,"1");
    client.publish(LED2,"1");
    isPUSH = 1;
  }
  else if((digitalRead(BUTTON_PIN)==HIGH) && isPUSH) {
    Serial.println("Switch is not pushed");
    client.publish(LED1,"0");
    client.publish(LED2,"0");
    isPUSH = 0;
  }
  delay(10);

  if (now - lastMsg2 > 10000) {
    lastMsg2 = now;
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", digitalRead(LED1_PIN));
    Serial.print("LED1 Status msg Published : ");
    Serial.println(msg);
    client.publish(LED1Status, msg);

    lastMsg2 = now;
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", digitalRead(LED2_PIN));
    Serial.print("LED2 Status msg Published : ");
    Serial.println(msg);
    client.publish(LED2Status, msg);
  }
}
