#ifndef CONFIG_H
#define CONFIG_H

// Пины для WeAct RP2040
#define LED_PIN 25
#define KEY_PIN 23

// I2C для BME280
#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5
#define BME280_ADDR 0x76 // стандартный адресс (0x77 если SDO подключен к VCC)

#define MEASURE_INTERVAL_S 10 // (10 минут) Заменить потом на 3600 (1 час)

// LittleFS
#define FS_SIZE (256 * 1024)
#define FS_OFFSET (1024 * 1024)

// Имена файлов
#define DATA_FNAME "meteo.csv"

#endif // CONFIG_H
