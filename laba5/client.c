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

    // Считываем имя файла от пользователя
    printf("Введите имя файла: ");
    char filename[256];
    fgets(filename, 256, stdin);
    filename[strcspn(filename, "\n")] = '\0';  // Убираем символ новой строки

    // Отправляем имя файла в разделяемую память
    strcpy(shmaddr, filename);

    // Ожидаем ответ от сервера
    while (strlen(shmaddr) == 0) {
        sleep(1);
    }

    // Выводим результат (содержимое файла или сообщение об ошибке)
    printf("Результат:\n%s\n", shmaddr);

    // Очищаем разделяемую память, чтобы сервер мог обработать следующий запрос
    memset(shmaddr, 0, SHM_SIZE);

    // Отключение от сегмента разделяемой памяти
    shmdt(shmaddr);
    return 0;
}
