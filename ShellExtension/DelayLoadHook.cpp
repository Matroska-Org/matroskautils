/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  DelayLoadHook.cpp
 *
 *  Copyright (C) Jory Stone - February 2004
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
    \file DelayLoadHook.cpp
		\version \$Id: DelayLoadHook.cpp,v 1.1 2004/02/18 08:45:50 jcsston Exp $
    \brief Delay Loaded DLL Hook Function
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#include <windows.h>
#include <Shlwapi.h>
#include <DelayImp.h>

FARPROC WINAPI delayHook(unsigned dliNotify, PDelayLoadInfo pdli)
{
	switch (dliNotify) {
				case dliStartProcessing :

					// If you want to return control to the helper, return 0.
					// Otherwise, return a pointer to a FARPROC helper function
					// that will be used instead, thereby bypassing the rest 
					// of the helper.

					break;

				case dliNotePreLoadLibrary :
				{
					// If you want to return control to the helper, return 0.
					// Otherwise, return your own HMODULE to be used by the 
					// helper instead of having it call LoadLibrary itself.					
					HMODULE hLib = LoadLibraryA(pdli->szDll);
					if (!hLib) {
						char dllFilename[MAX_PATH];
						dllFilename[0] = 0;
#ifdef MATROSKA_PROP
						GetModuleFileNameA(GetModuleHandle(TEXT("MatroskaProp")), dllFilename, MAX_PATH);
#else
						GetModuleFileNameA(GetModuleHandle(TEXT("MatroskaCDL")), dllFilename, MAX_PATH);
#endif
						PathRemoveFileSpecA(dllFilename);
						PathAddBackslashA(dllFilename);
						StrCatA(dllFilename, pdli->szDll);
						hLib = LoadLibraryA(dllFilename);
					}

					return (FARPROC)hLib;

					break;
				}
				case dliNotePreGetProcAddress :

					// If you want to return control to the helper, return 0.
					// If you choose you may supply your own FARPROC function 
					// address and bypass the helper's call to GetProcAddress.

					break;

				case dliFailLoadLib : 
				{
					// LoadLibrary failed.
					// If you don't want to handle this failure yourself, return 0.
					// In this case the helper will raise an exception 
					// (ERROR_MOD_NOT_FOUND) and exit.
					// If you want to handle the failure by loading an alternate 
					// DLL (for example), then return the HMODULE for 
					// the alternate DLL. The helper will continue execution with 
					// this alternate DLL and attempt to find the
					// requested entrypoint via GetProcAddress.
					
					char buffer[1024];
					StrCpyA(buffer, "MatroskaProp failed to load \"");
					StrCatA(buffer, pdli->szDll);
					StrCatA(buffer, "\"."
					"\nTry reinstalling MatroskaProp."
					"\nIf the problem persists, please contact me at vbman@toughguy.net.");
					MessageBoxA(NULL, buffer, "Dll Load Failure", 0);

					break;
				}
				case dliFailGetProc :

					// GetProcAddress failed.
					// If you don't want to handle this failure yourself, return 0.
					// In this case the helper will raise an exception 
					// (ERROR_PROC_NOT_FOUND) and exit.
					// If you choose you may handle the failure by returning 
					// an alternate FARPROC function address.

					char buffer[1024];
					StrCpyA(buffer, "Failed to get function \"");
					StrCatA(buffer, pdli->dlp.szProcName);
					StrCatA(buffer, "\" from \"");
					char dllFilename[MAX_PATH];
					dllFilename[0] = 0;
					GetModuleFileNameA(pdli->hmodCur, dllFilename, MAX_PATH);
					StrCatA(buffer, dllFilename);
					StrCatA(buffer, "\"."
					"\nTry removing the .dll and/or reinstalling MatroskaProp."
					"\nIf the problem persists please contact me at vbman@toughguy.net.");
					MessageBoxA(NULL, buffer, "Dll Function Load Failure", 0);

					break;

				case dliNoteEndProcessing : 

					// This notification is called after all processing is done. 
					// There is no opportunity for modifying the helper's behavior
					// at this point except by longjmp()/throw()/RaiseException. 
					// No return value is processed.

					break;

				default :
					return NULL;
	}

	return NULL;
}

PfnDliHook __pfnDliNotifyHook2 = delayHook;
PfnDliHook __pfnDliFailureHook2 = delayHook;