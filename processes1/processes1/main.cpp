#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <string>

// Event names
const wchar_t* PARENT_EVENT_NAME = L"Global\\ParentEvent";
const wchar_t* CHILD_EVENT_NAME = L"Global\\ChildEvent";

// Function to launch child process
bool LaunchChildProcess() {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Get the full path to the current executable
    wchar_t szPath[MAX_PATH];
    if (!GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
        std::wcerr << L"Failed to get module file name. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Prepare command line: executable path + "child"
    std::wstring cmdLine = L"\"" + std::wstring(szPath) + L"\" child";

    // Create child process with inheritable handles and a new console window
    if (!CreateProcessW(
        NULL,
        &cmdLine[0],
        NULL,
        NULL,
        TRUE, // Inherit handles
        CREATE_NEW_CONSOLE, // New console window
        NULL,
        NULL,
        &si,
        &pi)) {
        std::wcerr << L"Failed to create child process. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

// Parent process function
void ParentProcess() {
    // Create security attributes to allow handle inheritance
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    // Create events
    HANDLE hParentEvent = CreateEventW(&sa, TRUE, TRUE, PARENT_EVENT_NAME); // Initially signaled
    HANDLE hChildEvent = CreateEventW(&sa, TRUE, FALSE, CHILD_EVENT_NAME);   // Initially non-signaled

    if (hParentEvent == NULL || hChildEvent == NULL) {
        std::wcerr << L"Failed to create events. Error: " << GetLastError() << std::endl;
        if (hParentEvent) CloseHandle(hParentEvent);
        if (hChildEvent) CloseHandle(hChildEvent);
        return;
    }

    std::wcout << L"Parent: Events created." << std::endl;

    // Launch child process
    if (!LaunchChildProcess()) {
        std::wcerr << L"Parent: Failed to launch child process." << std::endl;
        CloseHandle(hParentEvent);
        CloseHandle(hChildEvent);
        return;
    }

    bool exitFlag = false;

    while (!exitFlag) {
        // Wait for ParentEvent
        DWORD waitResult = WaitForSingleObject(hParentEvent, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            std::wcerr << L"Parent: WaitForSingleObject failed. Error: " << GetLastError() << std::endl;
            break;
        }

        // Reset ParentEvent
        ResetEvent(hParentEvent);

        // Interact with user
        std::wcout << L"Parent: Enter a string (next to pass control, exit to quit): ";
        std::wstring input;
        std::getline(std::wcin, input);

        if (input == L"next") {
            // Signal ChildEvent
            if (!SetEvent(hChildEvent)) {
                std::wcerr << L"Parent: Failed to set ChildEvent. Error: " << GetLastError() << std::endl;
                break;
            }
        }
        else if (input == L"exit") {
            // Signal ChildEvent and set exit flag
            if (!SetEvent(hChildEvent)) {
                std::wcerr << L"Parent: Failed to set ChildEvent. Error: " << GetLastError() << std::endl;
            }
            exitFlag = true;
        }
        else {
            // Echo the input
            std::wcout << L"Parent: You entered: " << input << std::endl;
            // Signal ChildEvent to continue
            if (!SetEvent(hChildEvent)) {
                std::wcerr << L"Parent: Failed to set ChildEvent. Error: " << GetLastError() << std::endl;
                break;
            }
        }
    }

    // Close event handles
    CloseHandle(hParentEvent);
    CloseHandle(hChildEvent);

    std::wcout << L"Parent: Exiting." << std::endl;
}

// Child process function
void ChildProcess() {
    // Open existing events
    HANDLE hParentEvent = OpenEventW(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, PARENT_EVENT_NAME);
    HANDLE hChildEvent = OpenEventW(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, CHILD_EVENT_NAME);

    if (hParentEvent == NULL || hChildEvent == NULL) {
        std::wcerr << L"Child: Failed to open events. Error: " << GetLastError() << std::endl;
        if (hParentEvent) CloseHandle(hParentEvent);
        if (hChildEvent) CloseHandle(hChildEvent);
        return;
    }

    std::wcout << L"Child: Events opened." << std::endl;

    bool exitFlag = false;

    while (!exitFlag) {
        // Wait for ChildEvent
        DWORD waitResult = WaitForSingleObject(hChildEvent, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            std::wcerr << L"Child: WaitForSingleObject failed. Error: " << GetLastError() << std::endl;
            break;
        }

        // Reset ChildEvent
        ResetEvent(hChildEvent);

        // Interact with user
        std::wcout << L"Child: Enter a string (next to pass control, exit to quit): ";
        std::wstring input;
        std::getline(std::wcin, input);

        if (input == L"next") {
            // Signal ParentEvent
            if (!SetEvent(hParentEvent)) {
                std::wcerr << L"Child: Failed to set ParentEvent. Error: " << GetLastError() << std::endl;
                break;
            }
        }
        else if (input == L"exit") {
            // Signal ParentEvent and set exit flag
            if (!SetEvent(hParentEvent)) {
                std::wcerr << L"Child: Failed to set ParentEvent. Error: " << GetLastError() << std::endl;
            }
            exitFlag = true;
        }
        else {
            // Echo the input
            std::wcout << L"Child: You entered: " << input << std::endl;
            // Signal ParentEvent to continue
            if (!SetEvent(hParentEvent)) {
                std::wcerr << L"Child: Failed to set ParentEvent. Error: " << GetLastError() << std::endl;
                break;
            }
        }
    }

    // Close event handles
    CloseHandle(hParentEvent);
    CloseHandle(hChildEvent);

    std::wcout << L"Child: Exiting." << std::endl;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc == 1) {
        // Parent process
        ParentProcess();
    }
    else if (argc == 2 && wcscmp(argv[1], L"child") == 0) {
        // Child process
        ChildProcess();
    }
    else {
        std::wcerr << L"Invalid arguments." << std::endl;
        std::wcerr << L"Usage:" << std::endl;
        std::wcerr << L" - To start parent: " << argv[0] << std::endl;
        std::wcerr << L" - To start child: " << argv[0] << L" child" << std::endl;
    }

    return 0;
}
