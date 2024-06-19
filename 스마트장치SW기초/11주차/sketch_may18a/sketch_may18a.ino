#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
const char *ssid = "chajinwoo";
const char *password = "20051677";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);
//timeClient(NTP 클라이언트 이름, NTP 서버 주소, GMT조절, 클라이언트 업데이트 주기)
//GMT조절: 대한민국 GMT+9이므로 9*60*60 = 32400초
//클라이언트 업데이트 주기는 ms단위(1시간 주기:1시간*3600초*1000=3600000ms)
unsigned long lastMsg = 0;
const int led = 2;
const int hour = 10;
const int minute = 0;

void setup(){
Serial.begin(115200);
pinMode(led, OUTPUT);

IPAddress address(10, 74, 128, 127);
IPAddress gateway(10, 74, 0, 1);
IPAddress mask(255, 255, 0, 0);
IPAddress DNS(8, 8, 8, 8);

WiFi.begin(ssid, password);

if(!WiFi.config(address,gateway,mask,DNS))
Serial.println("STA Failed to configure");
while ( WiFi.status() != WL_CONNECTED ) {
delay ( 500 ); Serial.print ( "." );
}
timeClient.begin();
}
void loop() {
  time_t nowTime;
  struct tm *ts;
  char buf[80];
  timeClient.update();
  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    nowTime = timeClient.getEpochTime();
    ts = localtime(&nowTime);
    }
    Serial.print(ts->tm_year+1900); Serial.print("년 ");
    Serial.print(ts->tm_mon + 1); Serial.print("월 ");
    Serial.print(ts->tm_mday); Serial.print("일 ");
    Serial.print(ts->tm_hour); Serial.print("시 ");
    Serial.print(ts->tm_min); Serial.print("분 ");
    Serial.print(ts->tm_sec); Serial.println("초 ");

    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    Serial.println(buf);
    Serial.println(timeClient.getFormattedTime());

  if(timeClient.isTimeSet()) {
    if (hour == timeClient.getHours() && minute == timeClient.getMinutes()) {
      digitalWrite(led, 0);
    }
  }
  Serial.println();
}
