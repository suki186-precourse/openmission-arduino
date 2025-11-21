#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- 핀 및 상수 정의 ---
const int JOYSTICK_X_PIN = A0;
const int JOYSTICK_BUTTON_PIN = 5; 

// 3색 LED 핀
const int LED_PIN_R = 9;
const int LED_PIN_G = 10;
const int LED_PIN_B = 11;

// 부저 핀
const int BUZZER_PIN = 12;

// 디스플레이 상수
#define SCREEN_WIDTH 128 // OLED 너비
#define SCREEN_HEIGHT 64 // OLED 높이
#define OLED_RESET -1    // 리셋 핀

// Adafruit_SSD1306 객체
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- 게임 상태 ---
enum GameState {
  STATE_READY,
  STATE_PLAYING,
  STATE_GAME_OVER,
  STATE_CLEAR
};
GameState currentState = STATE_READY;

// --- 게임 변수 ---
#define INITIAL_LIVES 3 // 초기 생명 개수

// 디바운스 변수
unsigned long lastButtonTime = 0; // 버튼을 마지막으로 누른 시간
unsigned long buttonDelay = 50;   // 버튼 인식 간격
bool lastButtonState = HIGH;      // 버튼의 이전 상태 
bool currentButtonState = HIGH;   // 버튼의 현재 상태

// 게임 상태 변수
float paddlePos = 60.0;            // 패들 현재 위치
int targetPaddlePos = 60;          // 패들 목표 위치
const float smoothingFactor = 0.2; // 스무딩 계수(부드러움)

bool isButtonPressed = false; // 최종 버튼 눌림 상태

// 공 변수
float ballX, ballY;
const float ballRadius = 2.0;
float ballSpeedX = 0;
float ballSpeedY = 0;

// 벽돌 변수
const int BRICK_ROWS = 4;
const int BRICK_COLS = 5;
bool bricks[BRICK_ROWS][BRICK_COLS];
const int brickWidth = 24;
const int brickHeight = 3;
int activeBricks = 0; // 남은 벽돌

// 생명(하트) 변수
int lives = INITIAL_LIVES;

// 하트 비트맵
const unsigned char heart_bmp[] PROGMEM = {
  0b00000000, 0b01100110, 0b11111111, 0b11111111,
  0b11111111, 0b01111110, 0b00111100, 0b00011000
};

void setLedColor(int r, int g, int b);
void resetGame();
  void updateLed();

// --- 초기화 ---
void setup() {
  Serial.begin(9600);
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // --- LED, 부저 테스트 ---
  setLedColor(255, 0, 0); 
  tone(BUZZER_PIN, 262, 200);
  delay(300); 

  setLedColor(0, 255, 0); 
  tone(BUZZER_PIN, 294, 200);
  delay(300); 

  setLedColor(0, 0, 255); 
  tone(BUZZER_PIN, 330, 200);
  delay(300); 

  setLedColor(0, 0, 0); 

  // OLED 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 초기화 실패"));
    while (true);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("BRICK BREAKER");
  display.println("Ready...");
  display.display();

  setLedColor(0, 255, 0); // LED 초록
  delay(1000);

  resetGame(); // 게임 변수 초기화
}

// --- 메인 루프 ---
void loop() {
  // 조이스틱 X축 값 읽기 (중앙값 521) -> 0~127값으로 변환
  int joyX = analogRead(JOYSTICK_X_PIN);
  targetPaddlePos = constrain(map(joyX, 100, 900, 0, 118), 0, 118);
  paddlePos = (paddlePos * (1.0 - smoothingFactor)) + (targetPaddlePos * smoothingFactor);

  // 조이스틱 버튼 값 읽기
  currentButtonState = digitalRead(JOYSTICK_BUTTON_PIN);
  isButtonPressed = false;

  // 버튼이 눌리거나 떼어진 순간
  if (currentButtonState != lastButtonState) {
    if ((millis() - lastButtonTime) > buttonDelay) {
      // 50ms 이상 상태가 유지되었다면 상태 업데이트
      lastButtonTime = millis();
      lastButtonState = currentButtonState;

      // 버튼이 눌린 상태
      if (currentButtonState == LOW) {
        isButtonPressed = true;
      }
    }
  }

  // 게임 상태별 로직
  display.clearDisplay(); 

  if (currentState == STATE_READY) {
    // [READY 상태] 공이 패들에 붙어 다님
    ballX = paddlePos + (15 / 2.0);
    ballY = 60 - ballRadius - 1;

    // 버튼을 누르면 -> 공 발사, PLAYING 상태로
    if (isButtonPressed) {
      currentState = STATE_PLAYING;

      ballSpeedX = 2.0; 
      ballSpeedY = -2.0; 

      tone(BUZZER_PIN, 1000, 100); 
    }

    display.fillRect((int)paddlePos, 60, 15, 4, SSD1306_WHITE);
    display.fillCircle((int)ballX, (int)ballY, (int)ballRadius, SSD1306_WHITE);

  } else if (currentState == STATE_PLAYING) {
    // [PLAYING 상태]
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // 벽 충돌 처리
    if (ballX - ballRadius <= 0) { // 왼쪽
      ballX = ballRadius;
      ballSpeedX = -ballSpeedX; // 방향 반전
      tone(BUZZER_PIN, 500, 20);
    }
    else if (ballX + ballRadius >= SCREEN_WIDTH) { // 오른쪽
      ballX = SCREEN_WIDTH - ballRadius;
      ballSpeedX = -ballSpeedX;
      tone(BUZZER_PIN, 500, 20);
    }

    if (ballY - ballRadius <= 0) { // 위쪽
      ballY = ballRadius;
      ballSpeedY = -ballSpeedY;
      tone(BUZZER_PIN, 500, 20);
    }

    // 패들 충돌 처리
    if (ballSpeedY > 0 && 
        ballY + ballRadius >= 60 && 
        ballY - ballRadius <= 64) {
        
      if (ballX + ballRadius >= paddlePos && 
          ballX - ballRadius <= paddlePos + 15) {
        ballSpeedY = -ballSpeedY; 
        
        // 공이 패들에 박히지 않도록
        ballY = 60 - ballRadius - 0.1;

        // X축 속도 변화
        float hitPoint = ballX - (paddlePos + 7.5);
        ballSpeedX = hitPoint * 0.3; 

        // 속도 제한
        if (ballSpeedX > 2.5) ballSpeedX = 2.5;
        if (ballSpeedX < -2.5) ballSpeedX = -2.5;
        if (ballSpeedX > -0.5 && ballSpeedX < 0.5) ballSpeedX = (ballSpeedX > 0) ? 0.5 : -0.5;
        tone(BUZZER_PIN, 1500, 30); 
      }
    }

    // 벽돌 충돌 처리
    const int brickOffsetY = 15;
    bool hitBrick = false;

    for (int r = 0; r < BRICK_ROWS; r++) {
      for (int c = 0; c < BRICK_COLS; c++) {
        if (bricks[r][c] == true) {
          // 벽돌의 좌표
          int bX = c * (brickWidth + 1);
          int bY = brickOffsetY + r * (brickHeight + 1);

          // 충돌 감지
          if (ballX + ballRadius > bX && ballX - ballRadius < bX + brickWidth &&
              ballY + ballRadius > bY && ballY - ballRadius < bY + brickHeight) {
            
            bricks[r][c] = false;
            activeBricks--;
            ballSpeedY = -ballSpeedY;
            hitBrick = true;
            
            tone(BUZZER_PIN, 2000, 20);
            break;
          }
        }
      }
      if (hitBrick) break; 
    }

    // 게임 클리어
    if (activeBricks == 0) {
      currentState = STATE_CLEAR;
      tone(BUZZER_PIN, 1000, 100); delay(100);
      tone(BUZZER_PIN, 1200, 100); delay(100);
      tone(BUZZER_PIN, 1500, 200); delay(200);
      noTone(BUZZER_PIN);
    }

    // 바닥 충돌 (생명 감소)
    if (ballY - ballRadius > SCREEN_HEIGHT) {
      lives--; 
      
      setLedColor(255, 0, 0); // 빨강
      tone(BUZZER_PIN, 200, 300);
      delay(500);

      if (lives > 0) {
        // 생명이 남았으면 -> READY 상태
        currentState = STATE_READY;
        paddlePos = 118 / 2.0;
        setLedColor(0, 255, 0); // 초록
      } else {
        // 생명 0 -> GAME_OVER 상태
        currentState = STATE_GAME_OVER;
        setLedColor(255, 0, 0); // 빨강
        tone(BUZZER_PIN, 100, 1000);
      }
    }

    display.fillRect((int)paddlePos, 60, 15, 4, SSD1306_WHITE);
    display.fillCircle((int)ballX, (int)ballY, (int)ballRadius, SSD1306_WHITE);

  } else if (currentState == STATE_GAME_OVER) {
    // [GAME_OVER 상태]
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.println("GAME OVER");
    
    display.setTextSize(1);
    display.setCursor(25, 45);
    display.println("Press Button");

    // 버튼 누르면 게임 리셋
    if (isButtonPressed) {
      resetGame();
      tone(BUZZER_PIN, 1500, 100);
      tone(BUZZER_PIN, 2000, 200);
    }
  } else if (currentState == STATE_CLEAR) {
    // [CLEAR 상태]
    display.setTextSize(2);
    display.setCursor(16, 20);
    display.println("YOU WIN!");
    display.setTextSize(1);
    display.setCursor(25, 45);
    display.println("Press Button");

    if (isButtonPressed) {
      resetGame();
      tone(BUZZER_PIN, 1500, 100); 
      tone(BUZZER_PIN, 2000, 200);
    }
  }

  // --- 공통 요소 그리기 ---
  // 벽돌 (GAME_OVER 아닌 경우만)
  if (currentState != STATE_GAME_OVER) {
    const int brickOffsetY = 15;
    for (int r = 0; r < BRICK_ROWS; r++) {
      for (int c = 0; c < BRICK_COLS; c++) {
        if (bricks[r][c] == true) {
          display.fillRect(c * (brickWidth + 1), brickOffsetY + r * (brickHeight + 1), 
                           brickWidth, brickHeight, SSD1306_WHITE);
        }
      }
    }

    // 생명
    for (int i = 0; i < lives; i++) {
      display.drawBitmap(118 - (i * 10), 2, heart_bmp, 8, 8, SSD1306_WHITE);
    }
  }

  display.display();
  updateLed();
}

void setLedColor(int r, int g, int b) {
  analogWrite(LED_PIN_R, r);
  analogWrite(LED_PIN_G, g);
  analogWrite(LED_PIN_B, b);
}

// LED 상태 관리 함수
void updateLed() {
  // 버튼이 눌려있으면 흰색
  if (digitalRead(JOYSTICK_BUTTON_PIN) == LOW) {
    setLedColor(255, 255, 255);
    return;
  }

  switch (currentState) {
    case STATE_READY:
      setLedColor(0, 255, 0); // 초록
      break;
      
    case STATE_PLAYING:
      setLedColor(0, 0, 255); // 파랑
      break;
      
    case STATE_GAME_OVER:
      setLedColor(255, 0, 0); // 빨강
      break;
      
    case STATE_CLEAR:
      // 무지개 효과
      long now = millis();
      if (now % 300 < 100) setLedColor(255, 0, 0);
      else if (now % 300 < 200) setLedColor(0, 255, 0);
      else setLedColor(0, 0, 255);
      break;
  }
}

// 게임 초기화 함수
void resetGame() {
  lives = INITIAL_LIVES; 
  paddlePos = 118 / 2.0;
  targetPaddlePos = 118 / 2;
  currentState = STATE_READY;
  setLedColor(0, 255, 0); // 녹색

  // 벽돌 복구
  activeBricks = 0;
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      bricks[r][c] = true;
      activeBricks++;
    }
  }
}