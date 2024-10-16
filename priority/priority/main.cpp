#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

void PrintProcessPriority(HANDLE hProcess) {
    DWORD priorityClass = GetPriorityClass(hProcess);
    if (priorityClass == 0) {
        std::cout << "Error getting class priority: " << GetLastError() << std::endl;
        return;
    }

    std::cout << "current class priority: " << priorityClass << std::endl;
}

void PrintThreadPriority(HANDLE hThread) {
    int threadPriority = GetThreadPriority(hThread);
    if (threadPriority == THREAD_PRIORITY_ERROR_RETURN) {
        std::cout << "Error getting stream priority: " << GetLastError() << std::endl;
        return;
    }

    std::cout << "Current stream priority: " << threadPriority << std::endl;
}

int main() {
    DWORD processId;
    std::cout << "Process ID: ";
    std::cin >> processId;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess == NULL) {
        std::cout << "Error opening the process: " << GetLastError() << std::endl;
        return 1;
    }

    // Печать текущего класса приоритета процесса
    PrintProcessPriority(hProcess);

    // Изменение класса приоритета
    if (!SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
        std::cout << "Error setting priority class: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Priority class is set to HIGH_PRIORITY_CLASS." << std::endl;
    }

    // Запрос списка потоков процесса
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);
    if (Thread32First(hThreadSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == processId) {
                HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                if (hThread) {
                    // Печать текущего приоритета потока
                    PrintThreadPriority(hThread);

                    // Изменение приоритета потока
                    if (!SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL)) {
                        std::cout << "Error setting thread priority: " << GetLastError() << std::endl;
                    }
                    else {
                        std::cout << "Thread priority is set to ABOVE_NORMAL." << std::endl;
                    }

                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te));
    }

    CloseHandle(hThreadSnap);
    CloseHandle(hProcess);

    return 0;
}
//Разработайте программу, которая позволит изменять класс приоритета
//процесса и приоритеты потов этого процесса.Как значение приоритета
//влияет на выделение процессорного времени ? Что будет, если запретить
//динамическое изменение приоритета ?
//Изучите и используйте в своей программе функции : Изучите и используйте в
//своей программе функции : GetPriorityClass, SetPriorityClass,
//SetProcessPriorityBoost, GetProcessPriorityBoost, ExitProcess,
//TerminateProcess, GetThreadPriority, SetThreadPriority,
//SetThreadPriorityBoost, ThreadPriorityBoost, SuspendThread и ResumeThread.