/*
 * ラーメン完成！歓喜のチャルメラ
 * T=150
 * TMR_MUSIC_QUARTER    200U
 * TMR_MUSIC_PRESCALER  1U
 */

/* ============================================================
 *  Const
 * ============================================================ */
#define TMR_MUSIC_QUARTER       200U    // T=150

// プリスケーラー 2を設定すると音の長さが*2になりテンポが1/2になる
// T=120より遅いテンポを設定する場合に使用する
#define TMR_MUSIC_PRESCALER     1U

#define TMR_MUSIC_EIGHTH        (uint8_t)(TMR_MUSIC_QUARTER / 2U)    // 8分音符
#define TMR_MUSIC_TRIPLET       (uint8_t)(TMR_MUSIC_QUARTER / 3U)    // 3連符（1拍3連）
#define TMR_MUSIC_SIXTEENTH     (uint8_t)(TMR_MUSIC_QUARTER / 4U)    // 16分音符
#define TMR_MUSIC_8TRIPLET      (uint8_t)(TMR_MUSIC_QUARTER / 6U)    // 3連符（半拍3連）

/* ============================================================
 *  Global
 * ============================================================ */
// デフォルトの音符長
static uint8_t play_length_default = TMR_MUSIC_QUARTER;
// 音符の初期値
static uint8_t play_length = TMR_MUSIC_QUARTER;
// 発音毎に音符長をリセットする
static uint8_t play_length_reset = 1;
// 音符の長さのscaler
static uint8_t play_length_scaler = TMR_MUSIC_PRESCALER;
// 発音毎にscalerをリセットする
static uint8_t play_length_scaler_reset = 1;

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


