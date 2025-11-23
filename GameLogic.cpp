#include "GameLogic.h"
#include "Config.h"
#include "Globals.h"
#include "Hardware.h"

void updateReadyState();
void updatePlayingState();
void handleGameReset();
void handleWallCollision();
void handlePaddleCollision();
void handleBrickCollision();
void handleGameClear();
void handleFloorCollision();

// 점수, 생명, 벽돌 초기화 -> READY 상태
void resetGame() {
  lives = INITIAL_LIVES;
  score = 0;

  // 중앙 정렬
  paddle.x = PADDLE_MAP_MAX / 2.0;
  paddle.targetX = PADDLE_MAP_MAX / 2;

  currentState = STATE_READY;
  setLedColor(0, 255, 0);

  activeBricks = 0;
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      bricks[r][c] = true;
      activeBricks++;
    }
  }
}

// 상태에 따른 로직 함수 호출
void updateGame() {
  if (currentState == STATE_READY) updateReadyState();
  else if (currentState == STATE_PLAYING) updatePlayingState();
  else if (currentState == STATE_GAME_OVER || currentState == STATE_CLEAR) handleGameReset();
}

// [READY 상태] 패들위에 공 따라다님
void updateReadyState() {
  ball.x = paddle.x + (PADDLE_W / 2.0);
  ball.y = PADDLE_Y - BALL_R - 1;

  if (isButtonPressed) {
    currentState = STATE_PLAYING;
    ball.vx = BALL_SPEED_INIT; 
    ball.vy = -BALL_SPEED_INIT; 
    playSound(SND_START, 100); 
  }
}

// [PLAYING 상태] 공 움직임, 각종 충돌 처리
void updatePlayingState() {
  ball.x += ball.vx;
  ball.y += ball.vy;

  handleWallCollision();
  handlePaddleCollision();
  handleBrickCollision();
  handleGameClear();
  handleFloorCollision();
}

// 벽 충돌 처리
void handleWallCollision() {
  if (ball.x - BALL_R <= 0) { 
    ball.x = BALL_R; ball.vx = -ball.vx; playSound(SND_HIT_WALL, 20);
  } else if (ball.x + BALL_R >= SCREEN_WIDTH) { 
    ball.x = SCREEN_WIDTH - BALL_R; ball.vx = -ball.vx; playSound(SND_HIT_WALL, 20);
  }
  if (ball.y - BALL_R <= TOP_BAR_HEIGHT) { 
    ball.y = TOP_BAR_HEIGHT + BALL_R; ball.vy = -ball.vy; playSound(SND_HIT_WALL, 20);
  }
}

// 패들 충돌 처리
void handlePaddleCollision() {
  if (ball.vy <= 0) return;
  if (ball.y + BALL_R < PADDLE_Y || ball.y - BALL_R > PADDLE_Y + PADDLE_H) return;

  if (ball.x + BALL_R >= paddle.x && ball.x - BALL_R <= paddle.x + PADDLE_W) {
      ball.vy = -ball.vy; 
      ball.y = PADDLE_Y - BALL_R - 0.1;

      float hitPoint = ball.x - (paddle.x + (PADDLE_W / 2.0));
      ball.vx = hitPoint * 0.3; 

      if (ball.vx > BALL_SPEED_MAX) ball.vx = BALL_SPEED_MAX;
      if (ball.vx < -BALL_SPEED_MAX) ball.vx = -BALL_SPEED_MAX;
      if (ball.vx > -BALL_SPEED_MIN && ball.vx < BALL_SPEED_MIN) ball.vx = (ball.vx > 0) ? BALL_SPEED_MIN : -BALL_SPEED_MIN;

      playSound(SND_HIT_PADDLE, 30); 
  }
}

// 벽돌 충돌 처리
void handleBrickCollision() { 
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      if (!bricks[r][c]) continue;

      int bX = c * (BRICK_W + BRICK_GAP);
      int bY = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);

      if (ball.x + BALL_R > bX && ball.x - BALL_R < bX + BRICK_W &&
          ball.y + BALL_R > bY && ball.y - BALL_R < bY + BRICK_H) {
        bricks[r][c] = false;
        activeBricks--;
        ball.vy = -ball.vy;

        score += 100; 
        playSound(SND_HIT_BRICK, 20);

        return;
      }
    }
  }
}

// [GAME_CLEAR 상태] 벽돌 모두 제거한 경우
void handleGameClear() {
  if (activeBricks == 0) {
    currentState = STATE_CLEAR;

    playSound(SND_WIN, 100); delay(100);
    playSound(SND_WIN + 200, 100); delay(100);
    playSound(SND_WIN + 500, 200); delay(200);
    noTone(BUZZER_PIN);
  }
}

// 바닥에 충돌한 경우
// 생명이 0이 되면 [GAME_OVER 상태]
void handleFloorCollision() {
  if (ball.y - BALL_R > SCREEN_HEIGHT) {
    lives--;
    score -= 50; if (score < 0) score = 0;
    setLedColor(255, 0, 0); playSound(SND_DIE, 300); delay(500);

    if (lives > 0) {
      currentState = STATE_READY; paddle.x = PADDLE_MAP_MAX / 2.0; setLedColor(0, 255, 0);
    } else {
      currentState = STATE_GAME_OVER; setLedColor(255, 0, 0); playSound(SND_GAMEOVER, 1000);
    }
  }
}

// 게임 초기화
void handleGameReset() {
  if (isButtonPressed) {
    resetGame();
    playSound(SND_HIT_PADDLE, 100);
    playSound(SND_HIT_BRICK, 200);
  }
}
