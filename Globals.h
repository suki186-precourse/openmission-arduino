// 점수, 공의 위치 등 전역 변수
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Types.h"
#include "Config.h"

// 전역 변수 공유 선언
extern Adafruit_SSD1306 display;
extern Paddle paddle;
extern Ball ball;
extern bool bricks[BRICK_ROWS][BRICK_COLS];
extern int activeBricks;
extern int lives;
extern long score;
extern GameState currentState;
extern bool isButtonPressed;

#endif
