esp32

◯文献類
・環境構築　　　　　https://note.com/iwoujima/n/n6d03f0fded36
・環境構築　　　　　https://qiita.com/jp-96/items/63bc5d28bf78287b55d1
・esp-idfコマンド
　https://zenn.dev/nnn112358/scraps/4123e59786969fs/4123e59786969f
・esp32実験
　https://risusan.net/esp32-experiment-discussion/#toc69

◯ドキュメント系
・INA219プログラムドキュメント
 https://components.espressif.com/components/esp-idf-lib/ina219/versions/1.0.7/readme


◯毎回のループ
・環境変数設定 　　　". $HOME/esp/esp-idf/export.sh"
・esp32接続
・esp32にアクセス権限付与(いらない？)
・プロジェクトディレクトリに移動
・esp32をターゲットとして設定　"idf.py set-target esp32" 
・menuconfig実行・設定 
・プロジェクトビルド　"idf.py build"
・デバイスに書き込み　"idf.py -p /USB/pass flash"
・実行　"idf.py -p /dev/ttyUSB0 monitor"

◯絶対条件
・ALL C言語で開発する


◯ロードマップ
0.[]:　ルーブリック確認
1.[]:8/20 電圧測定
2.[]:　シリアル出力
3.[]:　Excelシリアル入力(VBA)
4.[]:　


初日(8/16)
・Arduino IDE -> x
・ESP-IDF　-> o
・[x]PCとつなぐ
・[x]書き込み方法
・[x]実行方法
・[x]サンプルプログラムの実行

2日目(8/17)
目標：esp32で遊べるようになる
・[o]PCとつなぐ
・[o]書き込み方法
・[o]実行方法
・[o]サンプルプログラムの実行

追加タスク
・[o]サンプルプログラムを動作させる
・[]これまでに作ったcプログラムを動作させる
　・プロジェクト作成
　・プログラムコピー
　　・ヘッダファイル確認
　・ビルド
　・書き込み
　・実行

esp32はこれ:[ 8月17日(月) 11:34:26 2026] usb 1-1: ch341-uart converter now attached to ttyUSB0
chmod 777: 1桁目->所有者　2桁目->グループ　3桁目->その他　7->読み書きすべて実行可能(権限)
毎回環境変数設定が必要？


進まないので。プログラムを沢山書こう
Geminiにプログラム作成の課題を出してもらう

3日目(8/18)
　慣れる
・[x]自作プログラム作成1:esp3
・[x]自作プログラム作成2:esp4
　GPIOピンからの入力
・[x]esp5
・[x]esp6
　割り込みハンドラ(ISR)：CPUに負担をかけずにイベントを実施する
　割り込みハンドラ内では重い処理を減らす(なるべく軽くする)
　・割り込みトリガー設定
　・割り込みサービスインストール・有効化
　・ピンと割り込み処理関数の結びつけ
　
4日目(8/19)
・[]esp7
　PWM制御でLEDの明度を変更
　ヘッダ :driver/ledc.h
　1:ledc_timer_config_t(構造体)を設定(5つ)
　2:ledc_channel_config_t(構造体)を設定(7つ)
　3:PWM出力の準備
　　・タイマー設定
　　・チャンネル設定
　4:点灯
　　・デューティ比設定
　　・デューティ比反映
　　
5日目(8/20)
・[x]esp9:パソコンからの入力1
・[x]esp10:パソコンからの入力2
・[x]esp11:電圧測定
　adc(Analog to Digital Converter)
　1:ADCユニット初期化
　　構造体adc_oneshot_unit_handle_t(宣言のみ), adc_oneshot_unit_init_cfg_t(1つ)を用意。
　　adc_oneshot_config_channel()でADC1のユニットを作成・初期化。
　2:チャンネルの設定
　　構造体adc_oneshot_chan_cfg_t(2つ)を用意。adc_oneshot_config_channel()でADC_CHANNEL_6(GPIO 34)を設定。
　   アッテネーター(減衰器)を用意しないと、入力できる電圧が小さくなってしまう
　3:値の読み取り・変換
　　adc_oneshot_read()で電圧を読み取る。
　　Raw値から電圧に変換する。Raw値の最大は構造体adc_oneshot_chan_cfg_tで設定する。
　　
　ループ内で毎回扱う変数は毎回宣言し直すと、正確に値を更新できる確率が高い
　
6日目(8/21)

7日目(8/22)
　WindowsPCでData Streamerを使用してグラフをリアルタイム作成
　データが捨てられる->データの記録＆データ許容量を増やす
　
　気づいたこと:測定した電圧は正しいのか？(ズレを感じる:電池を話してもすぐに電圧が下がらない)

8日目(8/24)
・INA219モジュールでの電圧測定
　・導入方法検討(C言語)
　　・ライブラリ(IDF):esp-idf-lib/ina219
　
　//没
　・ライブラリ探し
　・導入方法
　　・[中止]python3-full　インストール
　　・[中止]ampy　インストール

9日目(8/26)
・回路設計のための勉強
　・アース
　・分圧器

10日目(8/27)
・回路設計(してみた)
　・esp32で分圧抵抗を用いて測定することを想定
　・回路図Ver1
・疑問:INA219は直列につなぐのか?並列?電圧は並列でないと測れないのでは

11日目(8/28)
・INA219で電圧・電流両方を測定する方法を探る
◯電圧
・I2C通信を用いてINA219とやり取りする
・i2c_master_bus_config_t
・i2c_master_bus_handle_t
・i2c_new_master_bus()
・i2c_device_config_t
・i2c_master_dev_handle_t
・i2c_master_bus_add_device()
・i2c_master_transmit()
・i2c_master_transmit_receive() ループ内
-電圧が測定できない

12日目(9/2)
◯電圧測定できない問題を解消する(esp13.c)
1.INA219の初期化(0x00への書き込みをしてみる)
・i2c_master_transmit()
　i2cデバイスに書き込みのみを行う関数
transmit_receive()はレジスタ番号を送信して、データを受けとる関数
引数1:dev_handle    ハンドル
引数2:write_buffer  送信したいアドレス先とデータの配列
引数3:write_size    送信データサイズ
引数4:timeout_ms    タイムアウト時間(ミリ秒指定)

引数を配列化した

3:回路の見直し
・esp32のGndは3本が集まるように配線
・INA219のGnd,電池の-,負荷の-をesp32のGndに接続。
・INA219のVin-は負荷の+ -> 負荷- -> esp32のGnd

13日目(9/3)
・esp14.c
　・[未達成]電流測定
  ・基本は電圧測定(esp13.c)と同じ
  ・0x01レジスタからの値の読み出し
  ・変換作業が異なるっぽい
　・[未達成]電力測定

14日目(9/4)
・esp14.cの続き
  ・電流測定
    ・シャント電圧というのは、シャント抵抗での電圧降下[V]らしい。そのため、オームの法則を適用しないと電流値が出ない。
      なので、先に電圧の値を計算する必要がある。
    ・読みだしたシャント電圧の値も同様に2バイト連結、bit->数値変換。電流のLSB(1分解能)は0.1mAらしい。そのため、数値変換した値に0.1fを掛ける。


15日目(9/5)


◯初期プロンプト
あなたはC言語とesp32に詳しい組み込み開発エンジニアです。私がesp32のプログラミングを理解するのを手伝ってください。
◯背景
・esp32で遊びたい!!
・プログラムの書き方がわからない

◯環境
・Ubuntu 26.04
・esp32(初代)

◯条件
・不足情報があれば質問すること
・私のC言語のスキルは入出力、反復・分岐、関数作成、配列、ポインタ、構造体
・私がスキルアップするために、小さく明確な課題を出題すること。ここで言う"明確"とは、何について学習するかがはっきりとわかるような課題のことを指す。

◯最初の依頼
・添付するコードのヘッダファイルの役割について解説してほしい。
・その次に課題を出題すること

◯コード
"esp-idf付属サンプルコード(長いため省略)
"
