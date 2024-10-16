#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid;

    // Создание дочернего процесса
    pid = fork();

    if (pid < 0) {
        // Ошибка при создании процесса
        perror("Ошибка при создании процесса");
        return 1;
    } else if (pid == 0) {
        // Это дочерний процесс
        printf("Это процесс ребенка, запускаем son.c\n");
        execl("./son", "./son", NULL);  // Запуск программы son
        perror("Ошибка при запуске son");
        exit(1);
    } else {
        // Это родительский процесс
        printf("Это процесс отца, PID ребенка: %d\n", pid);

        // Ожидание завершения дочернего процесса
        wait(NULL);
        printf("Ребенок завершил выполнение\n");

        // Сохраняем состояние таблицы процессов (исправлены опции ps)
        system("ps -ef > father_ps.txt");
        printf("Состояние процессов сохранено в father_ps.txt\n");
    }

    return 0;
}
