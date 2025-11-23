// 상수, 데이터 타입 정의
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// 핀 번호
const int JOYSTICK_X_PIN = A0;
const int JOYSTICK_BUTTON_PIN = 5; 
const int LED_PIN_R = 9;
const int LED_PIN_G = 10;
const int LED_PIN_B = 11;
const int BUZZER_PIN = 12;

// 디스플레이 및 UI
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1 
#define TOP_BAR_HEIGHT 11 

// 패들
#define PADDLE_W 15
#define PADDLE_H 4
#define PADDLE_Y 60       
#define PADDLE_MAP_MAX 118

// 공
#define BALL_R 2.0
#define BALL_SPEED_INIT 2.0 
#define BALL_SPEED_MAX 2.5  
#define BALL_SPEED_MIN 0.5  

// 벽돌
#define BRICK_ROWS 4
#define BRICK_COLS 5
#define BRICK_W 24
#define BRICK_H 3
#define BRICK_OFFSET_Y 14 
#define BRICK_GAP 1       

// 사운드
#define SND_START 1000
#define SND_HIT_WALL 500
#define SND_HIT_PADDLE 1500
#define SND_HIT_BRICK 2000
#define SND_DIE 200
#define SND_GAMEOVER 100
#define SND_WIN 1000

// 생명
#define INITIAL_LIVES 3

#endif
