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

/* ============================================================
 *  Include
 * ============================================================ */

#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

/* ============================================================
 *  Configuration bits
 * ============================================================ */

/* CONFIG1 */
#pragma config  MCLRE   = 0     /* MCLRを使用しない(GP3として使用)  */
#pragma config  CP      = 0     /* Code Protection disagled */
#pragma config  WDTE    = 0     /* Watchdogtimer disabled */
#pragma config  MCPU    = 0     /* MCLR Pullup enabled */
#pragma config  IOSCFS  = 1     /* Clock 8MHz */

/* ============================================================
 *  Clock Speed
 * ============================================================ */

/* Clock 8MHz */
#define _XTAL_FREQ  8000000UL

/* ============================================================
 *  Version
 * ============================================================ */

/* version */
#define VERSION_STRING   "1.00"

/* ============================================================
 *  Construction
 * ============================================================ */

/* ボタンのチャタリング防止のため、ボタン押下判定するサイクル数 */
#define BUTTON_PRESS_DETECTION_CYCLE    20U /* 1cycle = 0.5u * 20 = 10usec */

// 本来は250だが、大きくずれる場合は調整する
// 数字を減らすと時間が短くなる、増やすと長くなる。
// 基本的には命令実行の分時間が伸びるので、値を減らして短くする方向に調整するハズ
#define TMR_8MS_LOOP_COUNT  250U

/* ============================================================
 *  Pin Define
 * ============================================================ */

#define BUZZER_PIN      GPIObits.GP1
#define LED_PIN         GPIObits.GP2
#define SW_PIN          GPIObits.GP3

/* ============================================================
 *  Music Select
 * ============================================================ */

//#define PLAY_TEST
//#define SEIJA
//#define MINUET
//#define ORG_CRYSTAL_BREEZE
#define RAMEN
//#define COPILOT_ORIGINAL
//#define GOOGLE_ORIGINAL

/* ============================================================
 *  Global
 * ============================================================ */

// 音楽再生を停止するためのフラグ
static uint8_t is_music_stop = 0;

/* ============================================================
 *  システム初期化
 * ============================================================ */
static void system_init() {

    /*
     *  GP2へのクロック出力 0 disabled (GPIOとして利用)
     *   デフォルトが0なので設定不要
     */
    //    OSCCALbits.FOSC4 = 0;

    /*
     *  OPTION
     *    7:GPWU    = 0   PIN変化のウェイクアップ有効
     *    6:GPPU    = 0   GP0,1,3 プルアップ有効
     *    5:T0CS    = 0   TMR0ソース Focs/4
     *    4:T0SE    = 0   TMR0 Source Edge Low=>High
     *    3:PSA     = 0   プリスケーラ TMR0 で使用
     *  2-0:PS      = 101 1:64
     */
    OPTION = 0b00000101;

    /* 
     * ADCON0
     *     7:ANS1    = 0   AN1/GP1をデジタルI/Oとして利用
     *     6:ANS0    = 1   AN0/GP0をアナログ入力として利用
     *   5-4:        = 00  reserved
     *   3-2:CHS     = 00  ADCチャンネル選択 AN0
     *     1:GO/DONE = 0
     *     0:ADON    = 0   ADC停止
     *  */
    ADCON0 = 0b01000000;

    /* 
     * TRIS
     *     3:GP3 = 1 input
     *     2:GP2 = 0 output
     *     1:GP1 = 0 output
     *     0:GP0 = 0 output
     */
    TRIS = 0b00001000;

    /*
     * GPIO
     * 全て0に設定
     */
    GPIO = 0x00;

}

/* ADC読み取り */
static void adc_go() {
    ADCON0bits.ADON = 1;
    __delay_us(8);
    ADCON0bits.GO = 1;
    while (ADCON0bits.nDONE == 1);
    ADCON0bits.ADON = 0;
}

/* 1秒間WAIT
 * GP3が押され続けた場合は 1 を返却
 *  */
static uint8_t wait_second() {

    uint8_t button = 1;

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
        while (TMR0 < TMR_8MS_LOOP_COUNT) {
            if (SW_PIN == 1) {
                button = 0;
            }
        }
        TMR0 = 0;
    }

    return button;
}

/*
 * ボタンの状態が変化するまでwait
 * GPIOはプルアップされているので、statusは
 *  0 push
 *  1 release
 * 
 *   */
static void wait_button(uint8_t status) {
    uint8_t button = BUTTON_PRESS_DETECTION_CYCLE;
    while (1) {
        if (SW_PIN == status) {
            button--;
            if (!button) {
                return;
            }
        } else {
            button = BUTTON_PRESS_DETECTION_CYCLE;
        }
    }
}

/*
 * LEDを点滅させる
 * loop 1sec = 20
 */
static void flush_led(uint8_t loop) {
    while (loop--) {
        LED_PIN = loop & 0x01U;
        __delay_ms(50);
    }
}

/*
 *  指定時間タイマー動作する
 *  途中キャンセルされた場合は 1。タイマー完了の場合は 0
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
 * 音楽再生 
 * 音の長さは大体0.5秒 (T=120想定)
 * プリスケーラを 1:16 に設定しておく
 * 255,128,64 で音符の1/1,1/2,1/4の休符とする。
 */
static void play(uint8_t key) {

    // キャンセル済み判定
    if (is_music_stop) return;

    // 0.5秒間音を鳴らす場合のループ数を算出する
    // 65280 / x 簡易計算
    // 割り算を行うとプログラム容量が増えるので引き算でザックリ算出する
    // 仕組み上音の長さが結構バラバラになる
    uint8_t res = 0;
    uint8_t rem = 255U; // 65280 >> 8 で8bit演算にする

    // 8bit / 8bit の簡易引き算方式
    while (rem >= key) {
        rem -= key;
        res++;
    }

    // << 8 して元に戻す
    uint16_t loop = (uint16_t) res << 8;
    uint8_t note = 1;
    LED_PIN = 1;

    // 休符の場合は長さ毎にシフト量を変更して1/1,1/2,1/4にする。
    switch (key) {
        case 64U:
            // 1/4休符とするため2bitシフト
            // 128Uと255Uの処理も行うのでbreakしない
            loop >>= 1;
        case 128U:
            // 1/2休符とするため1bitシフト
            // 255Uの処理も行うのでbreakしない
            loop >>= 1;
        case 255U:
            // 休符のフラグとLED消灯
            note = 0;
            LED_PIN = 0;
            break;
    }

    uint8_t button = BUTTON_PRESS_DETECTION_CYCLE;

    // BUZZEのPINを指定の周波数でON/OFFさせて音階を出力する
    while (loop--) {

        if (note) {
            // 半周期毎にブザーをON/OFFする
            BUZZER_PIN = (uint8_t) loop & 0x01U;
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
 * 音楽再生
 */
static void play_music() {

#ifdef PLAY_TEST
    play(60); // C6
    play(60); // C6
    play(255); // 休符(音と同じ長さ)
    play(53); // D6
    play(53); // D6
    play(128); // 休符(音の半分の長さ)
    play(47); // E6
    play(47); // E6
    play(64); // 休符(音の四分の一の長さ)
    play(45); // F6
    play(45); // F6
    play(255); // 休符(音と同じ長さ)
    play(255); // 休符(音と同じ長さ)
    play(255); // 休符(音と同じ長さ)
    play(60); // C6
    play(60); // C6
    play(255); // 休符(音と同じ長さ)
    play(53); // D6
    play(53); // D6
    play(128); // 休符(音の半分の長さ)
    play(47); // E6
    play(47); // E6
    play(64); // 休符(音の四分の一の長さ)
    play(45); // F6
    play(45); // F6

#endif

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

#ifdef MINUET

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
    play(255); // 休符
    play(30); // C7
    play(30); // C7

#endif

#ifdef COPILOT_ORIGINAL

    play(255); // 休符
    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(128); // 休符(半分)
    play(63); // B5
    play(71); // A5
    play(80); // G5
    play(64); // 休符(1/4)
    play(95); // E5
    play(106); // D5
    play(119); // C5
    play(128); // 休符(半分)
    play(159); // G4
    play(127); // B4
    play(119); // C5
    play(255); // 休符
    play(119); // C5
    play(134); // A#4
    play(142); // A4
    play(150); // G#4
    play(159); // G4
    play(128); // 休符(半分)
    play(95); // E5
    play(106); // D5
    play(119); // C5
    play(64); // 休符(1/4)
    play(119); // C5
    play(150); // G#4
    play(159); // G4
    play(127); // B4
    play(128); // 休符(半分)
    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(255); // 休符
    play(63); // B5
    play(71); // A5
    play(80); // G5
    play(128); // 休符(半分)
    play(95); // E5
    play(106); // D5
    play(119); // C5
    play(64); // 休符(1/4)
    play(142); // A4
    play(150); // G#4
    play(159); // G4
    play(169); // F#4
    play(179); // F4
    play(128); // 休符(半分)
    play(119); // C5
    play(95); // E5
    play(60); // C6
    play(30); // C7
    play(15); // C8  フィナーレ（高音で締め）

#endif

#ifdef GOOGLE_ORIGINAL
    // タイマー完了メロディー (56音)
    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(128); // 休符(1/2)
    play(60); // C6
    play(128); // 休符(1/2)

    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(128); // 休符(1/2)
    play(60); // C6
    play(255); // 休符(1/1)

    play(106); // D5
    play(89); // F5
    play(71); // A5
    play(53); // D6
    play(128); // 休符(1/2)
    play(53); // D6
    play(128); // 休符(1/2)

    play(106); // D5
    play(89); // F5
    play(71); // A5
    play(53); // D6
    play(128); // 休符(1/2)
    play(53); // D6
    play(255); // 休符(1/1)

    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(47); // E6
    play(128); // 休符(1/2)
    play(47); // E6
    play(128); // 休符(1/2)

    play(89); // F5
    play(71); // A5
    play(53); // D6
    play(45); // F6
    play(128); // 休符(1/2)
    play(45); // F6
    play(128); // 休符(1/2)

    play(80); // G5
    play(60); // C6
    play(47); // E6
    play(40); // G6
    play(128); // 休符(1/2)
    play(40); // G6
    play(128); // 休符(1/2)

    play(60); // C6
    play(47); // E6
    play(40); // G6
    play(30); // C7
    play(128); // 休符(1/2)
    play(30); // C7
    play(255); // 休符(1/1)

#endif  

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

    // スリープ解除ではない場合スリープする
    if (!STATUSbits.GPWUF) {
        goto go_sleep;
    }

    // チャタリング判定期間にボタンが離されたら再度スリープする
    for (uint8_t i = 0; i < BUTTON_PRESS_DETECTION_CYCLE; i++) {
        if (SW_PIN == 1) {
            goto go_sleep;
        }
    }

    // LED点灯
    LED_PIN = 1;

    // AN0の電圧からタイマーの時間を取得
    adc_go();
    uint16_t timer_seconds = 299U;
    uint8_t timer_minutes = 5U;
    if (ADRES <= 0x33U) {
        timer_seconds = 59U;
        timer_minutes = 1U;
    } else if (ADRES <= 0x66U) {
        timer_seconds = 119U;
        timer_minutes = 2U;
    } else if (ADRES <= 0x99U) {
        timer_seconds = 179U;
        timer_minutes = 3U;
    } else if (ADRES <= 0xCCU) {
        timer_seconds = 239U;
        timer_minutes = 4U;
    }

    // ボタンが1秒以上押下されていた場合は設定時間分LEDを点滅させる
    if (wait_second()) {
        LED_PIN = 0;
        wait_button(1);
        while (timer_minutes--) {
            LED_PIN = 1;
            __delay_ms(200);
            LED_PIN = 0;
            __delay_ms(200);
        }
        goto go_sleep;
    }

    // タイマー処理呼び出し
    if (timer_main(timer_seconds)) {
        // キャンセルされた場合

        // ボタンが離されるまで待つ
        wait_button(1);
        // LEDを2秒間点滅させる
        flush_led(40U);


    } else {

    }

    // プリスケーラを 1:16 に変更
    OPTION = 0b00000011;

    // 音楽再生
    play_music();

go_sleep:

    // ボタンが離されるまで待つ
    wait_button(1);

    // スリープ
    SLEEP();

    // returnがないと警告が出るのでreturn記載しておく
    // warning: non-void function does not return a value [-Wreturn-type]
    return EXIT_SUCCESS;

}

