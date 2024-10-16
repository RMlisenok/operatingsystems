#include <iostream>
#include <windows.h>
#include <aclapi.h>
#include "security_operations.h"

void DisplayFilePermissions(const std::string& filename) {
    PSECURITY_DESCRIPTOR pSD = nullptr;
    PACL pDACL = nullptr;

    DWORD dwRes = GetNamedSecurityInfoA(
        filename.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pDACL,
        nullptr,
        &pSD
    );

    if (dwRes != ERROR_SUCCESS) {
        std::cerr << "GetNamedSecurityInfo failed. Error: " << dwRes << std::endl;
        return;
    }

    std::cout << "Permissions for file: " << filename << std::endl;

    if (pDACL != nullptr) {
        for (DWORD i = 0; i < pDACL->AceCount; i++) {
            LPVOID pAce;
            if (!GetAce(pDACL, i, &pAce)) {
                std::cerr << "GetAce failed. Error: " << GetLastError() << std::endl;
                continue;
            }

            ACCESS_ALLOWED_ACE* pAllowedAce = reinterpret_cast<ACCESS_ALLOWED_ACE*>(pAce);
            PSID pSid = (PSID)&pAllowedAce->SidStart;

            char name[256], domain[256];
            DWORD nameSize = sizeof(name);
            DWORD domainSize = sizeof(domain);
            SID_NAME_USE eUse;

            if (!LookupAccountSidA(nullptr, pSid, name, &nameSize, domain, &domainSize, &eUse)) {
                std::cerr << "LookupAccountSid failed. Error: " << GetLastError() << std::endl;
                continue;
            }

            std::cout << "User: " << domain << "\\" << name
                << " - Access Mask: " << pAllowedAce->Mask << std::endl;
        }
    }
    else {
        std::cout << "No DACL present. Everyone has full access." << std::endl;
    }

    if (pSD != nullptr) {
        LocalFree(pSD);
    }
}

void ModifyFilePermissions(const std::string& filename) {
    // Получаем текущую DACL
    PSECURITY_DESCRIPTOR pSD = nullptr;
    PACL pOldDACL = nullptr;
    BOOL bDaclPresent = FALSE;
    BOOL bDaclDefaulted = FALSE;

    DWORD dwRes = GetNamedSecurityInfoA(
        filename.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pOldDACL,
        nullptr,
        &pSD
    );

    if (dwRes != ERROR_SUCCESS) {
        std::cerr << "GetNamedSecurityInfo failed. Error: " << dwRes << std::endl;
        return;
    }

    PACL pNewDACL = nullptr;
    if (pOldDACL != nullptr) {
        dwRes = SetEntriesInAclA(
            0,
            nullptr,
            pOldDACL,
            &pNewDACL
        );

        if (dwRes != ERROR_SUCCESS) {
            std::cerr << "SetEntriesInAcl failed. Error: " << dwRes << std::endl;
            if (pSD) LocalFree(pSD);
            return;
        }
    }
    else {
        // Создаём пустую DACL
        pNewDACL = (PACL)LocalAlloc(LPTR, sizeof(ACL));
        if (pNewDACL == nullptr) {
            std::cerr << "LocalAlloc failed." << std::endl;
            if (pSD) LocalFree(pSD);
            return;
        }
        if (!InitializeAcl(pNewDACL, sizeof(ACL), ACL_REVISION)) {
            std::cerr << "InitializeAcl failed. Error: " << GetLastError() << std::endl;
            LocalFree(pNewDACL);
            if (pSD) LocalFree(pSD);
            return;
        }
    }

    // Создаём SID для группы "Everyone"
    PSID pEveryoneSID = nullptr;
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &SIDAuthWorld,
        1,
        SECURITY_WORLD_RID,
        0, 0, 0, 0, 0, 0, 0,
        &pEveryoneSID)) {
        std::cerr << "AllocateAndInitializeSid failed. Error: " << GetLastError() << std::endl;
        if (pNewDACL) LocalFree(pNewDACL);
        if (pSD) LocalFree(pSD);
        return;
    }
    if (!AddAccessAllowedAce(
        pNewDACL,
        ACL_REVISION,
        GENERIC_READ | GENERIC_EXECUTE,
        pEveryoneSID)) {
        std::cerr << "AddAccessAllowedAce failed. Error: " << GetLastError() << std::endl;
        FreeSid(pEveryoneSID);
        if (pNewDACL) LocalFree(pNewDACL);
        if (pSD) LocalFree(pSD);
        return;
    }

    // Устанавливаем новую DACL для файла
    dwRes = SetNamedSecurityInfoA(
        const_cast<char*>(filename.c_str()),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        pNewDACL,
        nullptr
    );

    if (dwRes != ERROR_SUCCESS) {
        std::cerr << "SetNamedSecurityInfo failed. Error: " << dwRes << std::endl;
    }
    else {
        std::cout << "Modified permissions for file: " << filename << std::endl;
    }

    FreeSid(pEveryoneSID);
    if (pNewDACL) LocalFree(pNewDACL);
    if (pSD) LocalFree(pSD);
}
