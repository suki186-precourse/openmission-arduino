## 🕹️ 아두이노 브릭 브레이커 (Arduino Brick Breaker)

아두이노 UNO, 조이스틱 쉴드, I2C OLED를 사용하여 간단한 벽돌깨기 게임기를 구현한다.

**[프로젝트 환경]**

- 하드웨어: Arduino Uno
- 디스플레이: 128x64 I2C OLED

---

### ✨ 구현할 기능 목록

**🔹 1. 기본 설정**

- [x] 조이스틱 쉴드 핀맵 확인 (A0: X축, D8: 버튼)
- [x] I2C OLED 핀 연결 (A4: SDA, A5: SCL)
- [x] 3색 LED 핀 연결 (D9: R, D10: G, D11: B)
- [x] 부저 핀 연결 (D12)
- [x] setup() 함수에서 각 핀 모드(INPUT_PULLUP, OUTPUT) 설정
- [x] I2C OLED 라이브러리 초기화

**🔹 2. 입력 처리**

- [x] 조이스틱 X축 값 읽기
- [x] 읽어온 값을 패들 X좌표(0~127)로 변환
- [x] 조이스틱 버튼(D8) 값 읽기 (PULLUP)
- [x] 버튼 입력 시 디바운스 처리

**🔹 3. 게임 로직**

- [x] 게임 상태(Game State) 관리 (TITLE, READY, PLAYING, GAME_OVER, GAME_CLEAR)

[Ready 상태]

- [x] 공이 패들 위에 붙어서 함께 이동
- [x] 버튼 입력 시 공 발사 및 PLAYING 상태로 변경

[Playing 상태]

- [x] 공의 위치(x, y)와 속도(dx, dy)를 기반으로 이동 처리
- [x] 공-벽(상, 좌, 우) 충돌 검사 및 방향 반전
- [x] 공-패들 충돌 검사 및 방향 반전
- [x] 공-바닥 충돌 검사 (생명 감소, READY 상태로 복귀)
- [x] 벽돌 배열 생성 및 관리 (예: 2차원 배열)
- [x] 공-벽돌 충돌 검사 (벽돌 삭제, 공 방향 반전)
- [x] 점수: 벽돌 제거 +100, 바닥 충돌 -50

[GameOver 상태]

- [x] 생명 0개 시 GAME_OVER 상태로 변경
- [x] 점수(Score) 보이기

[Clear 상태]

- [x] 벽돌 0개 시 CLEAR 상태로 변경
- [x] 점수(Score) 보이기

**🔹 4. 디스플레이 출력**

- [x] TITLE 상태: "BRICK BREAKER" 텍스트 출력
- [x] GAME_OVER 상태: "GAME OVER" 텍스트 출력
- [x] CLEAR 상태: "CLEAR" 텍스트 출력
- [x] PLAYING 상태: 패들, 공, 벽돌, 하트 그리기
- [x] 화면 업데이트

**🔹 5. 피드백 출력**

LED

- [x] BLUE: TITLE, READY 상태
- [x] GREEN: PLAYING 상태
- [x] RED: GAME_OVER 상태, 생명 감소
- [x] ALL: GAME_CLEAR 상태 (RGB 순차 점등)
- [x] WHITE: 버튼 클릭

부저

- [x] 게임 시작 / 오버 / 클리어 시 사운드
- [x] 공-패들, 공-벽 충돌 시 사운드
- [x] 공-벽돌 충돌 시 사운드
- [x] 생명 감소 시 사운드

---

### 📁 디렉토리 구조

```bash
myProject/
├── myProject.ino       # 메인 파일
├── Config.h            # 상수, 게임 밸런스 설정
├── Types.h             # 구조체, 게임 상태 정의
├── Globals.h           # 점수, 공의 위치 등 전역 변수
├── Hardware.h          # 조이스틱, 부저, LED를 제어하는 함수 목록
├── Hardware.cpp        # 실제 조이스틱 값을 읽거나 소리를 내는 코드
├── GameLogic.h         # 게임 규칙, 충돌 감지, 점수 계산 함수 목록
├── GameLogic.cpp       # 공이 움직이고 벽돌이 깨지는 수학적 계산 코드
├── Renderer.h          # 화면에 점수, 공, 패들을 그리는 함수 목록
└── Renderer.cpp        # 실제로 화면에 픽셀을 찍는 코드
```

---

### 💡 트러블 슈팅, 고민했던 부분
