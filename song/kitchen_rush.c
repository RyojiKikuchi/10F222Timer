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
            play(30); // C7
            play(40); // G6
            play(30); // C7
            play(255); // 8分休符
        }

        // フレーズ2：少し音程を上げて急かす (D7-A6)
        for (i = 0; i < 4; i++) {
            play_length = TMR_MUSIC_EIGHTH;
            play(27); // D7
            play(36); // A6
            play(27); // D7
            play(255); // 8分休符
        }

        // フレーズ3：最高音での警告音 (C8)
        for (i = 0; i < 8; i++) {
            play_length = TMR_MUSIC_SIXTEENTH;
            play(15); // C8
            play(255); // 16分休符
        }
    }

    // --- 締め：完了を知らせるチャイム
    play(20); // G7
    play(24); // E7
    play(30); // C7
    play(255); // 終了

}


