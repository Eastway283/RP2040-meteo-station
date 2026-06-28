#include "cli.h"
#include "file_sys.h"
#include "config.h" // DATA_FNAME, CALIB_FILENAME
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>


// ------------ Вспомогательные функции ---------------

static void read_line(char *buf, size_t max_len) {
    size_t idx = 0;
    absolute_time_t deadline = make_timeout_time_ms(5000); // 5 секунд
    while (idx < max_len - 1) {
        int c = getchar_timeout_us(100000); // 100 мс
        if (c == PICO_ERROR_TIMEOUT) {
            if (absolute_time_diff_us(get_absolute_time(), deadline) <= 0) break;
            continue;
        }
        if (c == '\r' || c == '\n') break;
        buf[idx++] = (char)c;
    }
    buf[idx] = '\0';
}

static void dump_writer(const void *buf, size_t len) {
    fwrite(buf, 1, len, stdout);
}

static void cmd_dump(const char *filename) {
    printf("--- Content of %s ---\n", filename);
    bool ok = file_sys_read(filename, dump_writer);
    if (!ok)
        fprintf(stderr, "File not found or empty\n");
    printf("--- End of file ---\n");
    fflush(stdout);
}

static void cmd_help(void) {
    printf("Available commands:\n");
    printf("dump  - show all recorded data\n");
    printf("clear - delete data file\n");
    printf("ls    - list files on LittleFS\n");
    printf("help  - this message\n");
    fflush(stdout);
}

// ------------- Публичный API --------------------

void cli_process(void) {
    printf("\n> ");
    fflush(stdout);

    char line[32];
    read_line(line, sizeof(line));
    if (!strlen(line))
        return;

    if (strcmp(line, "dump") == 0)
        cmd_dump(DATA_FNAME);
    else if (strcmp(line, "clear") == 0) {
        if (file_sys_delete(DATA_FNAME))
            printf("Data file deleted\n");
        else
            printf("Could not delete data file (maybe not present)\n");
        fflush(stdout);
    }
    else if (strcmp(line, "ls") == 0)
        file_sys_list();
    else if (strcmp(line, "help") == 0)
        cmd_help();
    else {
        printf("Unknown command. Type 'help' for a list\n");
        fflush(stdout);
    }

}
