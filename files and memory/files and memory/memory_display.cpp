#include <iostream>
#include <windows.h>
#include "memory_display.h"

void DisplayFileInMemory(const std::string& filename) {
    HANDLE hFile = CreateFileA(
        filename.c_str(),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file: " << GetLastError() << std::endl;
        return;
    }

    HANDLE hMapping = CreateFileMappingA(
        hFile,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL
    );

    if (hMapping == NULL) {
        std::cerr << "Error creating file mapping: " << GetLastError() << std::endl;
        CloseHandle(hFile);
        return;
    }

    LPVOID pMapView = MapViewOfFile(
        hMapping,
        FILE_MAP_READ,
        0,
        0,
        0
    );

    if (pMapView == NULL) {
        std::cerr << "Error mapping view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        std::cerr << "Error getting file size: " << GetLastError() << std::endl;
        UnmapViewOfFile(pMapView);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    std::cout << "File contents:\n";
    std::cout.write(static_cast<char*>(pMapView), fileSize);
    std::cout << std::endl;

    // Освобождаем ресурсы
    UnmapViewOfFile(pMapView);
    CloseHandle(hMapping);
    CloseHandle(hFile);
}
