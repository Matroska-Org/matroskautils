/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  mdump.h
 *
 *  Copyright (C) Jory Stone - 2003
 *
 *	Based from sample code from http://www.eptacom.net/pubblicazioni/pub_eng/except.html
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
    \file mdump.h
		\version \$Id: mdump.h,v 1.11 2004/03/08 07:56:51 jcsston Exp $
    \brief Crash Catcher
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#ifndef _MDUMP_H_
#define _MDUMP_H_

#include <stdio.h>
#include <windows.h>
#include <TCHAR.h>
#include <assert.h>
#include <Shlwapi.h>
#include <string>
#include <vector>
#include <string.h>
#include "resource.h"
#include "ZipArchive.h"

#if _MSC_VER < 1300
#define DECLSPEC_DEPRECATED
// VC6: change this path to your Platform SDK headers
#include "M:\\dev7\\vs\\devtools\\common\\win32sdk\\include\\dbghelp.h"			// must be XP version of file
#else
// VC7: ships with updated headers
#include "dbghelp.h"
#endif

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

struct CrashInfo {
	std::basic_string<TCHAR> title;
	std::basic_string<TCHAR> report;
	std::basic_string<TCHAR> user_text;
	std::basic_string<TCHAR> filename;
};

#ifdef USING_MAPPING_FILE	
struct MappingFileEntry {
	DWORD functionAddress;
	std::string functionName;
};
#endif

class CrashCatcher
{
public:
	CrashCatcher(const TCHAR *szAppName);
	~CrashCatcher();

protected:
	static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
	static std::basic_string<TCHAR> ProcessExceptionRecord(EXCEPTION_RECORD *pExceptionRecord);	

	static std::basic_string<TCHAR> DumpRegisters(CONTEXT *pContext);
	static std::basic_string<TCHAR> StackDump(DWORD EBP, DWORD EIP);		
	static LPCTSTR GetAddressInfo(const BYTE* caller, HINSTANCE hInstance);
#ifdef USING_MAPPING_FILE	
	static void LoadMapping(std::vector<MappingFileEntry> &targetMap);
	static std::basic_string<TCHAR> LookupMapping(std::vector<MappingFileEntry> &sourceMap, void *address);
#endif
	static std::basic_string<TCHAR> GetSystemInfomation();
	static LPCTSTR GetDllVersion(LPCTSTR lpszDllName);
	
	static BOOL CALLBACK CrashDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

	// A 'scratch' string buffer for rendering strings
	static TCHAR szScratchPad[256];
	static const TCHAR *m_szAppName;	
	LPTOP_LEVEL_EXCEPTION_FILTER m_prevFilter;
};

#endif // _MDUMP_H_
