/* 
 * File:   main.h
 * Author: r-kik
 *
 * Created on 2026/07/13, 9:24
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

    /* ============================================================
     *  Include
     * ============================================================ */
#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

    /* ============================================================
     *  Configuration bits
     * ============================================================ */

    // CONFIG
#pragma config IOSCFS = 8MHZ    // Internal Oscillator Frequency Select bit (8 MHz)
#pragma config MCPU = ON        // Master Clear Pull-up Enable bit (Pull-up enabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config CP = ON          // Code protection bit (Code protection on)
#pragma config MCLRE = OFF      // GP3/MCLR Pin Function Select bit (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)

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

    /*  */
#define BUTTON_PRESS_DETECTION_TMR  250U    // ボタンのチャタリング防止のため、ボタン押下判定するタイマー値(8ms,PSA 1:64)
#define TMR_MUSIC_2MS_LOOP_COUNT  250U      // 音楽再生用のタイマー値(2ms,PSA 1:16)

#define SW_PUSH         0U
#define SW_RELEASE      1U
#define PIN_LOW         0U         
#define PIN_HIGH        1U

#ifndef VOL_REVERSE
#define VOL_REVERSE 0                       // ボリューム極性誤り対応
#endif

    /* ============================================================
     *  Pin Define
     * ============================================================ */

#define BUZZER_PIN      GPIObits.GP1    // Push-Pull
#define LED_PIN         GPIObits.GP2    // Push-Pull
#define SW_PIN          GPIObits.GP3    // internal pull up

    /* ============================================================
     *  Notes
     * ============================================================ */

#define NOTES_RESTS 255U    // 休符
#define NOTES_B3    253U    // シ　 247.036(246.942)Hz
#define NOTES_C4    239U    // ド　 261.506(261.626)Hz
#define NOTES_C4S   225U    // 　　 277.778(277.183)Hz
#define NOTES_D4    213U    // レ　 293.427(293.665)Hz
#define NOTES_D4S   201U    // 　　 310.945(311.127)Hz
#define NOTES_E4    190U    // ミ　 328.947(329.628)Hz
#define NOTES_F4    179U    // ファ 349.162(349.228)Hz
#define NOTES_F4S   169U    // 　　 369.822(369.994)Hz
#define NOTES_G4    159U    // ソ　 393.082(391.995)Hz
#define NOTES_G4S   150U    // 　　 416.667(415.305)Hz
#define NOTES_A4    142U    // ラ　 440.141(440.000)Hz
#define NOTES_A4S   134U    // 　　 466.418(466.164)Hz
#define NOTES_B4    127U    // シ　 492.126(493.883)Hz
#define NOTES_C5    119U    // ド　 525.210(523.251)Hz
#define NOTES_C5S   113U    // 　　 553.097(554.365)Hz
#define NOTES_D5    106U    // レ　 589.623(587.330)Hz
#define NOTES_D5S   100U    // 　　 625.000(622.254)Hz
#define NOTES_E5    95U     // ミ　 657.895(659.255)Hz
#define NOTES_F5    89U     // ファ 702.247(698.456)Hz
#define NOTES_F5S   84U     // 　　 744.048(739.989)Hz
#define NOTES_G5    80U     // ソ　 781.250(783.991)Hz
#define NOTES_G5S   75U     // 　　 833.333(830.609)Hz
#define NOTES_A5    71U     // ラ　 880.282(880.000)Hz
#define NOTES_A5S   67U     // 　　 932.836(932.328)Hz
#define NOTES_B5    63U     // シ　 992.063(987.767)Hz
#define NOTES_C6    60U     // ド　 1041.667(1046.502)Hz
#define NOTES_C6S   56U     // 　　 1116.071(1108.731)Hz
#define NOTES_D6    53U     // レ　 1179.245(1174.659)Hz
#define NOTES_D6S   50U     // 　　 1250.000(1244.508)Hz
#define NOTES_E6    47U     // ミ　 1329.787(1318.510)Hz
#define NOTES_F6    45U     // ファ 1388.889(1396.913)Hz
#define NOTES_F6S   42U     // 　　 1488.095(1479.978)Hz
#define NOTES_G6    40U     // ソ　 1562.500(1567.982)Hz
#define NOTES_G6S   38U     // 　　 1644.737(1661.219)Hz
#define NOTES_A6    36U     // ラ　 1736.111(1760.000)Hz
#define NOTES_A6S   34U     // 　　 1838.235(1864.655)Hz
#define NOTES_B6    32U     // シ　 1953.125(1975.533)Hz
#define NOTES_C7    30U     // ド　 2083.333(2093.005)Hz
#define NOTES_C7S   28U     // 　　 2232.143(2217.461)Hz
#define NOTES_D7    27U     // レ　 2314.815(2349.318)Hz
#define NOTES_D7S   25U     // 　　 2500.000(2489.016)Hz
#define NOTES_E7    24U     // ミ　 2604.167(2637.020)Hz
#define NOTES_F7    22U     // ファ 2840.909(2793.826)Hz
#define NOTES_F7S   21U     // 　　 2976.190(2959.955)Hz
#define NOTES_G7    20U     // ソ　 3125.000(3135.963)Hz
#define NOTES_G7S   19U     // 　　 3289.474(3322.438)Hz
#define NOTES_A7    18U     // ラ　 3472.222(3520.000)Hz
#define NOTES_A7S   17U     // 　　 3676.471(3729.310)Hz
#define NOTES_B7    16U     // シ　 3906.250(3951.066)Hz
#define NOTES_C8    15U     // ド　 4166.667(4186.009)Hz
#define NOTES_C8S   14U     // 　　 4464.286(4434.922)Hz

    // ♭系
#define NOTES_D4F   NOTES_C4S
#define NOTES_E4F   NOTES_D4S
#define NOTES_G4F   NOTES_F4S
#define NOTES_A4F   NOTES_G4S
#define NOTES_B4F   NOTES_A4S
#define NOTES_D5F   NOTES_C5S
#define NOTES_E5F   NOTES_D5S
#define NOTES_G5F   NOTES_F5S
#define NOTES_A5F   NOTES_G5S
#define NOTES_B5F   NOTES_A5S
#define NOTES_D6F   NOTES_C6S
#define NOTES_E6F   NOTES_D6S
#define NOTES_G6F   NOTES_F6S
#define NOTES_A6F   NOTES_G6S
#define NOTES_B6F   NOTES_A6S
#define NOTES_D7F   NOTES_C7S
#define NOTES_E7F   NOTES_D7S
#define NOTES_G7F   NOTES_F7S
#define NOTES_A7F   NOTES_G7S
#define NOTES_B7F   NOTES_A7S
#define NOTES_D8F   NOTES_C8S    

    /* ============================================================
     *  Global
     * ============================================================ */

    static uint8_t is_music_stop = 0;

    /* ============================================================
     *  prototype
     * ============================================================ */
    static void play(uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */
