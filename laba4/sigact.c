#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

// Функция для установки обработчика сигналов с использованием sigaction
void (*mysig(int sig, void (*handler)(int)))(int) {
    struct sigaction act;

    act.sa_handler = handler;  // Установка обработчика сигнала № sig
    sigemptyset(&act.sa_mask); // Обнуление маски
    sigaddset(&act.sa_mask, SIGINT); // Блокировка сигнала SIGINT
    act.sa_flags = 0;

    if (sigaction(sig, &act, 0) < 0) {
        return (SIG_ERR);
    }
    
    return (act.sa_handler);
}

// Обработчик сигнала для SIGUSR1 и SIGUSR2
void signal_handler(int signum) {
    printf("Получен сигнал %d. Процесс будет заблокирован на 1 минуту.\n", signum);
    
    // Засыпаем на одну минуту (эмуляция обработки длительной задачи)
    sleep(60);

    // Отправляем сигнал SIGINT самому себе
    printf("Отправляем сигнал SIGINT процессу.\n");
    kill(getpid(), SIGINT);
}

// Обработчик сигнала SIGINT
void sigint_handler(int signum) {
    printf("Получен сигнал SIGINT. Процесс завершен.\n");
    exit(0);
}

int main() {
    struct sigaction act, oldact;

    // Устанавливаем обработчик для сигнала SIGUSR1 и SIGUSR2
    mysig(SIGUSR1, signal_handler);
    mysig(SIGUSR2, signal_handler);

    // Устанавливаем обработчик для сигнала SIGINT
    mysig(SIGINT, sigint_handler);

    // Ожидание сигналов
    printf("Процесс ожидает сигналы. PID: %d\n", getpid());

    // Ожидание бесконечное, пока не придет сигнал
    while (1) {
        pause(); // Ожидание сигнала
    }

    return 0;
}
