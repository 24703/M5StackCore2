#include <Arduino.h>
#include <M5Unified.H>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

String ssid, pass;
String DATA_FILE = "/data.txt";

void WiFi_setting();

void setup()
{
  Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg); // 本体初期化

  M5.Display.setTextColor(GREEN); // 文字色
  M5.Display.setTextSize(2);      // 文字サイズ

  // SDカードの初期化
  if (!SD.begin(GPIO_NUM_4))
  {
    Serial.println("SD Wait...");
    M5.Display.println("SD Init Failed...");
    return;
  }
  
  File file = SD.open(DATA_FILE);
  if (!file)
  {
    M5.Display.println("file not found !");
    return;
  }
  
  String id[4];
  for (int i = 0; i < 4; i++)
  {
    id[i] = file.readStringUntil('\n');
  }
  
  // 💡配列は0から始まるので、1行目がid[0]、2行目がid[1]になります
  ssid = id[1]; // ファイルの2行目
  pass = id[3]; // ファイルの4行目

  ssid.trim();
  pass.trim();
  file.close();

  // Wi-Fiへ接続
  WiFi_setting();
}

void loop()
{
  M5.update();
}

void WiFi_setting()
{
  M5.Display.fillScreen(BLACK); // 画面を一度クリア
  M5.Display.setCursor(0, 0);
  M5.Display.print("--- WiFi connect TEST ---\n\n");
  M5.Display.print("Connecting");
  
  WiFi.mode(WIFI_STA);
  // 💡 .c_str() をつけて型を変換する
  WiFi.begin(ssid.c_str(), pass.c_str());
  
  // 接続完了を待つループ
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    M5.Display.print("."); // 画面にも . を出す
    delay(500);            // 💡 500ms（0.5秒）ごとにチェック
  }

  // 💡 括弧をつけ、WL_CONNECTED と比較する
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    
    M5.Display.print("\n\n");
    M5.Display.println("WiFi connected!");
    M5.Display.print("IP: ");
    M5.Display.println(WiFi.localIP());
  }
}
