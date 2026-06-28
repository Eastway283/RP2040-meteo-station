# RP2040 Autonomous Weather Station
 #### Русский:

Автономная метеостанция на базе микроконтроллера RP2040 (WeAct) и датчика BME280. Измеряет температуру, влажность и давление, сохраняет данные в энергонезависимую файловую систему LittleFS и позволяет выгружать CSV-логи через USB-терминал. Проект ориентирован на низкое энергопотребление, полностью написан на C с использованием Pico SDK и не требует постоянного подключения к компьютеру.

#### English:

Autonomous weather station based on RP2040 (WeAct) and BME280. It measures temperature, humidity and pressure, stores data in LittleFS, and provides a CLI for CSV export via USB. Optimized for low power consumption and built with pure C and Pico SDK. No permanent PC connection needed.

## Возможности/Features
#### Русский:
- Опрос датчика **BME280** (I²C) с настраиваемым интервалом
- Сохранение измерений в `meteo.csv` на LittleFS
- Встроенный CLI (`dump`, `clear`, `ls`, `help`)
- Автономная работа без постоянного подключения к компьютеру
- Низкое энергопотребление в режиме ожидания
- Полная изоляция файловой системы и драйверов – легко переносить и модифицировать

#### English:
- Reads **BME280** (I²C) at configurable intervals
- Saves measurements as CSV (`meteo.csv`) in LittleFS
- Built‑in CLI (`dump`, `clear`, `ls`, `help`)
- Fully autonomous – no permanent PC connection required
- Low‑power sleep between measurements
- Clean modular structure – easy to modify and extend

## Аппаратное обеспечение/Hardware
#### Русский:
- **Плата**: WeAct RP2040 Pico (8 МБ Flash) или любая совместимая
- **Датчик**: BME280 (I²C, адрес `0x76` / `0x77`)
- Соединительные провода «мама‑мама»

#### English:
- **Board**: WeAct RP2040 Pico (16 MB Flash) or compatible
- **Sensor**: BME280 (I²C, address `0x76` / `0x77`)
- Jumper wires

### Подключение BME280/Wiring
| BME280 | RP2040   |
|--------|----------|
| VCC    | 3.3V     |
| GND    | GND      |
| SDA    | GP4      |
| SCL    | GP5      |
| CS     | 3.3V     |

> **Внимание:** Для выбора режима I²C вывод CS должен быть подключён к 3.3V. Программная подтяжка через GPIO не гарантирует стабильную работу.
>
> **Important:** CS must be tied to 3.3V to enable I²C mode. Software GPIO pull‑up is not reliable.

## Структура проекта/Project Structure
```
weather-station/
├── src/
│ ├── main.c # entry point and main loop
│ └── config.h # pins, timings, filenames
├── drivers/
│ ├── bme280/ # BME280 driver (fivdi/pico-devices)
│ │ ├── bme280.h / .c
│ │ ├── blocking.h / .c
│ │ └── callbacks.h
│ ├── cli/ # command line interface
│ │ ├── cli.h / .c
│ └── fsys/ # filesystem abstraction
│ ├── file_sys.h / .c
├── lib/
│ └── littlefs/ # LittleFS (submodule)
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── README.md
└── LICENSE
```

## Зависимости и инструменты/Requirements
#### Русский:

- **Raspberry Pi Pico SDK** (версия 1.5.0 или новее)
- **CMake** ≥ 3.13
- **GNU Arm Embedded Toolchain** (`gcc-arm-none-eabi`)
- **LittleFS** (поставляется с SDK или клонирована локально)
- **picocom** (или другой последовательный терминал)

Установка под Ubuntu / Debian:

```bash
sudo apt update
sudo apt install build-essential cmake gcc-arm-none-eabi git picocom
```

Клонирование Pico SDK:
```bash
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk && git submodule update --init --recursive && cd ..
```

#### English:

- **Raspberry Pi Pico SDK** (1.5.0 or later)
- **CMake** ≥ 3.13
- **GNU Arm Embedded Toolchain** (`gcc-arm-none-eabi`)
- **LittleFS** (bundled with SDK or cloned locally)
- **picocom** (or any serial terminal)

Installation on Ubuntu / Debian:

```bash
sudo apt update
sudo apt install build-essential cmake gcc-arm-none-eabi git picocom
```

Clone the Pico SDK:
```bash
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk && git submodule update --init --recursive && cd ..
```

## Сборка/Build

#### Русский:

1. Клонируйте репозиторий станции:
```bash
git clone https://Eastway283/weather-station.git
cd weather-station
```

2. Укажите путь к SDK (также можно прописать в ~/.bashrc):
```bash
export PICO_SDK_PATH=/absolute/path/to/pico-sdk
```

3. Соберите проект:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

4. Прошейте плату:

- Зажмите кнопку BOOT на плате

- Подключите USB‑кабель

- Скопируйте файл meteo.uf2 на появившийся диск

#### English:

1. Clone this repository:
```bash
git clone https://github.com/Eastway283/weather-station.git
cd weather-station
```

2. Set the SDK path (can be added to ~/.bashrc):
```bash
export PICO_SDK_PATH=/absolute/path/to/pico-sdk
```

3. Compile:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

4. Flash the board:

- Hold the BOOT button on the board

- Connect USB cable

- Copy meteo.uf2 to the mounted drive

## Использование/Usage

#### Русский:

После прошивки станция немедленно начинает измерения. Для взаимодействия откройте последовательный порт:
```bash
picocom /dev/ttyACM0 -b 115200 --echo
```
### Команды CLI

Нажмите любую клавишу и введите команду, завершив Enter:
| Команда	| Описание                            |
|--------- | ---------------------------------- |
| dump	| Показать весь файл данных (meteo.csv) |
| clear	| Удалить файл данных                   |
| ls	| Список файлов в LittleFS                |
| help	| Краткая справка                       |

Пример вывода данных:
```text
10,23.45,54.12,1013.25
20,23.50,54.07,1013.20
30,23.47,53.98,1013.22
```

### Сохранение данных на ПК

Через логирование самого picocom:
```bash
picocom /dev/ttyACM0 -b 115200 --logfile data.csv
# введите dump, выйдите по Ctrl+A, Ctrl+Q
```

Файл data.csv будет содержать весь вывод команды dump.

#### English:

Once flashed, the station starts collecting data immediately. Open a serial terminal:
```bash
picocom /dev/ttyACM0 -b 115200 --echo
```

### CLI Commands

Press any key, then type a command and press Enter:
| Command	| Description |
|--------- | ------------- | 
| dump	| Print entire data file (meteo.csv) |
| clear	| Delete the data file |
| ls	| List files on LittleFS |
| help	| Show command list |

Sample output:
```text
10,23.45,54.12,1013.25
20,23.50,54.07,1013.20
30,23.47,53.98,1013.22
```

### Saving data to PC

Use picocom logging:
```bash
picocom /dev/ttyACM0 -b 115200 --logfile data.csv
# type dump, exit with Ctrl+A, Ctrl+Q
```

The file data.csv will contain the full dump.

## Настройка/Configuration

#### Русский:

Все параметры собраны в файле `src/config.h` (кроме файловой системы):
 - `MEASURE_INTERVAL_S` – период измерений (по умолчанию 10 секунд; для реальной работы рекомендуется 3600)

- `LED_PIN, KEY_PIN` – пины светодиода и кнопки

- `I2C_PORT, SDA_PIN, SCL_PIN, BME280_ADDR` – конфигурация I²C

- `FS_SIZE, FS_OFFSET` – размер и смещение файловой системы

- `DATA_FNAME` – имя файла данных (по умолчанию meteo.csv)

#### English:

All settings are in `src/config.h` (except for the FS):

- MEASURE_INTERVAL_S – measurement interval (default 10 s; use 3600 for hourly logs)

- `LED_PIN, KEY_PIN` – LED and button pins

- `I2C_PORT, SDA_PIN, SCL_PIN, BME280_ADDR` – I²C settings

- `FS_SIZE, FS_OFFSET` – filesystem size and offset

- `DATA_FNAME` – data filename (default meteo.csv)

## Ограничения и известные проблемы/Limitations & Known Issues

#### Русский:

- Режим глубокого сна (DORMANT) не используется. Применяется sleep_ms, который даёт потребление ~1.3 мА в SLEEP. Этого достаточно для большинства автономных сценариев, но не является рекордным.

- CS датчика должен быть подтянут к 3.3V аппаратно. Программная подтяжка нестабильна.

- Файловая система при первом запуске автоматически форматируется. При повторных запусках монтируется уже существующая.

- CLI иногда требует нажатия Enter перед вводом команды (особенность USB‑CDC и picocom).

- Все вычисления выполняются на МК с плавающей точкой (float), что может замедлять работу на очень малых интервалах. Для периодических измерений это некритично.

#### English:

- Deep sleep (DORMANT) is not implemented. The project uses sleep_ms, which keeps the MCU in SLEEP mode (~1.3 mA). Still acceptable for most battery‑powered scenarios.

- CS pin must be hardwired to 3.3V. Software pull‑up is unreliable on many BME280 modules.

- Filesystem is automatically formatted on first boot. Subsequent boots mount the existing filesystem.

- CLI occasionally requires an extra Enter before accepting a command (due to USB‑CDC/picocom behaviour).

- All sensor computations are done on the MCU using floating‑point arithmetic. For very short intervals (<1 s) this may affect timing, but is perfectly fine for periodic logging.

## Благодарности/Acknowledgements

- BME280 driver – fivdi/pico-devices (MIT License)

- LittleFS – littlefs-project/littlefs (BSD‑3‑Clause)

- Pico SDK – raspberrypi/pico-sdk

## Авторы/Author's

Разработано в учебных целях Eastway283 & Kosto4ka. Вопросы, предложения и критика приветствуются

Developed by Eastway283 & Kosto4ka as an embedded systems learning project. Feedback and contributions are welcome
