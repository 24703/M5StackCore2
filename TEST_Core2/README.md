# M5Stack CORE2 PlatformIO 開発ガイド

[M5Stack CORE2](https://logikara.blog/core2_2/#mokuji_1) の開発環境として **PlatformIO (VS Code)** を使用したセットアップ手順、サンプルプログラム、および工場出荷状態に戻す方法をまとめた README です。

---

## 目次

1. USBドライバのインストール
2. 開発環境の準備 (VS Code + PlatformIO)
3. プロジェクトの作成と初期設定
4. ライブラリのインストール
5. サンプルプログラムの書き込み・動作確認
6. トラブルシューティング: 通信ポートの強制指定
7. 工場出荷状態に戻す方法

---

## 1. USBドライバのインストール

CORE2のUSBチップには **CP2104** と **CH9102F** の2種類が存在します。正常に認識させるため、**両方のドライバ（CH9102およびCP210x）をあらかじめ両方インストール**してください。

* 📥 ドライバのダウンロード: [M5Stack Docs 公式ページ](https://docs.m5stack.com)

---

## 2. 開発環境の準備 (VS Code + PlatformIO)

高機能エディタ VS Code 内で動作する [PlatformIO](https://logikara.blog/core2_2/#mokuji_4-1) を使用します。以下の順番でインストールを行います。

1. **[Visual Studio Code (VS Code)](https://logikara.blog/vscode_install/)** のインストール（および日本語化）
2. **[Python](https://logikara.blog/python_install/)** のインストール
3. VS Codeの拡張機能（Extensions）から **[PlatformIO](https://logikara.blog/install_platform/)** をインストール

---

## 3. プロジェクトの作成と初期設定

1. PlatformIOを起動し、画面左下の「ホームアイコン」（Home）をクリックします。
2. 「New Project」をクリックします。
3. 設定画面で以下のように入力・選択します。
* **Name**: プロジェクト名（例: `CORE2`）
* **Board**: `M5Stack Core2` を選択
* **Framework**: `Arduino` を選択


4. プロジェクト作成時の確認画面で「はい」を選択します。

---

## 4. ライブラリのインストール

1. PlatformIO Homeのタブから **「Libraries」** を開く。<br>
   <img width="359" height="245" alt="image" src="https://github.com/user-attachments/assets/67cd31e2-9285-4a7e-bed0-cc3cb2c56160" />
3. 検索窓に `core2` と入力して、少し下にスクロールし **「M5Core2」** を選択します。
   <img width="353" height="197" alt="image" src="https://github.com/user-attachments/assets/effa9608-c177-4c30-9381-705202501da4" />
   <img width="607" height="123" alt="image" src="https://github.com/user-attachments/assets/076c6e62-78f3-47ee-a949-a6a7d5171f1a" />
5. **「Add to Project」** をクリックし、作成したプロジェクト名を選択して追加します。
   <img width="320" height="188" alt="image" src="https://github.com/user-attachments/assets/c39a4172-d237-42d0-b8e7-989c24ec8101" />
   <img width="381" height="141" alt="image" src="https://github.com/user-attachments/assets/56ad2b93-7c2b-4dfd-a6b7-089516efeebf" />
7. 画面表示の拡張用として、必要に応じて **`LovyanGFX`** ライブラリも同様の手順で追加しておくと便利です。

---

## 5. サンプルプログラムの書き込み・動作確認

プロジェクト内の `src/main.cpp` を開き、以下のサンプルコード（ボタン操作および振動モータ制御の例）をコピー＆ペーストします。

```cpp
#include <M5Core2.h>

// 初期設定 ----------------------------------------------------
void setup() {
  M5.begin();                      // 本体初期化
  M5.Lcd.setTextColor(YELLOW);     // 文字色
  M5.Lcd.setTextSize(2);           // 文字サイズ
  M5.Lcd.setCursor(65, 10);        // 座標指定
  M5.Lcd.println("Button example");
  M5.Lcd.setCursor(3, 35);         // 座標指定
  M5.Lcd.println("Press button B for 700ms");
  M5.Lcd.println("to clear screen.");
  M5.Lcd.setTextColor(RED);        // 文字色
}

// メイン ------------------------------------------------------
void loop() {
  M5.update();                     // ボタン状態初期化
  M5.Axp.SetLDOEnable(3, false);   // AXP_IO3 を false で振動モータ停止

  if (M5.BtnA.wasPressed() || M5.BtnA.pressedFor(1000, 200)) {
    M5.Lcd.print('A');
    M5.Axp.SetLDOEnable(3, true);  // 振動モータ動作
  } else if (M5.BtnB.wasPressed() || M5.BtnB.pressedFor(1000, 200)) {
    M5.Lcd.print('B');
    M5.Axp.SetLDOEnable(3, true);  
  } else if (M5.BtnC.wasPressed() || M5.BtnC.pressedFor(1000, 200)) {
    M5.Lcd.print('C');
    M5.Axp.SetLDOEnable(3, true);  
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.clear(WHITE);           // 画面白クリア
    M5.Lcd.setCursor(0, 0);
    M5.Axp.SetLDOEnable(3, true);  
  }
  delay(80);
}

```

### 書き込み方法

画面左下の **「→」アイコン（Upload）** をクリックすると、コンパイルおよび書き込みが実行されます。ターミナルに `SUCCESS` と表示されたら完了です。

---

## 6. トラブルシューティング: 通信ポートの強制指定

複数のUSBドライバをインストールしている影響などで、PlatformIOが正しいポートを自動選択せず書き込みエラーになることがあります。その場合は、プロジェクト内の `platformio.ini` に以下の2行を追加してポートを明示的に指定してください。

```ini
[env:m5stack-core2]
platform = espressif32
board = m5stack-core2
framework = arduino
lib_deps = m5stack/M5Core2@^0.1.5

; ポートを手動指定する場合の追記
upload_port = "COM6"       ; ご自身の環境のポート番号に変更
monitor_port = "COM6"      ; ご自身の環境のポート番号に変更

```

---

## 7. 工場出荷状態に戻す方法

購入時のデモ画面に戻したい場合は、**M5Burner** を利用するのが最も簡単です。

1. M5Burnerを起動し、左メニューから `CORE` を選択します。
2. プログラム一覧から **`Core2FactoryTest`** をダウンロードし、`Burn` を実行します。
