# M5Stack Core2 開発環境の M5Unified への移行手順

M5Core2専用ライブラリ（`M5Core2.h`）は非推奨となったため、最新の統合ライブラリである **`M5Unified`** へ移行するための手順と、ビルドエラー（`multiple definition of 'M5'`）が発生した際の対処法です。

---

## 🛠️ 1. ソースコードの書き換え

### ❌ 従来の書き方（非推奨）
```cpp
#include <M5Core2.h>

void setup() {
  M5.begin(); // ⚠️ 警告：'M5Core2::begin(...)' is deprecated
  M5.Lcd.print("Hello World");
}
```

### ⭕ 新しい書き方（M5Unified）
```cpp
#include <M5Unified.h> // M5Unifiedをインクルード

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);          // 新しい初期化方法

  M5.Display.print("Hello World"); // M5.Lcd から M5.Display に変更
}
```

### 💡 主な変更点
* **画面描画**: `M5.Lcd.〜` から **`M5.Display.〜`** に変更されました（M5GFXがベース）。
* **ボタン処理**: `pressedFor(ms, repeat)` の第2引数（リピート間隔）が廃止され、**`pressedFor(ms)`**（引数1つ）になりました。連打を実装する場合は `loop()` 内の `delay()` で間隔を調整します。
* **振動モーター**: `M5.Axp.SetLDOEnable(3, true/false)` の代わりに、共通命令の **`M5.Power.setVibration(強度)`**（0〜255）が使用可能です。

---

## 🚨 トラブルシューティング：`multiple definition of 'M5'` エラーの解決

ビルド時に以下のエラーが発生した場合、古い `M5Core2` ライブラリと新しい `M5Unified` ライブラリが競合しています。
> `multiple definition of 'M5'` / `first defined here`

### PlatformIOでの対処手順

1. **`platformio.ini` の修正**
   `lib_deps` から `m5stack/M5Core2` を削除し、**`m5stack/M5Unified`** のみを残します。
   <img width="511" height="214" alt="image" src="https://github.com/user-attachments/assets/ae693517-28b4-46c2-8293-7d2ec7200bc6" />

    ```ini
   lib_deps =
       m5stack/M5Unified
   ```

3. **キャッシュ・古いライブラリの完全削除**
   * プロジェクト内の `.pio/libdeps/m5stack-core2/M5Core2` フォルダを**フォルダごと手動で削除**します。
     <img width="450" height="279" alt="image" src="https://github.com/user-attachments/assets/63046295-b197-4b28-b1c5-972c4a7b6e11" />
   * VS CodeのPlatformIOメニュー（アリのアイコン）から、**`Clean`** タスクを実行します。
    <img width="488" height="234" alt="image" src="https://github.com/user-attachments/assets/23ec3b73-2dc1-44c1-b641-649813471ecb" />


4. **再ビルド**
   * 通常通り `Build` または `Upload` を実行すると、`M5Unified` のみが正しく読み込まれ、エラーが解消します。
