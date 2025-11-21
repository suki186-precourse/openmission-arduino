#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- 상수 정의 ---
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
#define PADDLE_Y 60       // Y 위치
#define PADDLE_MAP_MAX 118

// 공
#define BALL_R 2.0
#define BALL_SPEED_INIT 2.0 // 초기 속도
#define BALL_SPEED_MAX 2.5  // 최대 속도
#define BALL_SPEED_MIN 0.5  // 최소 속도

// 벽돌
#define BRICK_ROWS 4
#define BRICK_COLS 5
#define BRICK_W 24
#define BRICK_H 3
#define BRICK_OFFSET_Y 14 // 시작 Y위치
#define BRICK_GAP 1       // 벽돌 사이 간격

// 사운드
#define SND_START 1000
#define SND_HIT_WALL 500
#define SND_HIT_PADDLE 1500
#define SND_HIT_BRICK 2000
#define SND_DIE 200
#define SND_GAMEOVER 100
#define SND_WIN 1000

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

// 패들 구조체
struct Paddle {
  float x;
  int targetX;
  float smoothing;
};

// 공 구조체
struct Ball {
  float x, y;
  float vx, vy;
};

// --- 게임 변수 ---
#define INITIAL_LIVES 3 // 초기 생명 개수

Paddle paddle;
Ball ball;

// 디바운스 변수
unsigned long lastButtonTime = 0; // 버튼을 마지막으로 누른 시간
unsigned long buttonDelay = 50;   // 버튼 인식 간격
bool lastButtonState = HIGH;      // 버튼의 이전 상태 
bool currentButtonState = HIGH;   // 버튼의 현재 상태

bool isButtonPressed = false; // 최종 버튼 눌림 상태

// 벽돌 변수
bool bricks[BRICK_ROWS][BRICK_COLS];
int activeBricks = 0; // 남은 벽돌

int lives = INITIAL_LIVES; // 생명(하트) 변수
long score = 0; // 점수

// 하트 비트맵
const unsigned char heart_bmp[] PROGMEM = {
  0b00000000, 0b01100110, 0b11111111, 0b11111111,
  0b11111111, 0b01111110, 0b00111100, 0b00011000
};

void setLedColor(int r, int g, int b);
void resetGame();
void updateLed();
void processInput();
void updateGame();
void renderGame();

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
  
  paddle.smoothing = 0.4; // 스무딩 계수

  resetGame(); // 게임 변수 초기화
}

// --- 메인 루프 ---
void loop() {
  processInput(); // 1. 입력 처리
  updateGame();   // 2. 게임 로직 처리
  renderGame();   // 3. 화면 그리기
  updateLed();    // 4. LED 처리
}

// --- 입력 처리 함수 ---
void processInput() {
  // 조이스틱 X축 값 읽기 (중앙값 521) -> 0~127값으로 변환
  int joyX = analogRead(JOYSTICK_X_PIN);

  paddle.targetX = constrain(map(joyX, 100, 900, 0, PADDLE_MAP_MAX), 0, PADDLE_MAP_MAX);

  paddle.x = (paddle.x * (1.0 - paddle.smoothing)) + (paddle.targetX * paddle.smoothing);

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
}

// --- 게임 로직 업데이트 함수 ---
void updateGame() {
  // 게임 상태별 로직
  if (currentState == STATE_READY) {
    // [READY 상태] 공이 패들에 붙어 다님
    ball.x = paddle.x + (PADDLE_W / 2.0);
    ball.y = PADDLE_Y - BALL_R - 1;

    // 버튼을 누르면 -> 공 발사, PLAYING 상태로
    if (isButtonPressed) {
      currentState = STATE_PLAYING;

      ball.vx = BALL_SPEED_INIT; 
      ball.vy = -BALL_SPEED_INIT; 

      tone(BUZZER_PIN, SND_START, 100); 
    }

  } else if (currentState == STATE_PLAYING) {
    // [PLAYING 상태]
    ball.x += ball.vx;
    ball.y += ball.vy;

    // 벽 충돌 처리
    if (ball.x - BALL_R <= 0) { // 왼쪽
      ball.x = BALL_R;
      ball.vx = -ball.vx; // 방향 반전
      tone(BUZZER_PIN, SND_HIT_WALL, 20);
    }
    else if (ball.x + BALL_R >= SCREEN_WIDTH) { // 오른쪽
      ball.x = SCREEN_WIDTH - BALL_R;
      ball.vx = -ball.vx;
      tone(BUZZER_PIN, SND_HIT_WALL, 20);
    }

    if (ball.y - BALL_R <= TOP_BAR_HEIGHT) { // 위쪽
      ball.y = TOP_BAR_HEIGHT + BALL_R;
      ball.vy = -ball.vy; 
      tone(BUZZER_PIN, SND_HIT_WALL, 20);
    }

    // 패들 충돌 처리
    if (ball.vy > 0 && 
        ball.y + BALL_R >= PADDLE_Y && 
        ball.y - BALL_R <= PADDLE_Y + PADDLE_H) {
        
      if (ball.x + BALL_R >= paddle.x && 
          ball.x - BALL_R <= paddle.x + PADDLE_W) {
        ball.vy = -ball.vy; 
        
        // 공이 패들에 박히지 않도록
        ball.y = PADDLE_Y - BALL_R - 0.1;

        // X축 속도 변화
        float hitPoint = ball.x - (paddle.x + (PADDLE_W / 2.0));
        ball.vx = hitPoint * 0.3; 

        // 속도 제한
        if (ball.vx > BALL_SPEED_MAX) ball.vx = BALL_SPEED_MAX;
        if (ball.vx < -BALL_SPEED_MAX) ball.vx = -BALL_SPEED_MAX;
        if (ball.vx > -BALL_SPEED_MIN && ball.vx < BALL_SPEED_MIN) ball.vx = (ball.vx > 0) ? BALL_SPEED_MIN : -BALL_SPEED_MIN;
        tone(BUZZER_PIN, SND_HIT_PADDLE, 30); 
      }
    }

    // 벽돌 충돌 처리
    bool hitBrick = false;

    for (int r = 0; r < BRICK_ROWS; r++) {
      for (int c = 0; c < BRICK_COLS; c++) {
        if (bricks[r][c] == true) {
          // 벽돌의 좌표
          int bX = c * (BRICK_W + BRICK_GAP);
          int bY = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);

          // 충돌 감지
          if (ball.x + BALL_R > bX && ball.x - BALL_R < bX + BRICK_W &&
              ball.y + BALL_R > bY && ball.y - BALL_R < bY + BRICK_H) {
            
            bricks[r][c] = false;
            activeBricks--;
            ball.vy = -ball.vy;
            hitBrick = true;

            score += 100; // 점수 증가
            
            tone(BUZZER_PIN, SND_HIT_BRICK, 20);
            break;
          }
        }
      }
      if (hitBrick) break; 
    }

    // 게임 클리어
    if (activeBricks == 0) {
      currentState = STATE_CLEAR;
      tone(BUZZER_PIN, SND_WIN, 100); delay(100);
      tone(BUZZER_PIN, SND_WIN + 200, 100); delay(100);
      tone(BUZZER_PIN, SND_WIN + 500, 200); delay(200);
      noTone(BUZZER_PIN);
    }

    // 바닥 충돌 (생명 감소)
    if (ball.y - BALL_R > SCREEN_HEIGHT) {
      lives--;
      score -= 50; // 점수 감소
      if (score < 0) score = 0;
      
      setLedColor(255, 0, 0); // 빨강
      tone(BUZZER_PIN, SND_DIE, 300);
      delay(500);

      if (lives > 0) {
        // 생명이 남았으면 -> READY 상태
        currentState = STATE_READY;
        paddle.x = PADDLE_MAP_MAX / 2.0;
        setLedColor(0, 255, 0); // 초록
      } else {
        // 생명 0 -> GAME_OVER 상태
        currentState = STATE_GAME_OVER;
        setLedColor(255, 0, 0); // 빨강
        tone(BUZZER_PIN, SND_GAMEOVER, 1000);
      }
    }

  } else if (currentState == STATE_GAME_OVER) {
    // 버튼 누르면 게임 리셋
    if (isButtonPressed) {
      resetGame();
      tone(BUZZER_PIN, SND_HIT_PADDLE, 100);
      tone(BUZZER_PIN, SND_HIT_BRICK, 200);
    }
  } else if (currentState == STATE_CLEAR) {
    if (isButtonPressed) {
      resetGame();
      tone(BUZZER_PIN, SND_HIT_PADDLE, 100); 
      tone(BUZZER_PIN, SND_HIT_BRICK, 200);
    }
  }
}

// --- 화면 그리기 함수 ---
void renderGame() {
  display.clearDisplay(); 

  if (currentState == STATE_READY) {
    // 점수 표시
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SCORE:");
    display.print(score);

    display.drawLine(0, TOP_BAR_HEIGHT, SCREEN_WIDTH, TOP_BAR_HEIGHT, SSD1306_WHITE);

    // 패들, 공 그리기
    display.fillRect((int)paddle.x, PADDLE_Y, PADDLE_W, PADDLE_H, SSD1306_WHITE);
    display.fillCircle((int)ball.x, (int)ball.y, (int)BALL_R, SSD1306_WHITE);

  } else if (currentState == STATE_PLAYING) {
    // 점수 표시
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SCORE:");
    display.print(score);

    display.drawLine(0, TOP_BAR_HEIGHT, SCREEN_WIDTH, TOP_BAR_HEIGHT, SSD1306_WHITE);

    display.fillRect((int)paddle.x, PADDLE_Y, PADDLE_W, PADDLE_H, SSD1306_WHITE);
    display.fillCircle((int)ball.x, (int)ball.y, (int)BALL_R, SSD1306_WHITE);

  } else if (currentState == STATE_GAME_OVER) {
    // [GAME_OVER 상태]
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.println("GAME OVER");
    
    display.setTextSize(1);
    display.setCursor(25, 45);
    display.print("SCORE: ");
    display.print(score);

  } else if (currentState == STATE_CLEAR) {
    // [CLEAR 상태]
    display.setTextSize(2);
    display.setCursor(16, 20);
    display.println("YOU WIN!");
    
    // 점수 표시
    display.setTextSize(1);
    display.setCursor(25, 45); 
    display.print("SCORE: ");
    display.print(score);
  }

  // --- 공통 요소 그리기 ---
  // 벽돌 (GAME_OVER 아닌 경우만)
  if (currentState != STATE_GAME_OVER) {
    for (int r = 0; r < BRICK_ROWS; r++) {
      for (int c = 0; c < BRICK_COLS; c++) {
        if (bricks[r][c] == true) {
          display.fillRect(c * (BRICK_W + BRICK_GAP), BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP), 
                           BRICK_W, BRICK_H, SSD1306_WHITE);
        }
      }
    }

    // 생명
    for (int i = 0; i < lives; i++) {
      display.drawBitmap(PADDLE_MAP_MAX - (i * 10), 2, heart_bmp, 8, 8, SSD1306_WHITE);
    }
  }

  display.display();
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
  score = 0;

  paddle.x = PADDLE_MAP_MAX / 2.0;
  paddle.targetX = PADDLE_MAP_MAX / 2;
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