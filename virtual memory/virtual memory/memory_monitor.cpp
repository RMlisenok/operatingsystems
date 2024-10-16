#include "memory_operations.h"
#include <thread>
#include <chrono>
#include <iostream>

void memoryMonitor() {
    while (true) {
        MemoryStatus status = GetMemoryStatus();
        std::cout << "Page Size: " << status.pageSize << " bytes\n";
        std::cout << "Granularity: " << status.granularity << " bytes\n";
        std::cout << "Physical Memory Usage: " << status.physicalMemoryUsage << " bytes\n";
        std::cout << "Virtual Memory Usage: " << status.virtualMemoryUsage << " bytes\n";
        std::cout << "Page File Usage: " << status.pageFileUsage << " bytes\n";
        std::cout << "------------------------------------\n";

        // Задержка перед следующей проверкой состояния памяти
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
