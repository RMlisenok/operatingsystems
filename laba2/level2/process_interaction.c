#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void case1_wait_for_child() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при создании процесса");
        exit(1);
    }

    if (pid == 0) {
        printf("Процесс-ребенок (PID: %d) выполняется и завершает работу.\n", getpid());
        sleep(2);
        printf("Процесс-ребенок завершен.\n");
        exit(0);
    } else {
        printf("Процесс-родитель (PID: %d) ждет завершения процесса-ребенка (PID: %d).\n", getpid(), pid);
        wait(NULL);
        printf("Процесс-ребенок завершен, родитель продолжает работу.\n");
    }
}

void case2_parent_exits_first() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при создании процесса");
        exit(1);
    }

    if (pid == 0) {
        printf("Процесс-ребенок (PID: %d) выполняется. Родитель завершен.\n", getpid());
        sleep(5);
        printf("Процесс-ребенок завершен.\n");
        exit(0);
    } else {
        printf("Процесс-родитель (PID: %d) не ждет ребенка и завершает выполнение.\n", getpid());
        exit(0);
    }
}

void case3_zombie_process() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при создании процесса");
        exit(1);
    }
    if (pid == 0) {
        printf("Процесс-ребенок (PID: %d) завершен.\n", getpid());
        exit(0);
    } else {
        printf("Процесс-родитель (PID: %d) не ждет завершения ребенка (PID: %d).\n", getpid(), pid);
        sleep(5);
        printf("Проверьте зомби-процесс с PID: %d.\n", pid);
        while (1) {
	    sleep(1);
	}
    }
}

int main() {
    printf("\n===== Случай 1: Ожидание завершения ребенка =====\n");
    case1_wait_for_child();

    printf("\n===== Случай 2: Родитель завершает работу первым =====\n");
    case2_parent_exits_first();

    printf("\n===== Случай 3: Появление зомби процесса =====\n");
    case3_zombie_process();
    return 0;
}
