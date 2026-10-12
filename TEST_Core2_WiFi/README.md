M5Stack Core2でmicroSDカード内のデータ（SSIDとパスワード）を読み出し、Wi-Fiへ自動接続するまでの開発プロセス、トラブルシューティング、および最終的なプログラムコードをまとめたREADMEです。
---
📅 開発の軌跡とトラブルシューティング
1. 初期のSDカード読み出し（数値が返ってくる問題）<br>
【現象】
SDカード内のテキストファイルに `Hello` という文字列を書き込んで読み出した際、画面やシリアルモニタに文字列ではなく以下のような数字が表示される現象が発生しました。<br>
```
シリアルモニタへの出力
```text
72
101
108
108
111
```
【原因】
これはエラーではなく、「Hello」という文字がアスキーコード（文字コード）の「数値」に変換されて表示されてしまっている状態です。<br>
72 = `H`
101 = `e`
108 = `l`
108 = `l`
111 = `o`<br>
`M5.Display.print(dataFile.read())` のようにファイルから1バイト（1文字）ずつ読み出す際に `print` を使うと、マイコンが「数値をそのまま表示しなさい」と解釈してしまいます。<br>
【解決策】
文字として正しく表示させるには、`print` ではなく `write` を使用するか、**`char`型にキャスト（変換）**する必要があります。
```cpp
// ✖ 数値になってしまう例
M5.Display.print(dataFile.read()); 

// 〇 文字として表示される例
M5.Display.write(dataFile.read()); 
// または
M5.Display.print((char)dataFile.read());
```
---
2. プログラム内で「文字列(String)」として扱う方法
読み出したデータを画面に表示するだけでなく、条件分岐（`if`文）などで活用するために、`String`型の変数に丸ごと格納する方法へ改良しました。<br>
💡 `readString()` と `trim()` の重要性<br>
`dataFile.readString()` / `readStringUntil('\n')`: ファイルの中身や指定した改行コードまでの文字列を一括で読み込めます。<br>
`trim()`: テキストファイルから読み込んだ文字列の末尾には、目に見えない「改行コード（`\n` や `\r`）」が自動的に付着します。これを残したままだと、`if (myText == "Hello")` などの文字列比較や、`WiFi.begin()` での接続が100%失敗します。<br>`.trim()` を実行して不要な改行やスペースを綺麗に消去することが必須です。
---
💾 SDカードの準備とテキストファイルの構成<br>
ハードウェア仕様 (M5Stack Core2)<br>
対応SDカード: 最大16GBまでのmicroSDカード（FAT32形式でフォーマット）<br>
内蔵SPIピン配置:<br>
`CS`: GPIO 4<br>
`SCK`: GPIO 18<br>
`MISO`: GPIO 38<br>
`MOSI`: GPIO 23<br>
`data.txt` のファイル構成<br>
今回のプログラムでは、SDカードのルートディレクトリ（一番上の階層）に `data.txt` を作成します。<br>
プログラムの抽出ロジック（配列のインデックス）に合わせ、2行目にSSID、4行目にパスワードを記述する構成にしています。<br>
```text
[SSID]
あなたのWi-FiのSSID
[PASSWORD]
あなたのWi-Fiのパスワード
```
(※ 1行目と3行目はコメントや識別用の文字列として無視されます)<br>
> ⚠️ **Wi-Fiに関する注意点**: ESP32（Core2）は **2.4GHz帯**（g/nなど）のWi-Fiにのみ対応しています。5GHz帯（a/acなど）のSSIDは接続できないため指定しないでください。<br>
---
💻 最終的なソースコード (`src/main.cpp`)
```cpp
#include <Arduino.h>
#include <M5Unified.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

String ssid, pass;
String DATA_FILE = "/data.txt";

// 関数宣言
void WiFi_setting();

void setup()
{
  Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg); // 本体初期化

  M5.Display.setTextColor(GREEN); // 文字色を緑に設定
  M5.Display.setTextSize(2);      // 文字サイズを2に設定

  // 1. SDカードの初期化（Core2のCSピンはGPIO_NUM_4）
  if (!SD.begin(GPIO_NUM_4))
  {
    Serial.println("SD Wait...");
    M5.Display.println("SD Init Failed...");
    return;
  }
  
  // 2. ファイルのオープン
  File file = SD.open(DATA_FILE);
  if (!file)
  {
    M5.Display.println("file not found !");
    return;
  }
  
  // 3. データの読み込み（4行分を配列に格納）
  String id[4];
  for (int i = 0; i < 4; i++)
  {
    id[i] = file.readStringUntil('\n');
  }
  
  // 配列は0から始まるため、2行目(インデックス1)と4行目(インデックス3)を抽出
  ssid = id[1]; 
  pass = id[3];

  // 💡 最重要：末尾の不要な改行コードやスペースを消去
  ssid.trim();
  pass.trim();
  
  file.close(); // ファイルを閉じる

  // 4. Wi-Fi接続処理へ
  WiFi_setting();
}

void loop()
{
  M5.update(); // M5Unifiedの内部状態更新
}

void WiFi_setting()
{
  M5.Display.fillScreen(BLACK); // 画面を一度クリア
  M5.Display.setCursor(0, 0);
  M5.Display.print("--- WiFi connect TEST ---\n\n");
  M5.Display.print("Connecting");
  
  WiFi.mode(WIFI_STA);
  
  // 💡 修正ポイント：.c_str() を用いてC言語スタイルの文字列(char*)に変換して確実につなぐ
  WiFi.begin(ssid.c_str(), pass.c_str());
  
  // 接続完了を待つループ
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    M5.Display.print("."); // M5Stackの画面上にも「.」を進捗として表示
    delay(500);            // 💡 修正ポイント：500ms（0.5秒）の適切なウェイトを挟みWDTリセットを防止
  }

  // 💡 修正ポイント：WiFi.statusに()を付け、正しく接続ステータスを判定
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    
    M5.Display.print("\n\n");
    M5.Display.println("WiFi connected!");
    M5.Display.print("IP: ");
    M5.Display.println(WiFi.localIP()); // 割り当てられたIPアドレスを表示
  }
}
```
---
🛠️ 改良・修正ポイントのまとめ<br>
`c_str()`による文字列変換の適用<br>
`WiFi.begin(ssid, pass)` から `WiFi.begin(ssid.c_str(), pass.c_str())` へ変更。ESP32のWi-Fiライブラリが求めるデータ型へ明示的に変換することで、接続の失敗やハングアップを防ぎます。<br>
待機型デイループ（`delay`値）の適正化<br>
接続を待つ `while` ループ内の待ち時間を `10ms` から `500ms` に延長。ループ回数が過剰になり、マイコンのセーフティ機能（ウォッチドッグタイマー）が働いて強制再起動がかかるのを防止しました。<br>
条件分岐の記述不備の解消<br>
`if (WiFi.status)` となっていた部分を `if (WiFi.status() == WL_CONNECTED)` へ修正。関数呼び出しの括弧漏れを解消し、成否を論理的に正しく判定できるようになりました。<br>
