#include <iostream>
#include <windows.h>
#include "security_operations.h"

void DisplayFilePermissions(const std::string& filename);
void ModifyFilePermissions(const std::string& filename);

int main() {
    std::string filename;

    std::cout << "Enter the filename to check permissions: ";
    std::getline(std::cin, filename);

    // ��������� � ���������� ����� �������
    DisplayFilePermissions(filename);

    // �������� ����� �������
    ModifyFilePermissions(filename);

    // ��������� � ���������� ����� ������� �����
    DisplayFilePermissions(filename);

    return 0;
}

//C:\Users\lislu\source\repos\virtual memory\operations.txt