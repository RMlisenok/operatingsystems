#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 1024  // Размер сегмента разделяемой памяти

int main() {
    key_t key = ftok("shmfile", 65);  // Генерация уникального ключа
    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);  // Создание сегмента памяти
    char *shmaddr = (char *) shmat(shmid, (void *)0, 0);  // Подключение сегмента

    while (1) {
        // Ожидаем появления имени файла в разделяемой памяти
        while (strlen(shmaddr) == 0) {
            sleep(1);
        }

        // Чтение имени файла из разделяемой памяти
        char filename[256];
        strcpy(filename, shmaddr);
        printf("Сервер получил запрос на файл: %s\n", filename);

        // Открытие файла
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            strcpy(shmaddr, "Ошибка: не удалось открыть файл");
        } else {
            // Чтение содержимого файла в память
            char buffer[SHM_SIZE];
            fread(buffer, sizeof(char), SHM_SIZE, file);
            fclose(file);
            strcpy(shmaddr, buffer);
        }

        // Ожидаем, пока клиент обработает результат, перед тем как принимать новый запрос
        while (strlen(shmaddr) > 0) {
            sleep(1);
        }
    }

    // Отключение от сегмента и удаление разделяемой памяти
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
