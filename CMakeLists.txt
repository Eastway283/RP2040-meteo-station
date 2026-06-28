cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(meteo-station C CXX ASM)
pico_sdk_init()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Все исходные файлы
add_executable(meteo
    src/main.c
    drivers/bme280/bme280.c
    drivers/bme280/blocking.c
    drivers/cli/cli.c
    drivers/fsys/file_sys.c
    lib/littlefs/lfs.c
    lib/littlefs/lfs_util.c
)

# Пути к заголовкам
target_include_directories(meteo PRIVATE
    src
    drivers/bme280
    drivers/cli
    drivers/fsys
    lib/littlefs
    ${PICO_SDK_PATH}/src/rp2_common/hardware_powman/include
)

# Системный include для ARM
target_include_directories(meteo SYSTEM PRIVATE
    /usr/lib/gcc/arm-none-eabi/13.2.1/include
)

# Библиотеки SDK
target_link_libraries(meteo
    pico_stdlib
    hardware_i2c
    hardware_rtc
    hardware_flash
    hardware_powman
)

# USB и UART
pico_enable_stdio_usb(meteo 1)
pico_enable_stdio_uart(meteo 0)

# Оставляем только UF2
pico_add_extra_outputs(meteo NO_BIN NO_HEX NO_DIS NO_ELF_MAP)
