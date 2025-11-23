#include <Wire.h>
#include "Config.h"
#include "Types.h"
#include "Globals.h"
#include "Hardware.h"
#include "GameLogic.h"
#include "Renderer.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Paddle paddle;
Ball ball;
bool bricks[BRICK_ROWS][BRICK_COLS];
int activeBricks = 0;
int lives = INITIAL_LIVES;
long score = 0;
GameState currentState = STATE_READY;
bool isButtonPressed = false;

void setup() {
  Serial.begin(9600);
  initHardware(); // 하드웨어 초기화
  
  // 테스트 사운드
  setLedColor(255, 0, 0); playSound(262, 200); delay(300); 
  setLedColor(0, 255, 0); playSound(294, 200); delay(300); 
  setLedColor(0, 0, 255); playSound(330, 200); delay(300); 
  setLedColor(0, 0, 0); 

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 init failed"));
    while (true);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("BRICK BREAKER");
  display.println("Ready...");
  display.display();

  setLedColor(0, 255, 0); 
  delay(1000);
  
  paddle.smoothing = 0.4; 

  resetGame(); // 게임 초기화
}

void loop() {
  processInput();
  updateGame();
  renderGame();
  updateLed();
}