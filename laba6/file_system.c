#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

// Функция для создания и записи данных в файл
void create_and_write_file(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Ошибка создания файла");
        exit(EXIT_FAILURE);
    }
    char data[256];
    printf("Введите строку для записи в файл: ");
    fgets(data, sizeof(data), stdin);
    if (write(fd, data, strlen(data)) == -1) {
        perror("Ошибка записи в файл");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    printf("Файл %s создан и данные записаны.\n", filename);
}

// Функция для чтения содержимого файла
void read_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Ошибка открытия файла для чтения");
        exit(EXIT_FAILURE);
    }
    char buffer[256];
    ssize_t bytesRead;
    printf("Содержимое файла %s:\n", filename);
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }
    if (bytesRead == -1) {
        perror("Ошибка чтения файла");
    }
    close(fd);
}

// Функция для получения информации о файле
void file_info(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) == -1) {
        perror("Ошибка получения информации о файле");
        exit(EXIT_FAILURE);
    }
    printf("Информация о файле %s:\n", filename);
    printf("Размер: %ld байт\n", fileStat.st_size);
    printf("Права доступа: %o\n", fileStat.st_mode & 0777);
    printf("Последняя модификация: %s", ctime(&fileStat.st_mtime));
}

// Функция для переименования файла
void rename_file(const char *oldname, const char *newname) {
    if (rename(oldname, newname) == -1) {
        perror("Ошибка переименования файла");
        exit(EXIT_FAILURE);
    }
    printf("Файл переименован с %s на %s.\n", oldname, newname);
}

// Функция для удаления файла
void delete_file(const char *filename) {
    if (unlink(filename) == -1) {
        perror("Ошибка удаления файла");
        exit(EXIT_FAILURE);
    }
    printf("Файл %s удалён.\n", filename);
}

// Функция для поиска файлов по маске в директории
void search_files(const char *dirpath, const char *mask) {
    DIR *dir = opendir(dirpath);
    if (dir == NULL) {
        perror("Ошибка открытия директории");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, mask)) {
            printf("Найден файл: %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

int main() {
    char filename[256];
    char newname[256];
    char dirpath[256];
    char mask[256];
    printf("Введите имя файла для создания: ");
    scanf("%s", filename);
    getchar(); // очистка буфера
    // Создание и запись в файл
    create_and_write_file(filename);
    // Чтение содержимого файла
    read_file(filename);
    // Получение информации о файле
    file_info(filename);
    // Переименование файла
    printf("Хотите переименовать файл? (y/n)");
    char choice1;
    getchar();
    scanf("%c", &choice1);
    if (choice1 == 'y' || choice1 == 'Y'){
        printf("Введите новое имя файла: ");
        scanf("%s", newname);
        rename_file(filename, newname);
    }
    // Удаление файла
    printf("Удалить файл? (y/n): ");
    char choice2;
    getchar(); // очистка буфера
    scanf("%c", &choice2);
    if (choice2 == 'y' || choice2 == 'Y') {
        delete_file(newname);
    }
    // Поиск файлов в директории по маске
    printf("Введите путь к директории для поиска файлов: ");
    scanf("%s", dirpath);
    printf("Введите маску для поиска (например, .txt): ");
    scanf("%s", mask);
    search_files(dirpath, mask);
    return 0;
}
