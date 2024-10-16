#include "memory_operations.h"
#include <windows.h>
#include <iostream>
#include <mutex>
#include <vector>

// Глобальные переменные и структуры
std::mutex memoryMutex; 

// Структура для хранения информации о зарезервированных регионах
struct ReservedRegion {
    void* address;
    size_t size;
    MemoryAccess access;
};

std::vector<ReservedRegion> reservedRegions;

// Функция для резервирования региона памяти
void ReserveRegion(size_t size, MemoryAccess access) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    void* addr = VirtualAlloc(NULL, size, MEM_RESERVE, access);
    if (addr == NULL) {
        std::cerr << "Failed to reserve region of size " << size << " bytes. Error: " << GetLastError() << std::endl;
        return;
    }
    reservedRegions.push_back(ReservedRegion{ addr, size, access });
    std::cout << "Reserved region: Address = " << addr << ", Size = " << size << " bytes." << std::endl;
}

// Функция для выделения блока памяти (commit)
void CommitBlock(size_t blockNumber, size_t size, MemoryAccess access) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    if (blockNumber >= reservedRegions.size()) {
        std::cerr << "Invalid block number: " << blockNumber << std::endl;
        return;
    }

    ReservedRegion& region = reservedRegions[blockNumber];
    void* commitAddr = VirtualAlloc(region.address, size, MEM_COMMIT, access);
    if (commitAddr == NULL) {
        std::cerr << "Failed to commit block " << blockNumber << ". Error: " << GetLastError() << std::endl;
        return;
    }
    std::cout << "Committed block " << blockNumber << ": Address = " << commitAddr << ", Size = " << size << " bytes." << std::endl;
}

// Функция, чтобы не сохранять блок в страничном файле при его изменении
void DontSaveBlock(size_t blockNumber) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    if (blockNumber >= reservedRegions.size()) {
        std::cerr << "Invalid block number: " << blockNumber << std::endl;
        return;
    }

    ReservedRegion& region = reservedRegions[blockNumber];
    DWORD oldProtect;

    // Пытаемся изменить защиту на PAGE_READONLY
    if (!VirtualProtect(region.address, region.size, PAGE_READONLY, &oldProtect)) {
        DWORD errorCode = GetLastError();
        LPVOID errorMsg;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorMsg,
            0,
            NULL
        );
        std::cerr << "Failed to change protection for block " << blockNumber
            << ". Error: " << errorCode << " (" << (char*)errorMsg << ")"
            << std::endl;
        LocalFree(errorMsg);
        return;
    }

    std::cout << "Set block " << blockNumber << " to PAGE_READONLY." << std::endl;
}


// Функция для освобождения региона памяти
void FreeRegion(size_t regionNumber) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    if (regionNumber >= reservedRegions.size()) {
        std::cerr << "Invalid region number: " << regionNumber << std::endl;
        return;
    }

    ReservedRegion& region = reservedRegions[regionNumber];
    if (VirtualFree(region.address, 0, MEM_RELEASE)) {
        std::cout << "Freed region " << regionNumber << ": Address = " << region.address << std::endl;
        // Удаляем регион из списка
        reservedRegions.erase(reservedRegions.begin() + regionNumber);
    }
    else {
        std::cerr << "Failed to free region " << regionNumber << ". Error: " << GetLastError() << std::endl;
    }
}

// Функция для возвращения блока (освобождение commit)
void ReturnBlock(size_t blockNumber) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    if (blockNumber >= reservedRegions.size()) {
        std::cerr << "Invalid block number: " << blockNumber << std::endl;
        return;
    }

    ReservedRegion& region = reservedRegions[blockNumber];
    if (VirtualFree(region.address, 0, MEM_DECOMMIT)) {
        std::cout << "Returned block " << blockNumber << ": Address = " << region.address << std::endl;
    }
    else {
        std::cerr << "Failed to return block " << blockNumber << ". Error: " << GetLastError() << std::endl;
    }
}

// Функция для блокировки блока памяти (можно использовать VirtualLock)
void LockBlock(size_t blockNumber) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    if (blockNumber >= reservedRegions.size()) {
        std::cerr << "Invalid block number: " << blockNumber << std::endl;
        return;
    }

    ReservedRegion& region = reservedRegions[blockNumber];
    if (VirtualLock(region.address, region.size)) {
        std::cout << "Locked block " << blockNumber << ": Address = " << region.address << std::endl;
    }
    else {
        std::cerr << "Failed to lock block " << blockNumber << ". Error: " << GetLastError() << std::endl;
    }
}

// Функция для снятия блокировки блока памяти (VirtualUnlock)
void UnlockBlock(size_t blockNumber) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    if (blockNumber >= reservedRegions.size()) {
        std::cerr << "Invalid block number: " << blockNumber << std::endl;
        return;
    }

    ReservedRegion& region = reservedRegions[blockNumber];
    if (VirtualUnlock(region.address, region.size)) {
        std::cout << "Unlocked block " << blockNumber << ": Address = " << region.address << std::endl;
    }
    else {
        std::cerr << "Failed to unlock block " << blockNumber << ". Error: " << GetLastError() << std::endl;
    }
}

// Функция для получения текущего состояния памяти
MemoryStatus GetMemoryStatus() {
    MemoryStatus status;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    status.pageSize = sysInfo.dwPageSize;
    status.granularity = sysInfo.dwAllocationGranularity;

    // Получаем информацию о физической и виртуальной памяти
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    status.physicalMemoryUsage = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
    status.virtualMemoryUsage = memStatus.ullTotalVirtual - memStatus.ullAvailVirtual;
    status.pageFileUsage = memStatus.ullTotalPageFile - memStatus.ullAvailPageFile;

    return status;
}
