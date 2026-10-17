/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    /* ============================================================
     * テンポ
     * ============================================================ */

#define TMR_MUSIC_QUARTER_DEF       (30000 / MUSIC_TEMPO)
#if TMR_MUSIC_QUARTER_DEF > 500
    // T=30-59  PRESCALER 4
#define TMR_MUSIC_PRESCALER     4
#define TMR_MUSIC_PRESCALER_H   2
#elif TMR_MUSIC_QUARTER_DEF > 250
    // T=60-119 PRESCALER 2
#define TMR_MUSIC_PRESCALER     2
#define TMR_MUSIC_PRESCALER_H   1
#else
    // T=120-   PRESCALER 1
#define TMR_MUSIC_PRESCALER     1
#define TMR_MUSIC_PRESCALER_H   0
#endif

    /* ============================================================
     * 音符長
     * ============================================================ */

#define TMR_MUSIC_QUARTER       (uint8_t) (TMR_MUSIC_QUARTER_DEF / TMR_MUSIC_PRESCALER) // 4分音符
#define TMR_MUSIC_EIGHTH        (uint8_t) (TMR_MUSIC_QUARTER / 2)    // 8分音符
#define TMR_MUSIC_TRIPLET       (uint8_t) (TMR_MUSIC_QUARTER / 3)    // 3連符（1拍3連）
#define TMR_MUSIC_SIXTEENTH     (uint8_t) (TMR_MUSIC_QUARTER / 4)    // 16分音符
#define TMR_MUSIC_8TRIPLET      (uint8_t) (TMR_MUSIC_QUARTER / 6)    // 3連符（半拍3連）

    /* ============================================================
     * Scaler
     * 0.5刻みはT=120未満でのみ使用可能。非推奨
     * ============================================================ */

#define MUSIC_SCALER_0_5X       TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_1X         TMR_MUSIC_PRESCALER
#define MUSIC_SCALER_1_5X       (uint8_t) TMR_MUSIC_PRESCALER + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_2X         (uint8_t) (TMR_MUSIC_PRESCALER * 2)
#define MUSIC_SCALER_2_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 2) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_3X         (uint8_t) (TMR_MUSIC_PRESCALER * 3)
#define MUSIC_SCALER_3_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 3) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_4X         (uint8_t) (TMR_MUSIC_PRESCALER * 4)
#define MUSIC_SCALER_4_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 4) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_5X         (uint8_t) (TMR_MUSIC_PRESCALER * 5)
#define MUSIC_SCALER_5_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 5) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_6X         (uint8_t) (TMR_MUSIC_PRESCALER * 6)
#define MUSIC_SCALER_6_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 6) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_7X         (uint8_t) (TMR_MUSIC_PRESCALER * 7)
#define MUSIC_SCALER_7_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 7) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_8X         (uint8_t) (TMR_MUSIC_PRESCALER * 8)
#define MUSIC_SCALER_8_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 8) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_9X         (uint8_t) (TMR_MUSIC_PRESCALER * 9)
#define MUSIC_SCALER_9_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 9) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_10X         (uint8_t) (TMR_MUSIC_PRESCALER * 10)
#define MUSIC_SCALER_10_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 10) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_11X         (uint8_t) (TMR_MUSIC_PRESCALER * 11)
#define MUSIC_SCALER_11_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 11) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_12X         (uint8_t) (TMR_MUSIC_PRESCALER * 12)
#define MUSIC_SCALER_12_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 12) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_13X         (uint8_t) (TMR_MUSIC_PRESCALER * 13)
#define MUSIC_SCALER_13_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 13) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_14X         (uint8_t) (TMR_MUSIC_PRESCALER * 14)
#define MUSIC_SCALER_14_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 14) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_15X         (uint8_t) (TMR_MUSIC_PRESCALER * 15)
#define MUSIC_SCALER_15_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 15) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_16X         (uint8_t) (TMR_MUSIC_PRESCALER * 16)
#define MUSIC_SCALER_16_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 16) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_17X         (uint8_t) (TMR_MUSIC_PRESCALER * 17)
#define MUSIC_SCALER_17_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 17) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_18X         (uint8_t) (TMR_MUSIC_PRESCALER * 18)
#define MUSIC_SCALER_18_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 18) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_19X         (uint8_t) (TMR_MUSIC_PRESCALER * 19)
#define MUSIC_SCALER_19_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 19) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_20X         (uint8_t) (TMR_MUSIC_PRESCALER * 20)
#define MUSIC_SCALER_20_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 20) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_21X         (uint8_t) (TMR_MUSIC_PRESCALER * 21)
#define MUSIC_SCALER_21_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 21) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_22X         (uint8_t) (TMR_MUSIC_PRESCALER * 22)
#define MUSIC_SCALER_22_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 22) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_23X         (uint8_t) (TMR_MUSIC_PRESCALER * 23)
#define MUSIC_SCALER_23_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 23) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_24X         (uint8_t) (TMR_MUSIC_PRESCALER * 24)
#define MUSIC_SCALER_24_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 24) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_25X         (uint8_t) (TMR_MUSIC_PRESCALER * 25)
#define MUSIC_SCALER_25_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 25) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_26X         (uint8_t) (TMR_MUSIC_PRESCALER * 26)
#define MUSIC_SCALER_26_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 26) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_27X         (uint8_t) (TMR_MUSIC_PRESCALER * 27)
#define MUSIC_SCALER_27_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 27) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_28X         (uint8_t) (TMR_MUSIC_PRESCALER * 28)
#define MUSIC_SCALER_28_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 28) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_29X         (uint8_t) (TMR_MUSIC_PRESCALER * 29)
#define MUSIC_SCALER_29_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 29) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_30X         (uint8_t) (TMR_MUSIC_PRESCALER * 30)
#define MUSIC_SCALER_30_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 30) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_31X         (uint8_t) (TMR_MUSIC_PRESCALER * 31)
#define MUSIC_SCALER_31_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 31) + TMR_MUSIC_PRESCALER_H
#define MUSIC_SCALER_32X         (uint8_t) (TMR_MUSIC_PRESCALER * 32)
#define MUSIC_SCALER_32_5X       (uint8_t) (TMR_MUSIC_PRESCALER * 32) + TMR_MUSIC_PRESCALER_H

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

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */