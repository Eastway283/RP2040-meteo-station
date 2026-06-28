#include "file_sys.h"

#include "hardware/flash.h"
#include "pico/stdlib.h"
#include "lfs.h"

#include <string.h>
#include <stdio.h>

// ---------- Конфигурация памяти (должна совпадать с config.h) ----------
#ifndef FS_SIZE
#define FS_SIZE         (256 * 1024)    // 256 КБ
#endif

#ifndef FS_OFFSET
#define FS_OFFSET       (1024 * 1024)   // смещение от начала flash – 1 МБ
#endif

// ---------- Глобальные объекты LittleFS (видны только внутри модуля) ----------
static lfs_t lfs;
static struct lfs_config lfs_cfg;

// ---------- Низкоуровневые операции с flash ----------
static int flash_read(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, void *buffer, lfs_size_t size) {
    // Адрес в памяти: XIP_BASE (0x10000000) + FS_OFFSET + смещение блока/off
    uint32_t addr = XIP_BASE + FS_OFFSET + block * c->block_size + off;
    memcpy(buffer, (const void *)addr, size);
    return 0;
}

static int flash_prog(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, const void *buffer, lfs_size_t size) {
    // Смещение от начала flash: FS_OFFSET + блок + off
    uint32_t offset = FS_OFFSET + block * c->block_size + off;
    flash_range_program(offset, (const uint8_t *)buffer, size);
    return 0;
}

static int flash_erase(const struct lfs_config *c, lfs_block_t block) {
    uint32_t offset = FS_OFFSET + block * c->block_size;
    flash_range_erase(offset, c->block_size);
    return 0;
}

static int flash_sync(const struct lfs_config *c) {
    return 0;
}

// ---------- Инициализация ----------
bool file_sys_init(void) {
    lfs_cfg.context = NULL;
    lfs_cfg.read  = flash_read;
    lfs_cfg.prog  = flash_prog;
    lfs_cfg.erase = flash_erase;
    lfs_cfg.sync  = flash_sync;

    lfs_cfg.read_size      = 16;
    lfs_cfg.prog_size      = 256;
    lfs_cfg.block_size     = 4096;
    lfs_cfg.block_count    = FS_SIZE / 4096;
    lfs_cfg.cache_size     = 256;
    lfs_cfg.lookahead_size = 32;
    lfs_cfg.block_cycles   = 500;

    // Для чистоты эксперимента принудительно стираем первый блок
    flash_range_erase(FS_OFFSET, lfs_cfg.block_size);

    int err = lfs_format(&lfs, &lfs_cfg);
    if (err) {
        printf("lfs_format error: %d\n", err);
        fflush(stdout);
        return false;
    }
    err = lfs_mount(&lfs, &lfs_cfg);
    if (err) {
        printf("lfs_mount error: %d\n", err);
        fflush(stdout);
        return false;
    }
    return true;
}

// ---------- Проверка существования файла ----------
bool file_sys_exists(const char *filename) {
    lfs_file_t file;
    int err = lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY);
    if (err == 0) {
        lfs_file_close(&lfs, &file);
        return true;
    }
    return false;
}

// ---------- Запись блока данных ----------
bool file_sys_append(const char *filename, const void *data, size_t size) {
    lfs_file_t file;
    int err = lfs_file_open(&lfs, &file, filename,
                            LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND);
    if (err < 0) return false;
    lfs_file_write(&lfs, &file, data, size);
    lfs_file_close(&lfs, &file);
    return true;
}

// ---------- Запись строки (удобная обёртка) ----------
bool file_sys_append_line(const char *filename, const char *line) {
    return file_sys_append(filename, line, strlen(line));
}

// ---------- Чтение файла с callback ----------
bool file_sys_read(const char *filename, file_sys_read_callback callback) {
    lfs_file_t file;
    if (lfs_file_open(&lfs, &file, filename, LFS_O_RDONLY) < 0)
        return false;

    uint8_t buf[256];
    lfs_ssize_t n;
    while ((n = lfs_file_read(&lfs, &file, buf, sizeof(buf))) > 0) {
        callback(buf, (size_t)n);
    }
    lfs_file_close(&lfs, &file);
    return true;
}

// ---------- Удаление файла ----------
bool file_sys_delete(const char *filename) {
    return lfs_remove(&lfs, filename) == 0;
}

// ---------- Вывод списка файлов ----------
void file_sys_list(void) {
    lfs_dir_t dir;
    if (lfs_dir_open(&lfs, &dir, "/") < 0) {
        printf("Failed to open root directory.\n");
        fflush(stdout);
        return;
    }
    struct lfs_info info;
    while (lfs_dir_read(&lfs, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            printf("FILE: %s (%ld bytes)\n", info.name, info.size);
        } else if (info.type == LFS_TYPE_DIR) {
            printf("DIR : %s\n", info.name);
        }
    }
    fflush(stdout);
    lfs_dir_close(&lfs, &dir);
}
