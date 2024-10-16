#include <iostream>
#include <windows.h>
#include <string> 

void DisplayFileInMemory(const std::string& filename);

int main() {
    std::string filename;

    std::cout << "Enter the filename to display in memory: ";
    std::getline(std::cin, filename); 

    DisplayFileInMemory(filename);

    return 0;
}
//"C:\Users\lislu\source\repos\virtual memory\operations.txt"