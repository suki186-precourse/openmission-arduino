#include "Renderer.h"
#include "Config.h"
#include "Globals.h"
#include <Arduino.h>

// 하트 비트맵
const unsigned char heart_bmp[] PROGMEM = {
  0b00000000, 0b01100110, 0b11111111, 0b11111111,
  0b11111111, 0b01111110, 0b00111100, 0b00011000
};

void drawScore();
void drawGameObjects();
void drawBricks();
void drawLives();
void drawEndScreen(const char* title);

// 상태에 따른 화면 그리기
void renderGame() {
  display.clearDisplay(); 
  if (currentState == STATE_READY || currentState == STATE_PLAYING) {
    drawScore();
    drawGameObjects();
  } else {
    const char* title = (currentState == STATE_CLEAR) ? "YOU WIN!" : "GAME OVER";
    drawEndScreen(title);
  }

  // GAME_OVER 상태가 아닌 경우만 벽돌과 생명 그리기
  if (currentState != STATE_GAME_OVER) {
    drawBricks();
    drawLives();
  }

  display.display();
}

// 점수, 구분선 그리기
void drawScore() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("SCORE:");
  display.print(score);
  display.drawLine(0, TOP_BAR_HEIGHT, SCREEN_WIDTH, TOP_BAR_HEIGHT, SSD1306_WHITE);
}

// 제목, 최종 점수 그리기
void drawEndScreen(const char* title) {
  display.setTextSize(2);
  display.setCursor(10, 20); display.println(title);
  display.setTextSize(1);
  display.setCursor(25, 45); display.print("SCORE: "); display.print(score);
}

// 패들, 공 그리기
void drawGameObjects() {
  display.fillRect((int)paddle.x, PADDLE_Y, PADDLE_W, PADDLE_H, SSD1306_WHITE);
  display.fillCircle((int)ball.x, (int)ball.y, (int)BALL_R, SSD1306_WHITE);
}

// 벽돌 그리기
void drawBricks() {
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      if (bricks[r][c]) {
        display.fillRect(c * (BRICK_W + BRICK_GAP), BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP), 
                         BRICK_W, BRICK_H, SSD1306_WHITE);
      }
    }
  }
}

// 생명 그리기
void drawLives() {
  for (int i = 0; i < lives; i++) {
    display.drawBitmap(PADDLE_MAP_MAX - (i * 10), 2, heart_bmp, 8, 8, SSD1306_WHITE);
  }
}
