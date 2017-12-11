#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//plz check Ur I2C LCD display's address
LiquidCrystal_I2C lcd(0x3F, 16, 2);
HTTPClient http;
String payload;
int start_idx;
int end_idx ;
String prc;
int price;
int old_price;
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
  http.begin("http://api.coindesk.com/v1/bpi/currentprice/USD.json");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
    start_idx = payload.lastIndexOf("rate_float") + 12;
    end_idx = payload.lastIndexOf("}}}");
    prc = payload.substring(start_idx, end_idx);
    price = (float)prc.toInt();
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
      lcd.print("USD");
      lcd.setCursor(4, 1);
      lcd.print(price);
      old_price = price;
    }
  }
  else {
    lcd.setCursor(13, 0);
    lcd.print("ERR");
    ok_flag = 0;
  }
  delay(1500);
}
