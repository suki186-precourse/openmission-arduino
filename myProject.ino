void setup() {
  // 아두이노 보드에 내장된 LED를 출력 모드로 설정
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // LED 켜짐
  Serial.println("ON");
  delay(2000);                     // 1초 대기
  digitalWrite(LED_BUILTIN, LOW);  // LED 꺼짐
  Serial.println("OFF");
  delay(2000);                     // 1초 대기
}