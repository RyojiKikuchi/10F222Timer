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

