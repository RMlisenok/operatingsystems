#include "memory_operations.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>

void memorySimulator(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        long long time;
        int regionOrBlock;
        int operation;
        size_t size;
        std::string access;

        // Чтение данных из строки
        if (!(iss >> time >> regionOrBlock >> operation >> size >> access)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue;
        }

        // Ждем, пока не наступит нужное время
        std::this_thread::sleep_for(std::chrono::milliseconds(time));

        // Преобразуем строку доступа в перечисление
        MemoryAccess memAccess;
        if (access == "PAGE_READONLY") memAccess = PAGE_READONLY;
        else if (access == "PAGE_READWRITE") memAccess = PAGE_READWRITE;
        else if (access == "PAGE_EXECUTE") memAccess = PAGE_EXECUTE;
        else if (access == "PAGE_EXECUTE_READ") memAccess = PAGE_EXECUTE_READ;
        else if (access == "PAGE_EXECUTE_READWRITE") memAccess = PAGE_EXECUTE_READWRITE;
        else {
            std::cerr << "Invalid access type: " << access << std::endl;
            continue;
        }

        // Выполняем соответствующую операцию
        switch (operation) {
        case RESERVE_REGION:
            ReserveRegion(size, memAccess);
            break;
        case COMMIT_BLOCK:
            CommitBlock(regionOrBlock, size, memAccess);
            break;
        case DONT_SAVE_BLOCK:
            DontSaveBlock(regionOrBlock);
            break;
        case FREE_REGION:
            FreeRegion(regionOrBlock);
            break;
        case RETURN_BLOCK:
            ReturnBlock(regionOrBlock);
            break;
        case LOCK_BLOCK:
            LockBlock(regionOrBlock);
            break;
        case UNLOCK_BLOCK:
            UnlockBlock(regionOrBlock);
            break;
        default:
            std::cerr << "Invalid operation code: " << operation << std::endl;
            break;
        }
    }
    file.close();
}
