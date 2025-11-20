#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- 핀 및 상수 정의 ---
// 조이스틱 쉴드
const int JOYSTICK_X_PIN = A0;     // 조이스틱 X축
const int JOYSTICK_BUTTON_PIN = 5; // 조이스틱 버튼 D5

// 디스플레이 상수
#define SCREEN_WIDTH 128 // OLED 너비
#define SCREEN_HEIGHT 64 // OLED 높이
#define OLED_RESET -1    // 리셋 핀

// Adafruit_SSD1306 객체
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- 게임 변수 ---

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

// 벽돌 변수
const int BRICK_ROWS = 4;
const int BRICK_COLS = 5;
bool bricks[BRICK_ROWS][BRICK_COLS];
const int brickWidth = 24;
const int brickHeight = 3;

// 생명(하트) 변수
int lives = 3;

// 하트 비트맵
const unsigned char heart_bmp[] PROGMEM = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

// --- 초기화 ---
void setup() {
  Serial.begin(9600);
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);

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

  delay(1000);

  paddlePos = 118 / 2.0; 
  targetPaddlePos = 118 / 2;

  // 벽돌 배열 초기화
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      bricks[r][c] = true;
    }
  }
}

// --- 메인 루프 ---
void loop() {
  // 조이스틱 X축 값 읽기 (중앙값 521) -> 0~127값으로 변환
  int joyX = analogRead(JOYSTICK_X_PIN);
  targetPaddlePos = constrain(map(joyX, 100, 900, 0, 118), 0, 118);
  paddlePos = (paddlePos * (1.0 - smoothingFactor)) + (targetPaddlePos * smoothingFactor);

  // [Ready] 공이 패들 중앙 위를 따라다니도록
  ballX = paddlePos + (15 / 2.0);
  ballY = 60 - ballRadius - 1;

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

  // 화면 버퍼 지우기
  display.clearDisplay(); 

  // 패들 그리기
  display.fillRect((int)paddlePos, 60, 15, 4, SSD1306_WHITE);

  // 공 그리기
  display.fillCircle((int)ballX, (int)ballY, (int)ballRadius, SSD1306_WHITE);

  // 벽돌 그리기
  const int brickOffsetY = 15;
  for (int r = 0; r < BRICK_ROWS; r++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      if (bricks[r][c] == true) {
        display.fillRect(c * (brickWidth + 1), brickOffsetY + r * (brickHeight + 1), 
                         brickWidth, brickHeight, SSD1306_WHITE);
      }
    }
  }

  // 생명 그리기
  for (int i = 0; i < lives; i++) {
    int heartX = 118 - (i * 10);
    int heartY = 2;

    display.drawBitmap(heartX, heartY, heart_bmp, 8, 8, SSD1306_WHITE);
  }

  // 버퍼의 내용을 화면으로 전송
  display.display();

  // --- 시리얼 모니터 출력 ---
  Serial.print("Paddle Pos: ");
  Serial.print(paddlePos);
  Serial.print(" | Button Pressed: ");
  Serial.println(isButtonPressed ? "YES" : "NO");
}
