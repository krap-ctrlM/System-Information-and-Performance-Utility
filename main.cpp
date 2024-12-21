#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <Lmcons.h>

using namespace std;

int main() {
    // Определение версии операционной системы
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMINOR, VER_EQUAL);

    osvi.dwMajorVersion = 6;
    osvi.dwMinorVersion = 1;
    osvi.wServicePackMajor = 1;
    osvi.wServicePackMinor = 0;

    if (VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, conditionMask)) {
        wcout << L"Operating System Version: " << osvi.dwMajorVersion << L"." << osvi.dwMinorVersion << L"." << osvi.wServicePackMajor << L" " << osvi.szCSDVersion << endl;
    }

    // Получение системного каталога
    TCHAR systemDirectory[MAX_PATH];
    if (GetSystemDirectory(systemDirectory, MAX_PATH)) {
        wcout << L"System Directory: " << systemDirectory << endl;
    }

    // Получение имени компьютера
    TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerName(computerName, &size)) {
        wcout << L"Computer Name: " << computerName << endl;
    }

    // Получение имени текущего пользователя
    TCHAR userName[UNLEN + 1];
    DWORD userNameSize = UNLEN + 1;
    if (GetUserName(userName, &userNameSize)) {
        wcout << L"User Name: " << userName << endl;
    }

    // Перебор томов и вывод характеристик
    HANDLE hFindVolume;
    DWORD dwBytes;
    TCHAR volumeName[MAX_PATH];
    hFindVolume = FindFirstVolume(volumeName, ARRAYSIZE(volumeName));
    if (hFindVolume != INVALID_HANDLE_VALUE) {
        do {
            wcout << L"Volume Name: " << volumeName << endl;

            TCHAR volumePath[MAX_PATH];
            if (GetVolumePathNamesForVolumeName(volumeName, volumePath, ARRAYSIZE(volumePath), &dwBytes)) {
                wcout << L"First Path: " << volumePath << endl;
            }

            ULARGE_INTEGER totalBytes, freeBytes;
            if (GetDiskFreeSpaceEx(volumePath, &freeBytes, &totalBytes, NULL)) {
                wcout << L"Total Space: " << totalBytes.QuadPart << L" bytes" << endl;
                wcout << L"Free Space: " << freeBytes.QuadPart << L" bytes" << endl;
            }
        } while (FindNextVolume(hFindVolume, volumeName, ARRAYSIZE(volumeName)));
        FindVolumeClose(hFindVolume);
    }

    // Получение списка программ автозагрузки
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        vector<wstring> startupPrograms;
        TCHAR valueName[MAX_PATH];
        DWORD valueNameSize = MAX_PATH;
        DWORD valueType;
        TCHAR valueData[MAX_PATH];
        DWORD valueDataSize = MAX_PATH;
        int index = 0;
        while (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, &valueType, (LPBYTE)valueData, &valueDataSize) == ERROR_SUCCESS) {
            startupPrograms.push_back(valueData);
            index++;
            valueNameSize = MAX_PATH;
            valueDataSize = MAX_PATH;
        }
        RegCloseKey(hKey);

        if (!startupPrograms.empty()) {
            wcout << L"Startup Programs:" << endl;
            for (const wstring& program : startupPrograms) {
                wcout << program << endl;
            }
        }
    }

    // Измерение производительности ЦП
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency)) {
        wcout << L"CPU Frequency: " << frequency.QuadPart << L" ticks per second" << endl;

        LARGE_INTEGER start, end;
        QueryPerformanceCounter(&start);
        for (int i = 0; i < 10; i++) {
            i = i++;
        }
        QueryPerformanceCounter(&end);
        LONGLONG elapsedTime = end.QuadPart - start.QuadPart;
        double microseconds = (double)elapsedTime / frequency.QuadPart * 1e6;
        wcout << L"Elapsed Time: " << microseconds << L" microseconds" << endl;
    }

    return 0;
}
