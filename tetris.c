/*
 * tetris.c
 *
 * Created: 11/20/2025 1:11:21 AM
 *  Author: ajb21b
 */ 

#include "tetris.h"
#include "ACU_SH1106_gfxlib.h"

const uint8_t tetrominoes[7][16] = {
	 {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // I
	 {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // O
	 {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  // S
	 {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // Z
	 {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // T
	 {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // L
	 {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}   // J
};


void tetris_init(struct Tetris *t){
	t->score = 0;
	t->lines = 0;
	t->bTetrominoIdx = -1;
	memset(&t->arena, 0, sizeof(t->arena));
	tetris_newBlock(t);
}

int rotate(int x, int y, int rotation) {
    switch (rotation % 4) {
    case 0:
        return x + y * 4;
    case 1:
        return 12 + y - (x * 4);
    case 2:
        return 15 - (y * 4) - x;
    case 3:
        return 3 - y + (x * 4);
    default:
        return 0;
    }
}

void tetris_draw(struct Tetris *tetris){
	const uint8_t yoff = 128 - 4*21;
	const uint8_t xoff = 4*3;
	draw_outline();
	for(int x = 0; x < 10; x++){
		for(int y = 0; y < 20; y++){
            int8_t rotatedPos = rotate(x - tetris->cX, y - tetris->cY, tetris->cRot);
            int8_t validX = x >= tetris->cX && x < tetris->cX + 4;
            int8_t validY = y >= tetris->cY && y < tetris->cY + 4;
            int8_t xyfilled = 1 == tetrominoes[tetris->cTetrominoIdx][rotatedPos];


			if(tetris->arena[x][y] || (validX && validY && xyfilled)){
				block_draw(
					xoff+x*4,
					yoff+y*4,
					1
				);
			}
		}
	}
}

int8_t tetris_isValidPos(struct Tetris *t, int8_t tetromino, int8_t rot, int8_t posX, int8_t posY){
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {

            int index = rotate(x, y, rot);

            if (tetrominoes[tetromino][index] != 1)
                continue;

            int arenaX = posX + x;
            int arenaY = posY + y;

            if (arenaX < 0 || arenaX >= 10 || arenaY >= 20)
                return 0;

            if (arenaY < 0)
                continue;

            if (t->arena[arenaX][arenaY] != 0)
                return 0;
        }
    }

    return 1;
}


int8_t tetris_moveDown(struct Tetris *t){
    if (tetris_isValidPos(t, t->cTetrominoIdx, t->cRot, t->cX, t->cY + 1)) {
        t->cY++;
        return 1;
    }
    return 0;
}

void tetris_newBlock(struct Tetris *t){
	t->cTetrominoIdx = rand() % 7;
	t->cRot = 0;
	t->cX = 3;
	t->cY = 0;
	t->gameOver = !tetris_isValidPos(t, t->cTetrominoIdx, t->cRot, t->cX, t->cY);
}

void tetris_addBlock(struct Tetris *t){
	for (int8_t y = 0; y < 4; y++) {
        for (int8_t x = 0; x < 4; x++) {
            int8_t index = rotate(x, y, t->cRot);
            if (1 != tetrominoes[t->cTetrominoIdx][index]) {
                continue;
            }

            int arenaX = t->cX + x;
            int arenaY = t->cY + y;
            int8_t xInRange = (0 <= arenaX) && (arenaX < 10);
            int8_t yInRange = (0 <= arenaY) && (arenaY < 20);
            if (xInRange && yInRange) {
                t->arena[arenaX][arenaY] = 1;
            }
        }
    }
}

void tetris_move(struct Tetris *t, InputType input){
	switch(input){
		case Rotate: {
			int8_t nextRot = (t->cRot + 1) % 4;
			if(tetris_isValidPos(t, t->cTetrominoIdx, nextRot, t->cX, t->cY)){
				t->cRot = nextRot;
			}
			break;
		}
		case Down: {
			tetris_moveDown(t);
			break;
		}
		case Left: {
			if(tetris_isValidPos(t, t->cTetrominoIdx, t->cRot, t->cX-1, t->cY)){
				t->cX--;
			}
			break;
		}
		case Right: {
			if(tetris_isValidPos(t, t->cTetrominoIdx, t->cRot, t->cX, t->cY+1)){
				t->cX++;
			}
			break;
		}
		case Neutral: {
			t->joystickLock = 0;
		}
	}
}

void tetris_checkClear(struct Tetris *t) {
    int8_t clearedLines = 0;

    for (int8_t y = 19; y >= 0; y--) {

        int full = 1;
        for (int8_t x = 0; x < 10; x++) {
            if (t->arena[x][y] == 0) {
                full = 0;
                break;
            }
        }

        if (!full) continue;

        clearedLines++;

        for (int8_t yy = y; yy > 0; yy--) {
            for (int8_t xx = 0; xx < 10; xx++) {
                t->arena[xx][yy] = t->arena[xx][yy - 1];
            }
        }

        for (int8_t xx = 0; xx < 10; xx++) {
            t->arena[xx][0] = 0;
        }

        y++; 
    }

    if (clearedLines > 0) {
        t->score += 100 * clearedLines;
    }
}


void tetris_swap(struct Tetris *t){
	if(t->bTetrominoIdx == -1){
		t->bTetrominoIdx = t->cTetrominoIdx;
		tetris_newBlock(t);
	}
	else{
		int8_t temp = t->cTetrominoIdx;
		t->cTetrominoIdx = t->bTetrominoIdx;
		t->cX = 3;
		t->cY = 0;
		t->gameOver = !tetris_isValidPos(t, t->cTetrominoIdx, t->cRot, t->cX, t->cY);
		t->bTetrominoIdx = temp;			
	}	
}

void block_draw(uint8_t x, uint8_t y, uint8_t mode){
	for(uint8_t i = 0; i < 4; i++){
		for(uint8_t j = 0; j < 4; j++){
			if(mode == 0){
				if((i == 0 || i == 3) || (j == 0 || j == 3)){
					//buf[x+i+(y+j)*64] = 0xFFFFFF;
					writePixel(y+j, x+i, COLOR_WHITE);
				}
			}
			else{
				if(!((i == 0 || i == 3) || (j == 0 || j == 3))){
					writePixel(y+j, x+i, COLOR_WHITE);
				}
			}
		}
	}
}

void draw_outline(){
	const uint8_t yoff = 128 - 4*22;
	const uint8_t xoff = 4*2;
	for(uint8_t x = 0; x < 12; x++){
		for(uint8_t y = 0; y < 22; y++){
			if((x == 0 || x == 11) || (y == 0 || y == 21)){
				block_draw(
				xoff+x*4, yoff+(y*4),
				0
				);
			}
		}
	}
}
