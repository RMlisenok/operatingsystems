#pragma once
#ifndef MEMORY_OPERATIONS_H
#define MEMORY_OPERATIONS_H

#include <cstddef>

// Определение доступов к памяти
enum MemoryAccess {
    PAGE_READONLY = 1,
    PAGE_READWRITE,
    PAGE_EXECUTE,
    PAGE_EXECUTE_READ,
    PAGE_EXECUTE_READWRITE
};

// Определение типов операций
enum Operation {
    RESERVE_REGION = 1,
    COMMIT_BLOCK,
    DONT_SAVE_BLOCK,
    FREE_REGION,
    RETURN_BLOCK,
    LOCK_BLOCK,
    UNLOCK_BLOCK
};

// Структура для хранения состояния памяти
struct MemoryStatus {
    size_t pageSize;
    size_t granularity;
    size_t physicalMemoryUsage;
    size_t virtualMemoryUsage;
    size_t pageFileUsage;
};

// Функции управления виртуальной памятью
void ReserveRegion(size_t size, MemoryAccess access);
void CommitBlock(size_t blockNumber, size_t size, MemoryAccess access);
void DontSaveBlock(size_t blockNumber);
void FreeRegion(size_t regionNumber);
void ReturnBlock(size_t blockNumber);
void LockBlock(size_t blockNumber);
void UnlockBlock(size_t blockNumber);
MemoryStatus GetMemoryStatus();

#endif // MEMORY_OPERATIONS_H
