// CheckUsage.cpp : Defines the entry point for the application.
//

#include "CheckUsage.h"

int g_usageCount = 0;
char txt_buffer[1025] = { 0 };
HWND g_hwndDlg = NULL;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/*
	// test command-line code
	LPSTR commandLine = GetCommandLine();
	while ((commandLine[0] != ' ') && (lstrlen(commandLine) > 0))
		commandLine = CharNext(commandLine);
	wsprintf(txt_buffer, "hInstance: '0x%X' hPrevInstance: '0x%X' lpCmdLine: '0x%X' nCmdShow: '0x%X' commandLine: '%s'", hInstance, hPrevInstance, lpCmdLine, nCmdShow, commandLine);
	MessageBox(NULL, txt_buffer, "Test", 0);
	*/
	INITCOMMONCONTROLSEX common;			
	common.dwICC = ICC_LISTVIEW_CLASSES; 
	common.dwSize = sizeof(common);
	InitCommonControlsEx(&common);

	int ret = DialogBox((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_USAGE), NULL, WndProc);
	return 0;
}

BOOL CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message) 
	{
		case WM_INITDIALOG:
		{
			g_hwndDlg = hWnd;

			HWND hWndListView = GetDlgItem(hWnd, IDC_LIST_PROCESS_LIST);
			LV_COLUMN lvC;

			//Set the default values for the columns.
			lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvC.fmt = LVCFMT_LEFT;						

			//Set the column labels and add the columns
			lvC.cx = 150;
			lvC.pszText = "Name";
			lvC.iSubItem = 0;			
			ListView_InsertColumn(hWndListView, 0, &lvC);
			lvC.cx = 40;
			lvC.pszText = "PID";
			lvC.iSubItem = 1;			
			ListView_InsertColumn(hWndListView, 1, &lvC);
			lvC.cx = 60;
			lvC.fmt = LVCFMT_RIGHT;
			lvC.pszText = "Size";
			lvC.iSubItem = 2;			
			ListView_InsertColumn(hWndListView, 2, &lvC);
			lvC.cx = 75;
			lvC.fmt = LVCFMT_RIGHT;
			lvC.pszText = "Usage Count";
			lvC.iSubItem = 3;			
			ListView_InsertColumn(hWndListView, 3, &lvC);

			ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_LABELTIP);
			
			RefreshProcessList(hWndListView);

			SetTimer(hWnd, 456, 2000, NULL);
			break;
		}
		case WM_TIMER:
		{
			if (wParam == 456) {
				HWND hWndListView = GetDlgItem(hWnd, IDC_LIST_PROCESS_LIST);
				// Get the selected item
				int nItem = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
				// Update h
				RefreshProcessList(hWndListView);

				ListView_SetItemState(hWndListView, -1, 0, LVIS_SELECTED); // deselect all items
				if (nItem != -1) {
					ListView_EnsureVisible(hWndListView, nItem, TRUE); // if item is far, scroll to it
					ListView_SetItemState(hWndListView, nItem, LVIS_SELECTED, LVIS_SELECTED); // select item
					ListView_SetItemState(hWndListView, nItem, LVIS_FOCUSED, LVIS_FOCUSED); // optional
				}
			}
			break;
		}
		case WM_DESTROY:
		{
			KillTimer(hWnd, 456);
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hWnd, IDOK);
			break;
		}
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{		
				case IDC_BUTTON_REFRESH:
				{
					switch (wmEvent) 
					{ 
						case BN_CLICKED: 
						{
							RefreshProcessList(GetDlgItem(hWnd, IDC_LIST_PROCESS_LIST));
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_KILL_PROCESS:
				{
					switch (wmEvent) 
					{ 
						case BN_CLICKED: 
						{
							HWND hWndListView = GetDlgItem(hWnd, IDC_LIST_PROCESS_LIST);
							// Get the selected item
							int nItem = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);							
							if (nItem == -1)
								return TRUE;
							int ret = MessageBox(hWnd, "Are you sure you want to exit the selected process?", "Confirm", MB_YESNO);
							if (ret == IDYES) {								
								LVITEM itemData;
								memset(&itemData, 0, sizeof(LVITEM));
								itemData.mask = LVIF_PARAM;
								itemData.iSubItem = 0;
								itemData.iItem = nItem;

								ListView_GetItem(hWndListView, &itemData);
								
								HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)itemData.lParam);
								if (processHandle != NULL && processHandle != INVALID_HANDLE_VALUE) {
									// Exit the process
									HANDLE closeProcessThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)ExitProcess, 0, 0, NULL);
									WaitForSingleObject(closeProcessThread, INFINITE);									
									CloseHandle(closeProcessThread);
									CloseHandle(processHandle);								
								} else {
									MessageBox(hWnd, "Failed to get a handle to selected process.", "Failed", 0);
								}
								RefreshProcessList(hWndListView);
							}
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_FORCE_UNLOAD:
				{
					switch (wmEvent) 
					{ 
						case BN_CLICKED: 
						{
							/*HWND hWndListView = GetDlgItem(hWnd, IDC_LIST_PROCESS_LIST);
							// Get the selected item
							int nItem = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);							
							if (nItem == -1)
								return TRUE;
							int ret = MessageBox(hWnd, "Are you sure you want to unload MatroskaProp.dll from the selected process? It's better to kill the process because the process always crashes after a forced unload.", "Confirm", MB_YESNO);
							if (ret == IDYES) {																	
								LVITEM itemData;
								memset(&itemData, 0, sizeof(LVITEM));
								itemData.mask = LVIF_PARAM;
								itemData.iSubItem = 0;
								itemData.iItem = nItem;
								ListView_GetItem(hWndListView, &itemData);

								HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)itemData.lParam);

								memset(&itemData, 0, sizeof(LVITEM));
								itemData.mask = LVIF_PARAM;
								itemData.iSubItem = 2;
								itemData.iItem = nItem;
								ListView_GetItem(hWndListView, &itemData);

								HMODULE dllHandle = (HMODULE)itemData.lParam;

								if (dllHandle != NULL && processHandle != NULL && processHandle != INVALID_HANDLE_VALUE) {
									// Exit the process
									CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, dllHandle, 0, NULL);
									CloseHandle(processHandle);								
								} else {
									MessageBox(hWnd, "Failed to get a handle to selected process.", "Failed", 0);
								}
								RefreshProcessList(hWndListView);					
							}*/
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_CLOSE:
				{
					switch (wmEvent) 
					{ 
						case BN_CLICKED: 
						{
							EndDialog(hWnd, IDOK);
							break;
						}
					}
					break;
				}
				default:
					return FALSE;
			}
			break;
	}
	return FALSE;
}

void RefreshProcessList(HWND hWndListView)
{
	GetProcessList(hWndListView);
	return;

	/*int ret;	
	static DWORD processArray[1024];	
	DWORD processArrayCount;
	DWORD processArraySize;
	
	ret = EnumProcesses(processArray, sizeof(DWORD)*1024, &processArraySize);
	if (ret == 0)
		return;

	processArrayCount = processArraySize / sizeof(DWORD);

	int index = -1;
	LVITEM lvI;
	//Setup the default item values
	lvI.mask = LVIF_TEXT | LVIF_STATE;
	lvI.state = 0;
	lvI.stateMask = 0;								
			
	for (DWORD p = 0; p < processArrayCount; p++)
	{
		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		// wsprintf is safe to use because it cannot format buffers larger than 1024. my buffer is 1025 :)
		wsprintf(txt_buffer, "%i", processArray[p]);		
		lvI.pszText = txt_buffer;
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);		

		HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processArray[p]);


		//Do a subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		wsprintf(txt_buffer, "%i", processArray[p]);		
		lvI.pszText = txt_buffer;
		ListView_SetItem(hWndListView, &lvI);

		static HMODULE moduleArray[1024];
		DWORD moduleArrayCount;
		DWORD moduleArraySize;
		ret = EnumProcessModules(processHandle, moduleArray, sizeof(HMODULE)*1024, &moduleArraySize);
		if (ret == 0)
			continue;
		
		moduleArrayCount = moduleArraySize / sizeof(HMODULE);
		for (DWORD m = 0; m < moduleArraySize; m++)
		{
			static char moduleName[256];
			ret = GetModuleBaseName(processHandle, moduleArray[m], moduleName, 255);
			if (ret == 0)
				continue;

			MODULEINFO moduleInfo;
			ret = GetModuleInformation(processHandle, moduleArray[m], &moduleInfo, sizeof(MODULEINFO));
			if (ret == 0)
				break;
		}
	}*/
};

BOOL GetProcessList(HWND hWndListView) 
{ 
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 

	//  Take a snapshot of all processes in the system. 

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	//  Fill in the size of the structure before using it. 

	pe32.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 
	if (Process32First(hProcessSnap, &pe32)) 
	{ 
		DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32       = {0}; 

		int index = -1;
		LVITEM lvI;
		//Setup the default item values
		lvI.mask = LVIF_TEXT | LVIF_PARAM;
		//Clear the previous items
		ListView_DeleteAllItems(hWndListView);
		g_usageCount = 0;

		do 
		{ 
			HANDLE hProcess; 

			// Get the actual priority class. 
			hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
			dwPriorityClass = GetPriorityClass (hProcess); 			

			MODULEENTRY32 matroskaPropModule;
			matroskaPropModule.dwSize = sizeof(MODULEENTRY32);
			if (FindMatroskaPropModule(pe32.th32ProcessID, &matroskaPropModule, matroskaPropModule.dwSize))
			{
				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.lParam = pe32.th32ProcessID;
				lvI.pszText = pe32.szExeFile;				
				//Insert the item	
				index = ListView_InsertItem(hWndListView, &lvI);		

				// wsprintf is safe to use because it cannot format buffers larger than 1024. my buffer is 1025 :)
				wsprintf(txt_buffer, "%i", pe32.th32ProcessID);		
				ListView_SetItemText(hWndListView, index, 1, txt_buffer);

				wsprintf(txt_buffer, "%i KB", matroskaPropModule.modBaseSize / 1024);		
#ifdef _DEBUG
				OutputDebugString(txt_buffer);
				OutputDebugString("\n");
#endif
				ListView_SetItemText(hWndListView, index, 2, txt_buffer);   


				wsprintf(txt_buffer, "%i", matroskaPropModule.ProccntUsage);		
				ListView_SetItemText(hWndListView, index, 3, txt_buffer);

				g_usageCount += matroskaPropModule.ProccntUsage;
			}
			CloseHandle(hProcess); 
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
		bRet = TRUE; 
	} 
	else 
		bRet = FALSE;    // could not walk the list of processes 

	// Do not forget to clean up the snapshot object. 
	CloseHandle(hProcessSnap); 

	// Update the global usage count
	wsprintf(txt_buffer, "Global Usage Count: %i", g_usageCount);
	SetDlgItemText(g_hwndDlg, IDC_STATIC_GLOBAL_USAGE_COUNT, txt_buffer);

	return (bRet); 
} 

BOOL FindMatroskaPropModule(DWORD dwPID, LPMODULEENTRY32 lpMe32, DWORD cbMe32) 
{ 
	BOOL          bRet        = FALSE; 
	BOOL          bFound      = FALSE; 
	HANDLE        hModuleSnap = NULL; 
	MODULEENTRY32 me32        = {0}; 

	// Take a snapshot of all modules in the specified process. 

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
	if (hModuleSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	// Fill the size of the structure before using it. 

	me32.dwSize = sizeof(MODULEENTRY32); 

	// Walk the module list of the process, and find the module of 
	// interest. Then copy the information to the buffer pointed 
	// to by lpMe32 so that it can be returned to the caller. 

	if (Module32First(hModuleSnap, &me32)) 
	{ 
		do 
		{ 
			if (!lstrcmpi(me32.szModule, "MatroskaProp.dll")) 
			{ 
				CopyMemory (lpMe32, &me32, cbMe32); 
				bFound = TRUE; 
			} 
		} 
		while (!bFound && Module32Next(hModuleSnap, &me32)); 

		bRet = bFound;   // if this sets bRet to FALSE, dwModuleID 
		// no longer exists in specified process 
	} 
	else 
		bRet = FALSE;           // could not walk module list 

	// Do not forget to clean up the snapshot object. 

	CloseHandle (hModuleSnap); 

	return (bRet); 
} 
