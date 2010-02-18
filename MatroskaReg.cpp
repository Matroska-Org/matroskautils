/**
	*  Part of The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
	*
	*  MatroskaReg.cpp
	*
	*  Copyright (C) Jory Stone - June 2003
	*
	*
	*  The TCMP Matroska CDL is free software; you can redistribute it and/or modify
	*  it under the terms of the GNU General Public License as published by
	*  the Free Software Foundation; either version 2, or (at your option)
	*  any later version.
	*
	*  The TCMP Matroska CDL is distributed in the hope that it will be useful,
	*  but WITHOUT ANY WARRANTY; without even the implied warranty of
	*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	*  GNU General Public License for more details.
	*
	*  You should have received a copy of the GNU General Public License
	*  along with The TCMP Matroska CDL; see the file COPYING.  If not, write to
	*  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
	*
*/

/*!
    \file MatroskaReg.cpp
		\version \$Id: MatroskaReg.cpp,v 1.3 2003/11/13 22:33:32 jcsston Exp $
    \brief Some settings saving code
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#include "MatroskaReg.h"

DWORD MatroskaCDL_GetRegistryValue(TCHAR *value_key, DWORD default_value)
{	
	TCHAR *reg_key = _T("Software\\CoreCodec\\The Core Media Player\\Matroska Info CDL\\");
	DWORD ret_value = default_value;
	HKEY key_handle = NULL;
	DWORD lpType = NULL;
	DWORD state = 0;

	RegCreateKeyEx(HKEY_CURRENT_USER, reg_key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
	//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, temp, 0, reg_key, 1024, NULL); 
	if(state == 2)
	{
		DWORD size = sizeof(ret_value);
		RegQueryValueEx(key_handle, value_key, 0, &lpType, (BYTE*)&ret_value, &size);
	}
	RegCloseKey(key_handle);
	return ret_value;
}

void MatroskaCDL_SetRegistryValue(TCHAR *value_key, DWORD the_value)
{
	TCHAR *reg_key = _T("Software\\CoreCodec\\The Core Media Player\\Matroska Info CDL\\");
	HKEY key_handle = NULL;
	DWORD lpType = NULL;
	DWORD state = 0;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), 0,1};

	RegCreateKeyEx(HKEY_CURRENT_USER, reg_key, 0, _T(""), 0, KEY_WRITE, &sa, &key_handle, &state);

	DWORD size = sizeof(the_value);
	RegSetValueEx(key_handle, value_key, 0, REG_DWORD, (CONST BYTE*)&the_value, size);
	//char *err_key = new char[1024];
	//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, temp, 0, err_key, 1024, NULL); 
	RegCloseKey(key_handle);
};

DWORD MatroskaShellExt_GetRegistryValue(TCHAR *value_key, DWORD default_value)
{	
	TCHAR *reg_key = _T("Software\\MatroskaProp\\");
	DWORD ret_value = default_value;
	HKEY key_handle = NULL;
	DWORD lpType = NULL;
	DWORD state = 0;
	DWORD size = sizeof(ret_value);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
	//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, temp, 0, reg_key, 1024, NULL); 
	if(state == REG_OPENED_EXISTING_KEY)
		RegQueryValueEx(key_handle, value_key, 0, &lpType, (BYTE*)&ret_value, &size);	
	else if (state == REG_CREATED_NEW_KEY)
		//We write the default value
		RegSetValueEx(key_handle, value_key, 0, REG_DWORD, (CONST BYTE*)&ret_value, size);

	RegCloseKey(key_handle);
	return ret_value;
}

void MatroskaShellExt_SetRegistryValue(TCHAR *value_key, DWORD the_value)
{
	TCHAR *reg_key = _T("SOFTWARE\\MatroskaProp\\");
	HKEY key_handle = NULL;
	DWORD lpType = NULL;
	DWORD state = 0;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), 0,1};

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_key, 0, _T(""), 0, KEY_WRITE, &sa, &key_handle, &state);

	DWORD size = sizeof(the_value);
	RegSetValueEx(key_handle, value_key, 0, REG_DWORD, (CONST BYTE*)&the_value, size);
	//char *err_key = new char[1024];
	//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, temp, 0, err_key, 1024, NULL); 
	RegCloseKey(key_handle);
};

JString MatroskaShellExt_GetRegistryValueStr(TCHAR *value_key, JString default_value)
{	
	TCHAR *reg_key = _T("Software\\MatroskaProp\\");
	JString ret_value = default_value;
	HKEY key_handle = NULL;
	DWORD lpType = NULL;
	DWORD state = 0;
	TCHAR key_text[MAX_PATH+2];
	DWORD size = MAX_PATH;

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);
	//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, temp, 0, reg_key, 1024, NULL); 		
	if(state == REG_OPENED_EXISTING_KEY) {
		RegQueryValueEx(key_handle, value_key, 0, &lpType, (BYTE*)key_text, &size);	
		ret_value = key_text;
	} else if (state == REG_CREATED_NEW_KEY) {
		//We write the default value
		RegSetValueEx(key_handle, value_key, 0, REG_SZ, (CONST BYTE*)&ret_value, (ret_value.length()+1)*sizeof(TCHAR));
	}

	RegCloseKey(key_handle);
	return ret_value;
}

void MatroskaShellExt_SetRegistryValueStr(TCHAR *value_key, JString the_value)
{
	TCHAR *reg_key = _T("SOFTWARE\\MatroskaProp\\");
	HKEY key_handle = NULL;
	DWORD lpType = NULL;
	DWORD state = 0;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), 0,1};

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_key, 0, _T(""), 0, KEY_WRITE, &sa, &key_handle, &state);

	DWORD size = (the_value.length()+1)*sizeof(TCHAR);
	RegSetValueEx(key_handle, value_key, 0, REG_SZ, (CONST BYTE*)the_value.t_str(), size);
	//char *err_key = new char[1024];
	//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, temp, 0, err_key, 1024, NULL); 
	RegCloseKey(key_handle);
};
