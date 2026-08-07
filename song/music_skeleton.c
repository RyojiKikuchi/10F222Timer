/*
 * ｘｘｘｘｘ
 * T=
 * TMR_MUSIC_QUARTER    250U
 * TMR_MUSIC_PRESCALER  1U
 */

/* ============================================================
 *  Const
 * ============================================================ */
#define TMR_MUSIC_QUARTER       250U    // T=120
//#define TMR_MUSIC_QUARTER       240U    // T=125
//#define TMR_MUSIC_QUARTER       200U    // T=150
//#define TMR_MUSIC_QUARTER       190U    // T=160
//#define TMR_MUSIC_QUARTER       176U    // T=170
//#define TMR_MUSIC_QUARTER       166U    // T=180
//#define TMR_MUSIC_QUARTER       158U    // T=190
//#define TMR_MUSIC_QUARTER       150U    // T=200
//#define TMR_MUSIC_QUARTER       143U    // T=210
//#define TMR_MUSIC_QUARTER       136U    // T=220
//#define TMR_MUSIC_QUARTER       125U    // T=240

// プリスケーラー 2を設定すると音の長さが*2になりテンポが1/2になる
// T=120より遅いテンポを設定する場合に使用する
#define TMR_MUSIC_PRESCALER     1U
//#define TMR_MUSIC_PRESCALER     2U

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

    play(NOTES_RESTS);
    
}

