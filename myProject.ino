#include <Arduino.h>

// --- 핀 정의 ---
// 조이스틱 쉴드
const int JOYSTICK_X_PIN = A0;      // 조이스틱 X축
const int JOYSTICK_BUTTON_PIN = 5;  // 조이스틱 버튼

// --- 초기화 ---
void setup() {

  Serial.begin(9600); // PC와 시리얼 통신을 9600 속도로 시작
  
  // 핀 모드 설정
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP); // D5 핀 PULLUP 설정

  Serial.println("조이스틱을 움직이거나 버튼을 눌러보세요.");
}


// --- 메인 루프 ---
void loop() {
  // 조이스틱 X축 값 읽기 (0~1023 사이의 정수 값)
  int joyX = analogRead(JOYSTICK_X_PIN);

  // 조이스틱 버튼 값 읽기
  bool isPressed = (digitalRead(JOYSTICK_BUTTON_PIN) == LOW);

  // 시리얼 모니터에 값 출력
  Serial.print("Joystick X (A0): ");
  Serial.print(joyX);
  Serial.print("  |  Button (D5) Pressed: ");
  Serial.println(isPressed ? "YES (0)" : "NO (1)");

  delay(100); 
}
