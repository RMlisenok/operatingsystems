#include <windows.h>
#include <iostream>
#include <string>

#define PIPE_NAME "\\\\.\\pipe\\filepipe" 

int main() {
    HANDLE hPipe;
    const int bufferSize = 4096; // Increased buffer size for larger file contents
    char buffer[bufferSize] = { 0 };
    DWORD bytesRead = 0, bytesWritten = 0;

    // Attempt to connect to the named pipe using ANSI CreateFileA
    hPipe = CreateFileA(
        PIPE_NAME,              // Pipe name
        GENERIC_READ |          // Read and write access
        GENERIC_WRITE,
        0,                      // No sharing
        NULL,                   // Default security attributes
        OPEN_EXISTING,          // Opens existing pipe
        0,                      // Default attributes
        NULL);                  // No template file

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to pipe. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::string filename;
    std::cout << "Enter the full file name: ";
    std::getline(std::cin, filename);

    // Send the file name to the server using WriteFile
    BOOL isWritten = WriteFile(
        hPipe,
        filename.c_str(),
        static_cast<DWORD>(filename.size()),
        &bytesWritten,
        NULL);

    if (!isWritten) {
        std::cerr << "Failed to send file name to server. Error: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Receive the data from the server
    BOOL isRead = ReadFile(
        hPipe,
        buffer,
        bufferSize - 1, // Leave space for null terminator
        &bytesRead,
        NULL);

    if (isRead && bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the string
        std::cout << "Server response:\n" << buffer << std::endl;
    }
    else {
        std::cerr << "Failed to read data from server. Error: " << GetLastError() << std::endl;
    }

    // Close the pipe handle
    CloseHandle(hPipe);
    return 0;
}
