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


/* ============================================================
 *  Pin Define
 * ============================================================ */

#define BUZZER_PIN      GPIObits.GP1    // Push-Pull
#define LED_PIN         GPIObits.GP2    // Push-Pull
#define SW_PIN          GPIObits.GP3    // internal pull up

/* ============================================================
 *  Notes
 * ============================================================ */

#define NOTES_RESTS 255U
#define NOTES_B3    253U
#define NOTES_C4    239U
#define NOTES_C4S   225U
#define NOTES_D4    213U
#define NOTES_D4S   201U
#define NOTES_E4    190U
#define NOTES_F4    179U
#define NOTES_F4S   169U
#define NOTES_G4    159U
#define NOTES_G4S   150U
#define NOTES_A4    142U
#define NOTES_A4S   134U
#define NOTES_B4    127U
#define NOTES_C5    119U
#define NOTES_C5S   113U
#define NOTES_D5    106U
#define NOTES_D5S   100U
#define NOTES_E5    95U
#define NOTES_F5    89U
#define NOTES_F5S   84U
#define NOTES_G5    80U
#define NOTES_G5S   75U
#define NOTES_A5    71U
#define NOTES_A5S   67U
#define NOTES_B5    63U
#define NOTES_C6    60U
#define NOTES_C6S   56U
#define NOTES_D6    53U
#define NOTES_D6S   50U
#define NOTES_E6    47U
#define NOTES_F6    45U
#define NOTES_F6S   42U
#define NOTES_G6    40U
#define NOTES_G6S   38U
#define NOTES_A6    36U
#define NOTES_A6S   34U
#define NOTES_B6    32U
#define NOTES_C7    30U
#define NOTES_C7S   28U
#define NOTES_D7    27U
#define NOTES_D7S   25U
#define NOTES_E7    24U
#define NOTES_F7    22U
#define NOTES_F7S   21U
#define NOTES_G7    20U
#define NOTES_G7S   19U
#define NOTES_A7    18U
#define NOTES_A7S   17U
#define NOTES_B7    16U
#define NOTES_C8    15U
#define NOTES_C8S   14U
    
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

