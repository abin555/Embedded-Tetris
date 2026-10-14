/*
 * tetris.h
 *
 * Created: 11/20/2025 1:11:30 AM
 *  Author: ajb21b
 */ 


#ifndef TETRIS_H_
#define TETRIS_H_
#include <avr/io.h>
	
struct Tetris {
	int arena[10][20];
	uint8_t score;
	uint8_t lines;
	uint8_t gameOver;
	int8_t cTetrominoIdx;
	int8_t bTetrominoIdx;
	int8_t cRot;
	int8_t cX;
	int8_t cY;
	int8_t joystickLock;
};
	
void tetris_init(struct Tetris *t);
void tetris_draw(struct Tetris *tetris);
int8_t tetris_isValidPos(struct Tetris *t, int8_t tetromino, int8_t rot, int8_t posX, int8_t posY);
int8_t tetris_moveDown(struct Tetris *t);
void tetris_newBlock(struct Tetris *t);
void tetris_addBlock(struct Tetris *t);
void tetris_swap(struct Tetris *t);

typedef enum InputTypes {
	Rotate,
	Down,
	Left,
	Right,
	Neutral
} InputType;

void tetris_move(struct Tetris *t, InputType input);
void tetris_checkClear(struct Tetris *t);

void block_draw(uint8_t x, uint8_t y, uint8_t mode);
void draw_outline();

#endif /* TETRIS_H_ */