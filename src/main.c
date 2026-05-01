/*
 * PIC10F222 Timer
 * 1～5分計測するタイマー
 * 
 * GP0 ADC入力(analog)
 * GP1 ブザー(push pull)
 * GP2 LED出力(push pull)
 * GP3 スイッチ入力(pull-up)
 * 
 */

#include <xc.h>
#include <stdint.h>

/* ============================================================
 * Configuration bits
 * ============================================================ */

/* CONFIG1 */
#pragma config  MCLRE   = 0     /* MCLRを使用しない(GP3として使用)  */
#pragma config  CP      = 0     /* Code Protection disagled */
#pragma config  WDTE    = 0     /* Watchdogtimer disabled */
#pragma config  MCPU    = 0     /* MCLR Pullup enabled */
#pragma config  IOSCFS  = 1     /* Clock 8MHz */

/* Clock 8MHz */
#define _XTAL_FREQ  8000000UL

/* version */
#define VERSION_STRING   "1.00"

/* ボタンのチャタリング防止のため、ボタン押下判定するサイクル数 */
#define BUTTON_PRESS_DETECTION_CYCLE    20U /* 1cycle = 0.5u * 20 = 10usec */

// 本来は250だが、大きくずれる場合は調整する
// 数字を減らすと時間が短くなる、増やすと長くなる。
#define TMR_8MS_LOOP_COUNT  250U

#define BUZZER_PIN      GPIObits.GP1
#define LED_PIN         GPIObits.GP2
#define SW_PIN          GPIObits.GP3

// 音楽再生のテンポ 250でT=120、早くする場合は小さくする。遅くは出来ない
// T=150なら187程度にする
#define  MUSIC_TEMPO  255;
static uint8_t is_music_stop = 0;

/* 初期化 */
static void system_init() {

    /* GP2へのクロック出力 0 disabled (GPIOとして利用)*/
    //    OSCCALbits.FOSC4 = 0;

    /* OPTION
     *    7:GPWU    = 0   PIN変化のウェイクアップ有効
     *    6:GPPU    = 0   GP0,1,3 プルアップ有効
     *    5:T0CS    = 0   TMR0ソース Focs/4
     *    4:T0SE    = 0   TMR0 Source Edge Low=>High
     *    3:PSA     = 0   プリスケーラ TMR0 で使用
     *  2-0:PS      = 101 1:64
     *      */
    OPTION = 0b00000101;

    /* ADCON0
     *     7:ANS1    = 0   AN1/GP1をデジタルI/Oとして利用
     *     6:ANS0    = 1   AN0/GP0をデジタルI/Oとして利用
     *   5-4:        = 0
     *   3-2:CHS     = 00  ADCチャンネル選択 AN0
     *     1:GO/DONE = 0
     *     0:ADON    = 0   ADC停止
     *  */
    ADCON0 = 0b01000000;

    /* TRIS
     *     3:GP3 = 1 input
     *     2:GP2 = 0 output
     *     1:GP1 = 0 output
     *     0:GP0 = 0 output
     */
    TRIS = 0b00001000;

    // GPIOを全て0(出力無し)設定
    GPIO = 0x00;

}

/* ADC読み取り */
static uint8_t adc_go() {
    ADCON0bits.ADON = 1;
    __delay_us(8);
    ADCON0bits.GO = 1;
    while (ADCON0bits.nDONE == 1);
    ADCON0bits.ADON = 0;
    return ADRES;
}

/* 1秒間WAIT
 * GP3が押され続けた場合は 1 を返却
 *  */
static uint8_t wait_second() {

    uint8_t btn_on = 1;

    /* 
     * 8MHz / 4 = 2MHz = 0.5us
     * プリスケーラ 1:64なので、TMR0は 0.5us * 64 = 32us 毎にカウントアップ
     * 32us * 250 = 8000us = 8ms で250となる
     * 8ms * 125 = 1000ms なので、125回のループで1secとなる
     */
    uint8_t loop = 125U;
    TMR0 = 0;
    while (loop--) {
        // 8msecのループ
        // 32us * 250 = 8ms loop
        // 本来は250とするが、コード実行の遅れがあるので 232とする
        while (TMR0 < TMR_8MS_LOOP_COUNT) {
            if (SW_PIN == 1) {
                btn_on = 0;
            }
        }
        TMR0 = 0;
    }

    return btn_on;
}

/*
 *  ボタンの状態が変化するまでwait
 *  GPIOはプルアップされているので、statusは
 *   0 push
 *   1 release
 *   */
static void wait_button(uint8_t status) {
    uint8_t button = BUTTON_PRESS_DETECTION_CYCLE;
    while (button) {
        if (SW_PIN == status) {
            button--;
        } else {
            button = BUTTON_PRESS_DETECTION_CYCLE;
        }
    }
}

/*
 * LEDを点滅させる
 * loop 1sec = 20
 */
static void flush_led(uint16_t loop) {
    while (loop--) {
        LED_PIN = loop & 0x01U;
        __delay_ms(50);
    }
}

/*
 *  指定時間タイマー動作する
 *  途中キャンセルされた場合は true。タイマー完了の場合は false
 */
static uint8_t timer_main(uint16_t sec) {
    // (8000000 / 4) = 2000000 MHz = 0.5usec
    // プリスケーラ 1:64 = 0.5 * 64 = 32us
    for (uint16_t i = 0; i < sec; i++) {
        LED_PIN = i & 0x01U;
        if (wait_second()) {
            return 1;
        }
    }
    return 0;
}

/*
 * 65280 / x 簡易計算
 * 割り算を行うとプログラム容量が増えるので引き算でザックリ算出する
 * 
 */
static uint16_t play_get_loop_count(uint8_t x) {
    uint8_t res = 0;
    uint8_t rem = MUSIC_TEMPO;

    // 8bit / 8bit の簡易引き算方式
    while (rem >= x) {
        rem -= x;
        res++;
    }

    uint8_t shift = 8;
    if (x == 128U) shift = 7;
    if (x == 64U) shift = 6;
    // 結果を256倍（8ビット左シフト）して16bitにする
    return (uint16_t) res << shift;
}

/*
 * 音楽再生 
 * 音の長さは大体0.5秒 (T=120想定)
 * 
 * 休符は wait_second() で0.25秒のWaitを入れる。
 * if (!is_music_stop) is_music_stop = wait_second();
 */
static void play(uint8_t key) {

    if (is_music_stop) return;

    uint8_t button = BUTTON_PRESS_DETECTION_CYCLE;

    // 0.5秒間音を鳴らす場合のループ数を算出する
    uint16_t loop = play_get_loop_count(key);

    while (loop--) {
        if (key != 255 && key != 128 && key != 64) {
            BUZZER_PIN = (uint8_t)loop & 0x01U;
            LED_PIN = 1;
        }
        // 半周期分ループ
        TMR0 = 0;
        while (TMR0 < key) {
            if (SW_PIN == 0) {
                button--;
                if (!button) {
                    is_music_stop = 1;
                    goto play_exit;
                }
                continue;
            }
            button = BUTTON_PRESS_DETECTION_CYCLE;
        }
    }
play_exit:
    BUZZER_PIN = 0;
    LED_PIN = 0;
}

/*
 * main
 */
int main(void) {
    //asm("MOVWF OSCCAL");

    // 書込失敗で校正値が削除されてしまったので、過去のHEXファイルにあった値を設定
    // 別チップ使用する場合は asm("MOVWF OSCCAL"); に戻す
    OSCCAL = 0x22;

    // 初期化
    system_init();

    uint8_t work1;
    uint8_t work2;
    
    // main loop
    while (1) {

        // LED 消灯
        LED_PIN = 0;

        /* ボタンが押されるまでwait */
        wait_button(0);

        // LED点灯
        LED_PIN = 1;

        // AN0の電圧からタイマーの時間を取得
        work1 = adc_go();
        uint16_t timer_seconds = 299U;
        work2 = 5U;
        if (work1 <= 0x33U) {
            timer_seconds = 59U;
            work2 = 1U;
        } else if (work1 <= 0x66U) {
            timer_seconds = 119U;
            work2 = 2U;
        } else if (work1 <= 0x99U) {
            timer_seconds = 179U;
            work2 = 3U;
        } else if (work1 <= 0xCCU) {
            timer_seconds = 239U;
            work2 = 4U;
        }

        // ボタンが1秒以上押下されていた場合は設定時間分LEDを点滅させる
        if (wait_second()) {
            LED_PIN = 0;
            wait_button(1);
            while (work2--) {
                LED_PIN = 1;
                __delay_ms(200);
                LED_PIN = 0;
                __delay_ms(200);
            }
            continue;
        }

        // タイマー処理呼び出し
        work1 = timer_main(timer_seconds);

        // ボタンが離されるまで待つ
        wait_button(1);

        if (work1) {
            // LEDを3秒間点滅させる
            flush_led(60U);
            continue;
        } else {

            is_music_stop = 0;

            // プリスケーラを 1:16 に変更
            OPTION = 0b00000011;

#ifdef SEIJA
            // 聖者の行進（ロングVer. 52音構成）

            // --- メロディ 1回目 ---
            play(119); // C5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(89); // F5
            play(128); // 休符(1/2)
            play(80); // G5
            play(255); // 休符(1)

            // --- メロディ 2回目 ---
            play(119); // C5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(89); // F5
            play(128); // 休符(1/2)
            play(80); // G5
            play(255); // 休符(1)

            // --- メロディ 3回目（展開部） ---
            play(119); // C5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(89); // F5
            play(128); // 休符(1/2)
            play(80); // G5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(119); // C5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(106); // D5
            play(255); // 休符(1)

            // --- 後半サビ（Oh, when the saints...） ---
            play(95); // E5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(106); // D5
            play(128); // 休符(1/2)
            play(119); // C5
            play(255); // 休符(1)
            play(119); // C5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(80); // G5
            play(128); // 休符(1/2)
            play(80); // G5
            play(128); // 休符(1/2)
            play(89); // F5
            play(255); // 休符(1)

            // --- 締め ---
            play(89); // F5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(106); // D5
            play(128); // 休符(1/2)
            play(119); // C5

            play(89); // F5
            play(128); // 休符(1/2)
            play(95); // E5
            play(128); // 休符(1/2)
            play(95); // E5
#endif
#ifdef RADETUKI

            // ラデツキー行進曲（メインテーマ・フレーズ）
            // 全32音（休符含む）

            play(142); // A4
            play(255); // 休符
            play(142); // A4
            play(255); // 休符
            play(142); // A4
            play(255); // 休符

            play(159); // G4
            play(142); // A4
            play(127); // B4
            play(119); // C5
            play(106); // D5
            play(255); // 休符
            play(106); // D5
            play(255); // 休符

            play(142); // A4
            play(255); // 休符
            play(142); // A4
            play(255); // 休符
            play(142); // A4
            play(255); // 休符

            play(159); // G4
            play(142); // A4
            play(127); // B4
            play(119); // C5
            play(106); // D5
            play(255); // 休符
            play(106); // D5
            play(255); // 休符

            play(80); // G5
            play(95); // E5
            play(106); // D5
            play(119); // C5
#endif

#ifdef TENGOKU

            // 天国と地獄（地獄のギャロップ）
            // 合計 48音
            play(159); // G4
            play(127); // B4
            play(106); // D5
            play(80); // G5
            play(80); // G5
            play(255); // 休符
            play(80); // G5
            play(255); // 休符

            play(71); // A5
            play(80); // G5
            play(84); // F#5
            play(80); // G5
            play(71); // A5
            play(255); // 休符
            play(71); // A5
            play(255); // 休符

            play(80); // G5
            play(84); // F#5
            play(89); // F5
            play(84); // F#5
            play(80); // G5
            play(255); // 休符
            play(80); // G5
            play(255); // 休符

            play(71); // A5
            play(63); // B5
            play(60); // C6
            play(53); // D6
            play(47); // E6
            play(45); // F6
            play(42); // F#6
            play(40); // G6

            play(40); // G6
            play(255); // 休符
            play(40); // G6
            play(255); // 休符
            play(40); // G6
            play(255); // 休符
            play(40); // G6
            play(255); // 休符

            play(40); // G6
            play(47); // E6
            play(53); // D6
            play(60); // C6
            play(71); // A5
            play(80); // G5
            play(89); // F5
            play(106); // D5

#endif

#ifdef MENUETTO

            // メヌエット（ト長調）
            // 合計 32音

            play(80); // G5
            play(106); // D5
            play(95); // E5
            play(84); // F#5
            play(80); // G5
            play(106); // D5
            play(106); // D5
            play(106); // D5

            play(71); // A5
            play(106); // D5
            play(95); // E5
            play(84); // F#5
            play(80); // G5
            play(106); // D5
            play(106); // D5
            play(106); // D5

            play(80); // G5
            play(89); // F5
            play(95); // E5
            play(106); // D5
            play(119); // C5
            play(127); // B4
            play(142); // A4
            play(159); // G4

            play(84); // F#5
            play(80); // G5
            play(71); // A5
            play(106); // D5
            play(127); // B4
            play(142); // A4
            play(159); // G4
            play(159); // G4

#endif

#ifdef ORG_CRYSTAL_BREEZE
            // オリジナル曲：Crystal Breeze
            // 合計 32音

            play(60); // C6
            play(80); // G5
            play(95); // E5
            play(106); // D5
            play(119); // C5
            play(255); // 休符
            play(119); // C5
            play(106); // D5

            play(95); // E5
            play(80); // G5
            play(71); // A5
            play(63); // B5
            play(60); // C6
            play(255); // 休符
            play(60); // C6
            play(255); // 休符

            play(53); // D6
            play(60); // C6
            play(63); // B5
            play(71); // A5
            play(80); // G5
            play(255); // 休符
            play(95); // E5
            play(106); // D5

            play(119); // C5
            play(142); // A4
            play(159); // G4
            play(142); // A4
            play(119); // C5
            play(255); // 休符
            play(119); // C5
            play(255); // 休符

#endif

#define RAMEN
#ifdef RAMEN
            // ラーメン完成！歓喜のチャルメラ
            // 合計 60音

            // --- 導入：チャルメラ・オマージュ ---
            play(142); // A4
            play(127); // B4
            play(113); // C#5 (少し外したチャルメラ風)
            play(127); // B4
            play(142); // A4
            play(255); // 休符
            play(142); // A4
            play(127); // B4
            play(113); // C#5
            play(127); // B4
            play(142); // A4
            play(127); // B4
            play(255); // 休符

            // --- 期待感：音階が上がっていく ---
            play(119); // C5
            play(106); // D5
            play(95); // E5
            play(80); // G5
            play(119); // C5
            play(106); // D5
            play(95); // E5
            play(80); // G5

            // --- メイン：喜びのメロディ（歓喜の歌風） ---
            play(95); // E5
            play(95); // E5
            play(89); // F5
            play(80); // G5
            play(80); // G5
            play(89); // F5
            play(95); // E5
            play(106); // D5
            play(119); // C5
            play(119); // C5
            play(106); // D5
            play(95); // E5
            play(95); // E5
            play(128); // 休符(短)
            play(106); // D5
            play(106); // D5
            play(255); // 休符

            play(95); // E5
            play(95); // E5
            play(89); // F5
            play(80); // G5
            play(80); // G5
            play(89); // F5
            play(95); // E5
            play(106); // D5
            play(119); // C5
            play(119); // C5
            play(106); // D5
            play(95); // E5
            play(106); // D5
            play(128); // 休符(短)
            play(119); // C5
            play(119); // C5
            play(255); // 休符

            // --- フィナーレ：高らかに終了 ---
            play(80); // G5
            play(60); // C6
            play(47); // E6
            play(40); // G6
            play(30); // C7
            play(255); // 休符
            play(30); // C7

#endif
            // プリスケーラを 1:64 に変更
            OPTION = 0b00000101;

        }

        // ボタンが離されるまで待つ
        wait_button(1);

    }

}

