#include <ESP32WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP32HTTPClient.h>
#include "RTClib.h"

RTC_DS3231 rtc;

#define teganganTV D12
#define teganganAC D13
#define teganganLamp D14
#define teganganRefrigator D25
#define teganganPC D26

#define arusTV D27
#define arusAC D32
#define arusLamp D33
#define arusRefrigator D34
#define arusPC D35

int sensitivitas = 185;
int offset = 2500;
double VA_TV = 00;
double VA_AC = 00;
double VA_PC = 00;
double VA_Lamp = 00;
double VA_Refrigator = 00;

double nilaiarusTV = 00;
double nilaiarusAC = 00;
double nilaiarusPC = 00;
double nilaiarusLamp = 00;

double nilaiarusRefrigator = 00;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFiManager wifiManager;
  wifiManager.autoConnect("Tugas LISTRIK");
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Menghubungkan...");
    for (int c = 0; c < 3; c++) {
      Serial.print(" .");
      delay(1000);
    }
  }
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();
  int bacaTeganganTV = analogRead(teganganTV);
  int bacaTeganganAC = analogRead(teganganAC);
  int bacaTeganganPC = analogRead(teganganPC);
  int bacaTeganganLamp = analogRead(teganganLamp);
  int bacaTeganganRefrigator = analogRead(teganganRefrigator);

  int bacaArusTV = analogRead(arusTV);
  int bacaArusAC = analogRead(arusAC);
  int bacaArusPC = analogRead(arusPC);
  int bacaArusLamp = analogRead(arusLamp);
  int bacaArusRefrigator = analogRead(arusRefrigator);

  nilaiTeganganTV = bacaTeganganTV * (5.0 / 1023.0);
  nilaiTeganganAC = bacaTeganganAC * (5.0 / 1023.0);
  nilaiTeganganPC = bacaTeganganPC * (5.0 / 1023.0);
  nilaiTeganganLamp = bacaTeganganLamp * (5.0 / 1023.0);
  nilaiTeganganRefrigator = bacaTeganganRefrigator * (5.0 / 1023.0);

  VA_TV = (bacaArusTV / 1024.0) * 5000;
  VA_AC = (bacaArusAC / 1024.0) * 5000;
  VA_PC = (bacaArusPC / 1024.0) * 5000;
  VA_Lamp = (bacaArusLamp / 1024.0) * 5000;
  VA_Refrigator = (bacaArusRefrigator / 1024.0) * 5000;

  nilaiarusTV = ((VA_TV - offset) / sensitivitas);
  nilaiarusAC = ((VA_AC - offset) / sensitivitas);
  nilaiarusPC = ((VA_PC - offset) / sensitivitas);
  nilaiarusLamp = ((VA_LAMP - offset) / sensitivitas);
  nilaiarusRefrigator = ((VA_Refrigator - offset) / sensitivitas);

  Serial.print("Tegangan TV = ");
  Serial.print(nilaiTeganganTV);
  Serial.print(" , Tegangan AC = ");
  Serial.print(nilaiTeganganAC);
  Serial.print(" , Tegangan PC = ");
  Serial.print(nilaiTeganganPC);
  Serial.print(" , Tegangan Lamp = ");
  Serial.print(nilaiTeganganLamp);
  Serial.print(" , Tegangan Refrigator = ");
  Serial.println(nilaiTeganganRefrigator);

  Serial.print("arus TV = ");
  Serial.print(nilaiarusTV);
  Serial.print(" , arus AC = ");
  Serial.print(nilaiarusAC);
  Serial.print(" , arus PC = ");
  Serial.print(nilaiarusPC);
  Serial.print(" , arus Lamp = ");
  Serial.print(nilaiarusLamp);
  Serial.print(" , arus Refrigator = ");
  Serial.println(nilaiarusRefrigator);
}
void kirimdata() {
  String sendDataTegangan = String(tanggal) + ',' + String(nilaiTeganganTV) + ',' + String(nilaiTeganganAC) + ',' + String(nilaiTeganganPC) + ',' + String(nilaiTeganganLamp) + ',' + String(nilaiTeganganRefrigator) + ',';
  String sendDataArus = String(nilaiarusTV) + ',' + String(nilaiarusAC) + ',' + String(nilaiarusPC) + ',' + String(nilaiarusLamp) + ',' + String(nilaiarusRefrigator);
  String sendData = sendDataTegangan + sendDataArus;
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  https.begin(*client, sendData);
  int httpResponseCode = https.GET();
  String payload = https.getString();
  Serial.println(payload);
  https.end();
}
