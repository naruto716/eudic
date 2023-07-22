#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <TlHelp32.h>
#include <string>

LONG set_reg(DWORD time_left){
    HKEY hKey;
    DWORD dwDisposition;

    LONG lError = RegCreateKeyEx(HKEY_CURRENT_USER,
        _T("SOFTWARE\\Francophonie\\Eudic\\Customer Info"),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        &dwDisposition);

    if (lError != ERROR_SUCCESS)
    {
        // Handle error
        return false;
    }

    // Define the value data
    LPCTSTR lpSerialCode = _T("Welcome back to eudic");
    DWORD dwTimesLeft3 = time_left;
    LPCTSTR lpRegDate = _T("2020/4/20 0000");
    LPCTSTR lpLicenseCode = _T("Welcome back to eudic");

    // Set the value data
    RegSetValueEx(hKey, _T("SerialCode"), 0, REG_SZ, (LPBYTE)lpSerialCode, sizeof(TCHAR) * (_tcslen(lpSerialCode) + 1));
    RegSetValueEx(hKey, _T("TimesLeft3"), 0, REG_DWORD, (LPBYTE)&dwTimesLeft3, sizeof(DWORD));
    RegSetValueEx(hKey, _T("regDate"), 0, REG_SZ, (LPBYTE)lpRegDate, sizeof(TCHAR) * (_tcslen(lpRegDate) + 1));
    RegSetValueEx(hKey, _T("LicenseCode"), 0, REG_SZ, (LPBYTE)lpLicenseCode, sizeof(TCHAR) * (_tcslen(lpLicenseCode) + 1));

    // Close the key
    RegCloseKey(hKey);

    return 0;
}

bool IsProcessRunning(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return false;
    }

    bool found = false;
    do {
        if (wcscmp(pe.szExeFile, processName.c_str()) == 0) {
            found = true;
            break;
        }
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);
    return found;
}

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    
	HWND hwnd = FindWindow(NULL, _T("欧路词典"));
    if (hwnd) {
		ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        return 0;
    }
    
    //bool is_running = IsProcessRunning(L"eudic.exe");

    DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
	HLOCAL hLocal = NULL;
    DWORD dwRet;
    
    if (dwRet = set_reg(0x000c85e7)) {
        dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
            dwRet, systemLocale, (PTSTR)&hLocal, 0, NULL);
        _tprintf(_T("Fail to open registry: %s"), (PTSTR)hLocal);
        LocalFree(hLocal);
        return -1;
    }

    TCHAR current_path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, current_path);
    _tcscat_s(current_path, _T("\\eudic.exe"));

    HINSTANCE result = ShellExecute(NULL, _T("open"), current_path, NULL, NULL, SW_SHOWNORMAL);
    if ((int)result <= 32)
    {
        _tprintf(_T("Error: %d\n"), (int)result);
        return 1;
    }

    Sleep(5000);

    if (dwRet = set_reg(1)) {
        dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
            dwRet, systemLocale, (PTSTR)&hLocal, 0, NULL);
        _tprintf(_T("Fail to open registry: %s"), (PTSTR)hLocal);
        LocalFree(hLocal);
        return -1;
    };

    _tprintf(_T("Software Upgraded Successfully!"));

    return 0;
    
}