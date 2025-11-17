#include <Arduino.h>

// --- 핀 정의 ---
// 조이스틱 쉴드
const int JOYSTICK_X_PIN = A0;      // 조이스틱 X축
const int JOYSTICK_BUTTON_PIN = 5;  // 조이스틱 버튼

// --- 게임 변수
unsigned long lastButtonTime = 0;   // 버튼을 마지막으로 누른 시간
unsigned long buttonDelay = 50;     // 버튼 인식 간격
bool lastButtonState = HIGH;        // 버튼의 이전 상태 
bool currentButtonState = HIGH;     // 버튼의 현재 상태

bool isButtonPressed = false;       // 최종 버튼 눌림 상태

// --- 초기화 ---
void setup() {

  Serial.begin(9600); // PC와 시리얼 통신을 9600 속도로 시작
  
  // 핀 모드 설정
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP); // D5 핀 PULLUP 설정

  Serial.println("조이스틱을 움직이거나 버튼을 눌러보세요.");
}


// --- 메인 루프 ---
void loop() {
  // 조이스틱 X축 값 읽기 (중앙값 521) -> 0~127값으로 변환
  int joyX = analogRead(JOYSTICK_X_PIN);
  int paddlePos = constrain(map(joyX, 100, 900, 0, 127), 0, 127);

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

  // 시리얼 모니터에 값 출력
  Serial.print("Joystick X (A0): ");
  Serial.print(joyX);
  Serial.print("  |  Paddle Pos (0-127): ");
  Serial.print(paddlePos);
  Serial.print("  |  Button (D5) Pressed: ");
  Serial.println(isButtonPressed ? "YES" : "NO");

  delay(100); 
}
