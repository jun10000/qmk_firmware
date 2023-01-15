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

#pragma once

#include <stdlib.h>
#include <time.h>
#include "quantum.h"



typedef struct {
      int8_t x;
      int8_t y;
} Position;



void PrintError(const char *message);
void PrintIfError(bool exp, const char *message);
int8_t GetRGBIndex(Position pos);
bool SetRGBBuffer(uint8_t r, uint8_t g, uint8_t b, Position pos);
bool SetRGBBufferRange(uint8_t r, uint8_t g, uint8_t b, Position startpos, Position endpos);
void ApplyRGBBuffer(void);

void GetRandomBoolArray(bool input[], uint8_t length, uint8_t trueCount);
void GetRandomBoolArrayWithSeed(bool input[], uint8_t length, uint8_t trueCount, unsigned int seed);
