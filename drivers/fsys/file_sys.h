#ifndef FILE_SYS_H
#define FILE_SYS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif 

// Инициализация и монтирование LittleFS (только первый запуск)
bool file_sys_init(void);

// Проверка на существование файла с указанным именем
bool file_sys_exists(const char *filename);

// Запись блока данных в файл (append)
// Создает файл, если он не существует
// Возвращает true при успехе
bool file_sys_append(const char *filename, const void *data, size_t size);

// Обертка для функции выше, работает со строками
bool file_sys_append_line(const char *filename, const char *data);

// Читает файл целиком и вызывает callback для каждого прочитанного блока
// тип callback: void (*callback)(const void *buf, size_t len)
// возвращает true, если файл успешно открыт и прочитан
typedef void (*file_sys_read_callback)(const void *buf, size_t len);
bool file_sys_read(const char *filename, file_sys_read_callback callback);

// Удаление файла. Возвращает true при успехе
bool file_sys_delete(const char *filename);

// Вывод списка файлов в корне LittleFS через printf (для CLI)
void file_sys_list(void);

#ifdef __cplusplus
}

#endif

#endif // FILE_SYS_H
