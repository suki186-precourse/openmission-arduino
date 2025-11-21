## 🕹️ 아두이노 브릭 브레이커 (Arduino Brick Breaker)

아두이노 UNO, 조이스틱 쉴드, I2C OLED를 사용하여 간단한 벽돌깨기 게임기를 구현한다.

**[프로젝트 환경]**

- 하드웨어: Arduino Uno
- 디스플레이: 128x64 I2C OLED
- 함수는 한 가지 일만 하도록 최대한 작게 분리

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

- [ ] 게임 상태(Game State) 관리 (TITLE, READY, PLAYING, GAME_OVER, CLEAR)

[Ready 상태]

- [x] 공이 패들 위에 붙어서 함께 이동
- [x] 버튼 입력 시 공 발사 및 PLAYING 상태로 변경

[Playing 상태]

- [x] 공의 위치(x, y)와 속도(dx, dy)를 기반으로 이동 처리
- [x] 공-벽(상, 좌, 우) 충돌 검사 및 방향 반전
- [x] 공-패들 충돌 검사 및 방향 반전
- [x] 공-바닥 충돌 검사 (생명 감소, READY 상태로 복귀)
- [ ] 벽돌 배열 생성 및 관리 (예: 2차원 배열)
- [ ] 공-벽돌 충돌 검사 (벽돌 삭제, 공 방향 반전)

[GameOver 상태]

- [x] 생명 0개 시 GAME_OVER 상태로 변경

[Clear 상태]

- [ ] 벽돌 0개 시 GAME_CLEAR 상태로 변경

**🔹 4. 디스플레이 출력**

- [x] TITLE 상태: "BRICK BREAKER" 텍스트 출력
- [ ] GAME_OVER 상태: "GAME OVER" 텍스트 출력
- [ ] CLEAR 상태: "CLEAR" 텍스트 출력
- [x] PLAYING 상태: 패들, 공, 벽돌, 하트 그리기
- [x] 화면 업데이트

**🔹 5. 피드백 출력**

LED

- [ ] BLUE: TITLE, READY 상태
- [ ] GREEN: PLAYING 상태
- [ ] RED: GAME_OVER 상태, 생명 감소 시 깜빡임
- [ ] ALL: CLEAR 상태 (RGB 순차 점등)

부저

- [ ] 게임 시작 / 오버 / 클리어 시 사운드
- [ ] 공-패들, 공-벽 충돌 시 사운드
- [ ] 공-벽돌 충돌 시 사운드
- [ ] 생명 감소 시 사운드

---

### 📁 디렉토리 구조

```bash
myProject/
├── Myproject.ino    # 메인 스케치 파일
│
└── logics
    ├── ball.h       # 공 로직
    ├── paddle.h     # 패들 로직
    └── display.h    # OLED 출력 함수
```

---

### 💡 트러블 슈팅, 고민했던 부분
