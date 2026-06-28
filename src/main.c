#include <hardware/gpio.h>
#include <hardware/regs/intctrl.h>
#include <stdbool.h>
#include <stdio.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "hardware/irq.h"

#include "config.h"
#include "bme280.h"
#include "file_sys.h"
#include "cli.h"
#include "blocking.h"

// Глобальные переменные
static uint32_t elapsed_seconds = 0;        // время с момента старта

void blink(uint8_t times) {
    for (int i = 0; i < times; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(150);
        gpio_put(LED_PIN, 0);
        sleep_ms(150);
    }
}

// Точка входа
int main(void) {

    stdio_init_all();
    rtc_init();
    sleep_ms(500);

    // Инициализация светодиода
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Настройка I2C 
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    i2c_init(I2C_PORT, 100 * 1000);

    // Инициализация сенсора
    bme280_t bme280;

    bme280_init_struct(&bme280, I2C_PORT, BME280_ADDR, &pico_callbacks_blocking);

    bme280.mode = BME280_MODE_FORCED;
    bme280.temperature_sampling = BME280_SAMPLING_X16;
    bme280.pressure_sampling = BME280_SAMPLING_X16;
    bme280.humidity_sampling = BME280_SAMPLING_X16;
    bme280.filter_coefficient = BME280_FILTER_COEFF_16;

    if (bme280_init(&bme280) != BME280_OK) {
        fprintf(stderr, "Error: BME280 init failed\n");
        blink(2);
        return 1;
    }

    // Инитиализация ФС
    if (!file_sys_init()) {
        fprintf(stderr, "Error: LittleFS init failed\n");
        blink(3);
        return 1;
    }

    // Сообщаем, что все хорошо и заходим в главный цикл
    gpio_put(LED_PIN, 1);
    sleep_ms(500);
    gpio_put(LED_PIN, 0);

    // Основной цикл
    while (1) {
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT)
            cli_process(); // обрабатываем одну команду

        // Чтение показаний с сенсора
        bme280_reading_t record;
        if (bme280_forced_read(&bme280, &record) != BME280_OK) {
            blink(5);
            continue;
        }

        elapsed_seconds += MEASURE_INTERVAL_S;
        char line[128];
        snprintf(line, sizeof(line), "%lu,%.2f,%.2f,%.2f\n",
                elapsed_seconds, record.temperature, record.humidity, record.pressure);
        if (!file_sys_append_line(DATA_FNAME, line)) {
            blink(6);
        }

        // Если подключен терминал, то выводим в него
        if (stdio_usb_connected()) {
            printf("%lu,%.2f,%.2f,%.2f\n",
                elapsed_seconds, record.temperature, record.humidity, record.pressure);
            fflush(stdout);
        }

        sleep_ms(MEASURE_INTERVAL_S * 1000);
        blink(1);
     }

}
