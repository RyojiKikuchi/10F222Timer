/*
 * ｘｘｘｘｘ
 * T=
 */

/* ============================================================
 *  Const
 * ============================================================ */

#define MUSIC_TEMPO             120

#include "play_music.h"
// TMR_MUSIC_QUARTER       4分音符
// TMR_MUSIC_EIGHTH        8分音符
// TMR_MUSIC_TRIPLET       3連符（1拍3連）
// TMR_MUSIC_SIXTEENTH     16分音符
// TMR_MUSIC_8TRIPLET      3連符（半拍3連）

// TMR_MUSIC_PRESCALER     T=120-   1
//                         T=60-119 2
//                         T=30-59  4

/* ============================================================
 *  Global
 * ============================================================ */
// デフォルトの音符長
static volatile uint8_t play_length_default = TMR_MUSIC_QUARTER;
// 音符の初期値
static volatile uint8_t play_length = TMR_MUSIC_QUARTER;
// 発音毎に音符長をリセットする
static volatile uint8_t play_length_reset = 1;
// 音符の長さのscaler
static volatile uint8_t play_length_scaler = TMR_MUSIC_PRESCALER;
// 発音毎にscalerをリセットする
static volatile uint8_t play_length_scaler_reset = 1;

static void play_music() {

    play(NOTES_RESTS);
    
}

