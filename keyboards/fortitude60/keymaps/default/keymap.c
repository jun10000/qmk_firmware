#include "jun.h"

#define _LOWER 1
#define _RAISE 2

#define LOWER TO(_LOWER)
#define RAISE TO(_RAISE)
#define ENG   KC_LANG2
#define JPN   KC_LANG1
#define SPACE KC_SPC
#define ENTER KC_ENT

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
