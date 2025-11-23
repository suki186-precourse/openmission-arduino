#include "Hardware.h"
#include "Config.h"
#include "Globals.h"
#include <Arduino.h>

unsigned long lastButtonTime = 0;
unsigned long buttonDelay = 50;
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;

// 하드웨어 핀 모드 설정
void initHardware() {
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

// 부저음
void playSound(int freq, int duration) {
  tone(BUZZER_PIN, freq, duration);
}

// LED
void setLedColor(int r, int g, int b) {
  analogWrite(LED_PIN_R, r);
  analogWrite(LED_PIN_G, g);
  analogWrite(LED_PIN_B, b);
}

void updateLed() {
  if (digitalRead(JOYSTICK_BUTTON_PIN) == LOW) {
    setLedColor(255, 255, 255);
    return;
  }

  switch (currentState) {
    case STATE_READY: setLedColor(0, 255, 0); break;
    case STATE_PLAYING: setLedColor(0, 0, 255); break;
    case STATE_GAME_OVER: setLedColor(255, 0, 0); break;
    case STATE_CLEAR:
      long now = millis();
      if (now % 300 < 100) setLedColor(255, 0, 0);
      else if (now % 300 < 200) setLedColor(0, 255, 0);
      else setLedColor(0, 0, 255);
      break;
  }
}

// 조이스틱 좌표 -> 패들 위치 계산
// 버튼 입력 감지
void processInput() {
  int joyX = analogRead(JOYSTICK_X_PIN);
  paddle.targetX = constrain(map(joyX, 100, 900, 0, PADDLE_MAP_MAX), 0, PADDLE_MAP_MAX);
  paddle.x = (paddle.x * (1.0 - paddle.smoothing)) + (paddle.targetX * paddle.smoothing);

  currentButtonState = digitalRead(JOYSTICK_BUTTON_PIN);
  isButtonPressed = false;

  if (currentButtonState != lastButtonState) {
    if ((millis() - lastButtonTime) > buttonDelay) {
      lastButtonTime = millis();
      lastButtonState = currentButtonState;
      if (currentButtonState == LOW) isButtonPressed = true;
    }
  }
}
