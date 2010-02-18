
#ifndef _CHECK_USAGE_H_
#define _CHECK_USAGE_H_

#include <windows.h>
#include <commctrl.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include "resource.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void RefreshProcessList(HWND hWndListView);

BOOL GetProcessList(HWND hWndListView);
BOOL FindMatroskaPropModule(DWORD dwPID, LPMODULEENTRY32 lpMe32, DWORD cbMe32);

#endif // _CHECK_USAGE_H_
