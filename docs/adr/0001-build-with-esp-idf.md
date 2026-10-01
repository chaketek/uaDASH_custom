# ADR 0001: ESP32-S3-Touch-LCD-7B を ESP-IDF でビルドする

- ステータス: 採用 (Accepted)
- 日付: 2026-10-01
- 対象: Waveshare ESP32-S3-Touch-LCD-7B (`WAVESHARE_S3_LCD7B`)

## 背景

7B (1024x600 RGB パネル, ST7262) を Arduino IDE / arduino-cli でビルドすると、静止画面でもちらつきが見えた。

切り分けの結果、原因はパネルのリフレッシュ率の低さだった。

- TFT 液晶は 60Hz 前後での駆動を前提にしており、低いと極性反転や電荷保持の揺らぎが見える
- 他ボードの既存設定も 31〜39Hz で、5 インチ機 (LCD-5, 38.7Hz) でも同様にちらつく
- 7B で 17.5Hz → 36.8Hz にすると「明らかに減った」。バックライト輝度 (PWM) には依存しない
- 静止時の LVGL 再描画はほぼゼロで、UI 側の点滅などではない

RGB パネルはフレームメモリを持たないため、毎フレーム全画素 (1024x600x2 = 1.2MB) を PSRAM のフレームバッファから送り続ける必要がある。リフレッシュ率を上げると、その分 PSRAM 帯域を消費する。

Arduino IDE 版は ESP-IDF をビルド済みライブラリとして使っており、sdkconfig を変更できない (PSRAM 80MHz、データキャッシュ 32KB / 32B ライン固定)。この条件では、LVGL が全画面を描き直す画面切替時に表示が上下に崩れない上限が 16MHz (17.5Hz) だった。Espressif ESP-FAQ でも、Octal PSRAM 80MHz での上限は約 22MHz、120MHz で約 30MHz とされている。

## 決定

1. 7B は ESP-IDF v5.4.2 + Arduino core 3.2.1 (ESP-IDF コンポーネント `espressif/arduino-esp32`) でビルドする。
   - アプリのソース (`firmware/`) は Arduino IDE 版と共通。`main/main.cpp` から `firmware.ino` を取り込む
   - Arduino API (Preferences, Ticker, Serial など) はそのまま使える
2. 7B の標準 sdkconfig (`sdkconfig.defaults.WAVESHARE_S3_LCD7B`) は次のとおり。実験的機能は使わない。
   - PSRAM 80MHz (変更なし)
   - `CONFIG_ESP32S3_DATA_CACHE_64KB`, `CONFIG_ESP32S3_DATA_CACHE_LINE_64B`
   - `CONFIG_SPIRAM_XIP_FROM_PSRAM` (命令とフォント等の定数データを PSRAM から実行)
   - ピクセルクロック 24MHz (約 26Hz)、バウンスバッファ 20 ライン。`display_driver.h` が sdkconfig から自動選択する
3. PSRAM/Flash 120MHz の設定は `sdkconfig.defaults.psram120` として残すが、標準にはしない (後述の温度リスク)。
4. Arduino IDE / arduino-cli ビルドは引き続き動く (7B は 16MHz にフォールバック、他ボードは従来どおり)。CI は両方をビルドする。

## 計測結果

7B 実機、デモデータ (全項目が同時に変化) での値。`PERF_MONITOR` のシリアル出力 (1 秒あたり) と、USB カメラでの表示確認による。

| 構成 | pclk / リフレッシュ | 描画 ms/s | コピー ms/s | 描画 FPS | 画面切替時の崩れ | リスク |
|---|---|---|---|---|---|---|
| Arduino IDE | 16MHz / 17.5Hz | 約 730 | 約 350 | 22〜28 | なし (21MHz 以上で崩れる) | なし |
| **ESP-IDF 標準 (採用)** | 24MHz / 26.2Hz | 約 555 | 約 235 | 27〜30 | なし (バウンス 20 ライン) | なし |
| ESP-IDF + PSRAM 120MHz | 30MHz / 36.8Hz | 約 450 | 約 150 | 29〜33 | なし | 温度 (後述) |

- 描画 FPS はどの構成でも大差ない (CPU には余裕があり、画面更新タイマーやデータ更新周期で頭打ち)。主な違いはパネルのリフレッシュ率。
- 実際に見える更新回数は min(描画 FPS, リフレッシュ率) なので、採用構成は約 26 回/秒、120MHz 構成は約 30 回/秒。
- 描画/コピー時間は 24MHz で計測した値 (ピクセルクロックにはほぼ依存しない)。
- 静止時のちらつきは、26Hz で実用上気にならない、23Hz では少し気になる、という評価だった。

## 画面切替時の表示ずれとバウンスバッファ

24MHz・バウンスバッファ 10 ラインでは、カメラの低解像度の確認では問題が見えなかったが、実機操作で「画面切替後にフォントや線に横筋が入る」不具合が見つかった。

- 原因: 画面切替 (全画面の描き直し) で PSRAM へのアクセスが集中し、バウンスバッファ (PSRAM からパネルへの中継用内部 RAM バッファ) の補充が一時的に遅れると、画像全体がバウンスバッファ 1 個分 (`LCD_BOUNCE_LINES` ライン) ずれた状態で固定される。ESP-IDF の再同期処理 (`CONFIG_LCD_RGB_RESTART_IN_VSYNC`) はバウンス位置が 2 バッファ分を超えてずれた場合しか補正しないため、このずれは戻らない。
- 試験方法: 画面切替を 4 秒ごとに繰り返し、カメラで 1920x1080・2 フレーム/秒で撮影する。メイン画面の区切り線の縦位置を画像処理で求め、基準からずれたフレームを数える。
- 結果:

| 構成 | 試験時間 | 結果 |
|---|---|---|
| 24MHz、バウンス 10 ライン | 60 秒 | 2 回目の切替以降ずっとずれたまま |
| 24MHz、バウンス 10 ライン、画面切替アニメーションなし | 90 秒 | ずれあり |
| 21MHz、バウンス 10 ライン (+ 切替ごとに NVS 書き込み) | 各 90 秒 | ずれなし |
| 24MHz、バウンス 20 ライン | 90 秒 | ずれなし |
| **24MHz、バウンス 20 ライン + デモデータ + 切替ごとに NVS 書き込み** | 180 秒 (約 22 回切替) | ずれなし |
| 30MHz (`sdkconfig.defaults.psram120`)、バウンス 10 ライン + NVS 書き込み | 90 秒 | ずれなし |

平均の帯域は 24MHz でも足りており、ずれは全画面描き直し時の一時的な補充遅れで起きていた。バウンスバッファを 20 ラインにして補充の猶予を倍にしたことで解消した (内部 RAM は 40KB 増えて 80KB)。なお Arduino IDE 版 (キャッシュ 32KB、XIP なし) では帯域そのものが不足しており、バウンスバッファを増やすと逆に悪化した。

### 追記: 指スワイプでのずれと VSYNC ごとの再同期

バウンス 20 ラインでも、指で素早くスワイプして画面切替のフェード中に次の切替が重なると、同じずれが再発した (自動試験を「設定画面を含む切替 + フェード中に 100ms 間隔で連続切替」に変えて再現)。タッチの I2C 通信は無関係 (常時読み出しでもずれなし)。

- 原因: ESP-IDF のドライバは、どちらのバウンスバッファを補充するかを DMA EOF 割り込みの回数で決めており、その回数はリセットされない。PSRAM アクセス集中で割り込みが遅れて EOF 2 回分が 1 回の割り込みにまとまると、以降は補充先が 1 つずれたままになる。
- 対策: ドライバのフレームバッファ管理を使わず (`no_fb`)、`on_bounce_empty` コールバックでファームウェアが補充する。補充先と補充する位置は自前のカウンタで決め、ドライバが毎 VSYNC で DMA をバウンスバッファ 0 から再開する (`CONFIG_LCD_RGB_RESTART_IN_VSYNC`) のに合わせて `on_vsync` でカウンタを 0 に戻し、バッファ 0/1 の中身がフレーム先頭でなければ詰め直す (`display_driver_rgb.cpp`)。
- 結果: 割り込みを取りこぼしても乱れるのは最大 1 フレームで、ずれは残らない。上記の連続切替試験・指スワイプとも、ずれなし。平常時の処理量は変わらない。

ピクセルクロックやバウンスバッファを変えるときは、上記の試験で確認すること。

## 検討した代替案

| 案 | 結果 |
|---|---|
| Arduino IDE のまま、ドライバ側で工夫 | LVGL 直描画、別コアでのコピー、GDMA コピー、コピー速度の抑制、画面切替中だけ pclk を下げる、を試したがいずれも効果なし (帯域そのものが足りない) |
| PlatformIO (pioarduino) の `custom_sdkconfig` | Arduino ライブラリを ESP-IDF から再ビルドする方式。この PC の PlatformIO 環境ではツール導入に失敗し、sdkconfig の合成も不正確 (choice の旧値が残り 80MHz のまま) で不安定だったため不採用 |
| Arduino を使わない純 ESP-IDF | Preferences/Ticker/Serial/LovyanGFX 依存の書き換えが必要。性能面の差は sdkconfig で決まるため、まずは Arduino コンポーネント方式で移行し、必要になれば段階的に置き換える |
| 8bit (256 色パレット) フレームバッファ | 読み出し量が半分になるが、色数低下 (文字のアンチエイリアス劣化) とパレット展開の CPU 負荷 (コア 0 の 3〜4 割) が大きい。不採用 |
| PSRAM 120MHz を標準にする | 性能は最良だが温度リスクがあり、対策機能が 7B で使えない。オプションとして残す |

## PSRAM 120MHz の温度リスク (再検討時の注意)

- 120MHz の Octal PSRAM は ESP-IDF で実験的機能 (`CONFIG_IDF_EXPERIMENTAL_FEATURES` が必要)。
- PSRAM の読み取りタイミング (位相) は起動時に 1 回だけ、その時の温度で調整される。起動時からチップ温度が約 20℃ (チップによる) 変わると、PSRAM アクセスがランダムに失敗し、クラッシュや表示データ破損になる。
- 基準は「起動時からの温度差」。自己発熱 (ESP32、バックライト、電源) も含まれ、ケース内では起動後に 10〜20℃ 上がることがある。
- 車載では冬の始動 (低温起動 → 暖房 + 自己発熱で上昇) と夏の始動 (高温起動 → エアコンで低下) が危険側。
- 対策機能 `CONFIG_SPIRAM_TIMING_TUNING_POINT_VIA_TEMPERATURE_SENSOR` (温度センサーで位相を再調整) は、Flash が GigaDevice (0xC8) / XMC (0x20) の場合のみ対応。7B の Flash はメーカー ID 0x46 のため有効にすると起動時にエラーで止まる。
- 7B の Flash は 120MHz 用の HPM (High Performance Mode) も「未対応の型番」と警告が出る。

120MHz を標準化する場合の条件:

1. 内蔵温度センサーで、実車の 1 回の使用サイクル (電源 ON → OFF) 中の「起動時からの温度差」を記録し、自己発熱が落ち着いた後も含めて 15℃ 以内に収まることを確認する
2. 冬の低温始動・夏の高温始動の両方で長時間動作させ、クラッシュや表示崩れがないことを確認する
3. Flash 120MHz (HPM 未対応) での読み書き (設定保存) に問題がないことを確認する

## 影響

- ビルド手順が変わる (ESP-IDF v5.4.2 のインストールが必要)。手順は `README-ESP32-S3-Touch-LCD-7B.md`。
- Arduino コンポーネントは使わないもの (RainMaker、Zigbee、ESP-SR など) も依存として取り込むため、初回ビルドに時間がかかる (10 分程度)。アプリのサイズへの影響は小さい (約 870KB)。
- 依存の版は `main/idf_component.yml` と `dependencies.lock` で固定する。
- パーティション表は Arduino の `app3M_fat9M_16MB` と同じ配置 (`partitions.csv`) にしたので、Arduino 版との書き換え時も NVS (警告設定) は残る。
- ボード定義は共通のまま、`firmware/display_driver.h` が `CONFIG_SPIRAM_SPEED` / `CONFIG_SPIRAM_XIP_FROM_PSRAM` を見て 7B のピクセルクロックを選ぶ。sdkconfig とクロックの組み合わせを手で合わせる必要はない。

## 参考

- Espressif ESP-FAQ, LCD: https://docs.espressif.com/projects/esp-faq/en/latest/software-framework/peripherals/lcd.html
- ESP-IDF v5.4 Flash/PSRAM 設定 (120MHz): https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-guides/flash_psram_config.html
- ESP-IDF `components/esp_psram/esp32s3/Kconfig.spiram`, `components/esp_hw_support/mspi_timing_by_mspi_delay.c` (温度補正の Flash メーカー制限)
