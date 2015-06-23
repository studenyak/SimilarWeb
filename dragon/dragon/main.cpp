#include <windows.h>
#include "Dragon.h"
#include <Shlobj.h>

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
   return -1;

 g_hMainWnd = CreateWindow(wc.lpszClassName,
                   L"Dragon",
                   WS_OVERLAPPEDWINDOW|WS_VISIBLE,
				   0,
				   0,
				   190,
				   110,
				   0,
				   0,
				   hInstance,
				   NULL);
 if(!g_hMainWnd)
    return -1;

 g_hButton = CreateWindow( 
    L"BUTTON",
    L"Send", 
    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
    10,
    40, 
    100,
    25,
    g_hMainWnd,
    NULL,
    (HINSTANCE)GetWindowLong(g_hMainWnd, GWL_HINSTANCE), 
    NULL);

 DWORD dwProcessId = getProcessId(g_app);
 g_hTextLabel = CreateWindow( 
    L"static",
    (dwProcessId != -1) ? L"Chrome is running" : L"Chrome is not running", 
    WS_VISIBLE | WS_CHILD,
    10,
    10, 
    150,
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
		   sendChromeCurrentTabs();
	   }
	   break;
   default:
     return DefWindowProc(hWnd, message, wParam, lParam);
 }
 return 0;

}  
