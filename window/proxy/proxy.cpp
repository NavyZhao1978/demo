#include "proxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include <fcntl.h>
#include <Wininet.h>
#include <tchar.h>
#include <ras.h>
#include <raserror.h>

#pragma comment(lib, "rasapi32.lib")
#pragma comment(lib, "Wininet.lib")

void ShowError(long lerr)
{
	LPVOID lpMsgBuf;
	if (!FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lerr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL))
	{
		return;
	}
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
	LocalFree(lpMsgBuf);
}


void reportWindowsError(LPCTSTR action)
{
	LPTSTR pErrMsg = NULL;
	DWORD errCode = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL,
		errCode,
		LANG_NEUTRAL,
		(LPTSTR)&pErrMsg,
		0,
		NULL);
	_ftprintf(stderr, _T("Error %s: %lu %s\n"), action, errCode, pErrMsg);
	MessageBox(NULL, (LPCTSTR)pErrMsg, "Error", MB_OK | MB_ICONINFORMATION);
	LocalFree(pErrMsg);
}



/*
typedef struct {
DWORD dwSize;
LPTSTR pszConnection;
DWORD dwOptionCount;
DWORD dwOptionError;
LPINTERNET_PER_CONN_OPTION pOptions;
} INTERNET_PER_CONN_OPTION_LIST, *LPINTERNET_PER_CONN_OPTION_LIST;
*/
int initialize(INTERNET_PER_CONN_OPTION_LIST* options, int option_count)
{
	if (option_count < 1)
	{
		return -1;
	}

	DWORD dwBufferSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	options->dwSize = dwBufferSize;   //�ṹ���С

	options->dwOptionCount = option_count;  //Ҫ��ѯ�����õ�ѡ������
	options->dwOptionError = 0;   //�������������ʧ�ܵ�ѡ�

								  //ָ�������ѯ������ѡ���INTERNET_PER_CONN_OPTION�ṹ�������ָ�롣
								  /*
								  typedef struct {
								  DWORD dwOption;
								  union {
								  DWORD dwValue;
								  LPTSTR pszValue;
								  FILETIME ftValue;
								  } Value;
								  } INTERNET_PER_CONN_OPTION, *LPINTERNET_PER_CONN_OPTION;
								  */
	options->pOptions = (INTERNET_PER_CONN_OPTION*)calloc(option_count, sizeof(INTERNET_PER_CONN_OPTION));

	if (!options->pOptions)
	{
		return -1;
	}
	/*
	INTERNET_PER_CONN_FLAGS 				���û��ȡ�������͡� Value��������������һ������ֵ��
	PROXY_TYPE_DIRECT �����Ӳ�ʹ�ô����������
	PROXY_TYPE_PROXY  ������ʹ����ʽ���õĴ����������
	PROXY_TYPE_AUTO_PROXY_URL  ���ӽ���ָ����URL���ز������Զ����ýű���
	PROXY_TYPE_AUTO_DETECT  ���ӽ��Զ�������á�
	*/
	options->pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
}



int apply_connect(INTERNET_PER_CONN_OPTION_LIST* options, LPTSTR conn)
{
	options->pszConnection = conn;

	/*
	INTERNET_OPTION_PER_CONNECTION_OPTION
	causes the settings to be changed on a system-wide basis when a NULL handle is used in the call to InternetSetOption. To refresh the global proxy settings, you must call InternetSetOption with theINTERNET_OPTION_REFRESH option flag.
	ע : �� ����InternetSetOption.ʱ������ڶ����������ó��˴�ֵ������һ������ΪNULLʱ��������ı������ȫ���Ե�Ҳ����ϵͳ����ġ�ˢ��ȫ�ִ������ã���������InternetSetOption,��ʹ��INTERNET_OPTION_REFRESH
	*/
	/*
	hInternet: ������Ϣ��Internet�����
	dwOption: ָ��Ҫ���õ�Internetѡ�
	lpBuffer: ָ�����ѡ�����õĻ������ĳ�ָ�롣
	dwBufferLength: ָ��lpBuffer�������ĳ��ȡ�
	*/
	BOOL result = InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, options, sizeof(INTERNET_PER_CONN_OPTION_LIST));
	if (!result)
	{
		reportWindowsError(_T("setting options"));
		return -1;
	}
	/*
	INTERNET_OPTION_PROXY_SETTINGS_CHANGED
	Alerts the current WinInet instance that proxy settings have changed and that they must update with the new settings. To alert all available WinInet instances, set the Buffer parameter of InternetSetOption to NULL and BufferLength to 0 when passing this option. This option can be set on the handle returned by InternetConnect or HttpOpenRequest.
	֪ͨ��ǰ��WinInetʵ�����������Ѿ����ı��ˣ�����ȥ�������Ĵ������ã����Ҫ֪ͨ������Ч��WinInetʵ���Ļ����������������NULL�����һ���������0 ��
	*/
	result = InternetSetOption(NULL, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, NULL, 0);
	if (!result)
	{
		reportWindowsError(_T("propagating changes"));
		return -1;
	}
	/*
	INTERNET_OPTION_REFRESH
	ʹ������Ϣ��ע��������¶�ȡ�Ի�ȡ����� ����Ҫ�������� ������InternetOpen���ص�HINTERNET�����ʹ�ô�ѡ� InternetSetOptionʹ������
	*/
	result = InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	if (!result)
	{
		reportWindowsError(_T("refreshing"));
		return -1;
	}

	return 0;
}


int apply(INTERNET_PER_CONN_OPTION_LIST* options)
{
	DWORD dwCb = 0;
	DWORD dwRet;
	DWORD dwEntries = 0;

	// �ú����г����е�Զ�̷��ʵ����֡�
	dwRet = RasEnumEntries(NULL, NULL, NULL, &dwCb, &dwEntries);

	if (dwRet == ERROR_BUFFER_TOO_SMALL)
	{
		LPRASENTRYNAME lpRasEntryName = (LPRASENTRYNAME)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);

		if (lpRasEntryName == NULL)
		{
			printf(_T("HeapAlloc"));
			return -1;
		}

		for (DWORD i = 0; i < dwEntries; i++)
		{
			lpRasEntryName[i].dwSize = sizeof(RASENTRYNAME);
		}

		/*
		// ����������ΪNULL
		//�绰����ڻ���
		//���ص�RASENTRYNAME��ʽ������
		// �����С���ֽڣ�
		// д�뻺��ĸ���
		*/
		dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);

		int ret;

		if (ERROR_SUCCESS != dwRet)
		{
			_ftprintf(stderr, _T("Error RasEnumEntries: %d\n"), dwRet);

			ret = -1;
		}
		else
		{
			// First set default connection.
			ret = apply_connect(options, NULL);

			for (DWORD i = 0; i < dwEntries && ret == 0; i++)
			{
				ret = apply_connect(options, lpRasEntryName[i].szEntryName);
			}
		}

		HeapFree(GetProcessHeap(), 0, lpRasEntryName);

		return ret;
	}

	if (dwEntries >= 1)
	{
		printf(_T("acquire buffer size"));
		return -1;
	}

	// No ras entry, set default only.
	return apply_connect(options, NULL);
}




static int _setHttpProxy(const char* ip ,int port) {

	return 0;
}

static int _setPACProxy(LPSTR url) {
	INTERNET_PER_CONN_OPTION_LIST options;
	int ret = 0;
	initialize(&options, 2);

	//PROXY_TYPE_AUTO_PROXY_URL  ���ӽ���ָ����URL���ز������Զ����ýű���
	options.pOptions[0].Value.dwValue = PROXY_TYPE_AUTO_PROXY_URL | PROXY_TYPE_DIRECT;

	// INTERNET_PER_CONN_AUTOCONFIG_URL = 4 //��//���û��ȡ�����Զ����ýű���URL���ַ�����
	options.pOptions[1].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
	options.pOptions[1].Value.pszValue = (LPSTR)url;

	ret = apply(&options);

	free(options.pOptions);

	return ret;
}

static int _setSystemProxy(const char* ip, int port) {
	INTERNET_PER_CONN_OPTION_LIST options;
	int ret = 0;
	ret = initialize(&options, 3);

	// PROXY_TYPE_PROXY ������ʹ����ʽ���õĴ����������
	// PROXY_TYPE_DIRECT �����Ӳ�ʹ�ô����������
	options.pOptions[0].Value.dwValue = PROXY_TYPE_PROXY | PROXY_TYPE_DIRECT;

	// INTERNET_PER_CONN_PROXY_SERVER   ���û��������������������ַ�����
	options.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	options.pOptions[1].Value.pszValue = "TODO";

	// INTERNET_PER_CONN_PROXY_BYPASS ���û����������ʹ�ô����������URL���ַ�����
	options.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;

	if (ip != 0)
	{
		options.pOptions[2].Value.pszValue = "TODO";
	}
	else
	{
		options.pOptions[2].Value.pszValue = _T("<local>");
	}
	ret = apply(&options);

	free(options.pOptions);

	return ret;
}

static int _resetHttpProxy() {
	return 0;
}

static int _resetSystemProxy() {
	INTERNET_PER_CONN_OPTION_LIST options;
	int ret = 0;
	ret = initialize(&options, 1);
	// PROXY_TYPE_AUTO_DETECT ���ӽ��Զ��������
	// PROXY_TYPE_DIRECT �����Ӳ�ʹ�ô����������
	options.pOptions[0].Value.dwValue = PROXY_TYPE_AUTO_DETECT | PROXY_TYPE_DIRECT;

	ret = apply(&options);

	free(options.pOptions);

	return ret;
}



int setProxy(const char * ip, int port, int type)
{
	if (ip == 0 || strlen(ip) == 0) {
		return -1;
	}
	if (port <= 0 || port >= 65535) {
		return -1;
	}
	switch (type) {
	case 0: {

	}
	case 1: {

	}
	case 2: {

	}
	default:
		return -1;
	}

	return 0;
}

int resetProxy(int type)
{
	switch (type) {
	case 0: {

	}
	case 1: {

	}
	case 2: {

	}
	default:
		return -1;
	}

	return 0;
}



/*
//set proxy
const char* connection_name="Connection to adsl3";
SetConnectionOptions(connection_name,"62.81.236.23:80");
//disable proxy
DisableConnectionProxy(connection_name);
*/

BOOL SetConnectionOptions(LPCTSTR conn_name, LPCTSTR proxy_full_addr)
{
	//conn_name: active connection name. 
	//proxy_full_addr : eg "210.78.22.87:8000"
	INTERNET_PER_CONN_OPTION_LIST list;
	BOOL    bReturn;
	DWORD   dwBufSize = sizeof(list);
	// Fill out list struct.
	list.dwSize = sizeof(list);
	// NULL == LAN, otherwise connectoid name.
	list.pszConnection = (LPTSTR)conn_name;
	// Set three options.
	list.dwOptionCount = 3;
	list.pOptions = new INTERNET_PER_CONN_OPTION[3];
	// Make sure the memory was allocated.
	if (NULL == list.pOptions)
	{
		// Return FALSE if the memory wasn't allocated.
		OutputDebugString("failed to allocat memory in SetConnectionOptions()");
		return FALSE;
	}
	// Set flags.
	list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
	list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT |
		PROXY_TYPE_PROXY;

	// Set proxy name.
	list.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	list.pOptions[1].Value.pszValue = (LPTSTR)proxy_full_addr;//"http://proxy:80";

													  // Set proxy override.
	list.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	list.pOptions[2].Value.pszValue = "local";

	// Set the options on the connection.
	bReturn = InternetSetOption(NULL,
		INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize);

	// Free the allocated memory.
	delete[] list.pOptions;
	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	return bReturn;
}
BOOL DisableConnectionProxy(LPCTSTR conn_name)
{
	//conn_name: active connection name. 
	INTERNET_PER_CONN_OPTION_LIST list;
	BOOL    bReturn;
	DWORD   dwBufSize = sizeof(list);
	// Fill out list struct.
	list.dwSize = sizeof(list);
	// NULL == LAN, otherwise connectoid name.
	list.pszConnection = (LPTSTR)conn_name;
	// Set three options.
	list.dwOptionCount = 1;
	list.pOptions = new INTERNET_PER_CONN_OPTION[list.dwOptionCount];
	// Make sure the memory was allocated.
	if (NULL == list.pOptions)
	{
		// Return FALSE if the memory wasn't allocated.
		OutputDebugString("failed to allocat memory in DisableConnectionProxy()");
		return FALSE;
	}
	// Set flags.
	list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
	list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT;
	// Set the options on the connection.
	bReturn = InternetSetOption(NULL,
		INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize);
	// Free the allocated memory.
	delete[] list.pOptions;
	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	return bReturn;
}