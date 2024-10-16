#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#define PIPE_NAME "\\\\.\\pipe\\filepipe" 

void ReadFileAndSend(HANDLE hPipe, const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        // Read the entire file content into a string
        std::string fileContent((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        DWORD bytesWritten;
        // Send the file content to the client using ANSI WriteFileA
        if (!WriteFile(hPipe, fileContent.c_str(), static_cast<DWORD>(fileContent.size()), &bytesWritten, NULL)) {
            std::cerr << "Failed to send file content to client." << std::endl;
        }
    }
    else {
        const char* errorMsg = "Error: File not found!";
        DWORD bytesWritten;
        // Send the error message to the client using ANSI WriteFileA
        WriteFile(hPipe, errorMsg, static_cast<DWORD>(strlen(errorMsg)), &bytesWritten, NULL);
    }
}

int main() {
    HANDLE hPipe;
    char filename[256];

    // Create a named pipe using ANSI CreateNamedPipeA
    hPipe = CreateNamedPipeA(
        PIPE_NAME,             // Pipe name
        PIPE_ACCESS_DUPLEX,    // Duplex pipe (read/write)
        PIPE_TYPE_MESSAGE |    // Message type pipe
        PIPE_READMODE_MESSAGE |
        PIPE_WAIT,             // Blocking mode
        1,                     // Maximum number of instances
        1024 * 16,             // Output buffer size
        1024 * 16,             // Input buffer size
        NMPWAIT_USE_DEFAULT_WAIT, // Default timeout
        NULL);                 // Default security attributes

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create pipe. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Waiting for client connection..." << std::endl;

    // Wait for the client to connect
    if (ConnectNamedPipe(hPipe, NULL) == FALSE) {
        std::cerr << "Failed to connect to client. Error: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // Read the file name from the client
    DWORD bytesRead;
    if (ReadFile(hPipe, filename, sizeof(filename) - 1, &bytesRead, NULL)) {
        filename[bytesRead] = '\0'; // Null-terminate the string
        std::cout << "Requested file: " << filename << std::endl;

        // Read the file and send its content or an error message
        ReadFileAndSend(hPipe, filename);
    }
    else {
        std::cerr << "Failed to read file name from client. Error: " << GetLastError() << std::endl;
    }

    // Close the pipe handle
    CloseHandle(hPipe);
    return 0;
}
