/*
 * ラーメン完成！歓喜のチャルメラ
 * T=150
 */

/* ============================================================
 *  Const
 * ============================================================ */

#define MUSIC_TEMPO             150

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
    play_length = TMR_MUSIC_EIGHTH;
    play(255); // 休符(短)
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
    play_length = TMR_MUSIC_EIGHTH;
    play(255); // 休符(短)
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
    
}


