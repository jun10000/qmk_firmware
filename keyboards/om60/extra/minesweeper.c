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

#ifdef MINESWEEPER_ENABLE

#include "minesweeper.h"



Minesweeper* Minesweeper_Start(void) {
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);

    Minesweeper *ms = (Minesweeper *)malloc(sizeof(Minesweeper));
    PrintIfError(ms != NULL, "malloc failed: Minesweeper *");
    ms->mst = MSTable_New(MSWIDTH, MSHEIGHT, MSBOMBS, MSSEED);
    ms->state = MSSTATE_INITIAL;
    ms->cellmode = MSCELLMODE_OPEN;

    Minesweeper_Refresh(ms);

    return ms;
}

void Minesweeper_End(Minesweeper *ms) {
    rgblight_reload_from_eeprom();

    MSTable_Close(ms->mst);
    free(ms);
}

void Minesweeper_PrintConsoleAll(Minesweeper *ms) {
#ifdef CONSOLE_ENABLE
    Minesweeper_PrintConsole(ms);
    if (ms == NULL) { return; }

    MSTable_PrintConsoleAll(ms->mst);
#endif
}

void Minesweeper_PrintConsole(Minesweeper *ms) {
#ifdef CONSOLE_ENABLE
    uprintf("Minesweeper: ");
    if (ms == NULL) {
        uprintf("NULL\n");
        return;
    }

    uprintf("mst: ");
    if (ms->mst == NULL) {
        uprintf("NULL, ");
    } else {
        uprintf("EXISTS, ");
    }

    uprintf("state: %d, cellmode: %d\n", ms->state, ms->cellmode);
#endif
}

void Minesweeper_Refresh(Minesweeper *ms) {
    switch (ms->state) {
        case MSSTATE_INITIAL:
            SetRGBBuffer(MSCOLOR_ON, (Position){-1, 0});
            SetRGBBuffer(MSCOLOR_ON, (Position){-1, 2});
            SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 3});
            SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 4});
            SetRGBBufferRange(MSCOLOR_ON, (Position){0, 0}, (Position){MSWIDTH - 1, MSHEIGHT - 1});
            break;
        case MSSTATE_SETBOMBS:
            SetRGBBuffer(MSCOLOR_SETBOMB, (Position){-1, 0});
            SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 2});
            SetRGBBuffer(MSCOLOR_SETBOMB, (Position){-1, 3});
            SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 4});

            uint8_t bombs = ms->mst->Bombs;
            Position endpos = MSCell_GetPosition(ms->mst->Cells[bombs - 1]);
            SetRGBBufferRange(MSCOLOR_OFF, (Position){0, 0}, (Position){MSWIDTH - 1, MSHEIGHT - 1});
            SetRGBBufferRange(MSCOLOR_SETBOMB, (Position){0, 0}, (Position){MSWIDTH - 1, endpos.y - 1});
            SetRGBBufferRange(MSCOLOR_SETBOMB, (Position){0, endpos.y}, endpos);
            break;
        case MSSTATE_SETSEED:
            SetRGBBuffer(MSCOLOR_SETSEED, (Position){-1, 0});
            SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 2});
            SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 3});
            SetRGBBuffer(MSCOLOR_SETSEED, (Position){-1, 4});

            uint8_t seed = ms->mst->Seed;
            Position pos = MSCell_GetPosition(ms->mst->Cells[seed]);
            SetRGBBufferRange(MSCOLOR_OFF, (Position){0, 0}, (Position){MSWIDTH - 1, MSHEIGHT - 1});
            SetRGBBuffer(MSCOLOR_SETSEED, pos);
            break;
        case MSSTATE_PLAYING:
            SetRGBBuffer(MSCOLOR_ON, (Position){-1, 0});
            switch (ms->cellmode) {
                case MSCELLMODE_OPEN:
                    SetRGBBuffer(MSCOLOR_CM_OPEN, (Position){-1, 2});
                    SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 3});
                    SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 4});
                    break;
                case MSCELLMODE_MARKB:
                    SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 2});
                    SetRGBBuffer(MSCOLOR_CM_MARKB, (Position){-1, 3});
                    SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 4});
                    break;
                case MSCELLMODE_MARKQ:
                    SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 2});
                    SetRGBBuffer(MSCOLOR_OFF, (Position){-1, 3});
                    SetRGBBuffer(MSCOLOR_CM_MARKQ, (Position){-1, 4});
                    break;
            }

            MSTable_Draw(ms->mst);
            break;
        case MSSTATE_CLEAR:
            SetRGBBuffer(MSCOLOR_CLEAR, (Position){-1, 0});
            SetRGBBuffer(MSCOLOR_CLEAR, (Position){-1, 2});
            SetRGBBuffer(MSCOLOR_CLEAR, (Position){-1, 3});
            SetRGBBuffer(MSCOLOR_CLEAR, (Position){-1, 4});

            MSTable_Draw(ms->mst);
            break;
        case MSSTATE_GAMEOVER:
            SetRGBBuffer(MSCOLOR_GAMEOVER, (Position){-1, 0});
            SetRGBBuffer(MSCOLOR_GAMEOVER, (Position){-1, 2});
            SetRGBBuffer(MSCOLOR_GAMEOVER, (Position){-1, 3});
            SetRGBBuffer(MSCOLOR_GAMEOVER, (Position){-1, 4});

            MSTable_Draw(ms->mst);
            break;
    }
    ApplyRGBBuffer();
}

void Minesweeper_Press(Minesweeper *ms, Position *pos) {
#ifdef CONSOLE_ENABLE
      uprintf("Position: (%2d,%2d)\n", pos->x, pos->y);
#endif

    switch (ms->state) {
        case MSSTATE_INITIAL:
            if (pos->x == -1) {
                if (pos->y == 3) {
                    ms->state = MSSTATE_SETBOMBS;
                    Minesweeper_Refresh(ms);
                } else if (pos->y == 4) {
                    ms->state = MSSTATE_SETSEED;
                    Minesweeper_Refresh(ms);
                }
            } else {
                ms->state = MSSTATE_PLAYING;
                Minesweeper_OpenCell(ms, pos);
                Minesweeper_Refresh(ms);
            }
            break;
        case MSSTATE_SETBOMBS:
            if (pos->x == -1) {
                if (pos->y == 2) {
                    ms->state = MSSTATE_INITIAL;
                    Minesweeper_Refresh(ms);
                } else if (pos->y == 4) {
                    ms->state = MSSTATE_SETSEED;
                    Minesweeper_Refresh(ms);
                }
            } else {
                uint8_t bombs = MSTable_GetCellIndex(ms->mst, *pos) + 1;
                uint8_t bombs_max = ms->mst->Width * ms->mst->Height - 10;
                if (bombs < 2) { bombs = 2; }
                else if (bombs > bombs_max) { bombs = bombs_max; }

                ms->mst->Bombs = bombs;
                Minesweeper_Refresh(ms);
            }
            break;
        case MSSTATE_SETSEED:
            if (pos->x == -1) {
                if (pos->y == 2) {
                    ms->state = MSSTATE_INITIAL;
                    Minesweeper_Refresh(ms);
                } else if (pos->y == 3) {
                    ms->state = MSSTATE_SETBOMBS;
                    Minesweeper_Refresh(ms);
                }
            } else {
                ms->mst->Seed = MSTable_GetCellIndex(ms->mst, *pos);
                Minesweeper_Refresh(ms);
            }
            break;
        case MSSTATE_PLAYING:
            if (pos->x == -1) {
                switch (pos->y) {
                    case 2:
                        ms->cellmode = MSCELLMODE_OPEN;
                        Minesweeper_Refresh(ms);
                        break;
                    case 3:
                        ms->cellmode = MSCELLMODE_MARKB;
                        Minesweeper_Refresh(ms);
                        break;
                    case 4:
                        ms->cellmode = MSCELLMODE_MARKQ;
                        Minesweeper_Refresh(ms);
                        break;
                }
            } else {
                switch (ms->cellmode) {
                    case MSCELLMODE_OPEN:
                        Minesweeper_OpenCell(ms, pos);
                        break;
                    case MSCELLMODE_MARKB:
                        MSTable_MarkCellBomb(ms->mst, pos);
                        break;
                    case MSCELLMODE_MARKQ:
                        MSTable_MarkCellQuestion(ms->mst, pos);
                        break;
                }
                Minesweeper_Refresh(ms);
            }
            break;
        case MSSTATE_CLEAR:
            ms->state = MSSTATE_INITIAL;
            ms->cellmode = MSCELLMODE_OPEN;

            uint8_t oldw = ms->mst->Width;
            uint8_t oldh = ms->mst->Height;
            uint8_t oldb = ms->mst->Bombs;
            uint8_t olds = rand() % (MSWIDTH * MSHEIGHT);

            MSTable_Close(ms->mst);
            ms->mst = MSTable_New(oldw, oldh, oldb, olds);
            Minesweeper_Refresh(ms);
            break;
        case MSSTATE_GAMEOVER:
            if (pos->x == -1) {
                ms->state = MSSTATE_INITIAL;
                ms->cellmode = MSCELLMODE_OPEN;

                uint8_t oldw = ms->mst->Width;
                uint8_t oldh = ms->mst->Height;
                uint8_t oldb = ms->mst->Bombs;
                uint8_t olds = ms->mst->Seed;

                MSTable_Close(ms->mst);
                ms->mst = MSTable_New(oldw, oldh, oldb, olds);
            } else {
                ms->state = MSSTATE_PLAYING;
                ms->cellmode = MSCELLMODE_OPEN;

                uint8_t oldw = ms->mst->Width;
                uint8_t oldh = ms->mst->Height;
                uint8_t oldb = ms->mst->Bombs;
                uint8_t olds = ms->mst->Seed;
                Position firstPos = MSCell_GetPosition(ms->mst->FirstCell);

                MSTable_Close(ms->mst);
                ms->mst = MSTable_New(oldw, oldh, oldb, olds);
                Minesweeper_OpenCell(ms, &firstPos);
            }
            Minesweeper_Refresh(ms);
            break;
    }
#ifdef CONSOLE_ENABLE
      Minesweeper_PrintConsoleAll(ms);
#endif
}

void Minesweeper_OpenCell(Minesweeper *ms, Position *pos) {
    MSTable_OpenCell(ms->mst, *pos);
    switch (ms->mst->Status) {
        case MSTableStatesClear:
            ms->state = MSSTATE_CLEAR;
            break;
        case MSTableStatesGameOver:
            ms->state = MSSTATE_GAMEOVER;
            break;
        default:
            break;
    }
}



MSTable* MSTable_New(uint8_t width, uint8_t height, uint8_t bombs, uint8_t seed) {
    MSTable *mst = (MSTable *)malloc(sizeof(MSTable));
    PrintIfError(mst != NULL, "malloc failed: MSTable *");
    mst->Width = width;
    mst->Height = height;
    mst->Bombs = bombs;
    mst->Seed = seed;
    mst->NonOpenedCells = width * height;
    mst->Status = MSTableStatesNonInit;

    mst->Cells = (MSCell **)malloc(sizeof(MSCell *) * width * height);
    PrintIfError(mst->Cells != NULL, "malloc failed: MSCell **");
	for (uint8_t i = 0; i < (width * height); i++) {
		mst->Cells[i] = MSCell_New(mst, i);
	}

    mst->FirstCell = NULL;

	return mst;
}

void MSTable_Close(MSTable *mst) {
	for (uint8_t i = 0; i < (mst->Width * mst->Height); i++) {
		MSCell_Close(mst->Cells[i]);
	}
    free(mst->Cells);
    free(mst);
}

void MSTable_PrintConsoleAll(MSTable *mst) {
#ifdef CONSOLE_ENABLE
    MSTable_PrintConsole(mst);
    if (mst == NULL) { return; }
    if (mst->Cells == NULL) { return; }

    for (uint8_t i = 0; i < mst->Width * mst->Height; i++) {
        MSCell *msc = mst->Cells[i];
        MSCell_PrintConsole(msc);
    }
#endif
}

void MSTable_PrintConsole(MSTable *mst) {
#ifdef CONSOLE_ENABLE
    uprintf("MSTable: ");
    if (mst == NULL) {
        uprintf("NULL\n");
        return;
    }

    uprintf("Width: %d, Height: %d, Bombs: %d, Seed: %d\n", mst->Width, mst->Height, mst->Bombs, mst->Seed);
    uprintf("         NonOpenedCells: %d, Status: %d, ", mst->NonOpenedCells, mst->Status);

    uprintf("Cells: ");
    if (mst->Cells == NULL) {
        uprintf("NULL, ");
    } else {
        uprintf("EXISTS, ");
    }

    uprintf("FirstCell: ");
    if (mst->FirstCell == NULL) {
        uprintf("NULL");
    } else {
        uprintf("EXISTS");
    }
    uprintf("\n");
#endif
}

void MSTable_Draw(MSTable *mst) {
    for (uint8_t i = 0; i < mst->Width * mst->Height; i++) {
        MSCell_Draw(mst->Cells[i]);
    }
}

void MSTable_Init(MSTable *mst, MSCell *firstCell) {
	// NonBomb field max size: 3x3 (9 cells)
	MSCell *nearCells[8];
	uint8_t nearCellsCount = MSCell_GetNearCells(firstCell, nearCells);

	uint8_t nonBombCellsCount = nearCellsCount + 1;
	MSCell *nonBombCells[nonBombCellsCount];
	nonBombCells[0] = firstCell;
	for (uint8_t i = 0; i < nearCellsCount; i++) {
		nonBombCells[i + 1] = nearCells[i];
	}

	// GCC Extension
	bool func_ContainsInNonBombCells(MSCell *target) {
		for (uint8_t i = 0; i < nonBombCellsCount; i++) {
			if (target == nonBombCells[i]) {
				return true;
			}
		}
		return false;
	}

	uint8_t mayBombCellsCount = mst->Width * mst->Height - nonBombCellsCount;
	MSCell *mayBombCells[mayBombCellsCount];
	uint8_t mayBombCellsCursor = 0;

    for (uint8_t i = 0; i < mst->Width * mst->Height; i++) {
		MSCell *c = mst->Cells[i];
		if (!func_ContainsInNonBombCells(c)) {
			mayBombCells[mayBombCellsCursor] = c;
			mayBombCellsCursor++;
		}
	}

	bool bombArray[mayBombCellsCount];
	GetRandomBoolArrayWithSeed(bombArray, mayBombCellsCount, mst->Bombs, mst->Seed);

	for (uint8_t i = 0; i < mayBombCellsCount; i++) {
		mayBombCells[i]->HasBomb = bombArray[i];
	}

	mst->Status = MSTableStatesInit;
	mst->FirstCell = firstCell;
}

uint8_t MSTable_GetCellIndex(MSTable *mst, Position pos) {
	return mst->Width * pos.y + pos.x;
}

MSCell* MSTable_GetCell(MSTable *mst, Position pos) {
	return mst->Cells[MSTable_GetCellIndex(mst, pos)];
}

void MSTable_OpenCell(MSTable *mst, Position pos) {
	MSCell_Open(MSTable_GetCell(mst, pos));
}

void MSTable_MarkCellBomb(MSTable *mst, Position *pos) {
    MSCell *msc = MSTable_GetCell(mst, *pos);
    if (msc->MarkState == MSCellMarkStatesBomb) {
        msc->MarkState = MSCellMarkStatesNone;
    } else {
        msc->MarkState = MSCellMarkStatesBomb;
    }
}

void MSTable_MarkCellQuestion(MSTable *mst, Position *pos) {
    MSCell *msc = MSTable_GetCell(mst, *pos);
    if (msc->MarkState == MSCellMarkStatesQuestion) {
        msc->MarkState = MSCellMarkStatesNone;
    } else {
        msc->MarkState = MSCellMarkStatesQuestion;
    }
}

void MSTable_OnCellOpen(MSTable *mst, MSCell *msc) {
	if (mst->Status == MSTableStatesNonInit) {
		MSTable_Init(mst, msc);
	}

	mst->NonOpenedCells--;

	if (msc->HasBomb) {
		mst->Status = MSTableStatesGameOver;
	} else if (mst->NonOpenedCells <= mst->Bombs) {
		mst->Status = MSTableStatesClear;
	}
}



MSCell* MSCell_New(MSTable *parent, uint8_t index) {
    MSCell *msc = (MSCell *)malloc(sizeof(MSCell));
    PrintIfError(msc != NULL, "malloc failed: MSCell *");
	msc->Parent = parent;
    msc->Index = index;
    msc->HasBomb = false;
    msc->IsOpened = false;
    msc->MarkState = MSCellMarkStatesNone;
	return msc;
}

void MSCell_Close(MSCell *msc) {
    free(msc);
}

void MSCell_PrintConsole(MSCell *msc) {
#ifdef CONSOLE_ENABLE
    uprintf("MSCell: ");
    if (msc == NULL) {
        uprintf("NULL\n");
        return;
    }

    uprintf("Parent: ");
    if (msc->Parent == NULL) {
        uprintf("NULL, ");
    } else {
        uprintf("EXISTS, ");
    }

    uprintf("Index: %d, HasBomb: %d, IsOpened: %d, MarkState: %d\n", msc->Index, msc->HasBomb, msc->IsOpened, msc->MarkState);
#endif
}

void MSCell_Draw(MSCell *msc) {
    Position pos = MSCell_GetPosition(msc);

	if (msc->IsOpened) {
		if (msc->HasBomb) {
            SetRGBBuffer(MSCOLOR_BOMB, pos);
		} else {
			switch (MSCell_GetNearBombs(msc)) {
                case 0:
                    SetRGBBuffer(MSCOLOR_NEARBOMB0, pos);
                    break;
                case 1:
                    SetRGBBuffer(MSCOLOR_NEARBOMB1, pos);
                    break;
                case 2:
                    SetRGBBuffer(MSCOLOR_NEARBOMB2, pos);
                    break;
                case 3:
                    SetRGBBuffer(MSCOLOR_NEARBOMB3, pos);
                    break;
                case 4:
                    SetRGBBuffer(MSCOLOR_NEARBOMB4, pos);
                    break;
                case 5:
                    SetRGBBuffer(MSCOLOR_NEARBOMB5, pos);
                    break;
                case 6:
                    SetRGBBuffer(MSCOLOR_NEARBOMB6, pos);
                    break;
                case 7:
                    SetRGBBuffer(MSCOLOR_NEARBOMB7, pos);
                    break;
                case 8:
                    SetRGBBuffer(MSCOLOR_NEARBOMB8, pos);
                    break;
			}
		}
	} else {
		switch (msc->MarkState) {
            case MSCellMarkStatesNone:
                SetRGBBuffer(MSCOLOR_CM_OPEN, pos);
                break;
            case MSCellMarkStatesBomb:
                SetRGBBuffer(MSCOLOR_CM_MARKB, pos);
                break;
            case MSCellMarkStatesQuestion:
                SetRGBBuffer(MSCOLOR_CM_MARKQ, pos);
                break;
		}
	}
}

Position MSCell_GetPosition(MSCell *msc) {
	return (Position){
        msc->Index % msc->Parent->Width,
        msc->Index / msc->Parent->Width
    };
}

uint8_t MSCell_GetNearCells(MSCell *msc, MSCell *input_cells[8]) {
	Position pos = MSCell_GetPosition(msc);
	Position poslist[8] = {
		{pos.x - 1, pos.y - 1},
		{pos.x    , pos.y - 1},
		{pos.x + 1, pos.y - 1},
		{pos.x - 1, pos.y    },
		{pos.x + 1, pos.y    },
		{pos.x - 1, pos.y + 1},
		{pos.x    , pos.y + 1},
		{pos.x + 1, pos.y + 1},
	};

	uint8_t input_cells_cursor = 0;
	for (uint8_t i = 0; i < 8; i++) {
		Position p = poslist[i];
		if (p.x < 0 || p.x >= msc->Parent->Width || p.y < 0 || p.y >= msc->Parent->Height) {
			continue;
		}

		MSCell *cell = MSTable_GetCell(msc->Parent, p);
		input_cells[input_cells_cursor] = cell;
		input_cells_cursor++;
	}

	return input_cells_cursor;
}

uint8_t MSCell_GetNearBombs(MSCell *msc) {
	uint8_t count = 0;
	MSCell *nearCells[8];
	uint8_t nearCellCount = MSCell_GetNearCells(msc, nearCells);

	for (uint8_t i = 0; i < nearCellCount; i++) {
		MSCell *cell = nearCells[i];
		if (cell->HasBomb) {
			count++;
		}
	}

	return count;
}

void MSCell_SubOpen(MSCell *msc) {
	if (msc->IsOpened || msc->Parent->Status == MSTableStatesClear || msc->Parent->Status == MSTableStatesGameOver) {
		return;
	}

	MSTable_OnCellOpen(msc->Parent, msc);
	msc->IsOpened = true;
}

void MSCell_Open(MSCell *msc) {
	MSCell_SubOpen(msc);

	// AutoOpen (Search depth limited to 1 because of preventing stack overflow)
	if (!msc->HasBomb && MSCell_GetNearBombs(msc) == 0) {
		MSCell *cells[8];
		uint8_t cellCount = MSCell_GetNearCells(msc, cells);

		for (uint8_t i = 0; i < cellCount; i++) {
			MSCell *cell = cells[i];
			MSCell_SubOpen(cell);
		}
	}
}

#endif
