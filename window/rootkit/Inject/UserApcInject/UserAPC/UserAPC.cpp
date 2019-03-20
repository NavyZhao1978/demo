/************************************************************************
 * 
 * 1����EXE��ĳ���߳�ִ�е�SleepEx()����WaitForSingleObjectEx()ʱ��ϵͳ�ͻ����һ�����жϡ�
 * 2�����߳��ٴα�����ʱ�����̻߳�����ִ��APC�����еı�ע��ĺ�����
 * 3������QueueUserAPC()���API���������ж�ʱ���̵߳�APC���в���һ������ָ��
 *    ������ǲ������Loadlibrary()ִ�к����Ļ������ܴﵽע��DLL��Ŀ�ġ�
 * 4) �߳��и�������״̬�����ΪFALSE���򲻻�����û�APC����
*************************************************************************/
#include "stdafx.h"
#include "UserAPC.h"

#include <windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <string>
using namespace std;

#define DEF_BUF_SIZE 1024
BOOL AdjustPrivilege();
BOOL InjectModuleToProcessById(DWORD dwProcessId);
// ���ڴ洢ע��ģ��DLL��·��ȫ��
char szDllPath[DEF_BUF_SIZE] = {0} ;


int _tmain(int argc, _TCHAR* argv[])
{
    // ȡ�õ�ǰ����Ŀ¼·��
    GetCurrentDirectoryA(DEF_BUF_SIZE, szDllPath);

    // ����ע��ģ��DLL��·��ȫ��
#ifdef _WIN64
    strcat ( szDllPath, "\\Dllx64.dll" ) ;
#else
    strcat ( szDllPath, "\\Dllx86.dll" ) ;
#endif
    
    DWORD dwProcessId = 0 ;
    // �����û������Ŀ�����ID
    while( cout << "������Ŀ�����ID��" && cin >> dwProcessId && dwProcessId > 0 ) 
    {
        BOOL bRet = InjectModuleToProcessById(dwProcessId);
        cout << (bRet ? "ע��ɹ�":"ע��ʧ��") << endl ;
    }
    return 0;
}



// ʹ��APC������ָ��ID�Ľ���ע��ģ��
BOOL InjectModuleToProcessById(DWORD dwProcessId)
{
    SIZE_T dwRet = 0;
    BOOL    bStatus = FALSE ;
    LPVOID    lpData = NULL ;
    UINT    uLen = strlen(szDllPath) + 1;

    AdjustPrivilege(); //

    // ��Ŀ�����
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if(hProcess)
    {
        // ����ռ�
        lpData = VirtualAllocEx ( hProcess, NULL, uLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if ( lpData )
        {
            // д����Ҫע���ģ��·��ȫ��
            bStatus = WriteProcessMemory(hProcess, lpData, szDllPath, uLen, (SIZE_T*)(&dwRet));
        }
        CloseHandle(hProcess);
    }

    if (bStatus == FALSE)
        return FALSE ;

    // �����߳̿���
    THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(hThreadSnap == INVALID_HANDLE_VALUE) 
        return FALSE ; 

    bStatus = FALSE ;
    // ö�������߳�
    if(Thread32First(hThreadSnap, &te32))
    {
        do{
            // �ж��Ƿ�Ŀ������е��߳�
            if(te32.th32OwnerProcessID == dwProcessId)
            {
                // ���߳�
                HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
                if ( hThread )
                {
                    // ��ָ���߳����APC
                    DWORD dwRet1 = QueueUserAPC((PAPCFUNC)LoadLibraryA, hThread, (ULONG_PTR)lpData);
                    if ( dwRet1 > 0 )
                    {
                        bStatus = TRUE ;
                    }
                    CloseHandle(hThread);
                }
            } 
        }while(Thread32Next ( hThreadSnap, &te32));
    }

    CloseHandle(hThreadSnap);
    return bStatus;
}


BOOL AdjustPrivilege()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES pTP;
    LUID uID;
    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))   
    {
        printf("OpenProcessToken is Error\n");
        return false;
    }
    if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&uID))   //��ʽ
    {
        printf("LookupPrivilegeValue is Error\n");
        return false;
    }
    pTP.PrivilegeCount = 1;
    pTP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    pTP.Privileges[0].Luid = uID;
    //���������ǽ��е���Ȩ��
    if (!AdjustTokenPrivileges(hToken,false,&pTP,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
    {
        printf("AdjuestTokenPrivileges is Error\n");
        return  false;
    }
    return true;
}
