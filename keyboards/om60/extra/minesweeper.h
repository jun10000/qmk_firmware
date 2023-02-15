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

// Codes ported from "https://github.com/jun10000/Minesweeper" (Ver 1.0.0) are included.

#pragma once

#ifdef MINESWEEPER_ENABLE

#include <stdlib.h>
#include "quantum.h"
#include "utility.h"



#define MSWIDTH     13
#define MSHEIGHT    5
#define MSBOMBS     15
#define MSSEED      0

#define MSCOLOR_ON          128, 128, 128
#define MSCOLOR_OFF         RGB_OFF
#define MSCOLOR_SETBOMB     128,   0,   0
#define MSCOLOR_SETSEED     128, 128,   0
#define MSCOLOR_CM_OPEN     128, 128, 128
#define MSCOLOR_CM_MARKB    255,  64,  64
#define MSCOLOR_CM_MARKQ     64, 255,  64
#define MSCOLOR_BOMB        255,   0,   0
#define MSCOLOR_NEARBOMB0   RGB_OFF
#define MSCOLOR_NEARBOMB1     0,   0, 128
#define MSCOLOR_NEARBOMB2     0, 128, 128
#define MSCOLOR_NEARBOMB3     0, 128,   0
#define MSCOLOR_NEARBOMB4   128, 128,   0
#define MSCOLOR_NEARBOMB5   128,  64,   0
#define MSCOLOR_NEARBOMB6   128,   0,   0
#define MSCOLOR_NEARBOMB7   128,   0,  64
#define MSCOLOR_NEARBOMB8   128,   0, 128
#define MSCOLOR_CLEAR         0, 128, 255
#define MSCOLOR_GAMEOVER    255,   0,   0



typedef enum {
    MSSTATE_INITIAL,
    MSSTATE_SETBOMBS,
    MSSTATE_SETSEED,
    MSSTATE_PLAYING,
    MSSTATE_CLEAR,
    MSSTATE_GAMEOVER,
} MinesweeperStates;

typedef enum {
    MSCELLMODE_OPEN,
    MSCELLMODE_MARKB,
    MSCELLMODE_MARKQ,
} MinesweeperCellModes;

typedef enum {
	MSTableStatesNonInit,
	MSTableStatesInit,
	MSTableStatesClear,
	MSTableStatesGameOver,
} MSTableStates;

typedef enum {
	MSCellMarkStatesNone,
	MSCellMarkStatesBomb,
	MSCellMarkStatesQuestion,
} MSCellMarkStates;



struct Minesweeper;
struct MSTable;
struct MSCell;

typedef struct Minesweeper {
    struct MSTable *mst;
    MinesweeperStates state;
    MinesweeperCellModes cellmode;
} Minesweeper;

typedef struct MSTable {
	uint8_t Width;
	uint8_t Height;
	uint8_t Bombs;
	uint8_t Seed;
	uint8_t NonOpenedCells;
	MSTableStates Status;
	struct MSCell **Cells;
	struct MSCell *FirstCell;
} MSTable;

typedef struct MSCell {
	struct MSTable *Parent;
	uint8_t Index;
	bool HasBomb;
	bool IsOpened;
	MSCellMarkStates MarkState;
} MSCell;



Minesweeper* Minesweeper_Start(void);
void Minesweeper_End(Minesweeper *ms);
void Minesweeper_PrintConsoleAll(Minesweeper *ms);
void Minesweeper_PrintConsole(Minesweeper *ms);
void Minesweeper_Refresh(Minesweeper *ms);
void Minesweeper_Press(Minesweeper *ms, Position *pos);
void Minesweeper_OpenCell(Minesweeper *ms, Position *pos);

MSTable* MSTable_New(uint8_t width, uint8_t height, uint8_t bombs, uint8_t seed);
void MSTable_Close(MSTable *mst);
void MSTable_PrintConsoleAll(MSTable *mst);
void MSTable_PrintConsole(MSTable *mst);
void MSTable_Draw(MSTable *mst);
void MSTable_Init(MSTable *mst, MSCell *firstCell);
uint8_t MSTable_GetCellIndex(MSTable *mst, Position pos);
MSCell* MSTable_GetCell(MSTable *mst, Position pos);
void MSTable_OpenCell(MSTable *mst, Position pos);
void MSTable_MarkCellBomb(MSTable *mst, Position *pos);
void MSTable_MarkCellQuestion(MSTable *mst, Position *pos);
void MSTable_OnCellOpen(MSTable *mst, MSCell *msc);

MSCell* MSCell_New(MSTable *parent, uint8_t index);
void MSCell_Close(MSCell *msc);
void MSCell_PrintConsole(MSCell *msc);
void MSCell_Draw(MSCell *msc);
Position MSCell_GetPosition(MSCell *msc);
uint8_t MSCell_GetNearCells(MSCell *msc, MSCell *input_cells[8]);
uint8_t MSCell_GetNearBombs(MSCell *msc);
void MSCell_SubOpen(MSCell *msc);
void MSCell_Open(MSCell *msc);

#endif
