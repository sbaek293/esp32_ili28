# ESP32 ILI9341 Dongle Display

PC에 USB로 연결하는 2.8인치 터치 디스플레이 동글입니다.  
ESP32-C3 또는 ESP32-S3 + ILI9341 240×320 TFT + XPT2046 터치스크린 조합으로 동작합니다.

**Arduino IDE와 PlatformIO 두 가지 방법으로 펌웨어를 빌드할 수 있습니다.**

---

## 목차

1. [주요 기능](#주요-기능)
2. [화면 레이아웃](#화면-레이아웃-세로-240320)
3. [필요 부품](#필요-부품)
4. [핀 배선도](#핀-배선도)
5. [방법 A: Arduino IDE로 펌웨어 빌드](#방법-a-arduino-ide로-펌웨어-빌드-초보자-권장)
6. [방법 B: PlatformIO로 펌웨어 빌드](#방법-b-platformio로-펌웨어-빌드)
7. [터치 캘리브레이션](#터치-캘리브레이션)
8. [호스트(PC) 앱 설정](#호스트pc-앱-설정)
9. [통신 프로토콜](#통신-프로토콜)
10. [플랫폼별 미디어 지원](#플랫폼별-미디어-지원)
11. [프로젝트 구조](#프로젝트-구조)

---

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
+------------------------+  Y=0
| * PC Connected         |  상태바 (22px)
+------------------------+  Y=22
|                        |
|       14:32            |  시:분 (7-segment Font, 48px)
|         :25            |  초 (26px)
|  Thu Jan 15  2024      |  날짜 (16px)
+------------------------+  Y=116
|       Seoul            |  도시명
|  23°C  (21°C)          |  온도 / 체감온도 (26px, 오렌지)
|    Partly Cloudy       |  날씨 설명
|    Humidity: 60%       |  습도
+------------------------+  Y=208
| ~ Bohemian Rhapsody -> |  곡 제목 (긴 경우 자동 스크롤)
| * Queen                |  아티스트 (* = 재생 상태 표시)
| [########....] 75%     |  볼륨 바
| [|<] [V-] [>] [V+] [>|]|  터치 버튼 5개
+------------------------+  Y=320
```

---

## 필요 부품

| 부품 | 수량 | 비고 |
|------|------|------|
| ESP32-C3 DevKitM-1 **또는** ESP32-S3 DevKitC-1 | 1 | |
| 2.8" ILI9341 TFT LCD (XPT2046 터치 내장, SPI) | 1 | 240×320, 8핀 SPI 모듈 |
| USB-C 케이블 | 1 | PC 연결용 |
| 점퍼 와이어 | 약 10개 | |
| 브레드보드 (선택) | 1 | |

---

## 핀 배선도

> **중요**: VCC는 반드시 **3.3V**에 연결하세요. 5V를 연결하면 모듈이 손상됩니다.

### ESP32-C3 DevKitM-1 배선

```
ILI9341 모듈           ESP32-C3
─────────────────────────────────
VCC    ────────────── 3V3
GND    ────────────── GND
CS     ────────────── GPIO 10
RESET  ────────────── GPIO 8
DC/RS  ────────────── GPIO 4
MOSI   ────────────── GPIO 7
SCK    ────────────── GPIO 6
LED/BL ────────────── GPIO 3
MISO   ────────────── GPIO 2

(터치, XPT2046)
T_CLK  ────────────── GPIO 6   (SCK 공유)
T_CS   ────────────── GPIO 5
T_DIN  ────────────── GPIO 7   (MOSI 공유)
T_DO   ────────────── GPIO 2   (MISO 공유)
T_IRQ  ────────────── GPIO 9
```

| ILI9341 핀 이름 | ESP32-C3 GPIO | 기능 |
|----------------|--------------|------|
| VCC | 3V3 | 전원 (3.3V) |
| GND | GND | 접지 |
| CS | GPIO 10 | 디스플레이 칩셀렉 |
| RESET / RST | GPIO 8 | 디스플레이 리셋 |
| DC / RS | GPIO 4 | Data/Command 선택 |
| MOSI / SDI | GPIO 7 | SPI 데이터 출력 |
| SCK / CLK | GPIO 6 | SPI 클럭 |
| LED / BL | GPIO 3 | 백라이트 (HIGH=ON) |
| MISO / SDO | GPIO 2 | SPI 데이터 입력 |
| T_CS | GPIO 5 | 터치 칩셀렉 |
| T_IRQ | GPIO 9 | 터치 인터럽트 |

---

### ESP32-S3 DevKitC-1 배선

```
ILI9341 모듈           ESP32-S3
─────────────────────────────────
VCC    ────────────── 3V3
GND    ────────────── GND
CS     ────────────── GPIO 10
RESET  ────────────── GPIO 14
DC/RS  ────────────── GPIO 9
MOSI   ────────────── GPIO 11
SCK    ────────────── GPIO 12
LED/BL ────────────── GPIO 21
MISO   ────────────── GPIO 13

(터치, XPT2046)
T_CLK  ────────────── GPIO 12  (SCK 공유)
T_CS   ────────────── GPIO 46
T_DIN  ────────────── GPIO 11  (MOSI 공유)
T_DO   ────────────── GPIO 13  (MISO 공유)
T_IRQ  ────────────── GPIO 45
```

| ILI9341 핀 이름 | ESP32-S3 GPIO | 기능 |
|----------------|--------------|------|
| VCC | 3V3 | 전원 (3.3V) |
| GND | GND | 접지 |
| CS | GPIO 10 | 디스플레이 칩셀렉 |
| RESET / RST | GPIO 14 | 디스플레이 리셋 |
| DC / RS | GPIO 9 | Data/Command 선택 |
| MOSI / SDI | GPIO 11 | SPI 데이터 출력 |
| SCK / CLK | GPIO 12 | SPI 클럭 |
| LED / BL | GPIO 21 | 백라이트 (HIGH=ON) |
| MISO / SDO | GPIO 13 | SPI 데이터 입력 |
| T_CS | GPIO 46 | 터치 칩셀렉 |
| T_IRQ | GPIO 45 | 터치 인터럽트 |

> **핀 변경 방법**
> - **Arduino IDE**: `firmware/arduino/User_Setup_C3.h` (또는 S3) 파일의 `#define TFT_MOSI` 등 수정 후 TFT_eSPI 라이브러리 폴더에 복사
> - **PlatformIO**: `firmware/platformio.ini`의 `build_flags`에서 `-D TFT_MOSI=xx` 수정

---

## 방법 A: Arduino IDE로 펌웨어 빌드 (초보자 권장)

### 1단계 – Arduino IDE 설치

[https://www.arduino.cc/en/software](https://www.arduino.cc/en/software) 에서 Arduino IDE 2.x를 다운로드·설치합니다.

---

### 2단계 – ESP32 보드 지원 추가

1. Arduino IDE 메뉴 → **File > Preferences** 열기
2. "Additional boards manager URLs" 입력란에 아래 주소 추가:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **OK** 클릭
4. 메뉴 → **Tools > Board > Boards Manager** 열기
5. 검색창에 `esp32` 입력
6. **"esp32 by Espressif Systems"** 항목에서 **Install** 클릭 (최신 버전)
7. 설치 완료 후 보드 선택:
   - ESP32-C3: **Tools > Board > ESP32 Arduino > ESP32C3 Dev Module**
   - ESP32-S3: **Tools > Board > ESP32 Arduino > ESP32S3 Dev Module**

---

### 3단계 – 필수 라이브러리 설치

메뉴 → **Tools > Manage Libraries** (또는 Ctrl+Shift+I)

| 라이브러리 이름 | 제작자 | 최소 버전 |
|---------------|--------|---------|
| TFT_eSPI | Bodmer | 2.5.34 이상 |
| ArduinoJson | Benoit Blanchon | 7.1.0 이상 |

검색 후 **Install** 클릭.

---

### 4단계 – TFT_eSPI 핀 설정 (필수!)

TFT_eSPI는 라이브러리 폴더 안의 `User_Setup.h` 파일로 핀을 설정합니다.

**Arduino 라이브러리 폴더 위치:**

| OS | 경로 |
|----|------|
| Windows | `C:\Users\사용자명\Documents\Arduino\libraries\TFT_eSPI\` |
| macOS | `~/Documents/Arduino/libraries/TFT_eSPI/` |
| Linux | `~/Arduino/libraries/TFT_eSPI/` |

**복사 방법:**

```
# ESP32-C3를 사용하는 경우
firmware/arduino/User_Setup_C3.h  →  (위 경로)/TFT_eSPI/User_Setup.h 에 덮어쓰기

# ESP32-S3를 사용하는 경우
firmware/arduino/User_Setup_S3.h  →  (위 경로)/TFT_eSPI/User_Setup.h 에 덮어쓰기
```

> **주의**: 파일 이름을 반드시 `User_Setup.h`로 바꿔서 복사해야 합니다.

---

### 5단계 – 스케치 열기

1. Arduino IDE 메뉴 → **File > Open**
2. 이 저장소의 `firmware/arduino/esp32_ili28_display/esp32_ili28_display.ino` 파일 선택
3. Arduino IDE가 자동으로 같은 폴더의 `.h` / `.cpp` 파일들을 탭으로 표시합니다.

---

### 6단계 – 보드 및 포트 설정

**ESP32-C3:**

| 항목 | 설정값 |
|------|--------|
| Tools > Board | ESP32C3 Dev Module |
| Tools > USB CDC On Boot | **Enabled** |
| Tools > CPU Frequency | 160 MHz |
| Tools > Flash Mode | QIO |
| Tools > Flash Size | 4MB (32Mb) |
| Tools > Port | COM3 (Windows) / /dev/ttyUSB0 (Linux) |

**ESP32-S3:**

| 항목 | 설정값 |
|------|--------|
| Tools > Board | ESP32S3 Dev Module |
| Tools > USB CDC On Boot | **Enabled** |
| Tools > PSRAM | Disabled (필요 시 OPI PSRAM) |
| Tools > CPU Frequency | 240 MHz |
| Tools > Flash Mode | QIO 80MHz |
| Tools > Flash Size | 4MB (32Mb) |
| Tools > Port | COM3 (Windows) / /dev/ttyUSB0 (Linux) |

> **"USB CDC On Boot: Enabled"은 반드시 설정해야 합니다.**  
> 이 옵션이 있어야 USB로 Serial 통신이 작동합니다.

---

### 7단계 – 컴파일 및 업로드

1. 상단 툴바의 **✓ (Verify/Compile)** 버튼 클릭 → 오류 없이 컴파일 확인
2. ESP32 보드를 USB로 PC에 연결
3. **→ (Upload)** 버튼 클릭
4. 업로드 중 ESP32의 BOOT 버튼을 누르고 있다가 "Connecting..." 메시지가 나오면 놓기  
   (일부 보드는 자동으로 업로드됩니다)
5. "Done uploading" 메시지 확인

---

### 8단계 – 시리얼 모니터 확인

**Tools > Serial Monitor** 열기, 보드레이트 **115200** 선택  
디스플레이에 "Waiting for PC..." 가 표시되면 정상입니다.

---

## 방법 B: PlatformIO로 펌웨어 빌드

### 사전 조건

- [VSCode](https://code.visualstudio.com/) + [PlatformIO 익스텐션](https://platformio.org/platformio-ide) 설치  
  또는 PlatformIO Core CLI: `pip install platformio`

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

> PlatformIO는 `platformio.ini`의 `build_flags`로 핀을 설정하므로 `User_Setup.h` 파일을 수동으로 복사할 필요가 없습니다.

---

## 터치 캘리브레이션

처음 사용 시 터치 좌표를 캘리브레이션해야 정확하게 버튼이 인식됩니다.

### Arduino IDE에서 캘리브레이션

1. **File > Examples > TFT_eSPI > Generic > Touch_calibrate** 스케치 열기
2. 업로드 후 화면 안내에 따라 네 모서리의 흰 십자(+)를 순서대로 터치
3. 시리얼 모니터(115200 baud)에 5개 숫자가 출력됨:
   ```
   uint16_t calData[5] = { 275, 3620, 264, 3706, 1 };
   ```
4. `firmware/src/config.h` (또는 `firmware/arduino/esp32_ili28_display/config.h`)를 열어 수정:
   ```c
   #define TOUCH_CAL_DATA  { 275, 3620, 264, 3706, 1 }
   ```
5. 메인 스케치를 다시 업로드

> 기본값으로도 대부분의 2.8" XPT2046 모듈에서 동작하지만,  
> 터치가 어긋나면 반드시 캘리브레이션을 수행하세요.

---

## 호스트(PC) 앱 설정

### 1. Python 설치

Python 3.10 이상 필요. [python.org](https://www.python.org/downloads/) 에서 다운로드.

### 2. 의존성 설치

```bash
cd host
pip install -r requirements.txt
```

Linux에서 미디어 제어를 위해 playerctl이 필요합니다:
```bash
sudo apt install playerctl pulseaudio-utils
```

### 3. 설정 파일 작성

```bash
cp config.json.example config.json
```

`config.json` 편집:

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

| 항목 | 설명 |
|------|------|
| `port` | Windows: `COM3` / Linux: `/dev/ttyUSB0` / macOS: `/dev/cu.usbserial-*` |
| `api_key` | [openweathermap.org](https://openweathermap.org/api) 무료 가입 후 발급 |
| `city` | 날씨를 가져올 도시명 (영문, 예: `Seoul`, `Busan`) |
| `units` | `metric` (°C) 또는 `imperial` (°F) |

### 4. 실행

```bash
python main.py
```

포트를 직접 지정할 경우:
```bash
python main.py --port COM5
python main.py --port /dev/ttyUSB0
```

포트를 생략하면 USB 시리얼 장치를 자동 감지합니다.

---

## 통신 프로토콜

ESP32와 PC는 USB CDC 시리얼(115200 baud)로 JSON 줄을 주고받습니다.

### PC → ESP32 (JSON, 줄바꿈으로 종료)

| 타입 | JSON 예시 |
|------|----------|
| 시간 (T) | `{"t":"T","hh":"14","mm":"30","ss":"25","date":"Thu Jan 15","year":"2024"}` |
| 날씨 (W) | `{"t":"W","city":"Seoul","temp":"23","feels":"21","desc":"Partly Cloudy","hum":"60%"}` |
| 미디어 (M) | `{"t":"M","title":"Song","artist":"Artist","playing":1,"vol":75}` |

### ESP32 → PC (터치 이벤트)

```
CMD:prev        ← 이전 트랙
CMD:play_pause  ← 재생/일시정지
CMD:next        ← 다음 트랙
CMD:vol_down    ← 볼륨 내리기
CMD:vol_up      ← 볼륨 올리기
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
│   ├── platformio.ini                    # PlatformIO 빌드 설정 (C3 / S3)
│   ├── src/                              # PlatformIO 소스 (C++/Arduino)
│   │   ├── config.h                      # 핀, 색상, 레이아웃 상수
│   │   ├── main.cpp                      # setup() / loop()
│   │   ├── ui.h / ui.cpp                 # TFT 렌더링 (시계, 날씨, 미디어, 터치)
│   │   └── protocol.h / protocol.cpp     # USB 시리얼 JSON 파싱 & 커맨드 전송
│   └── arduino/                          # Arduino IDE용
│       ├── User_Setup_C3.h               # TFT_eSPI 핀 설정 (ESP32-C3)
│       ├── User_Setup_S3.h               # TFT_eSPI 핀 설정 (ESP32-S3)
│       └── esp32_ili28_display/
│           ├── esp32_ili28_display.ino   # 메인 스케치 (.ino)
│           ├── config.h
│           ├── ui.h / ui.cpp
│           └── protocol.h / protocol.cpp
└── host/                                 # PC Python 앱
    ├── main.py                           # 메인 루프
    ├── serial_comm.py                    # 시리얼 통신 래퍼
    ├── weather.py                        # OpenWeatherMap 클라이언트
    ├── media_control.py                  # 플랫폼별 미디어 제어
    ├── requirements.txt                  # pip 의존성
    └── config.json.example               # 설정 예시
```

---

## 자주 묻는 질문

**Q. 업로드 시 "A fatal error occurred: Failed to connect to ESP32" 오류**  
A. ESP32 보드의 BOOT 버튼을 누른 채로 업로드 버튼을 누르고, "Connecting..." 메시지가 나오면 BOOT 버튼을 놓으세요.

**Q. 디스플레이에 아무것도 표시되지 않음**  
A. 백라이트 핀(BL)이 HIGH인지 확인. `config.h`의 `TFT_BL` 핀 번호가 실제 배선과 일치하는지 확인.

**Q. 터치가 반응하지 않거나 위치가 어긋남**  
A. 터치 캘리브레이션을 다시 수행하세요. T_CS, T_IRQ 핀 배선을 확인하세요.

**Q. 호스트 앱에서 "No serial port found" 오류**  
A. `config.json`의 `port` 값을 직접 입력하거나, 장치 관리자(Windows) / `ls /dev/tty*` (Linux·macOS)로 포트명을 확인하세요.

**Q. 날씨가 표시되지 않음**  
A. `config.json`에 올바른 OpenWeatherMap API 키와 도시명(영문)이 입력되었는지 확인하세요. API 키는 발급 후 활성화되는 데 최대 2시간이 걸릴 수 있습니다.

---

## 라이선스

MIT
