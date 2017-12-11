#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//i2c 어드레스는 간혹 바뀌니까 안되면 i2c스켄 필요
LiquidCrystal_I2C lcd(0x3F, 16, 2);
WiFiClientSecure client;

//server info
const char* host = "api.korbit.co.kr";
const int httpsPort = 443;
String url = "/v1/ticker?currency_pair=btc_krw";

//response string
String payload;

//price position
int start_idx;
int end_idx ;
String prc;

//exported price
int price;
int old_price;

//state flag
int key_blink = 0;
int ok_flag = 0;

void setup() {
  WiFi.waitForConnectResult();
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1000);
}

void loop() {
  WiFi.waitForConnectResult();

  if (!client.connect(host, httpsPort)) {
    lcd.setCursor(13, 0);
    lcd.print("ERR");
    ok_flag = 0;
    return;
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    if (client.readStringUntil('\n') == "\r") {
      break;
    }
  }

  payload = client.readStringUntil('\n');
  start_idx = payload.lastIndexOf(":\"") + 2;
  end_idx = payload.lastIndexOf("\"}");
  prc = payload.substring(start_idx, end_idx);
  price = prc.toInt();

  lcd.setCursor(13, 0);
  lcd.print(key_blink == 0 ? "_" : "-");
  key_blink = 1 - key_blink;

  if (ok_flag == 0) {
    lcd.setCursor(14, 0);
    lcd.print("OK");
    ok_flag = 1;
  }

  if (old_price != price) {
    lcd.setCursor(0, 0);
    lcd.print("BTC 1");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("KRW");
    lcd.setCursor(4, 1);
    lcd.print(price);
    old_price = price;
  }

  delay(3000);
}
