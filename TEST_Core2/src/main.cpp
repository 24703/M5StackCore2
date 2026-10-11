#include <M5Core2.h>
// 初期設定 ----------------------------------------------------
void setup() {
  M5.begin();                       // 本体初期化
  M5.Lcd.setTextColor(YELLOW);      // 文字色
  M5.Lcd.setTextSize(2);            // 文字サイズ
  M5.Lcd.setCursor(65, 10);         // 座標指定
  M5.Lcd.println("Button example");
  M5.Lcd.setCursor(3, 35);          // 座標指定
  M5.Lcd.println("Press button B for 700ms");
  M5.Lcd.println("to clear screen.");
  M5.Lcd.setTextColor(RED);         // 文字色
}
// メイン ------------------------------------------------------
void loop() {
  M5.update();                      // ボタン状態初期化
  M5.Axp.SetLDOEnable(3, false);    // AXP_IO3 を false で振動モータ停止

  if (M5.BtnA.wasPressed() || M5.BtnA.pressedFor(1000, 200)) {        // BtnAを押した、または1秒以上の長押しなら
    M5.Lcd.print('A');                                                // Aを表示、または0.2秒ごとに連続表示
    M5.Axp.SetLDOEnable(3, true);                                     // AXP_IO3 を true で振動モータ動作
  } else if (M5.BtnB.wasPressed() || M5.BtnB.pressedFor(1000, 200)) { // BtnBを押した、または1秒以上の長押しなら
    M5.Lcd.print('B');                                                // Bを表示、または0.2秒ごとに連続表示
    M5.Axp.SetLDOEnable(3, true);                                     // AXP_IO3 を true で振動モータ動作
  } else if (M5.BtnC.wasPressed() || M5.BtnC.pressedFor(1000, 200)) { // BtnBを押した、または1秒以上の長押しなら
    M5.Lcd.print('C');                                                // Cを表示、または0.2秒ごとに連続表示
    M5.Axp.SetLDOEnable(3, true);                                     // AXP_IO3 を true で振動モータ動作
  } else if (M5.BtnB.wasReleasefor(700)) {                            // BtnBが0.7秒以上押されて離したら
    M5.Lcd.clear(WHITE);          // 画面白クリア
    M5.Lcd.setCursor(0, 0);       // 座標指定
    M5.Axp.SetLDOEnable(3, true); // AXP_IO3 を true で振動モータ動作
  }
  delay(80);  // 遅延時間（振動モータ動作時間）
}