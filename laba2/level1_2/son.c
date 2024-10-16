#include <stdio.h>
#include <unistd.h>

int main() {
    int pid, ppid;

    // Получаем PID и PPID процесса-ребенка
    pid = getpid();
    ppid = getppid();
    printf("\n SON PARAM: pid=%i ppid=%i \n", pid, ppid);

    // Имитируем выполнение программы
    sleep(15);

    printf("SON завершает работу.\n");
    return 0;
}
