#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pid, ppid, status;

    // Получаем PID и PPID процесса-отца
    pid = getpid();
    ppid = getppid();
    printf("\n\n FATHER PARAM: pid=%i ppid=%i \n", pid, ppid);

    // Создаем дочерний процесс
    if (fork() == 0) {
        // Это процесс-ребенок, выполняем программу "son"
        execl("./son", "son", NULL);

        // Если execl() не удалось
        perror("Ошибка выполнения execl");
        exit(1);
    }

    // Родительский процесс выводит состояние таблицы процессов
    system("ps xf");

    // Ожидаем завершения дочернего процесса
    wait(&status);
    printf("Процесс-ребенок завершился с кодом %d\n", WEXITSTATUS(status));

    return 0;
}
