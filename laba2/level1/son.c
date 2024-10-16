#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Это процесс ребенка (son.c), PID: %d\n", getpid());

    // Сохраняем состояние таблицы процессов (исправлены опции ps)
    system("ps -ef > son_ps.txt");
    printf("Состояние процессов сохранено в son_ps.txt\n");

    // Имитируем некоторую работу
    sleep(2);

    printf("Ребенок завершает выполнение.\n");
    return 0;
}
