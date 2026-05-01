# ESP32 ILI9341 Dongle Display

PC에 USB로 연결하는 2.8인치 터치 디스플레이 동글입니다.  
ESP32-C3 또는 ESP32-S3 + ILI9341 240×320 TFT + XPT2046 터치스크린 조합으로 동작합니다.

## 주요 기능

| 기능 | 내용 |
|------|------|
| 시계 | 실시간 HH:MM:SS + 날짜 (PC 시간 동기화) |
| 날씨 | OpenWeatherMap – 도시명, 온도, 체감온도, 날씨 설명, 습도 |
| 미디어 | 현재 재생 중인 곡 제목·아티스트·볼륨 표시 + 터치 컨트롤 |
| 터치 버튼 | 이전 트랙 / 볼륨 – / 재생·일시정지 / 볼륨 + / 다음 트랙 |

---

## 화면 레이아웃 (세로 240×320)

```
+------------------------+
| * PC Connected         |  상태바 (22 px)
+------------------------+
|                        |
|       14:32            |  시:분 (Font 7, 48 px)
|         :25            |  초 (Font 4)
|  Thu Jan 15  2024      |  날짜
+------------------------+
|       Seoul            |  도시
|  23°C  (21°C)          |  온도 / 체감온도
|    Partly Cloudy       |  날씨 설명
|    Humidity: 60%       |  습도
+------------------------+
| ~ Bohemian Rhapsody -> |  곡 제목 (긴 경우 자동 스크롤)
| * Queen                |  아티스트 (* 색으로 재생 상태 표시)
| ########....  75%      |  볼륨 바
| [|<] [V-] [>] [V+] [>|]|  터치 버튼
+------------------------+
```

---

## 하드웨어

### 준비물
- ESP32-C3 DevKitM-1 **또는** ESP32-S3 DevKitC-1
- 2.8인치 ILI9341 TFT (XPT2046 터치 내장, SPI)
- USB-C 케이블

### 배선

#### ESP32-C3

| ILI9341 핀 | ESP32-C3 GPIO |
|-----------|--------------|
| MOSI (SDI) | GPIO 7 |
| MISO (SDO) | GPIO 2 |
| SCK (CLK) | GPIO 6 |
| CS (LCD) | GPIO 10 |
| DC / RS | GPIO 4 |
| RST | GPIO 8 |
| BL (백라이트) | GPIO 3 |
| T_CS (터치) | GPIO 5 |
| T_IRQ | GPIO 9 |
| VCC | 3.3 V |
| GND | GND |

#### ESP32-S3

| ILI9341 핀 | ESP32-S3 GPIO |
|-----------|--------------|
| MOSI | GPIO 11 |
| MISO | GPIO 13 |
| SCK | GPIO 12 |
| CS | GPIO 10 |
| DC | GPIO 9 |
| RST | GPIO 14 |
| BL | GPIO 21 |
| T_CS | GPIO 46 |
| T_IRQ | GPIO 45 |

> **핀 변경**: `firmware/platformio.ini`의 `build_flags`에서 `TFT_*` / `TOUCH_CS` 값을 수정하세요.

---

## 터치 캘리브레이션

처음 사용 시 터치 좌표를 캘리브레이션해야 합니다.

1. PlatformIO에서 **TFT_eSPI** 라이브러리의 `Touch_calibrate` 예제 스케치를 빌드·업로드합니다.
2. 화면 안내에 따라 네 모서리를 차례로 터치합니다.
3. 시리얼 모니터에 출력된 5개의 숫자를 복사합니다.
4. `firmware/src/config.h`의 `TOUCH_CAL_DATA` 값을 교체합니다.

```c
// 예시 (실제 값은 모듈마다 다름)
#define TOUCH_CAL_DATA  { 275, 3620, 264, 3706, 1 }
```

---

## 펌웨어 빌드 & 업로드

### 사전 조건
- [PlatformIO](https://platformio.org/) (VSCode 익스텐션 또는 CLI)

### 빌드 및 업로드

```bash
cd firmware

# ESP32-C3
pio run -e esp32c3 --target upload

# ESP32-S3
pio run -e esp32s3 --target upload
```

시리얼 모니터:
```bash
pio device monitor
```

---

## 호스트(PC) 앱 설정

### 1. 의존성 설치

```bash
cd host
pip install -r requirements.txt
```

Linux에서 미디어 제어를 위해 playerctl이 필요합니다:
```bash
sudo apt install playerctl pulseaudio-utils
```

### 2. 설정 파일 작성

```bash
cp config.json.example config.json
```

`config.json` 예시:

```json
{
    "port": "COM3",
    "baud": 115200,
    "weather": {
        "api_key": "YOUR_OPENWEATHERMAP_API_KEY",
        "city": "Seoul",
        "units": "metric",
        "update_interval_sec": 300
    },
    "media": {
        "update_interval_sec": 2
    }
}
```

- **port**: Windows는 COMx, Linux는 /dev/ttyUSB0, macOS는 /dev/cu.usbserial-*
- **api_key**: [openweathermap.org](https://openweathermap.org/api) 무료 가입 후 발급 (Free 플랜으로 충분)

### 3. 실행

```bash
python main.py
```

포트를 명시적으로 지정하려면:
```bash
python main.py --port /dev/ttyUSB0
```

포트를 지정하지 않으면 연결된 USB 시리얼 장치를 자동으로 감지합니다.

---

## 통신 프로토콜

ESP32와 PC는 USB CDC 시리얼(115200 baud)로 JSON 줄을 주고받습니다.

### PC → ESP32

| 타입 | JSON 예시 |
|------|----------|
| 시간 | `{"t":"T","hh":"14","mm":"30","ss":"25","date":"Thu Jan 15","year":"2024"}` |
| 날씨 | `{"t":"W","city":"Seoul","temp":"23","feels":"21","desc":"Partly Cloudy","hum":"60%"}` |
| 미디어 | `{"t":"M","title":"Song","artist":"Artist","playing":1,"vol":75}` |

### ESP32 → PC (터치 이벤트)

```
CMD:prev
CMD:play_pause
CMD:next
CMD:vol_down
CMD:vol_up
```

---

## 플랫폼별 미디어 지원

| 기능 | Windows | macOS | Linux |
|------|---------|-------|-------|
| 재생/일시정지 | pynput 미디어 키 | osascript | playerctl |
| 이전/다음 트랙 | pynput | osascript | playerctl |
| 볼륨 조절 | pynput | osascript | pactl |
| 현재 곡 정보 | PowerShell SMTC | osascript | playerctl |

---

## 프로젝트 구조

```
esp32_ili28/
├── firmware/
│   ├── platformio.ini        # 빌드 설정 (C3 / S3)
│   └── src/
│       ├── config.h          # 핀, 색상, 레이아웃 상수
│       ├── main.cpp          # setup() / loop()
│       ├── ui.h / ui.cpp     # TFT 렌더링 (시계, 날씨, 미디어, 터치)
│       └── protocol.h / .cpp # USB 시리얼 JSON 파싱 & 커맨드 전송
└── host/
    ├── main.py               # 메인 루프 (시간/날씨/미디어 전송)
    ├── serial_comm.py        # 스레드 안전 시리얼 래퍼
    ├── weather.py            # OpenWeatherMap API 클라이언트
    ├── media_control.py      # 플랫폼별 미디어 제어
    ├── requirements.txt      # Python 의존성
    └── config.json.example   # 설정 예시
```

---

## 라이선스

MIT
