/*
 * Kitchen Rush
 * T=180
 * TMR_MUSIC_QUARTER    166U
 * TMR_MUSIC_PRESCALER  1U
 */

/* ============================================================
 *  Const
 * ============================================================ */
#define TMR_MUSIC_QUARTER       166U    // T=180

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

    uint8_t i, j;

    // --- メインループ：アラーム全体を2回繰り返す
    for (j = 0; j < 2; j++) {

        // フレーズ1：軽快な三連符風リズム (1オクターブ上げたC7-G6)
        for (i = 0; i < 4; i++) {
            play_length = TMR_MUSIC_EIGHTH;
            play(NOTES_C7); // ド
            play(NOTES_G6); // ソ
            play(NOTES_C7); // ド
            play(NOTES_RESTS); // 休符
        }

        // フレーズ2：少し音程を上げて急かす (D7-A6)
        for (i = 0; i < 4; i++) {
            play_length = TMR_MUSIC_EIGHTH;
            play(NOTES_D7); // レ
            play(NOTES_A6); // ラ
            play(NOTES_D7); // レ
            play(NOTES_RESTS); // 休符
        }

        // フレーズ3：最高音での警告音 (C8)
        for (i = 0; i < 8; i++) {
            play_length = TMR_MUSIC_SIXTEENTH;
            play(NOTES_C8); // ド
            play(NOTES_RESTS); // 休符
        }
    }

    // --- 締め：完了を知らせるチャイム
    play(NOTES_G7); // ソ
    play(NOTES_E7); // ミ
    play(NOTES_C7); // ド
    play(NOTES_RESTS); // 休符

}


