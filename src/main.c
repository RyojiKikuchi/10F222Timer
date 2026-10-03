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
static volatile uint8_t v1, v2, v3;
static volatile uint8_t v4, v5, v6;

/* ============================================================
 *  Variables
 * ============================================================ */
static volatile uint8_t timer_minutes = 1U;

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

}

/*
 * ボタンの状態が変化するまでwait
 * 
 *   */
static void wait_button(uint8_t status) {

    // statusをv2に待避
    v2 = status;

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

}

/*
 *  指定時間タイマー動作する
 *  途中キャンセルされた場合は 1。タイマー完了の場合は 0
 *  1秒はmain側で経過済みのため、最初は59秒とする。
 */
static uint8_t timer_main(void) {

    // v1 ～ v3はwait_second内で使用している
    // v4 指定時間計測

    // v4 = 59
    asm("MOVLW 59");
    asm("MOVWF _v4");

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
    asm("DECFSZ _v4, F");
    asm("GOTO TIMER_SEC_LOOP");

    // v4 = 60
    asm("MOVLW 60");
    asm("MOVWF _v4");

    // 分減算
    asm("DECFSZ _timer_minutes, F");
    asm("GOTO TIMER_MIN_LOOP");

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

    // Cだとループ内の処理がTMR0カウントアップの8usに間に間に合わず、
    // 半周期の計測が遅れて周期が延びてしまう。
    // 改善のためアセンブラに置き換え。

    // v1: 2ms計測ループカウント待避
    // v2: 半周期(key)計測
    // v3: 前回のTMR0の値
    // v4: 2ms計測
    // v5: 音符長のループ
    // v6: scalerのループ

    // キャンセル済み
    if (is_music_stop) return;
    
    // 引数(key)を待避。
    //   v2:半周期となるTMR0値-1を待避(後段でデクリメント)
    //   v3:前回のTMR0の値
    //   後続処理で (TMR0 - v3) > v2を判定するため、v2を-1しておく
    v2 = key -1;   // 半周期となる値
    v3 = v2;

    // 2msループするカウンタ待避
    //   v1:2msとなるTMR0値-1を待避
    //   v4:前回のTMR0の値
    //   後続処理で (TMR0 - v4) > v1を判定するため、v1を-1しておく
    v1 = TMR_MUSIC_2MS_LOOP_COUNT - 1;
    v4 = v1; 

    // スケーラーのループ回数を v6 にセット
    v6 = play_length_scaler;

    // 音符の場合BUZZERとLEDをON
    asm("MOVF _v2, W");         // key => W
    asm("XORLW 0xFE");          // key XOR 0xFE
    asm("BTFSC STATUS, 2");     // Zフラグ判定
    asm("GOTO PLAY_INIT_END");  // ゼロならば(休符なら)終了
    asm("MOVLW 0x06");          // BUZZER(0x02),LED(0x04)をONにする
    asm("MOVWF GPIO");          // 0x06をGPIOに設定
    asm("PLAY_INIT_END:");
    
    // v3(半周期計測),v4(2ms計測)初期値設定
    asm("MOVF TMR0, W");
    asm("MOVWF _v3");
    asm("MOVWF _v4");
    
    // scaler用のループ先頭
    asm("SCALER_LOOP_START:");

    // 音符長ループ回数(v5)セット
    asm("MOVF _play_length, W");
    asm("MOVWF _v5");

    // 音符長(v5)ループ先頭
    asm("NOTE_LOOP_START:");

    // 2msec(TMR0)ループ先頭
    asm("NOTE_2MS_LOOP_START:");

    // TMR0の値が半周期後の値になったらBUZZERの切替を行う
    asm("MOVF _v3, W");             // v3 => W
    asm("SUBWF TMR0, W");           // TMR0 - W
    asm("SUBWF _v2, W");            // (TMR0 - v3) - v2 > 0
    asm("BTFSC STATUS, 0");         // Cフラグ判定
    asm("GOTO NOTE_LOOP_BREAK");    // ループ継続

    // v3設定。v3 +=  key
    asm("MOVF _v2, W");
    asm("ADDWF _v3, F");

    // 半周期経過時の処理

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
    
    // 2ms経過したかチェック
    // TMR0 - v4(前回のタイマー値) が 250以上ならループ終了
    asm("MOVF _v4, W");                 // v4 => W
    asm("SUBWF TMR0, W");               // TMR0 - W
    asm("SUBWF _v1, W");                // (TMR0 - v4) - v1 > 0
    asm("BTFSC STATUS, 0");             // Cフラグ判定
    // 2msのループ先頭に戻る
    asm("GOTO NOTE_2MS_LOOP_START");    // ループ継続

    // v4設定。v4 += TMR_MUSIC_2MS_LOOP_COUNT
    asm("MOVF _v1, W");
    asm("ADDWF _v4, F");

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
    
/*
    // 半周期計測用
    uint8_t note_tmr = key;
    uint8_t prev_tmr = 0;

    // scaler設定
    uint8_t scaler = play_length_scaler;

    TMR0 = 0;
    // scalerのループ
    while (scaler--) {

        // 音符長分のループ
        uint8_t loop = play_length;
        while (loop--) {

            // 2ms分のループ
            while (TMR0 < TMR_MUSIC_2MS_LOOP_COUNT) {

                // 半周期たったらBUZZERの状態を反転させて note_tmr を初期化する
                // 処理が間に合わずにnote_tmrがデクリメントされたときにTMR0が2進むことがある
                // そのため周期が延びて音程がずれる＆音が濁る
                if (key != NOTES_RESTS && !--note_tmr) {
                    note_tmr = key;
                    BUZZER_PIN = ~BUZZER_PIN;
                    LED_PIN = PIN_HIGH;
                }

                // TMR0が更新するまでwait
                while (prev_tmr == TMR0);
                prev_tmr = TMR0;

            }

            TMR0 = 0;
            prev_tmr = 0;

        }

    }
*/    

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
 * ADConverterの結果判定
 */
static void check_adres(uint8_t v) {
    asm("SUBWF ADRES, W"); // ADRES - W
    asm("BTFSC STATUS, 0"); // Cフラグ判定(ADRES - W) >= 0
    asm("INCF _timer_minutes, F"); // _timer_minutes++
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
    asm("BTFSC STATUS, 7"); // GPWFフラグ = 0ならスリープ
    asm("BTFSC GPIO, 3"); // SW=0ならスリープ
    asm("GOTO GO_SLEEP");

    // LED点灯
    asm("BSF GPIO, 2");

    // AN0の電圧からタイマーの時間を取得
    // ADC ON
    asm("BSF ADCON0, 0"); // ADON = 1
    // アクイジションタイム(10us)
    __delay_us(10);
    // 変換開始
    asm("BSF ADCON0, 1"); // GO = 1
    // 変換終了wait
    asm("ADC_LOOP:");
    asm("BTFSC ADCON0, 1"); // while(DONE == 1)
    asm("GOTO ADC_LOOP");
    // ADC OFF
    asm("BCF ADCON0, 0"); // ADON = 0

    // ADCの値からタイマーの時間を決定する

    // timer_minutes = 1;
    // if (ADRES - 0x33 >= 0) timer_minutes++;
    check_adres(0x33U);
    // if (ADRES - 0x66 >= 0) timer_minutes++;
    check_adres(0x66U);
    // if (ADRES - 0x99 >= 0) timer_minutes++;
    check_adres(0x99U);
    // if (ADRES - 0xCC >= 0) timer_minutes++;
    check_adres(0xCCU);

#if VOL_REVERSE

    // PCB作成誤りで半固定抵抗の極性が誤っているため値を反転する
    //timer_minutes = 6 - timer_minutes;
    asm("MOVLW 6");
    asm("MOVWF _v1");
    asm("MOVF _timer_minutes, W");
    asm("SUBWF _v1, W");
    asm("MOVWF _timer_minutes");

#endif

    // 最初の1秒経過後にボタンが押されていた場合はタイマーの時間確認のため、設定時間をLEDの点滅で通知する
    wait_second();

    asm("BTFSC GPIO, 3");
    asm("GOTO TIMER_START"); // SWが押下されていたらタイマー設定を表示

    // LEDを消灯してボタンが離されるまでwait
    asm("BCF GPIO, 2");
    wait_button(SW_RELEASE);

    // LEDを設定時間分点滅させる
    asm("DECF _timer_minutes, F");
    asm("RLF _timer_minutes, F");
    asm("LED_TIMERSETTING_BLINK:");
    asm("MOVF GPIO, W");
    asm("XORLW 0x04");
    asm("MOVWF GPIO");
    delay(2);
    asm("DECFSZ _timer_minutes, F");
    asm("GOTO LED_TIMERSETTING_BLINK");
    asm("GOTO GO_SLEEP");

    // タイマースタート
    asm("TIMER_START:");

    // タイマー処理呼び出し
    if (timer_main()) {
        // キャンセルされた場合

        // LED ON
        //LED_PIN = PIN_HIGH;
        asm("BSF GPIO, 2");

        // ボタンが離されるまで待つ
        wait_button(SW_RELEASE);

        // LEDを2秒間点滅させる
        v4 = 20;
        asm("LED_CANCEL_BLINK:");
        asm("MOVF GPIO, W");
        asm("XORLW 0x04");
        asm("MOVWF GPIO");
        delay(1);
        asm("DECFSZ _v4, F");
        asm("GOTO LED_CANCEL_BLINK");

        asm("GOTO GO_SLEEP");
    }

    // プリスケーラを 1:16 に変更
    OPTION = 0b00000011;

    // 音楽再生
    play_music();

    // プリスケーラを 1:64 に変更
    OPTION = 0b00000101;

    // 100ms wait
    delay(1);

    // ボタンが離されるまで待つ
    wait_button(SW_RELEASE);

    asm("GO_SLEEP:");

    // LED OFF
    asm("CLRF GPIO");

    // SLEEP前にGPIO読み出し
    asm("MOVF GPIO, W");

    // スリープ
    // スリープ解除後はmain()の先頭から処理が行われる
    asm("SLEEP");

    // returnがないと警告が出るのでreturn記載しておく
    // warning: non-void function does not return a value [-Wreturn-type]
    return EXIT_SUCCESS;

}
