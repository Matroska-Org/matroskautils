/*
 *  Matroska Shell Extension
 *
 *  MatroskaProp.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
 * 
 *  The idea was based from ImgSize by Victor Sapojnikov
 *
 *  The Matroska Shell Extension is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  The Matroska Shell Extension is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Matroska Shell Extension; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*!
    \file MatroskaProp.cpp
		\version \$Id: MatroskaProp.cpp,v 1.39 2004/03/08 07:56:51 jcsston Exp $
    \brief This is the meat code of the Matroska Shell Extension ;)
		\author Jory Stone     <jcsston @ toughguy.net>

*/

//Memory Leak Debuging define
#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <mbstring.h>
#include <shobjidl.h>
#include <Shlwapi.h>
//
// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "MatroskaProp.h"
#pragma data_seg()

#include "Http.h"

#include "mdump.h"
CrashCatcher *g_objCrasher = NULL;

extern long g_MyVersionMajor;
extern long g_MyVersionMinor;
extern long g_MyBuildNumber;
//
// Global variables
//
// Reference count of this DLL.
UINT g_cRefThisDll = 0;    
// Handle to this DLL itself.
extern HINSTANCE g_hmodThisDll = NULL;	

// Matroska File info Cache
MatroskaInfoParserCache g_parserCache;

#ifdef _DEBUG
DWORD g_MatroskaInfoParserSize = sizeof(MatroskaInfoParser);
#endif

#include "resource.h"

#define UM_UPDATE (WM_USER+1)

using namespace MatroskaUtilsNamespace;

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _DEBUG
	// Setup the debug options
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF 
								| _CRTDBG_LEAK_CHECK_DF //Check for memory leaks on app exit
								);//| _CRTDBG_CHECK_ALWAYS_DF);
	_CrtSetAllocHook(YourAllocHook);	
#endif

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		ODS("In DLLMain, DLL_PROCESS_ATTACH");

		// Extension DLL one-time initialization
		g_hmodThisDll = hInstance;

		if (MatroskaShellExt_GetRegistryValue(_T("Crash Catcher"), 1)) {
			// Setup the crash catcher
			g_objCrasher = new CrashCatcher(_T(MATROSKA_SHELL_EXT_APP_NAME));
		}		

		//For some reason on my system Explorer always has High Priority, which is bad
		HANDLE hProcess = GetCurrentProcess();
		SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
		CloseHandle(hProcess);
		
		HMODULE myModule;
		//Get the dll filename
		TCHAR dllFilename[MAX_MATROSKA_PATH];
		myModule = GetModuleHandle(_T("MatroskaProp.dll"));		
		GetModuleFileName(myModule, dllFilename, MAX_MATROSKA_PATH/sizeof(TCHAR));
		
		// Setup gettext()
		if (gettext_is_loaded()) {
			// Tell gettext to use the MatroskaProp textdomain
			textdomain("MatroskaProp");

			// Tell gettext where the language files are
			JString dllFilenameStr = dllFilename;
			dllFilenameStr = dllFilenameStr.BeforeLast(L'\\');
			dllFilenameStr += "\\lang\\";

			bindtextdomain("MatroskaProp", dllFilenameStr.mb_str());
			// Set the output text encode to UTF-8
			//bind_textdomain_codeset("MatroskaProp", "UTF-8");

			// Load the translation to use
			gettext_putenv(MatroskaShellExt_GetRegistryValueStr(_T("Translation"), "LANGUAGE=English").mb_str());
		}

#if 0
    // Fill in the build number
		DWORD verSize = GetFileVersionInfoSize(dllFilename, NULL);
		if (verSize > 0) {
			binary *verData = new binary[verSize+1];
			GetFileVersionInfo(dllFilename, NULL, verSize, verData);
			void *verBuffer = NULL;
			UINT verBufferSize = 0;
			VerQueryValue(verData, _T("\\StringFileInfo\\040904b0\\FileVersion"), &verBuffer, &verBufferSize);
			if (verBuffer != NULL) {
				JString version((wchar_t *)verBuffer);	
				version = version.AfterLast(L',');
				g_MyBuildNumber = atol(version.mb_str());
			}
			delete verData;
		}
#endif
	} else if (dwReason == DLL_PROCESS_DETACH) {
		ODS("In DLLMain, DLL_PROCESS_DETACH");
		
		//Perform some cleanup		
		if (g_objCrasher != NULL)
			delete g_objCrasher;
	}

	return 1;   // ok
}

/// Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	TRACE("DllRegisterServer()");
#ifdef _DEBUG	
	MessageBox(NULL, _T(MATROSKA_SHELL_EXT_APP_NAME) _T(" Debug\nBuild ") _T(__DATE__) _T(" ") _T(__TIME__), _T(MATROSKA_SHELL_EXT_APP_NAME), 0);
#endif

	MatroskaProp setupShellExt;
	setupShellExt.Install();

	//if (MatroskaShellExt_GetRegistryValue(_T("First Time"), 1)) {
		setupShellExt.ChooseIcon();
		ConfigureOptions();
	//}
	//MatroskaShellExt_SetRegistryValue(_T("First Time"), 0);

	return S_OK;
};

/// Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	TRACE("DllUnregisterServer()");
	
	MatroskaProp setupShellExt;
	setupShellExt.Uninstall();

	return S_OK;
};

STDAPI GetMatroskaPropVersion(void)
{
	TRACE("GetMatroskaPropVersion()");
	return g_MyBuildNumber;
};

STDAPI DllGetVersion(DLLVERSIONINFO *dvI)
{
	TRACE("DllGetVersion(DLLVERSIONINFO *dvI)");
	if (dvI != NULL) {
		dvI->dwMajorVersion = g_MyVersionMajor;
		dvI->dwMinorVersion = g_MyVersionMinor;
		dvI->dwBuildNumber = g_MyBuildNumber;
	}
	return 0;
}

// Option Configuration Dialog Function
STDAPI ConfigureOptions(void)
{	
	TRACE("ConfigureOptions()");	
	int ret = DialogBox(GetModuleHandle(_T("MatroskaProp")), MAKEINTRESOURCE(IDD_DIALOG_SHELL_OPTIONS), NULL, ConfigurationDlgProc);	
	if (ret == -1)
	{
		TCHAR err_key[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, err_key, 255, NULL); 
		TCHAR msg_txt[512];
		_sntprintf(msg_txt, 511, _T("File: %s \nLine: %i \nFunction: %s\nSystem Message: %s"), _T(__FILE__) , __LINE__, _T(__FUNCTION__), err_key);
		MessageBox(NULL, msg_txt, _T("Program Error"), 0);
	}

	return 0;
};

STDAPI ChooseIcon(void)
{	
	TRACE("ChooseIcon()");	
	MatroskaProp iconChooser;
	return iconChooser.ChooseIcon();
};

// VistUnicowsWebsite function, I hope you know what it does :P
STDAPI VistUnicowsWebsite(void)
{		
	//I have to use the ASCII function no Unicode wrapper ;)
	ShellExecuteA(NULL, "open", "http://www.microsoft.com/globaldev/handson/dev/mslu_announce.mspx", NULL, NULL, SW_NORMAL);

	return 0;
};

STDAPI CheckForUpdates(void)
{		

	return 0;
};

STDAPI SubmitSurvey()
{
  JString survey = MatroskaShellExt_GetRegistryValueStr(_T("Survey"), _T("SelectedLanguage=English"));
  
  OSVERSIONINFO osVer;
  osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (GetVersionEx(&osVer)) {
    survey += ":OS=";
    if (osVer.dwMajorVersion == 4 && osVer.dwMinorVersion == 0) {
      survey += _T("Windows 95");

    } else if (osVer.dwMajorVersion == 4 && osVer.dwMinorVersion == 10) {
      survey += _T("Windows 98");

    } else if (osVer.dwMajorVersion == 4 && osVer.dwMinorVersion == 90) {
      survey += _T("Windows ME");

    } else if (osVer.dwMajorVersion == 3 && osVer.dwMinorVersion == 51) {
      survey += _T("Windows NT 3.51");

    } else if (osVer.dwMajorVersion == 4 && osVer.dwMinorVersion == 0) {
      survey += _T("Windows NT 4.0");

    } else if (osVer.dwMajorVersion == 5 && osVer.dwMinorVersion == 0) {
      survey += _T("Windows NT 2000");

    } else if (osVer.dwMajorVersion == 5 && osVer.dwMinorVersion == 1) {
      survey += _T("Windows XP");
    
    } else {
      survey += JString::Format(_T("Unknown %i.%i"), osVer.dwMajorVersion, osVer.dwMinorVersion);
    }
  }
    
  char szBuffer[256];
  szBuffer[0] = 0;
  GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_SLANGUAGE, szBuffer, 255);
  if (strlen(szBuffer) > 0) {
    survey += ":SystemLanguage=";
    survey += szBuffer;
  }

  survey = CHttp::ConvertToValidURI(survey);

  std::string url;
  url = "/MatroskaProp.php?Survey=";
  url += survey.mb_str();

  CHttp http;
  std::string result = http.ReadPage("www.jory.info", url);  
  //MessageBoxA(NULL, result.c_str(), "Survey Results", 0);
  return S_OK;
}

MatroskaProp::MatroskaProp()
{
	TRACE("MatroskaProp::MatroskaProp()");

	//Get the filename+path of this dll	
	GetModuleFileName(GetModuleHandle(_T("MatroskaProp")), dllFilename, MAX_MATROSKA_PATH/sizeof(TCHAR));
	
	m_Installed = false;
	m_Uninstalled = false;
};

MatroskaProp::~MatroskaProp()
{
	TRACE("MatroskaProp::~MatroskaProp()");

	if (m_Installed)
	{
		//Write the key list to reg
		std::list<JString>::iterator it, itEnd;
		for (it = m_keyList.begin(), itEnd = m_keyList.end(); it != itEnd; it++)
		{
			TCHAR *reg_key = _T("SOFTWARE\\MatroskaProp\\KeyList");
			HKEY key_handle = NULL;
			DWORD lpType = NULL;
			DWORD state = 0;
			SECURITY_ATTRIBUTES sa = {sizeof(sa), 0,1};

			RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_key, 0, _T(""), 0, KEY_WRITE, &sa, &key_handle, &state);

			DWORD the_value = 0;
			DWORD size = sizeof(the_value);
			RegSetValueEx(key_handle, ((JString)*it).c_str(), 0, REG_DWORD, (CONST BYTE*)&the_value, size);
			RegCloseKey(key_handle);
		}
	}
};

void MatroskaProp::SetupKeys()
{
	TRACE("MatroskaProp::SetupKeys()");

	TCHAR *key_list[28][3] =
	// Key location, name, data
	{{_T("CLSID\\") MatroskaPageCLSID, _T(""), _T("The Matroska Shell Extension, Prop Page CLSID")},
	{_T("CLSID\\") MatroskaPageCLSID _T("\\InProcServer32\\"), _T(""), dllFilename},
	{_T("CLSID\\") MatroskaPageCLSID _T("\\InProcServer32\\"), _T("ThreadingModel"), _T("Apartment")},
	{_T("*\\shellex\\PropertySheetHandlers\\") MatroskaPageCLSID, _T(""), _T("The Matroska Shell Extension, Prop Page")},
	
	//Thumbnail	
	{_T("CLSID\\") MatroskaThumbnailCLSID, _T(""), _T("The Matroska Shell Extension, Thumbnail Handler CLSID")},
	{_T("CLSID\\") MatroskaThumbnailCLSID _T("\\InProcServer32\\"), _T(""), dllFilename},
	{_T("CLSID\\") MatroskaThumbnailCLSID _T("\\InProcServer32\\"), _T("ThreadingModel"), _T("Apartment")},
	
	//Tooltip
	{_T("CLSID\\") MatroskaTooltipCLSID, _T(""), _T("The Matroska Shell Extension, Tooltip Provider CLSID")},
	{_T("CLSID\\") MatroskaTooltipCLSID _T("\\InProcServer32\\"), _T(""), dllFilename},
	{_T("CLSID\\") MatroskaTooltipCLSID _T("\\InProcServer32\\"), _T("ThreadingModel"), _T("Apartment")},

	//Context Menu
	{_T("CLSID\\") MatroskaContextMenuCLSID, _T(""), _T("The Matroska Shell Extension, Context Menu CLSID")},
	{_T("CLSID\\") MatroskaContextMenuCLSID _T("\\InProcServer32\\"), _T(""), dllFilename},
	{_T("CLSID\\") MatroskaContextMenuCLSID _T("\\InProcServer32\\"), _T("ThreadingModel"), _T("Apartment")},
	//Context Menu handler
	{_T("*\\shellex\\ContextMenuHandlers\\MatroskaContextMenu\\"), _T(""), MatroskaContextMenuCLSID},

	//Column
	{_T("CLSID\\") MatroskaColumnProviderCLSID, _T(""), _T("The Matroska Shell Extension, Column Provider CLSID")},
	{_T("CLSID\\") MatroskaColumnProviderCLSID _T("\\InProcServer32\\"), _T(""), dllFilename},
	{_T("CLSID\\") MatroskaColumnProviderCLSID _T("\\InProcServer32\\"), _T("ThreadingModel"), _T("Apartment")},
	//Column Handler
	{_T("Folder\\shellex\\ColumnHandlers\\") MatroskaColumnProviderCLSID _T(""), _T(""), _T("The Matroska Shell Extension, Column Provider CLSID")},
		
	//Shell Icon
  // Disabled for now, too buggy
	//{_T("CLSID\\") MatroskaShellIconCLSID, _T(""), _T("The Matroska Shell Extension, Shell Icon Provider CLSID")},
	//{_T("CLSID\\") MatroskaShellIconCLSID _T("\\InProcServer32\\"), _T(""), dllFilename},
	//{_T("CLSID\\") MatroskaShellIconCLSID _T("\\InProcServer32\\"), _T("ThreadingModel"), _T("Apartment")},

	//26
	{NULL, NULL, NULL}};

	TCHAR *supported_ext[4] = {
		_T(".mka"), 
		_T(".mkv"),
		_T(".mks"), 
		NULL};

	TCHAR *ext_key_list[4][3] =
	//Setup the Shell Ext
	{
	//InfoTip handler
	{_T("\\shellex\\{00021500-0000-0000-C000-000000000046}\\"), _T(""), MatroskaTooltipCLSID},
	//Thumbnail image handler
	{_T("\\shellex\\{BB2E617C-0920-11d1-9A0B-00C04FC2D6C1}\\"), _T(""), MatroskaThumbnailCLSID},	
	//Shell Icon handler
	//{_T("\\shellex\\IconHandler\\"), _T(""), MatroskaShellIconCLSID},
	{NULL, NULL, NULL}
	};

	TCHAR *approved_list[7][2] =
	// CLSID, Nice name
	{{MatroskaPageCLSID, _T("Matroska Shell Extension, Properties Page CLSID")},
	{MatroskaThumbnailCLSID, _T("Matroska Shell Extension, Thumbnail Handler CLSID")},
	{MatroskaTooltipCLSID, _T("Matroska Shell Extension, Tooltip Provider CLSID")},
	{MatroskaContextMenuCLSID, _T("Matroska Shell Extension, ContextMenu CLSID")},	
	{MatroskaColumnProviderCLSID, _T("Matroska Shell Extension, Column Provider CLSID")},
	//{MatroskaShellIconCLSID, _T("Matroska Shell Extension, Shell Icon CLSID")},
	{NULL, NULL}};

	//Copy the local arrays to the class arrays
	memcpy(this->approved_list, approved_list, sizeof(approved_list));
	memcpy(this->ext_key_list, ext_key_list, sizeof(ext_key_list));
	memcpy(this->supported_ext, supported_ext, sizeof(supported_ext));
	memcpy(this->key_list, key_list, sizeof(key_list));
};

int MatroskaProp::Install()
{
	TRACE("MatroskaProp::Install()");

	m_Installed = true;
	SetupKeys();
	HKEY key_handle = NULL;
	DWORD lpType = 0;	
	DWORD state = 0;

	uint8 k = 0;
	while (key_list[k][0] != NULL)
	{
		RegCreateKeyEx(HKEY_CLASSES_ROOT, key_list[k][0], 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
		RegSetValueEx(key_handle, key_list[k][1], 0, REG_SZ, (CONST BYTE *)key_list[k][2], sizeof(TCHAR)*_tcsclen(key_list[k][2])+1);
		//Add key to the list
		m_keyList.push_back(JString("HKEY_CLASSES_ROOT$") + JString(key_list[k][0]) + "$" + key_list[k][1]);
		RegCloseKey(key_handle);
		k++;
	}
	
	TCHAR targetKey[128];
	uint8 e = 0; 
	while (supported_ext[e] != NULL)
	{
		wcsncpy(targetKey, supported_ext[e], 127);
		RegCreateKeyEx(HKEY_CLASSES_ROOT, supported_ext[e], 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
		if(state == 2)
		{
			DWORD size = sizeof(targetKey);
			RegQueryValueEx(key_handle, _T(""), 0, &lpType, (BYTE *)targetKey, &size);
		}
		RegCloseKey(key_handle);

		k = 0;
		while (ext_key_list[k][0] != NULL)
		{
			//Construct the key string
			JString correctKey(targetKey);
			correctKey += ext_key_list[k][0];
			
			//Open the key
			RegCreateKeyEx(HKEY_CLASSES_ROOT, correctKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
			//Set the value
			RegSetValueEx(key_handle, ext_key_list[k][1], 0, REG_SZ, (CONST BYTE *)ext_key_list[k][2], sizeof(TCHAR)*_tcsclen(ext_key_list[k][2])+1);
			//Close the key
			RegCloseKey(key_handle);
			
			//Add the new key to the list
			m_keyList.push_back(JString("HKEY_CLASSES_ROOT$") + correctKey + "$" + ext_key_list[k][1]);

			//Goto the next key
			k++;
		}	
		//Goto the next ext key
		e++;
	}	

	k = 0;
	while (approved_list[k][0] != NULL)
	{
		//Add the CLSID's to the approved list
		RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
		RegSetValueEx(key_handle, approved_list[k][0], 0, REG_SZ, (CONST BYTE *)approved_list[k][1], sizeof(TCHAR)*_tcsclen(approved_list[k][1])+1);
		
		//Add the new key to the list
		m_keyList.push_back(JString("HKEY_LOCAL_MACHINE$") + _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved") + "$" + approved_list[k][0]);

		RegCloseKey(key_handle);
		k++;
	}	
	return 0;
};

void MatroskaProp::LoadKeyList()
{
	TRACE("MatroskaProp::LoadKeyList()");

	TCHAR *reg_key = _T("SOFTWARE\\MatroskaProp\\KeyList");
	HKEY key_handle = NULL;	
	DWORD state = 0;
	TCHAR keyName[1025];	
	DWORD keyNameSize = 1024;
	uint16 k = 0;

	//Open the base key
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);

	state = ERROR_SUCCESS;
	while (state == ERROR_SUCCESS)
	{
		state = RegEnumValue(key_handle, k++, keyName, &keyNameSize, NULL, NULL, NULL, NULL);
		keyNameSize = 1024;
		if (state != ERROR_NO_MORE_ITEMS)
			m_keyList.push_back(JString(keyName));
	}
};

int MatroskaProp::Uninstall()
{	
	TRACE("MatroskaProp::Uninstall()");

	m_Uninstalled = true;	
	LoadKeyList();

	std::list<JString>::iterator it, itEnd;
	for (it = m_keyList.begin(), itEnd = m_keyList.end(); it != itEnd; it++)
	{
		JString theKey = *it;
		if (theKey.length() > 3)
		{
			HKEY key_handle = NULL;
			DWORD lpType = NULL;
			DWORD state = 0;
			SECURITY_ATTRIBUTES sa = {sizeof(sa), 0,1};
			TCHAR err_key[1025];
			DWORD err = 0;
			
			if (!wcsicmp(theKey.BeforeFirst(L'$'), L"HKEY_LOCAL_MACHINE"))
				err = RegCreateKeyEx(HKEY_LOCAL_MACHINE, theKey.AfterFirst(L'$').BeforeLast(L'$').t_str(), 0, _T(""), 0, KEY_ALL_ACCESS, &sa, &key_handle, &state);
			else if (!wcsicmp(theKey.BeforeFirst(L'$'), L"HKEY_CLASSES_ROOT"))
				err = RegCreateKeyEx(HKEY_CLASSES_ROOT, theKey.AfterFirst(L'$').BeforeLast(L'$').t_str(), 0, _T(""), 0, KEY_ALL_ACCESS, &sa, &key_handle, &state);
			else
				break;		
			
			err = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, err_key, 1024, NULL); 

			JString keyName = theKey.AfterLast(L'$');
			err = RegDeleteKey(key_handle, keyName.t_str());
			err = RegCloseKey(key_handle);
		}
	}


	//Remove the settings
	SHDeleteKey(HKEY_LOCAL_MACHINE, _T("Software\\MatroskaProp\\"));
	
	return 0;
};

int MatroskaProp::ChooseIcon() {
	JString iconName("");
	
	DialogBoxParam(GetModuleHandle(_T("MatroskaProp")), MAKEINTRESOURCE(IDD_DIALOG_ICONS), NULL, IconDlgProc, (LPARAM)&iconName);

	if (iconName.length() < 1)
		return 1;

	iconName = JString(dllFilename) + iconName;

	SetupKeys();
	HKEY key_handle = NULL;
	DWORD lpType = 0;	
	DWORD state = 0;	
	TCHAR targetKey[128];
	uint8 e = 0; 
	while (supported_ext[e] != NULL)
	{
		wcscpy(targetKey, supported_ext[e]);
		RegCreateKeyEx(HKEY_CLASSES_ROOT, supported_ext[e], 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
		if(state == 2)
		{
			DWORD size = sizeof(targetKey);
			RegQueryValueEx(key_handle, _T(""), 0, &lpType, (BYTE *)targetKey, &size);
		}
		RegCloseKey(key_handle);
			
		JString correctKey(targetKey);
		correctKey.append(L"\\DefaultIcon\\");
		//Open the key
		RegCreateKeyEx(HKEY_CLASSES_ROOT, correctKey.t_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
		//Set the value
		RegSetValueEx(key_handle, _T(""), 0, REG_SZ, (CONST BYTE *)iconName.t_str(), sizeof(TCHAR)*iconName.length()+1);
		//Close the key
		RegCloseKey(key_handle);
		
		//Add the new key to the list
		m_keyList.push_back(JString("HKEY_CLASSES_ROOT$") + correctKey + "$" + iconName.t_str());
		//Goto the next ext key
		e++;
	}

	return 0;
};

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------
STDAPI DllCanUnloadNow(void)
{
	ODS("In DLLCanUnloadNow");

	if (g_cRefThisDll == 0)
		return S_OK;

	return S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
	ODS("DllGetClassObject()");

	*ppvOut = NULL;

	if (IsEqualIID(rclsid, CLSID_MatroskaPage_ShellExtension))
	{
		CShellExtClassFactory *pcf = new CShellExtClassFactory;

		return pcf->QueryInterface(riid, ppvOut);
	}
	else if (IsEqualIID(rclsid, CLSID_MatroskaTooltip_ShellExtension) && MatroskaShellExt_GetRegistryValue(_T("Tooltips"), 0))
	{
		CToolTipClassFactory *pcf = new CToolTipClassFactory;

		return pcf->QueryInterface(riid, ppvOut);
	}
	else if (IsEqualIID(rclsid, CLSID_MatroskaThumbnail_ShellExtension) && MatroskaShellExt_GetRegistryValue(_T("Thumbnails"), 0))
	{
		CThumbnailClassFactory *pcf = new CThumbnailClassFactory;

		return pcf->QueryInterface(riid, ppvOut);
	}
	else if (IsEqualIID(rclsid, CLSID_MatroskaColumnProvider_ShellExtension) && MatroskaShellExt_GetRegistryValue(_T("Column Provider"), 1))
	{
		CColumnProviderClassFactory *pcf = new CColumnProviderClassFactory;

		return pcf->QueryInterface(riid, ppvOut);
	}
	else if (IsEqualIID(rclsid, CLSID_MatroskaContextMenu_ShellExtension) && MatroskaShellExt_GetRegistryValue(_T("Context Menus"), 0))
	{
		CContextMenuClassFactory *pcf = new CContextMenuClassFactory;

		return pcf->QueryInterface(riid, ppvOut);
	}
	else if (IsEqualIID(rclsid, CLSID_MatroskaShellIcon_ShellExtension) && MatroskaShellExt_GetRegistryValue(_T("Shell Icons"), 1))
	{
		CShellIconClassFactory *pcf = new CShellIconClassFactory;

		return pcf->QueryInterface(riid, ppvOut);
	}

	return CLASS_E_CLASSNOTAVAILABLE;
}

#ifdef USE_MUXER
MatroskaSimpleMuxer::MatroskaSimpleMuxer(enum MatroskaSimpleMuxerModes type)
{
	muxerType = type;
};

MatroskaSimpleMuxer::~MatroskaSimpleMuxer()
{
	
};

HRESULT MatroskaSimpleMuxer::AddInputFile(JString &inputFilename)
{
	inputFilenames.push_back(inputFilename);
	
	//If we don't have an output filename, we append ".mkv" to the input and call it a day ;P
	if (outputFilename.length() < 1)
		outputFilename = inputFilename.append(_T(".mkv"));

	return S_OK;
};

HRESULT MatroskaSimpleMuxer::MuxToMatroska()
{
	if (muxerType == MatroskaMux_DShow)
		return MuxViaDShow();
	else if (muxerType == MatroskaMux_mkvmerge)
		return MuxViaMkvmerge();

	return E_INVALIDARG;
};

HRESULT MatroskaSimpleMuxer::MuxViaDShow()
{
	//Use DShow to get a frame of the video
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{				
		return E_FAIL;
	}	
	IGraphBuilder *pGraph;
	IBaseFilter *pInputFile;
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (FAILED(hr))
	{				
		return E_FAIL;
	}
	//Ok, now we add the Matroska Muxer filter
	IBaseFilter *pMatroskaMuxer;
	//Matroska Muxer CLISD {1E1299A2-9D42-4F12-8791-D79E376F4143}
	hr = AddFilterByCLSID(pGraph, CLSID_MatroskaMuxer, _T("Matroska Muxer"), &pMatroskaMuxer);
	if (!SUCCEEDED(hr))
	{		
		//Free the DShow interfaces
		pGraph->Release();
		CoUninitialize();
		//Failed
		return E_FAIL;	
	}
	//Find the out pint of the muxer
	IPin *pMuxer_outPin;
	hr = FindOutputPin(pMatroskaMuxer, &pMuxer_outPin);	
	//Get the MediaType
	IEnumMediaTypes *pMediaTypesEnum;
	hr = pMuxer_outPin->EnumMediaTypes(&pMediaTypesEnum);
	AM_MEDIA_TYPE *pMediaType;
	hr = pMediaTypesEnum->Next(1, &pMediaType, NULL);
	pMediaTypesEnum->Release();

	IBaseFilter *pFileWriter;	
	hr = AddFilterByCLSID(pGraph, CLSID_FileWriter, _T("File writer"), &pFileWriter);
	IFileSinkFilter *pFileSink;
	hr = pFileWriter->QueryInterface(IID_IFileSinkFilter, (void **)&pFileSink);	
	hr = pFileSink->SetFileName(outputFilename.c_str(),	pMediaType);

	hr = pGraph->AddFilter(pFileWriter, _T("File Writer"));
	//Find the input pin on the FileWriter filter
	IPin *pFileWriter_inPin;	
	hr = FindInputPin(pFileWriter, &pFileWriter_inPin);	

	//Now connect the muxer and File Writer
	hr = pGraph->Connect(pMuxer_outPin, pFileWriter_inPin);

	//Add source file(s)
	for (uint16 f = 0; f < inputFilenames.size(); f++)
	{
		hr = pGraph->AddSourceFilter(inputFilenames[f].c_str(), inputFilenames[f].c_str(), &pInputFile);
		//Get ready for the Source filter output pin
		IPin *pInputFile_outPin;
		hr = FindOutputPin(pInputFile, &pInputFile_outPin);
		IPin *pMuxer_inPin;
		hr = FindInputPin(pMatroskaMuxer, &pMuxer_inPin);
		
		hr = pGraph->Connect(pInputFile_outPin, pMuxer_inPin);	
	}
	
	//Now we try to render the graph
	IMediaControl *pControl;
	IMediaEvent *pEvent;
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	hr = pControl->Run();
	
	long evCode = 0;
	while ((evCode != EC_COMPLETE) && (hr != E_ABORT))
		hr = pEvent->WaitForCompletion(1000, &evCode);	

	// Free the DShow interfaces
	pControl->Release();
	pEvent->Release();
	pGraph->Release();
	CoUninitialize();

	return S_OK;
};

HRESULT MatroskaSimpleMuxer::MuxViaMkvmerge()
{
	return E_NOTIMPL;	
};
#endif // USE_MUXER
