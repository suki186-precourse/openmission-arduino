// 조이스틱, 부저, LED를 제어하는 함수 목록
#ifndef HARDWARE_H
#define HARDWARE_H

void initHardware();
void playSound(int freq, int duration);
void setLedColor(int r, int g, int b);
void updateLed();
void processInput();

#endif
