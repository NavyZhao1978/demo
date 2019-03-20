// Inject.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Inject.h"
#include <Windows.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

BOOL EnableDebugPrivilege();

VOID InjectDll(ULONG_PTR ProcessID);
BOOL InjectDllByRemoteThread(const TCHAR* wzDllFile, ULONG_PTR ProcessId);
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;
    ULONG_PTR ID = GetCurrentProcessId();
    cout<<"��ǰIDΪ:"<<ID<<endl;
    ULONG_PTR ProcessID = 0;

    EnableDebugPrivilege();

    printf("Input Inject ProcessID\r\n");

    cin>>ProcessID;

    InjectDll(ProcessID);

    getchar();
    getchar();

    return nRetCode;
}


VOID InjectDll(ULONG_PTR ProcessID)
{
    CString strPath;
#ifdef  _WIN64
    strPath =  L"Dll.dll";
#else
    strPath = L"Dll.dll";
#endif
    if (ProcessID == 0)
    {
        return;
    }
    if (PathFileExists(strPath))
    {
        WCHAR wzPath[MAX_PATH] = {0};
        GetCurrentDirectory(260,wzPath);
        wcsncat_s(wzPath, L"\\", 2);
        wcsncat_s(wzPath, strPath.GetBuffer(), strPath.GetLength());
        strPath.ReleaseBuffer();

        if (!InjectDllByRemoteThread(wzPath,ProcessID))    //Զ���߳̽���Inject
        {
            printf("Inject Fail\r\n");
        }
        else
        {
            printf("Inject Success\r\n");
        }
    }    
}


BOOL InjectDllByRemoteThread(const TCHAR* wzDllFile, ULONG_PTR ProcessId)
{
    if (NULL == wzDllFile || 0 == ::_tcslen(wzDllFile) || ProcessId == 0 || -1 == _taccess(wzDllFile, 0))
    {
        return FALSE;
    }
    HANDLE                 hProcess = NULL;
    HANDLE                 hThread  = NULL;
    DWORD                  dwRetVal    = 0;
    LPTHREAD_START_ROUTINE FuncAddress = NULL;
    DWORD  dwSize = 0;
    TCHAR* VirtualAddress = NULL;

    //���Ŀ����̾��
    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, ProcessId);

    if (NULL == hProcess)
    {
        printf("Open Process Fail LastError [%d]\r\n", GetLastError());
        return FALSE;
    }

    // ��Ŀ������з����ڴ�ռ�
    dwSize = (DWORD)::_tcslen(wzDllFile) + 1;
    VirtualAddress = (TCHAR*)::VirtualAllocEx(hProcess, NULL, dwSize * sizeof(TCHAR), 
        MEM_COMMIT,PAGE_READWRITE);
    if (NULL == VirtualAddress)
    {

        printf("Virtual Process Memory Fail LastError [%d]\r\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }

    // ��Ŀ����̵��ڴ�ռ���д���������(ģ����)
    if (FALSE == ::WriteProcessMemory(hProcess, VirtualAddress, (LPVOID)wzDllFile, dwSize * sizeof(TCHAR), NULL))
    {
        printf("Write Data Fail lastError [%d]\r\n", GetLastError());
        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }

#ifdef _UNICODE
    FuncAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#else
    FuncAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryA");
#endif

    //��Ŀ����̿���һ���߳�  �� ����ִ��LoadLibrary(Address)   Address һ���ǶԷ����ڴ��ַ  
    hThread = ::CreateRemoteThread(hProcess, NULL, 0, FuncAddress, VirtualAddress, 0, NULL);

    //Loadlirbrar(LPPARAMDATA);
    if (NULL == hThread)
    {
        printf("CreateRemoteThread Fail LastError [%d]\r\n", GetLastError());

        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }

    // �ȴ�Զ���߳̽���
    WaitForSingleObject(hThread, INFINITE);
    // ����
    VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}




BOOL EnableDebugPrivilege()
{
    HANDLE hToken;   
    TOKEN_PRIVILEGES TokenPrivilege;
    LUID uID;

    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
    {
        printf("OpenProcessToken is Error\n");

        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&uID))
    {
        printf("LookupPrivilegeValue is Error\n");

        return FALSE;
    }

    TokenPrivilege.PrivilegeCount = 1;
    TokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    TokenPrivilege.Privileges[0].Luid = uID;

    //���������ǽ��е���Ȩ��
    if (!AdjustTokenPrivileges(hToken,false,&TokenPrivilege,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
    {
        printf("AdjuestTokenPrivileges is Error\n");
        return  FALSE;
    }
    return TRUE;
}


