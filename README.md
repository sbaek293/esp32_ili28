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
5. [멀티미터 핀 전압 기준](#멀티미터-핀-전압-기준)
6. [방법 A: Arduino IDE로 펌웨어 빌드](#방법-a-arduino-ide로-펌웨어-빌드-초보자-권장)
7. [방법 B: PlatformIO로 펌웨어 빌드](#방법-b-platformio로-펌웨어-빌드)
8. [터치 캘리브레이션](#터치-캘리브레이션)
9. [호스트(PC) 앱 설정](#호스트pc-앱-설정)
10. [디스플레이 작동 확인 방법](#디스플레이-작동-확인-방법-호스트-pc)
11. [통신 프로토콜](#통신-프로토콜)
12. [플랫폼별 미디어 지원](#플랫폼별-미디어-지원)
13. [프로젝트 구조](#프로젝트-구조)

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
| ESP32-C3 DevKitM-1 **또는** ESP32-C3 Supermini | 1 | C3 계열 |
| ESP32-S3 DevKitC-1 **또는** ESP32-S3 Supermini | 1 | S3 계열 |
| 2.8" ILI9341 TFT LCD (XPT2046 터치 내장, SPI) | 1 | 240×320, 8핀 SPI 모듈 |
| USB-C 케이블 | 1 | PC 연결용 |
| 점퍼 와이어 | 약 10개 | |
| 브레드보드 (선택) | 1 | |

---

## 핀 배선도

> **중요**: VCC는 반드시 **3.3V**에 연결하세요. 5V를 연결하면 모듈이 손상됩니다.

> **보드 호환성 요약**
> | 보드 | 핀 설정 파일 | 비고 |
> |------|-------------|------|
> | ESP32-C3 DevKitM-1 | `User_Setup_C3.h` | GPIO 동일 |
> | ESP32-C3 Supermini | `User_Setup_C3.h` | GPIO 동일 – 같은 파일 사용 |
> | ESP32-S3 DevKitC-1 | `User_Setup_S3.h` | GPIO 45/46 사용 |
> | ESP32-S3 Supermini | `User_Setup_S3_Supermini.h` | GPIO 45/46 미노출 → T_CS=15, T_IRQ=16 |

### ESP32-C3 DevKitM-1 / Supermini 배선

> ESP32-C3 Supermini는 DevKitM-1과 핀 번호가 동일합니다. 같은 배선·설정 파일을 그대로 사용하세요.

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

---

### ESP32-S3 Supermini 배선

> **주의**: ESP32-S3 Supermini는 GPIO 45·46이 핀 헤더에 노출되지 않습니다.  
> 따라서 T_CS와 T_IRQ를 GPIO 15·16으로 변경하여 사용하세요.

```
ILI9341 모듈           ESP32-S3 Supermini
─────────────────────────────────────────
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
T_CS   ────────────── GPIO 15   ← Supermini 전용 (GPIO 46 미노출)
T_DIN  ────────────── GPIO 11  (MOSI 공유)
T_DO   ────────────── GPIO 13  (MISO 공유)
T_IRQ  ────────────── GPIO 16   ← Supermini 전용 (GPIO 45 미노출)
```

| ILI9341 핀 이름 | ESP32-S3 Supermini GPIO | 기능 |
|----------------|------------------------|------|
| VCC | 3V3 | 전원 (3.3V) |
| GND | GND | 접지 |
| CS | GPIO 10 | 디스플레이 칩셀렉 |
| RESET / RST | GPIO 14 | 디스플레이 리셋 |
| DC / RS | GPIO 9 | Data/Command 선택 |
| MOSI / SDI | GPIO 11 | SPI 데이터 출력 |
| SCK / CLK | GPIO 12 | SPI 클럭 |
| LED / BL | GPIO 21 | 백라이트 (HIGH=ON) |
| MISO / SDO | GPIO 13 | SPI 데이터 입력 |
| T_CS | GPIO 15 | 터치 칩셀렉 |
| T_IRQ | GPIO 16 | 터치 인터럽트 |

> **핀 변경 방법**
> - **Arduino IDE**: `firmware/arduino/User_Setup_C3.h` (또는 S3) 파일의 `#define TFT_MOSI` 등 수정 후 TFT_eSPI 라이브러리 폴더에 복사
> - **PlatformIO**: `firmware/platformio.ini`의 `build_flags`에서 `-D TFT_MOSI=xx` 수정

---

## 멀티미터 핀 전압 기준

멀티미터로 DC 전압을 측정할 때 예상되는 핀별 정상 전압입니다.  
**GND(검은 프로브)를 ESP32 GND 핀에, 빨간 프로브를 측정 핀에 대세요.**

> **주의**: SPI 신호 핀(MOSI, MISO, SCK)은 데이터 전송 중 0 V와 3.3 V 사이를 빠르게 오가므로,  
> 멀티미터로는 0~3.3 V 사이의 평균값(약 1~2 V)이 표시될 수 있습니다.

### ESP32-C3 핀 전압

| 핀 이름 | ESP32-C3 GPIO | 정상 전압 | 비고 |
|---------|--------------|----------|------|
| VCC | 3V3 | **3.3 V** | 전원 공급 |
| GND | GND | **0 V** | 기준 접지 |
| CS | GPIO 10 | **3.3 V** | 비선택(idle) 시 HIGH |
| RESET | GPIO 8 | **3.3 V** | 정상 동작 중 HIGH |
| DC/RS | GPIO 4 | 0 – 3.3 V | 명령/데이터 전환 |
| MOSI | GPIO 7 | 0 – 3.3 V | SPI 데이터 전송 중 변동 |
| SCK | GPIO 6 | 0 – 3.3 V | SPI 클럭 전송 중 변동 |
| LED/BL | GPIO 3 | **3.3 V** | 백라이트 ON 시 HIGH |
| MISO | GPIO 2 | 0 – 3.3 V | SPI 데이터 수신 중 변동 |
| T_CS | GPIO 5 | **3.3 V** | 비선택(idle) 시 HIGH |
| T_IRQ | GPIO 9 | **3.3 V** (터치 없을 때) / **0 V** (터치 중) | 터치 감지 시 LOW |

### ESP32-S3 DevKitC-1 핀 전압

| 핀 이름 | ESP32-S3 GPIO | 정상 전압 | 비고 |
|---------|--------------|----------|------|
| VCC | 3V3 | **3.3 V** | 전원 공급 |
| GND | GND | **0 V** | 기준 접지 |
| CS | GPIO 10 | **3.3 V** | 비선택(idle) 시 HIGH |
| RESET | GPIO 14 | **3.3 V** | 정상 동작 중 HIGH |
| DC/RS | GPIO 9 | 0 – 3.3 V | 명령/데이터 전환 |
| MOSI | GPIO 11 | 0 – 3.3 V | SPI 데이터 전송 중 변동 |
| SCK | GPIO 12 | 0 – 3.3 V | SPI 클럭 전송 중 변동 |
| LED/BL | GPIO 21 | **3.3 V** | 백라이트 ON 시 HIGH |
| MISO | GPIO 13 | 0 – 3.3 V | SPI 데이터 수신 중 변동 |
| T_CS | GPIO 46 | **3.3 V** | 비선택(idle) 시 HIGH |
| T_IRQ | GPIO 45 | **3.3 V** (터치 없을 때) / **0 V** (터치 중) | 터치 감지 시 LOW |

### ESP32-S3 Supermini 핀 전압

> T_CS/T_IRQ 핀만 다르고 나머지는 S3 DevKitC-1과 동일합니다.

| 핀 이름 | ESP32-S3 Supermini GPIO | 정상 전압 | 비고 |
|---------|------------------------|----------|------|
| T_CS | GPIO 15 | **3.3 V** | 비선택(idle) 시 HIGH |
| T_IRQ | GPIO 16 | **3.3 V** (터치 없을 때) / **0 V** (터치 중) | 터치 감지 시 LOW |

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
# ESP32-C3 DevKitM-1 또는 C3 Supermini를 사용하는 경우
firmware/arduino/User_Setup_C3.h          →  (위 경로)/TFT_eSPI/User_Setup.h 에 덮어쓰기

# ESP32-S3 DevKitC-1를 사용하는 경우
firmware/arduino/User_Setup_S3.h          →  (위 경로)/TFT_eSPI/User_Setup.h 에 덮어쓰기

# ESP32-S3 Supermini를 사용하는 경우
firmware/arduino/User_Setup_S3_Supermini.h →  (위 경로)/TFT_eSPI/User_Setup.h 에 덮어쓰기
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
| Tools > Port | COM3 (Windows) / /dev/ttyUSB0 (Linux) / /dev/cu.usbmodem* (macOS) |

**ESP32-S3:**

| 항목 | 설정값 |
|------|--------|
| Tools > Board | ESP32S3 Dev Module |
| Tools > USB CDC On Boot | **Enabled** |
| Tools > PSRAM | Disabled (필요 시 OPI PSRAM) |
| Tools > CPU Frequency | 240 MHz |
| Tools > Flash Mode | QIO 80MHz |
| Tools > Flash Size | 4MB (32Mb) |
| Tools > Port | COM3 (Windows) / /dev/ttyUSB0 (Linux) / /dev/cu.usbmodem* (macOS) |

> **"USB CDC On Boot: Enabled"은 반드시 설정해야 합니다.**  
> 이 옵션이 있어야 USB로 Serial 통신이 작동합니다.

> **macOS 포트 관련 참고사항**  
> macOS에서 ESP32-C3/S3 보드는 `/dev/cu.usbmodem*` 형태로 표시됩니다.  
> 포트가 목록에 나타나지 않으면 아래 **FAQ – macOS 포트 미표시** 항목을 참고하세요.

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

# ESP32-C3 (DevKitM-1 또는 Supermini – 동일 설정)
pio run -e esp32c3 --target upload

# ESP32-S3 DevKitC-1
pio run -e esp32s3 --target upload

# ESP32-S3 Supermini (T_CS=GPIO15, T_IRQ=GPIO16)
pio run -e esp32s3_supermini --target upload
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

> **지원 OS**: Windows 10/11, macOS (Monterey 12 이상 권장)

### 1. Python 설치

Python 3.10 이상 필요. [python.org](https://www.python.org/downloads/) 에서 다운로드.

### 2. 의존성 설치

```bash
cd host
pip install -r requirements.txt
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
| `port` | Windows: `COM3` / macOS: `/dev/cu.usbserial-*` 또는 `/dev/cu.usbmodem*` |
| `api_key` | [openweathermap.org](https://openweathermap.org/api) 무료 가입 후 발급 |
| `city` | 날씨를 가져올 도시명 (영문, 예: `Seoul`, `Busan`) |
| `units` | `metric` (°C) 또는 `imperial` (°F) |

### 4. 실행

```bash
python main.py
```

포트를 직접 지정할 경우:

**Windows:**
```bash
python main.py --port COM3
```

**macOS:**
```bash
python main.py --port /dev/cu.usbmodem101
```

포트를 생략하면 USB 시리얼 장치를 자동 감지합니다.

---

## 디스플레이 작동 확인 방법 (호스트 PC)

디스플레이가 제대로 동작하는지 확인하려면 아래 순서를 따르세요.

### 1단계 – 펌웨어 업로드 확인

- 디스플레이에 **"Waiting for PC..."** 문구가 표시되면 펌웨어가 정상 동작 중입니다.
- 아무것도 표시되지 않으면 배선(특히 VCC, GND, BL 핀)을 재확인하세요.

### 2단계 – 포트 확인

| OS | 확인 방법 |
|----|----------|
| Windows | 장치 관리자 → 포트(COM & LPT) → "Silicon Labs CP210x" 또는 "USB-SERIAL CH340" 항목의 COM 번호 확인 |
| macOS | 터미널에서 `ls /dev/cu.*` 실행 → `cu.usbmodem*` 또는 `cu.usbserial-*` 형태의 장치 확인 |

### 3단계 – 호스트 앱 실행

```bash
cd host
python main.py
```

앱이 정상 연결되면 다음 로그가 출력됩니다:
```
HH:MM:SS  INFO     Auto-detected port: COM3 (Silicon Labs CP210x USB to UART Bridge)
HH:MM:SS  INFO     Host running.  Press Ctrl-C to quit.
```

### 4단계 – 디스플레이 화면 확인

| 항목 | 정상 표시 |
|------|----------|
| 상태바 | `* PC Connected` 표시 |
| 시계 | 현재 시:분:초 + 날짜가 실시간 갱신 |
| 날씨 | 도시명·온도·날씨 설명 표시 (API 키가 설정된 경우) |
| 미디어 | 재생 중인 곡 제목·아티스트 표시 (음악 앱 실행 중인 경우) |

### 빠른 테스트 (API 키 없이)

날씨 API 없이도 시계 표시만으로 연결 여부를 확인할 수 있습니다.  
`config.json`의 `api_key`를 임의 문자열로 두면 날씨는 표시되지 않지만 시계는 즉시 동작합니다.

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

| 기능 | Windows | macOS |
|------|---------|-------|
| 재생/일시정지 | pynput 미디어 키 | osascript |
| 이전/다음 트랙 | pynput | osascript |
| 볼륨 조절 | pynput | osascript |
| 현재 곡 정보 | PowerShell SMTC | osascript |

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
│       ├── User_Setup_C3.h               # TFT_eSPI 핀 설정 (ESP32-C3 DevKitM-1 / Supermini)
│       ├── User_Setup_S3.h               # TFT_eSPI 핀 설정 (ESP32-S3 DevKitC-1)
│       ├── User_Setup_S3_Supermini.h     # TFT_eSPI 핀 설정 (ESP32-S3 Supermini)
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

**Q. macOS에서 `/dev/cu.usbmodem` 포트가 Arduino IDE 목록에 나타나지 않음**  
A. 다음 순서로 해결하세요:

1. **USB CDC On Boot 옵션 확인** – `Tools > USB CDC On Boot`가 **Enabled**인지 확인합니다.  
   (처음 펌웨어를 올리기 전이라 이 설정을 적용할 수 없을 경우 아래 2번으로 진행)

2. **수동 부트로더 모드 진입** – 포트가 보이지 않을 때:  
   a. ESP32 보드의 **BOOT** 버튼을 누른 채로  
   b. **RESET** 버튼을 짧게 누른 뒤 떼고  
   c. **BOOT** 버튼을 놓습니다  
   → 부트로더 모드에서 `/dev/cu.usbmodem*` 포트가 목록에 나타납니다.

3. **포트 확인** – 터미널에서 `ls /dev/cu.*` 를 실행하여 `/dev/cu.usbmodem101` 같은 이름이 있는지 확인합니다.

4. **케이블 교체** – 충전 전용 USB-C 케이블은 데이터 전송을 지원하지 않습니다. 데이터 통신이 가능한 케이블을 사용하세요.

5. **Arduino IDE 재시작** – 포트를 연결한 뒤 Arduino IDE를 재시작하면 목록이 갱신됩니다.

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
