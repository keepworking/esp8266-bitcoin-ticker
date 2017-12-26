#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//i2c 어드레스는 간혹 바뀌니까 안되면 i2c스켄 필요
LiquidCrystal_I2C lcd(0x3F, 16, 2);
WiFiClientSecure client;

//custom char
byte HEART1[8] = {0x0, 0xa, 0x15, 0x11, 0xa, 0x4, 0x0};
byte HEART2[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};

//server info
const char* host = "api.korbit.co.kr";
const int httpsPort = 443;
String btcurl = "/v1/ticker?currency_pair=btc_krw";
String bchurl = "/v1/ticker?currency_pair=bch_krw";

int getBIT(int type) {
  WiFi.waitForConnectResult();

  if (!client.connect(host, httpsPort)) {
    return 0;
  }
  client.print(String("GET ") + (type == 1 ? btcurl : bchurl) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    if (client.readStringUntil('\n') == "\r") {
      break;
    }
  }
  return 1;
}

int parsePrice() {
  static int start_idx;
  static int end_idx ;
  static String prc;
  static String payload;
  payload = client.readStringUntil('\n');
  start_idx = payload.lastIndexOf(":\"") + 2;
  end_idx = payload.lastIndexOf("\"}");
  prc = payload.substring(start_idx, end_idx);
  return prc.toInt();
}

int getCoin() {
  if (getBIT(1) == 0) {
    return -1;
  }
  return parsePrice();
}

int getCache() {
  if (getBIT(0) == 0) {
    return -1;
  }
  return parsePrice();
}

void btcDraw(int linePos) {
  static int old_price = 0;
  static int price;
  static int keyBlink = 0;
  price = getCoin();
  lcd.setCursor(15, linePos);
  if (keyBlink == 0) {
    lcd.write(byte(0));
  }
  else {
    lcd.write(byte(1));
  }
  keyBlink = 1 - keyBlink;
  if (old_price != price) {
    lcd.setCursor(0, linePos);
    lcd.print("BTC");
    lcd.setCursor(3, linePos);
    lcd.print("            ");
    lcd.setCursor(4, linePos);
    lcd.print(price == -1 ? "ERR" : String(price));
    old_price = price;
    ESP.wdtFeed();
  }
}

void bchDraw(int linePos) {
  static int old_price = 0;
  static int price;
  static int keyBlink = 0;
  price = getCache();
  lcd.setCursor(15, linePos);
  if (keyBlink == 0) {
    lcd.write(byte(0));
  }
  else {
    lcd.write(byte(1));
  }
  keyBlink = 1 - keyBlink;
  if (old_price != price) {
    lcd.setCursor(0, linePos);
    lcd.print("BCH");
    lcd.setCursor(3, linePos);
    lcd.print("            ");
    lcd.setCursor(4, linePos);
    lcd.print(price == -1 ? "ERR" : String(price));
    old_price = price;
  }
}
void setup() {
  ESP.wdtEnable(10000);
  WiFi.waitForConnectResult();
  Serial.begin(115200);
  lcd.begin();
  lcd.createChar(0, HEART1);
  lcd.createChar(1, HEART2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(4000);
  lcd.clear();
}

void loop() {
  btcDraw(0);
  delay(1000);
  bchDraw(1);
  delay(1000);
}

