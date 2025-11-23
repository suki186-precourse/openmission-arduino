// 구조체, 게임 상태 정의
#ifndef TYPES_H
#define TYPES_H

enum GameState {
  STATE_READY,
  STATE_PLAYING,
  STATE_GAME_OVER,
  STATE_CLEAR
};

struct Paddle {
  float x;
  int targetX;
  float smoothing;
};

struct Ball {
  float x, y;
  float vx, vy;
};

#endif
