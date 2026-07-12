/*
 * GAMEUP RUSH
 * T=210
 * TMR_MUSIC_QUARTER    143U
 * TMR_MUSIC_PRESCALER  1U
 */

/* ============================================================
 *  Const
 * ============================================================ */
#define TMR_MUSIC_QUARTER       143U    // T=210

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

    uint8_t i;
    
    // --- Part 1: 警告フェーズ (耳を引く)
    for (i = 0; i < 3; i++) {
        play_length = TMR_MUSIC_EIGHTH;
        play(30); // C7
        play(20); // G7
        play(15); // C8
        play(255); // 休符
    }

    // --- Part 2: 加速フェーズ (さらに急かす)
    for (i = 0; i < 6; i++) {
        play_length = TMR_MUSIC_SIXTEENTH;
        play(15); // C8
        play(20); // G7
    }

    // --- 追加：Part 2.5 ゲームミュージック風フレーズ (約50-60 words消費)
    // 少し跳ねるようなリズムのAメロ
    for (i = 0; i < 2; i++) {
        play(40); // G6 (ジャン)
        play_length = TMR_MUSIC_EIGHTH;
        play(47); // E6
        play(45); // F6
        play(40); // G6
        play(255); // 休符

        play_length = TMR_MUSIC_EIGHTH;
        play(30); // C7 (高音でアクセント)
        play(30); // C7
        play(40); // G6
        play(47); // E6
        play(255); // 休符
    }

    // Bメロ：音階が動く疾走感パート
    play_length = TMR_MUSIC_EIGHTH;
    play(40); // G6
    play(36); // A6
    play(34); // A#6
    play(30); // C7
    play(27); // D7
    play(24); // E7
    play(20); // G7
    play(15); // C8

    // --- ここから元のPart 3へ繋ぐ

    // --- Part 3: 完了フレーズ (達成感のあるメロディ)
    // ここで少し速度を落としたように感じさせるため、4分音符を混ぜます
    play(30); // C7
    play(24); // E7
    play(20); // G7
    play_length = TMR_MUSIC_EIGHTH;
    play(15); // C8 (タ)
    play(15); // C8 (タ)
    play(15); // C8 (タン！)

    // --- Part 4: メモリの許す限り「キラキラ音」を追加 (約20words)
    for (i = 0; i < 2; i++) {
        play_length = TMR_MUSIC_SIXTEENTH;
        play(30); // C7
        play(27); // D7
        play(24); // E7
        play(22); // F7
        play(20); // G7
        play(18); // A7
        play(16); // B7
        play(15); // C8
    }
    
}

