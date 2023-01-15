/*
Copyright 2023 jun10000

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utility.h"



void PrintError(const char *message) {
#ifdef CONSOLE_ENABLE
      uprintf("ERROR: %s\n", message);
#endif
}

void PrintIfError(bool exp, const char *message) {
#ifdef CONSOLE_ENABLE
      if (!exp) {
            PrintError(message);
      }
#endif
}

int8_t GetRGBIndex(Position pos) {
      if (pos.x == -1 && (pos.y < 0 || pos.y == 1 || pos.y > 4)) { return -1; }
      else if (pos.x < -1 || pos.x >= 13) { return -1; }
      else if (pos.y < 0 || pos.y >= 5) { return -1; }

      if (pos.x == -1 && pos.y == 0) { return 0; }

      int8_t ret = pos.x * 5 + pos.y + 4;
      if (ret < 0 || ret >= RGBLED_NUM) {
            return -1;
      }

      return ret;
}

bool SetRGBBuffer(uint8_t r, uint8_t g, uint8_t b, Position pos) {
      int8_t led_index = GetRGBIndex(pos);
      if (led_index == -1) { return false; }

      setrgb(r, g, b, (LED_TYPE *)&led[led_index]);
      return true;
}

bool SetRGBBufferRange(uint8_t r, uint8_t g, uint8_t b, Position startpos, Position endpos) {
      for (int8_t x = startpos.x; x <= endpos.x; x++) {
            for (int8_t y = startpos.y; y <= endpos.y; y++) {
                  if (!SetRGBBuffer(r, g, b, (Position){x, y})) { return false; }
            }
      }

      return true;
}

void ApplyRGBBuffer(void) {
      rgblight_set();
}

void GetRandomBoolArray(bool input[], uint8_t length, uint8_t trueCount) {
	GetRandomBoolArrayWithSeed(input, length, trueCount, (unsigned int)time(NULL));
}

void GetRandomBoolArrayWithSeed(bool input[], uint8_t length, uint8_t trueCount, unsigned int seed) {
	srand(seed);

      for (uint8_t i = 0; i < length; i++) {
            input[i] = false;
      }

	uint8_t c = 0;
	while (c < trueCount) {
		uint8_t index = rand() % length;
		if (!input[index]) {
			input[index] = true;
			c++;
		}
	}
}
