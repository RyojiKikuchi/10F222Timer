/*
 * PIC10F222 Timer
 * 1～5分計測するタイマー
 * 
 * GP0 ADC入力(analog)
 * GP1 ブザー出力(push pull)
 * GP2 LED出力(push pull)
 * GP3 スイッチ入力(pull-up)
 * 
 */

/* ============================================================
 *  Include
 * ============================================================ */
#include "main.h"

/* ============================================================
 *  Song Include
 * ============================================================ */
//#include "..\song\music_skeleton.c"           // 
//#include "..\song\gameup_rush.c"              //  Gameup Rush
#include "..\song\kitchen_rush.c"             //  Kitchen Rush
//#include "..\song\ramen.c"                    //  ラーメン完成！歓喜のチャルメラ

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

/* 1秒間WAIT
 * GP3が押され続けた場合は 1 を返却
 *  */
static uint8_t wait_second() {

    uint8_t sw_push = 1;
    
    /* 
     * 8MHz / 4 = 2MHz = 0.5us
     * プリスケーラ 1:64なので、TMR0は 0.5us * 64 = 32us 毎にカウントアップ
     * 32us * 250 = 8000us = 8ms = 250回ループで8msとなる
     * 8ms * 125 = 1000ms 
     * 合計で 250 * 125 のループで 1sec となる
     */
    uint8_t loop = 125U;
    while (loop--) {
        // 8msecのループ
        // 32us * 250 = 8ms loop
        TMR0 = 0;
        while (TMR0 < TMR_8MS_LOOP_COUNT);
        if (SW_PIN == SW_RELEASE) {
            sw_push = 0;
        }
    }

    return sw_push;
    
}

/*
 * ボタンの状態が変化するまでwait
 * GPIOはプルアップされているので、statusは
 * 
 *   */
static void wait_button(uint8_t status) {
    TMR0 = 0;
    while (TMR0 < BUTTON_PRESS_DETECTION_TMR) {
        if (SW_PIN != status) {
            TMR0 = 0;
        }
    }
}

/*
 *  指定時間タイマー動作する
 *  途中キャンセルされた場合は 1。タイマー完了の場合は 0
 *  1秒はmain側で経過済みのため、最初は59秒とする。
 */
static uint8_t timer_main(uint8_t min) {
    uint8_t sec = 59U;
    while (min--) {
        while (sec--) {
            LED_PIN = sec & 0x01U;
            if (wait_second()) {
                // キャンセルされた
                return 1;
            }
        }
        sec = 60U;
    }
    return 0;
}

/*
 * 音楽再生 
 * key で半周期分となるTMR0のカウント値を指定(1:16プリスケーラ(8us)を何回繰り返すか)
 * そのまま呼び出せば 4分音符 の長さで発音
 * 4分音符以外の場合は音符の長さは play_length に以下を設定。
 *     TMR_MUSIC_QUARTER   4分音符(デフォルト)
 *     TMR_MUSIC_EIGHTH    8分音符
 *     TMR_MUSIC_SIXTEENTH 16分音符
 *  play_length はplay()内で4分音符に初期化
 * 
 *  key = 255 は休符
 * 
 *  */
static void play(uint8_t key) {

    if (is_music_stop) return;
    
    // scaler設定
    uint8_t scaler = play_length_scaler;

    // 半周期計測用
    uint8_t note_tmr = 0U;

    TMR0 = 0;
    // scalerのループ
    while (scaler--) {

        // 音符長分のループ
        uint8_t loop = play_length;
        while (loop--) {

            // 2ms分のループ
            while (TMR0 < TMR_MUSIC_2MS_LOOP_COUNT) {
                uint8_t prev_tmr = TMR0;
                // 半周期たったらBUZZERの状態を反転させて note_tmr を初期化する
                if (key != NOTES_RESTS && note_tmr >= key) {
                    note_tmr = 0U;
                    BUZZER_PIN = ~BUZZER_PIN;
                    LED_PIN = PIN_HIGH;
                }
                if (SW_PIN == SW_PUSH) {
                    is_music_stop = 1;
                    goto play_exit;
                }                
                // TMR0が更新するまでwait
                while (prev_tmr == TMR0);
                // TMR0が更新される毎に半周期計測用の note_tmr をインクリメントする
                note_tmr++;
            }
            TMR0 = 0;

        }
    }

play_exit:
    if (play_length_reset) {
        play_length = play_length_default;
    }
    if (play_length_scaler_reset) {
        play_length_scaler = TMR_MUSIC_PRESCALER;
    }
    GPIO = 0x00U;
}

/*
 * main
 */
int main(void) {

    // クロック校正値をOSCCALに設定するオプションを有効化する
    // ver6.30での設定
    // XC8 Linker=>Runtime
    //  Calibrate oscillator をチェック
    //  Alternate oscillator calibration value をクリア

    // 初期化
    system_init();

    // スリープ解除ではない場合、またはSWが押されていない場合はスリープする
    if (!STATUSbits.GPWUF || SW_PIN == SW_RELEASE) {
        goto go_sleep;
    }

    // LED点灯
    LED_PIN = PIN_HIGH;

    // AN0の電圧からタイマーの時間を取得
    // ADC ON
    ADCON0bits.ADON = 1;
    // アクイジションタイム(10us)
    __delay_us(10);
    // 変換開始
    ADCON0bits.GO = 1;
    // 変換終了wait
    while (ADCON0bits.nDONE == 1);
    // ADC OFF
    ADCON0bits.ADON = 0;

    // ADCの値からタイマーの時間を決定する
    uint8_t timer_minutes = 5U;
    if (ADRES <= 0x33U) {
        timer_minutes = 1U;
    } else if (ADRES <= 0x66U) {
        timer_minutes = 2U;
    } else if (ADRES <= 0x99U) {
        timer_minutes = 3U;
    } else if (ADRES <= 0xCCU) {
        timer_minutes = 4U;
    }

    // 最初の1秒経過後にボタンが押されていた場合はタイマーの時間確認のため、設定時間をLEDの点滅で通知する
    wait_second();
    if (SW_PIN == SW_PUSH) {
        // LEXを消灯してボタンが離されるまでwait
        LED_PIN = PIN_LOW;
        wait_button(SW_RELEASE);
        // LEDを点滅させる
        timer_minutes <<= 1;
        while (timer_minutes--) {
            LED_PIN = ~LED_PIN;
            __delay_ms(200);
        }
        goto go_sleep;
    }

    // タイマー処理呼び出し
    if (timer_main(timer_minutes)) {
        // キャンセルされた場合

        // LED ON
        LED_PIN = PIN_HIGH;

        // ボタンが離されるまで待つ
        wait_button(SW_RELEASE);

        // LEDを2秒間点滅させる
        for (uint8_t i = 0; i < 40; i++) {
            LED_PIN = ~LED_PIN;
            __delay_ms(50);
        }

        goto go_sleep;

    }

    // プリスケーラを 1:16 に変更
    OPTION = 0b00000011;

    // 音楽再生ｓ
    play_music();

go_sleep:

    // LED OFF
    LED_PIN = PIN_LOW;

    // SLEEP前にGPIO読み出し
    (void)GPIO;
    
    // スリープ
    // スリープ解除後はmain()の先頭から処理が行われる
    SLEEP();

    // returnがないと警告が出るのでreturn記載しておく
    // warning: non-void function does not return a value [-Wreturn-type]
    return EXIT_SUCCESS;

}
