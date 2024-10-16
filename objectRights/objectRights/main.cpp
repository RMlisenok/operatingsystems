#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <tchar.h>
#include <iostream>
#include <string>

// ������� ��� ������ ������
void PrintError(const std::string& msg) {
    DWORD eNum;
    TCHAR sysMsg[256];
    eNum = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        sysMsg, 256, NULL);
    std::cerr << msg << " Error " << eNum << ": " << sysMsg << std::endl;
}

int main() {
    const std::wstring filename = L"testfile.txt";

    // 1. �������� ����� � �������� �������
    HANDLE hFile = CreateFileW(
        filename.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        PrintError("Failed to create file.");
        return 1;
    }
    std::cout << "File created successfully." << std::endl;

    // 2. ��������� ���������� DACL, ������������ ������ ���� (Everyone) �� ������ � ������
    PACL pOldDACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;

    // ��������� �������� DACL
    DWORD res = GetNamedSecurityInfoW(
        filename.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        NULL,
        NULL,
        &pOldDACL,
        NULL,
        &pSD
    );

    if (res != ERROR_SUCCESS) {
        PrintError("GetNamedSecurityInfo failed.");
        CloseHandle(hFile);
        return 1;
    }

    // �������� ������ DACL
    PACL pNewDACL = NULL;
    EXPLICIT_ACCESSW ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESSW));

    // ��������� SID ��� ������ "Everyone"
    PSID pEveryoneSID = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &SIDAuthWorld,
        1,
        SECURITY_WORLD_RID,
        0, 0, 0, 0, 0, 0, 0,
        &pEveryoneSID)) {
        PrintError("AllocateAndInitializeSid failed.");
        LocalFree(pSD);
        CloseHandle(hFile);
        return 1;
    }

    // ��������� ���������� ��� "Everyone" �� ������ � ������
    ea.grfAccessPermissions = GENERIC_READ | GENERIC_WRITE;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea.Trustee.ptstrName = (LPWSTR)pEveryoneSID;

    // �������� DACL � ����� ��������
    res = SetEntriesInAclW(1, &ea, pOldDACL, &pNewDACL);
    if (res != ERROR_SUCCESS) {
        PrintError("SetEntriesInAcl failed.");
        FreeSid(pEveryoneSID);
        LocalFree(pSD);
        CloseHandle(hFile);
        return 1;
    }

    // ��������� ������ DACL ��� �����
    res = SetNamedSecurityInfoW(
        const_cast<LPWSTR>(filename.c_str()),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        NULL,
        NULL,
        pNewDACL,
        NULL
    );

    if (res != ERROR_SUCCESS) {
        PrintError("SetNamedSecurityInfo failed.");
        LocalFree(pNewDACL);
        FreeSid(pEveryoneSID);
        LocalFree(pSD);
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "Initial DACL set to allow Everyone read/write access." << std::endl;

    // �������
    LocalFree(pNewDACL);
    FreeSid(pEveryoneSID);
    LocalFree(pSD);

    // 3. �������� ����� � ������������� ������� �������
    HANDLE hFileRead = CreateFileW(
        filename.c_str(),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFileRead == INVALID_HANDLE_VALUE) {
        PrintError("Failed to open file with read access.");
        CloseHandle(hFile);
        return 1;
    }
    std::cout << "File opened with read access." << std::endl;

    // 4. ��������� DACL ��� ������ � ������ ������ "Everyone"
    res = GetNamedSecurityInfoW(
        filename.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        NULL,
        NULL,
        &pOldDACL,
        NULL,
        &pSD
    );

    if (res != ERROR_SUCCESS) {
        PrintError("GetNamedSecurityInfo failed.");
        CloseHandle(hFileRead);
        CloseHandle(hFile);
        return 1;
    }

    // �������� ����� ������ ACE ��� ������ � ������
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESSW));
    ea.grfAccessPermissions = GENERIC_WRITE;
    ea.grfAccessMode = DENY_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea.Trustee.ptstrName = (LPWSTR)pEveryoneSID;

    // ���������� DENY ACE � DACL
    PACL pModifiedDACL = NULL;
    res = SetEntriesInAclW(1, &ea, pOldDACL, &pModifiedDACL);
    if (res != ERROR_SUCCESS) {
        PrintError("SetEntriesInAcl failed.");
        LocalFree(pSD);
        CloseHandle(hFileRead);
        CloseHandle(hFile);
        return 1;
    }

    // ��������� ����������� DACL ��� �����
    res = SetNamedSecurityInfoW(
        const_cast<LPWSTR>(filename.c_str()),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        NULL,
        NULL,
        pModifiedDACL,
        NULL
    );

    if (res != ERROR_SUCCESS) {
        PrintError("SetNamedSecurityInfo failed.");
        LocalFree(pModifiedDACL);
        LocalFree(pSD);
        CloseHandle(hFileRead);
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "DACL modified to deny Everyone write access." << std::endl;

    // �������
    LocalFree(pModifiedDACL);
    LocalFree(pSD);

    // 5. ��������, ��� ����� �������� ���������� ��� ��� ����� ������ �� ������
    DWORD bytesRead;
    char buffer[100];
    BOOL readResult = ReadFile(
        hFileRead,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead,
        NULL
    );

    if (readResult) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        std::cout << "Successfully read from the file using existing handle." << std::endl;
    }
    else {
        PrintError("Failed to read from the file using existing handle.");
    }

    // 6. ������� ������� ���� � �������� �� ������ ����� ��������� ACL
    HANDLE hFileWrite = CreateFileW(
        filename.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFileWrite == INVALID_HANDLE_VALUE) {
        PrintError("As expected, failed to open file with write access after ACL modification.");
    }
    else {
        std::cout << "Unexpectedly succeeded in opening file with write access after ACL modification." << std::endl;
        CloseHandle(hFileWrite);
    }

    // �������� ������������
    CloseHandle(hFileRead);
    CloseHandle(hFile);

    return 0;
}
