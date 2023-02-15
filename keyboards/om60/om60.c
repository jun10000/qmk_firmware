/*
Copyright 2021 Salicylic_Acid
Copyright 2022-2023 jun10000

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

#include "om60.h"



static const Position KEYPOS_MAPPING[][8] = {
    { {-1, 0}, { 0, 1}, { 1, 1}, { 2, 1}, { 3, 1}, { 4, 1}, { 5, 1}, { 6, 1} },
    { {-1, 2}, { 0, 2}, { 1, 2}, { 2, 2}, { 3, 2}, { 4, 2}, { 5, 2}, { 6, 2} },
    { {-1, 3}, { 0, 3}, { 1, 3}, { 2, 3}, { 3, 3}, { 4, 3}, { 5, 3}, { 6, 3} },
    { {-1, 4}, { 0, 4}, { 1, 4}, { 2, 4}, { 3, 4}, { 4, 4}, { 5, 4}, { 6, 4} },
    { { 0, 0}, { 1, 0}, { 2, 0}, { 3, 0}, { 4, 0}, { 5, 0}, { 6, 0}, { 7, 0} },
    { { 7, 1}, { 8, 1}, { 9, 1}, {10, 1}, {11, 1}, {12, 1}, {-1,-1}, {-9,-9} },
    { { 7, 2}, { 8, 2}, { 9, 2}, {10, 2}, {11, 2}, {12, 2}, {-9,-9}, {-9,-9} },
    { { 7, 3}, { 8, 3}, { 9, 3}, {10, 3}, {11, 3}, {12, 3}, {-9,-9}, {-9,-9} },
    { { 7, 4}, { 8, 4}, { 9, 4}, {10, 4}, {11, 4}, {12, 4}, {-9,-9}, {-9,-9} },
    { { 8, 0}, { 9, 0}, {10, 0}, {11, 0}, {12, 0}, {-1,-2}, {-9,-9}, {-9,-9} }
};

Position GetReadablePosition(keypos_t pos) {
      uint8_t x = pos.col;
      uint8_t y = pos.row;

      if (y == 252) { x = 5; y = 9; }
      else if (y == 253) { x = 6; y = 5; }

      return KEYPOS_MAPPING[y][x];
}

void keyboard_post_init_user(void) {
      // Customise these values to desired behaviour
      debug_enable=true;
      // debug_matrix=true;
      debug_keyboard=true;
      debug_mouse=true;
}

static OperationModes operation_mode = OM_NORMAL;

#ifdef MINESWEEPER_ENABLE
static Minesweeper *ms;
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
      switch (operation_mode) {
            case OM_NORMAL:
#ifdef MINESWEEPER_ENABLE
                  if (keycode == CK_MDMS && !record->event.pressed) {
                        operation_mode = OM_MINESWEEPER;
                        ms = Minesweeper_Start();
                        return false;
                  }
#endif
                  break;
            case OM_MINESWEEPER:
#ifdef MINESWEEPER_ENABLE
                  Position keypos = GetReadablePosition(record->event.key);

                  if (keypos.x == -1 && keypos.y == 0 && !record->event.pressed) {
                        operation_mode = OM_NORMAL;
                        Minesweeper_End(ms);
                  } else if (record->event.pressed) {
                        Minesweeper_Press(ms, &keypos);
                  }
#endif
                  return false;
      }
      return true;
}
