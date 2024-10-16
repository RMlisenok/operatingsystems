#include <iostream>
#include <windows.h>
#include "security_operations.h"

void DisplayFilePermissions(const std::string& filename);
void ModifyFilePermissions(const std::string& filename);

int main() {
    std::string filename;

    std::cout << "Enter the filename to check permissions: ";
    std::getline(std::cin, filename);

    // Проверяем и отображаем права доступа
    DisplayFilePermissions(filename);

    // Изменяем права доступа
    ModifyFilePermissions(filename);

    // Проверяем и отображаем права доступа снова
    DisplayFilePermissions(filename);

    return 0;
}

//C:\Users\lislu\source\repos\virtual memory\operations.txt