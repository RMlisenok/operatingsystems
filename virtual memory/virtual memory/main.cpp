#include <iostream>
#include <thread>
#include "memory_operations.h"

// ��������� ������� �������
void memorySimulator(const std::string& filename);
void memoryMonitor();

int main() {
    std::string operationsFile = "C:\\Users\\lislu\\source\\repos\\virtual memory\\operations.txt";

    // ������� � ��������� ������
    std::thread simulatorThread(memorySimulator, operationsFile);
    std::thread monitorThread(memoryMonitor);

    simulatorThread.join();
    monitorThread.detach(); // ��������� ����� ����������� �� ���������

    return 0;
}
