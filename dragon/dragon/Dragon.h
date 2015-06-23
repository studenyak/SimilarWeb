#pragma once
#include <windows.h>

DWORD sendFileToServer(LPCWSTR lpcsFilePath);
DWORD getProcessId(LPCWSTR strProcessName);
DWORD inject_library(DWORD dwProcessId, LPCWSTR lpcwsDllPath);
DWORD eject_library(DWORD dwProcessId,LPCWSTR lpcwsDllPath);