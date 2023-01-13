#include QMK_KEYBOARD_H

#define _DEFAULT 0
#define _LOWER   1
#define _RAISE   2
#define _ADJUST  3

#define DEFAULT  TO(_DEFAULT)
#define LOWER    TO(_LOWER)
#define RAISE    TO(_RAISE)
#define ADJUST   TO(_ADJUST)

#define ENG      KC_LANG2
#define JPN      KC_LANG1

#define SENG     LSFT_T(ENG)
#define SJPN     RSFT_T(JPN)
#define SSHOT    SGUI(KC_S)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_DEFAULT] = LAYOUT(
	//,--------+--------+--------+--------+--------+--------.                 ,--------+--------+--------+--------+--------+--------.
		 KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,  KC_EQL,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		 KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_MINS,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		KC_BSLS,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
	//|--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------|
		   SENG,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B, KC_LBRC, KC_RBRC,    KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,    SJPN,
	//`--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
		                  KC_LALT, KC_LGUI,   LOWER,  KC_SPC, KC_LCTL, KC_BSPC,  KC_ENT,   RAISE, KC_CAPS,  KC_DEL
	//                  `--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
	),
	[_LOWER] = LAYOUT(
	//,--------+--------+--------+--------+--------+--------.                 ,--------+--------+--------+--------+--------+--------.
		  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO, QK_BOOT,  EE_CLR,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		_______,   KC_NO,   KC_NO,   KC_UP,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		  KC_NO,   KC_NO, KC_LEFT, KC_DOWN, KC_RGHT,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
	//|--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------|
		_______,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO, _______,
	//`--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
		                  _______, _______, DEFAULT, _______, _______, _______, _______, DEFAULT, _______, _______
	//                  `--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
	),
	[_RAISE] = LAYOUT(
	//,--------+--------+--------+--------+--------+--------.                 ,--------+--------+--------+--------+--------+--------.
		 KC_ESC,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,   KC_NO,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		_______,  KC_F11,  KC_F12,   SSHOT,   KC_NO,   KC_NO,                     KC_NO, KC_PPLS, KC_PMNS, KC_PAST, KC_PSLS,   KC_NO,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		  KC_NO,   KC_P1,   KC_P2,   KC_P3,   KC_P4,   KC_P5,                     KC_P6,   KC_P7,   KC_P8,   KC_P9,   KC_P0,   KC_NO,
	//|--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------|
		_______,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO, KC_PDOT,   KC_NO, _______,
	//`--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
		                  _______, _______, DEFAULT, _______, _______, _______, _______, DEFAULT, _______, _______
	//                  `--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
	),
	[_ADJUST] = LAYOUT(
	//,--------+--------+--------+--------+--------+--------.                 ,--------+--------+--------+--------+--------+--------.
		_______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		_______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
	//|--------+--------+--------+--------+--------+--------|                 |--------+--------+--------+--------+--------+--------|
		_______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
	//|--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------|
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
	//`--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
		                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
	//                  `--------+--------+--------+--------+--------+--------+--------+--------+--------+--------'
	)
};

static bool lower_pressed = false;
static bool raise_pressed = false;
static uint16_t lower_pressed_time = 0;
static uint16_t raise_pressed_time = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
		case LOWER:
			if (record->event.pressed) {
				lower_pressed = true;
				lower_pressed_time = record->event.time;

				layer_off(_RAISE);
				layer_on(_LOWER);
			} else {
				if (!lower_pressed || (TIMER_DIFF_16(record->event.time, lower_pressed_time) >= TAPPING_TERM)) {
					layer_off(_LOWER);
				}

				lower_pressed = false;
			}
			return false;
			break;
		case RAISE:
			if (record->event.pressed) {
				raise_pressed = true;
				raise_pressed_time = record->event.time;

				layer_off(_LOWER);
				layer_on(_RAISE);
			} else {
				if (!raise_pressed || (TIMER_DIFF_16(record->event.time, raise_pressed_time) >= TAPPING_TERM)) {
				    layer_off(_RAISE);
				}

				raise_pressed = false;
			}
			return false;
			break;
		default:
			if (record->event.pressed) {
				lower_pressed = false;
				raise_pressed = false;
			}
			break;
	}
	return true;
}



#ifdef RGBLIGHT_LAYERS

// QMK Colors
// #define HSV_AZURE       132, 102, 255
// #define HSV_BLACK         0,   0,   0
// #define HSV_BLUE        170, 255, 255
// #define HSV_CHARTREUSE   64, 255, 255
// #define HSV_CORAL        11, 176, 255
// #define HSV_CYAN        128, 255, 255
// #define HSV_GOLD         36, 255, 255
// #define HSV_GOLDENROD    30, 218, 218
// #define HSV_GREEN        85, 255, 255
// #define HSV_MAGENTA     213, 255, 255
// #define HSV_ORANGE       21, 255, 255
// #define HSV_PINK        234, 128, 255
// #define HSV_PURPLE      191, 255, 255
// #define HSV_RED           0, 255, 255
// #define HSV_SPRINGGREEN 106, 255, 255
// #define HSV_TEAL        128, 255, 128
// #define HSV_TURQUOISE   123,  90, 112
// #define HSV_WHITE         0,   0, 255
// #define HSV_YELLOW       43, 255, 255
// #define HSV_OFF         HSV_BLACK

#define HSV_WHITE2    0,   0, 128
#define HSV_GREEN2   85, 255, 128
#define HSV_YELLOW2  43, 255, 128
#define HSV_RED2      0, 255, 128

const rgblight_segment_t PROGMEM my_rgb_layer0[] = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_WHITE2});
const rgblight_segment_t PROGMEM my_rgb_layer1[] = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_GREEN2});
const rgblight_segment_t PROGMEM my_rgb_layer2[] = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_YELLOW2});
const rgblight_segment_t PROGMEM my_rgb_layer3[] = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_RED2});

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_rgb_layer0,
    my_rgb_layer1,
    my_rgb_layer2,
    my_rgb_layer3
);

void keyboard_post_init_user(void) {
    rgblight_layers = my_rgb_layers;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(0, layer_state_cmp(state, _DEFAULT));
    return state;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(1, layer_state_cmp(state, _LOWER));
    rgblight_set_layer_state(2, layer_state_cmp(state, _RAISE));
    rgblight_set_layer_state(3, layer_state_cmp(state, _ADJUST));
    return state;
}

#endif
