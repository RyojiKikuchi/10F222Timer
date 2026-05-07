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

// 本来は250だが、大きくずれる場合は調整する
// 数字を減らすと時間が短くなる、増やすと長くなる。
// 基本的には命令実行の分時間が伸びるので、値を減らして短くする方向に調整するハズ
#define TMR_8MS_LOOP_COUNT  250U

/* ============================================================
 *  Version
 * ============================================================ */

/* version */
#define VERSION_STRING   "1.10"

/* ============================================================
 *  Construction
 * ============================================================ */

/* ボタンのチャタリング防止のため、ボタン押下判定するサイクル数 */
#define BUTTON_PRESS_DETECTION_CYCLE    20U /* 1cycle = 0.5u * 20 = 10usec */

/* ============================================================
 *  Pin Define
 * ============================================================ */

#define BUZZER_PIN      GPIObits.GP1
#define LED_PIN         GPIObits.GP2
#define SW_PIN          GPIObits.GP3

/* ============================================================
 *  Music Play
 * ============================================================ */

#define TMR_MUSIC_2MS_LOOP_COUNT  250U    // プリスケーラ 1:16用

// T=120で250
// 数字を小さくするとその分テンポが早くなる
// 遅くすることは出来ない
//#define TMR_MUSIC_QUARTER       250U    // T=120
//#define TMR_MUSIC_QUARTER       218U    // T=150
//#define TMR_MUSIC_QUARTER       187U    // T=180
#define TMR_MUSIC_QUARTER       156U    // T=210
//#define TMR_MUSIC_QUARTER       125U    // T=240
#define TMR_MUSIC_EIGHTH        (TMR_MUSIC_QUARTER / 2U)
#define TMR_MUSIC_SIXTEENTH     (TMR_MUSIC_QUARTER / 4U)

//#define PLAY_NONE
//#define PLAY_TEST
//#define SEIJA                 // 聖者の行進(T=150)
#define GAMEUP_RUSH            // ゲームアップ・ラッシュ(T=210)
//#define KITCHEN_RUSH          // キッチン・ラッシュ(T=180)
//#define RAMEN                 // ラーメン完成！歓喜のチャルメラ(T=150)
//#define COPILOT_ORIGINAL      // Copilot Original(T=120)
//#define GOOGLE_ORIGINAL       // GoogleAI Original(T=120)

/* ============================================================
 *  Global
 * ============================================================ */

// 音楽再生を停止するためのフラグ
static uint8_t is_music_stop = 0;
static uint8_t play_length = TMR_MUSIC_QUARTER;

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
 * key で半周期分となるTMR0のカウント値を指定(1:16プリスケーラ(8us)を何回繰り返すか)
 * そのまま呼び出せば 4分音符 の長さで発音
 * 
 *  音符の長さは play_length に以下を設定。
 *     TMR_MUSIC_QUARTER   4分音符(デフォルト)
 *     TMR_MUSIC_EIGHTH    8分音符
 *     TMR_MUSIC_SIXTEENTH 16分音符
 * 
 *  keyの最大値は 249 まで。245以上で休符。
 * 
 *  */
static void play(uint8_t key) {

    //　キャンセル済み判定
    if (is_music_stop) return;
    int8_t play_note = 1;

    if (key >= 245) {
        play_note = 0;
        LED_PIN = 0;
    } else {
        LED_PIN = 1;
    }

    // 音符長ループ回数
    uint8_t loop = play_length;
    // 半周期計測用
    uint8_t note_tmr = 0;
    // 前回タイマー値
    uint8_t prev_tmr = 0;
    // ボタン押下チェック用
    uint8_t button = BUTTON_PRESS_DETECTION_CYCLE;

    TMR0 = 0;

    while (loop--) {
        while (TMR0 < TMR_MUSIC_2MS_LOOP_COUNT) {
            if (prev_tmr != TMR0) {
                prev_tmr = TMR0;
                note_tmr++;
                if (note_tmr >= key && play_note) {
                    note_tmr = 0;
                    BUZZER_PIN = ~BUZZER_PIN;
                }
            }
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
        TMR0 = 0;
    }

play_exit:
    play_length = TMR_MUSIC_QUARTER;
    BUZZER_PIN = 0;
    LED_PIN = 0;
}

/* 
 * 音楽再生
 */
static void play_music() {

    uint8_t i, j;

#ifdef PLAY_NONE
    play(249); // 休符
#endif

#ifdef PLAY_TEST
    play(60); // C6
    play(60); // C6
    play(249); // 休符(1/4)
    play(53); // D6
    play(53); // D6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(1/8)
    play(47); // E6
    play(47); // E6
    play_length = TMR_MUSIC_SIXTEENTH;
    play(249); // 休符(1/16)
    play(45); // F6
    play(45); // F6
    play(249); // 休符
    play(249); // 休符
    play(249); // 休符
    play(60); // C6
    play(60); // C6
    play(249); // 休符
    play(53); // D6
    play(53); // D6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(47); // E6
    play(47); // E6
    play_length = TMR_MUSIC_SIXTEENTH;
    play(249); // 休符(1/8)
    play(45); // F6
    play(45); // F6

#endif

#ifdef SEIJA

    // テンポ T=150 推奨

    // --- Aメロ：ループで節約 (4分音符主体)
    // C E F G を3回繰り返す（3回目は後ろに続くので2回ループ＋アルファ）
    for (i = 0; i < 2; i++) {
        play(60); // C6
        play(47); // E6
        play(45); // F6
        play(40); // G6
        play(249); // 休符
    }

    play(60); // C6
    play(47); // E6
    play(45); // F6
    play(40); // G6
    play(47); // E6
    play(60); // C6
    play(47); // E6
    play(53); // D6
    play(249); // 休符

    // --- Bメロ：サビ
    play(47); // E6
    play(53); // D6
    play(60); // C6
    play(60); // C6
    play(47); // E6
    play(40); // G6
    play(40); // G6
    play(45); // F6
    play(249); // 休符

    // --- Cメロ：結び
    play(47); // E6
    play(45); // F6
    play(40); // G6
    play(47); // E6
    play(60); // C6
    play(53); // D6
    play(60); // C6

    // --- 2周目：8分音符を混ぜて豪華に (メモリを使い切る)
    play(249); // 間
    for (i = 0; i < 2; i++) {
        play_length = TMR_MUSIC_EIGHTH;
        play(60);
        play(60); // C6 C6
        play(47); // E6
        play(45); // F6
        play(40); // G6
        play(249); // 休符
    }

    // クライマックス：1オクターブ上で連打
    for (i = 0; i < 4; i++) {
        play_length = TMR_MUSIC_SIXTEENTH;
        play(30); // C7
    }
    play(24); // E7
    play(20); // G7
    play(15); // C8 (フィニッシュ)

    // --- 追加：豪華フィナーレ（メモリ残り64words活用）

    // 1. 裏打ちのリズムでさらに盛り上げる (約24words)
    for (i = 0; i < 4; i++) {
        play_length = TMR_MUSIC_EIGHTH;
        play(249); // 裏拍を感じさせるための短い休符
        play_length = TMR_MUSIC_EIGHTH;
        play(30); // C7
    }

    // 2. ブルース風の音階駆け上がり (約15words)
    // 圧電スピーカが最も得意な高音域でスライドさせます
    play_length = TMR_MUSIC_SIXTEENTH;
    play(40); // G6
    play(36); // A6
    play(34); // A#6
    play(32); // B6
    play(30); // C7

    // 3. 最後の最後：力強いロングトーンの三連打 (約12words)
    // 4分音符で堂々と締めます
    play(24); // E7
    play(20); // G7
    play(15); // C8 (最高音！)

    // 4. 残り僅かなメモリで、最後の余韻に16分音符のトリル (約10words)
    for (i = 0; i < 3; i++) {
        play_length = TMR_MUSIC_SIXTEENTH;
        play(15); // C8
        play_length = TMR_MUSIC_SIXTEENTH;
        play(18); // A7
    }
    play(15); // 本当の終止符 C8
    // --- 最終調整：15wordsを使い切る「キメ」 (約14words)
    play_length = TMR_MUSIC_EIGHTH;
    play(30); // C7 (チャッ)
    play(30); // C7 (チャッ)
    play(249); // 休符 (タメ)

    play_length = TMR_MUSIC_QUARTER; // 4分音符に戻る
    play(15); // C8 (チャーン！)
    //    play(249); // 最後の無音      バグ修正でメモリ不足になったので最後の休符は削除する

#endif

#ifdef KITCHEN_RUSH

    // --- メインループ：アラーム全体を2回繰り返す
    for (j = 0; j < 2; j++) {

        // フレーズ1：軽快な三連符風リズム (1オクターブ上げたC7-G6)
        for (i = 0; i < 4; i++) {
            play_length = TMR_MUSIC_EIGHTH;
            play(30); // C7
            play(40); // G6
            play(30); // C7
            play(249); // 8分休符
        }

        // フレーズ2：少し音程を上げて急かす (D7-A6)
        for (i = 0; i < 4; i++) {
            play_length = TMR_MUSIC_EIGHTH;
            play(27); // D7
            play(36); // A6
            play(27); // D7
            play(249); // 8分休符
        }

        // フレーズ3：最高音での警告音 (C8)
        for (i = 0; i < 8; i++) {
            play_length = TMR_MUSIC_SIXTEENTH;
            play(15); // C8
            play(249); // 16分休符
        }
    }

    // --- 締め：完了を知らせるチャイム
    play(20); // G7
    play(24); // E7
    play(30); // C7
    play(249); // 終了

#endif

#ifdef GAMEUP_RUSH

    // --- Part 1: 警告フェーズ (耳を引く)
    for (i = 0; i < 3; i++) {
        play_length = TMR_MUSIC_EIGHTH;
        play(30); // C7
        play(20); // G7
        play(15); // C8
        play(249); // 休符
    }

    // --- Part 2: 加速フェーズ (さらに急かす)
    for (i = 0; i < 6; i++) {
        play_length = TMR_MUSIC_SIXTEENTH;
        play(15); // C8
        play(20); // G7
    }

    // --- 追加：Part 2.5 ゲームミュージック風フレーズ (約50-60 words消費)
    // 少し跳ねるようなリズムのAメロ
    for (i = 0; i < 2; i++) {
        play(40); // G6 (ジャン)
        play_length = TMR_MUSIC_EIGHTH;
        play(47); // E6
        play(45); // F6
        play(40); // G6
        play(249); // 休符

        play_length = TMR_MUSIC_EIGHTH;
        play(30); // C7 (高音でアクセント)
        play(30); // C7
        play(40); // G6
        play(47); // E6
        play(249); // 休符
    }

    // Bメロ：音階が動く疾走感パート
    play_length = TMR_MUSIC_EIGHTH;
    play(40); // G6
    play(36); // A6
    play(34); // A#6
    play(30); // C7
    play(27); // D7
    play(24); // E7
    play(20); // G7
    play(15); // C8

    // --- ここから元のPart 3へ繋ぐ

    // --- Part 3: 完了フレーズ (達成感のあるメロディ)
    // ここで少し速度を落としたように感じさせるため、4分音符を混ぜます
    play(30); // C7
    play(24); // E7
    play(20); // G7
    play_length = TMR_MUSIC_EIGHTH;
    play(15); // C8 (タ)
    play(15); // C8 (タ)
    play(15); // C8 (タン！)

    // --- Part 4: メモリの許す限り「キラキラ音」を追加 (約20words)
    for (i = 0; i < 2; i++) {
        play_length = TMR_MUSIC_SIXTEENTH;
        play(30); // C7
        play(27); // D7
        play(24); // E7
        play(22); // F7
        play(20); // G7
        play(18); // A7
        play(16); // B7
        play(15); // C8
    }

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
    play(249); // 休符
    play(142); // A4
    play(127); // B4
    play(113); // C#5
    play(127); // B4
    play(142); // A4
    play(127); // B4
    play(249); // 休符

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
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(106); // D5
    play(106); // D5
    play(249); // 休符

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
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(119); // C5
    play(119); // C5
    play(249); // 休符

    // --- フィナーレ：高らかに終了 ---
    play(80); // G5
    play(60); // C6
    play(47); // E6
    play(40); // G6
    play(30); // C7
    play(249); // 休符
    play(30); // C7
    play(249); // 休符
    play(30); // C7
    play(30); // C7

#endif

#ifdef COPILOT_ORIGINAL

    play(249); // 休符
    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(63); // B5
    play(71); // A5
    play(80); // G5
    play_length = TMR_MUSIC_SIXTEENTH;
    play(249); // 休符(1/8)
    play(95); // E5
    play(106); // D5
    play(119); // C5
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(159); // G4
    play(127); // B4
    play(119); // C5
    play(249); // 休符
    play(119); // C5
    play(134); // A#4
    play(142); // A4
    play(150); // G#4
    play(159); // G4
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(95); // E5
    play(106); // D5
    play(119); // C5
    play_length = TMR_MUSIC_SIXTEENTH;
    play(249); // 休符(1/8)
    play(119); // C5
    play(150); // G#4
    play(159); // G4
    play(127); // B4
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(249); // 休符
    play(63); // B5
    play(71); // A5
    play(80); // G5
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(95); // E5
    play(106); // D5
    play(119); // C5
    play_length = TMR_MUSIC_SIXTEENTH;
    play(249); // 休符(1/8)
    play(142); // A4
    play(150); // G#4
    play(159); // G4
    play(169); // F#4
    play(179); // F4
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
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
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(60); // C6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)

    play(119); // C5
    play(95); // E5
    play(80); // G5
    play(60); // C6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(60); // C6
    play(249); // 休符

    play(106); // D5
    play(89); // F5
    play(71); // A5
    play(53); // D6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(53); // D6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)

    play(106); // D5
    play(89); // F5
    play(71); // A5
    play(53); // D6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(53); // D6
    play(249); // 休符

    play(95); // E5
    play(80); // G5
    play(60); // C6
    play(47); // E6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(47); // E6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)

    play(89); // F5
    play(71); // A5
    play(53); // D6
    play(45); // F6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(45); // F6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)

    play(80); // G5
    play(60); // C6
    play(47); // E6
    play(40); // G6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(40); // G6
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)

    play(60); // C6
    play(47); // E6
    play(40); // G6
    play(30); // C7
    play_length = TMR_MUSIC_EIGHTH;
    play(249); // 休符(短)
    play(30); // C7
    play(249); // 休符

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

