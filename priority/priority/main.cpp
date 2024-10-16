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

    // ������ �������� ������ ���������� ��������
    PrintProcessPriority(hProcess);

    // ��������� ������ ����������
    if (!SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
        std::cout << "Error setting priority class: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Priority class is set to HIGH_PRIORITY_CLASS." << std::endl;
    }

    // ������ ������ ������� ��������
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);
    if (Thread32First(hThreadSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == processId) {
                HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                if (hThread) {
                    // ������ �������� ���������� ������
                    PrintThreadPriority(hThread);

                    // ��������� ���������� ������
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
//������������ ���������, ������� �������� �������� ����� ����������
//�������� � ���������� ����� ����� ��������.��� �������� ����������
//������ �� ��������� ������������� ������� ? ��� �����, ���� ���������
//������������ ��������� ���������� ?
//������� � ����������� � ����� ��������� ������� : ������� � ����������� �
//����� ��������� ������� : GetPriorityClass, SetPriorityClass,
//SetProcessPriorityBoost, GetProcessPriorityBoost, ExitProcess,
//TerminateProcess, GetThreadPriority, SetThreadPriority,
//SetThreadPriorityBoost, ThreadPriorityBoost, SuspendThread � ResumeThread.