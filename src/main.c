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
 *  For Assembler
 * ============================================================ */

#define ASM
#define DELAY_ASM
#define WAIT_SECOND_ASM
#define WAIT_BUTTON_ASM
#define TIMER_MAIN_ASM
#define PLAY_ASM

#ifdef ASM
uint8_t v1, v2, v3, v4, v5, v6;
#endif

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

#ifdef WAIT_SECOND_ASM

    // v1: TMR_8MS_LOOP_COUNT待避
    // v2: 1sec計測(125);
    // v3: ボタンチェック

    v1 = TMR_8MS_LOOP_COUNT;
    v2 = 125U;
    v3 = 1;

    // 1secループ先頭
    asm("LOOP_1SEC_BEGIN:");

    asm("CLRF TMR0");
    asm("LOOP_8MS_BEGIN:");

    // TMR0 >= TMR_8MS_LOOP_COUNT (TMR0 >= v1)のチェック(8ms経過したか)
    // TMR0 - v1を行って、マイナスになればループ、0以上ならループ終了
    asm("MOVF _v1, W");
    asm("SUBWF TMR0, W"); // TMR0 - v1
    asm("BTFSS STATUS, 0"); // Cフラグ判定(C=0ならTMR0 < v1, C=1ならTMR0 >= v4)
    // 2msのループ先頭に戻る
    asm("GOTO LOOP_8MS_BEGIN"); // C=0の場合2msのループ継続    

    // キーチェック
    asm("BTFSC GPIO, 3");
    asm("BCF _v3, 0");

    // 1secループ(v2)をデクリメントして0になったらループ終了
    asm("DECFSZ _v2, F");
    asm("GOTO LOOP_1SEC_BEGIN");

    return v3;

#else

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

#endif

}

/*
 * ボタンの状態が変化するまでwait
 * GPIOはプルアップされているので、statusは
 * 
 *   */
static void wait_button(uint8_t status) {

#ifdef WAIT_BUTTON_ASM

    // statusをv2に待避
    asm("MOVWF _v2");

    v1 = BUTTON_PRESS_DETECTION_TMR;

    asm("CLRF TMR0");

    asm("LOOP_BUTTON_WAIT:");

    // ボタン判定
    asm("BTFSS GPIO, 3"); // SWPIN
    asm("GOTO LOOP_SW_0");
    // SW = 1 の処理
    asm("BTFSS _v2, 0");
    asm("CLRF TMR0"); // Zフラグ
    asm("GOTO LOOP_SW_BREAK");
    asm("LOOP_SW_0:");
    // SW = 0 の処理
    asm("BTFSC _v2, 0");
    asm("CLRF TMR0"); // Zフラグ
    asm("LOOP_SW_BREAK:");

    asm("MOVF _v1, W");
    asm("SUBWF TMR0, W"); // TMR0 - v1
    asm("BTFSS STATUS, 0"); // Cフラグ判定(C=0ならTMR0 < v1, C=1ならTMR0 >= v4)
    // 2msのループ先頭に戻る
    asm("GOTO LOOP_BUTTON_WAIT");

#else

    TMR0 = 0;
    while (TMR0 < BUTTON_PRESS_DETECTION_TMR) {
        if (SW_PIN != status) {
            TMR0 = 0;
        }
    }

#endif

}

/*
 *  指定時間タイマー動作する
 *  途中キャンセルされた場合は 1。タイマー完了の場合は 0
 *  1秒はmain側で経過済みのため、最初は59秒とする。
 */
static uint8_t timer_main(uint8_t min) {

#ifdef TIMER_MAIN_ASM

    // v1 ～ v3はwait_second内で使用している
    
    // v4 1分計測
    // v5 指定時間計測
    
    // v4 = min
    asm("MOVWF _v4");

    // v5 = 59
    asm("MOVLW 59");
    asm("MOVWF _v5");

    // 分のループ
    asm("TIMER_MIN_LOOP:");

    // 秒のループ
    asm("TIMER_SEC_LOOP:");

    // LEDを反転
    asm("MOVF GPIO, W");
    asm("XORLW 0x04");
    asm("MOVWF GPIO");

    // 一秒wait
    if (wait_second()) {
        // キャンセルされた
        return 1;
    }

    // 秒減算
    asm("DECFSZ _v5, F");
    asm("GOTO TIMER_SEC_LOOP");

    // v5 = 60
    asm("MOVLW 60");
    asm("MOVWF _v5");

    // 分減算
    asm("DECFSZ _v4, F");
    asm("GOTO TIMER_MIN_LOOP");

#else

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

#endif

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

#ifdef PLAY_ASM
    
    // Cだとループ内の処理がTMR0カウントアップの8usに間に間に合わず、
    // 半周期の計測が遅れて周期が延びてしまう。
    // 改善のためアセンブラに置き換え。

    // v1: 8us計測
    // v2: 半周期計測
    // v3: key待避
    // v4: 2ms計測
    // v5: 音符長のループ
    // v6: scalerのループ

    // 引数(key)を待避。v2(loop)とv3にkey(待避用)を設定
    asm("MOVWF _v2");
    asm("MOVWF _v3");

    // 8us計測用リセット
    asm("CLRF _v1");

    // 音符の場合BUZZERとLEDをON
    asm("XORLW 0xFF");          // key XOR 0xFF
    asm("BTFSC STATUS, 2");     // Zフラグ判定
    asm("GOTO PLAY_INIT_END");  // ゼロならば(休符なら)終了
    asm("MOVLW 0x06");          // BUZZER(0x02),LED(0x04)をONにする
    asm("MOVWF GPIO");          // 0x06をGPIOに設定
    asm("PLAY_INIT_END:");

    // キャンセル済み
    if (is_music_stop) return;
    
    // TMR0リセット
    asm("CLRF TMR0");

    // 2msループするカウンタ待避
    v4 = TMR_MUSIC_2MS_LOOP_COUNT;

    // スケーラーのループ回数を v6 にセット
    asm("MOVF _play_length_scaler, W");
    asm("MOVWF _v6");

    // scaler用のループ先頭
    asm("SCALER_LOOP_START:");

    // 音符長ループ回数(v5)セット
    asm("MOVF _play_length, W");
    asm("MOVWF _v5");

    // 音符長(v5)ループ先頭
    asm("NOTE_LOOP_START:");

    // 2msec(TMR0)ループ先頭
    asm("NOTE_2MS_LOOP_START:");

    // 8us調整。TMR0が変更(TMR0 != v1)されるまでループ
    asm("NOTE_8US_LOOP_START:");
    asm("MOVF _v1, W");
    asm("SUBWF TMR0, W");           // TMR0 - v1
    asm("BTFSC STATUS, 2");         // Zフラグ判定(Z=0なら TMR0 != v1, Z=1なら TMR0 == v1)
    asm("GOTO NOTE_8US_LOOP_START");    // Z=1ならループ継続
    // TMR0をv1に待避
    asm("MOVF TMR0, W");
    asm("MOVWF _v1");

    // v2(半周期計測)をデクリメントして0になったらBUZZERの切替を行う
    // TMR0がインクリメントされる毎にv2(loop)をデクリメントする必要がある
    // 処理が間に合わずにTMR0が進みすぎると半周期の期間が延びて音程が狂う
    // 今のところタブン最大24ticks程度なので間に合っているハズ
    asm("DECFSZ _v2, F");           // v2をデクリメント
    asm("GOTO NOTE_LOOP_BREAK");    // 0以外(半周期たっていない)ならNOTE_LOOP_BREAKへ

    // 半周期経過時の処理
    // BUZZERの状態を反転させてv2(note_tmr)を初期化する

    // v2をリセット。v2にv3(key)を設定する
    asm("MOVF _v3, W");
    asm("MOVWF _v2");

    // 休符(LED OFF)のチェック
    asm("BTFSS GPIO, 2");           // LEDの状態チェック
    asm("GOTO NOTE_LOOP_BREAK");    // 休符ならNOTE_LOOP_BREAKへ

    // BUZZER(GP1)を反転(XOR)
    asm("MOVF GPIO, W");
    asm("XORLW 0x02");
    asm("MOVWF GPIO");

    asm("NOTE_LOOP_BREAK:");

    // キャンセル処理
    asm("BTFSS GPIO, 3");
    asm("GOTO PLAY_CANCEL");
    
    // TMR0 >= TMR_MUSIC_2MS_LOOP_COUNT (TMR0 >= v4)のチェック(2ms経過したか)
    // TMR0 - v4を行って、マイナスになればループ、0以上ならループ終了
    asm("MOVF _v4, W");
    asm("SUBWF TMR0, W");               // TMR0 - v4
    asm("BTFSS STATUS, 0");             // Cフラグ判定(C=0ならTMR0 < v4, C=1ならTMR0 >= v4)
    // 2msのループ先頭に戻る
    asm("GOTO NOTE_2MS_LOOP_START");    // C=0の場合2msのループ継続

    // TMR0初期化
    asm("CLRF TMR0");
    asm("CLRF _v1");
    
    // 音符長(v5)のデクリメント＆ループ終了判定
    asm("DECFSZ _v5, F");
    asm("GOTO NOTE_LOOP_START");        // 0にならなかったら音符長ループ継続

    // scaler(v6)のデクリメント＆ループ終了判定)
    asm("DECFSZ _v6, F");
    asm("GOTO SCALER_LOOP_START");      // 0にならなかったらscalerのループ継続
    
    asm("GOTO PLAY_EXIT");

    asm("PLAY_CANCEL:");
    asm("INCF _is_music_stop");

    asm("PLAY_EXIT:");
    
#else

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

#endif
    
play_exit:
    GPIO = 0x00U;
    if (play_length_reset) {
        play_length = play_length_default;
    }
    if (play_length_scaler_reset) {
        play_length_scaler = TMR_MUSIC_PRESCALER;
    }
}


/* ============================================================
 *  delay
 *   100msのループを何回行うか指定
 *   1 => 100ms
 *   5 => 500ms
 *  10 => 1000ms
 * ============================================================ */
static void delay(uint8_t loop) {

    // 引数 loop は W レジスタに入って渡される（XC8の仕様）
    asm("MOVWF _v1");
    v3 = 125;

    asm("LOOPSTART:");
    // uint8_t wait100ms = 25;
    asm("MOVLW 25");
    asm("MOVWF _v2");

    asm("LOOP100MS:");
    // TMR0 = 0;
    asm("CLRF TMR0");

    asm("LOOPTMR0:");
    // while (TMR0 < _v3); の判定
    // SUBWF は「f - W」を行うため、WにTMR0を、fに_v3(125)を指定します
    asm("MOVF TMR0, W");
    asm("SUBWF _v3, W"); // W = _v3 - TMR0 (125 - TMR0)

    // _v3 - TMR0 の結果とZフラグの挙動：
    // TMR0 < _v3 のとき Z=0 → ループ継続
    // TMR0 = _v3 のとき Z=1 → ループ終了
    asm("BTFSS STATUS, 2"); // Z=1（TMR0==_v3）なら次をスキップしてループ終了
    asm("GOTO LOOPTMR0"); // Z=0（TMR0 < _v3）→ ループ継続

    // wait100ms--
    asm("DECFSZ _v2, F"); // _v2を-1し、0になったら次のGOTOをスキップ
    asm("GOTO LOOP100MS");

    // loop--
    asm("DECFSZ _v1, F"); // _v1を-1し、0になったら次のGOTOをスキップ
    asm("GOTO LOOPSTART");

    asm("LOOPEND:");

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
        // LEDを消灯してボタンが離されるまでwait
        LED_PIN = PIN_LOW;
        wait_button(SW_RELEASE);
        // LEDを点滅させる
        timer_minutes <<= 1;
        while (timer_minutes--) {
            asm("MOVF GPIO, W");
            asm("XORLW 0x04");
            asm("MOVWF GPIO");
            delay(2);
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
        uint8_t i = 20;
        while (i--) {
            asm("MOVF GPIO, W");
            asm("XORLW 0x04");
            asm("MOVWF GPIO");
            delay(1);
        }

        goto go_sleep;

    }

    // プリスケーラを 1:16 に変更
    OPTION = 0b00000011;

    // 音楽再生ｓ
    play_music();

go_sleep:

    // LED OFF
    GPIO = 0;

    // SLEEP前にGPIO読み出し
    (void) GPIO;

    // スリープ
    // スリープ解除後はmain()の先頭から処理が行われる
    SLEEP();

    // returnがないと警告が出るのでreturn記載しておく
    // warning: non-void function does not return a value [-Wreturn-type]
    return EXIT_SUCCESS;

}
