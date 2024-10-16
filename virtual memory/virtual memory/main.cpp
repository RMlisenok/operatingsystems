#include <iostream>
#include <thread>
#include "memory_operations.h"

// Объявляем функции потоков
void memorySimulator(const std::string& filename);
void memoryMonitor();

int main() {
    std::string operationsFile = "C:\\Users\\lislu\\source\\repos\\virtual memory\\operations.txt";

    // Создаем и запускаем потоки
    std::thread simulatorThread(memorySimulator, operationsFile);
    std::thread monitorThread(memoryMonitor);

    simulatorThread.join();
    monitorThread.detach(); // Отключаем поток мониторинга от основного

    return 0;
}
