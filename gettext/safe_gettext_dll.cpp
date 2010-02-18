/* This file is part of a Windows32 DLL Interface to:
   GNU gettext - internationalization aids
   Copyright (C) 1996, 1998 Free Software Foundation, Inc.

   This file was written by Franco Bez <franco.bez@gmx.de>
	 Changes and improvements by Jory Stone <jcsston@toughguy.net>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "safe_gettext_dll.h" 
// replaces libintl.h 

#include <Shlwapi.h>

extern "C" int  _nl_msg_cat_cntr;

ClsGettext clsGettext; // the global Gettext Object

ClsGettext::ClsGettext(){
	szuTextBuffer = new wchar_t[1025];
	dwTextBufferSize = 1024;

	// Loads Library - and gets the function pointers if possible
	hGettextLib = LoadLibrary(TEXT("gnu_gettext.dll"));
#ifdef MATROSKA_PROP
	if (!hGettextLib) {
		TCHAR dllFilename[MAX_PATH];
		dllFilename[0] = 0;
		GetModuleFileName(GetModuleHandle(TEXT("MatroskaProp")), dllFilename, MAX_PATH/sizeof(TCHAR));

		PathRemoveFileSpec(dllFilename);
		PathAddBackslash(dllFilename);
		StrCat(dllFilename, TEXT("gnu_gettext.dll"));
		hGettextLib = LoadLibrary(dllFilename);
	}
#endif
	if(!hGettextLib){ // DLL not found
		pGettext       =NULL;
		pDGettext      =NULL;
		pDCGettext     =NULL;
		pTextdomain    =NULL;
		pBindtextdomain=NULL;
		pGettext_putenv=NULL;
		return;
	}
	pGettext       =(pfGettext)        GetProcAddress(hGettextLib,"gettext");
	pDGettext      =(pfDGettext)       GetProcAddress(hGettextLib,"dgettext");
	pDCGettext     =(pfDCGettext)      GetProcAddress(hGettextLib,"dcgettext");
	pTextdomain    =(pfTextdomain)     GetProcAddress(hGettextLib,"textdomain");	
	pBindtextdomain=(pfBindtextdomain) GetProcAddress(hGettextLib,"bindtextdomain");
	pGettext_putenv=(pfGettext_putenv) GetProcAddress(hGettextLib,"gettext_putenv");	
}

ClsGettext::~ClsGettext(){
// Free Library if loaded
	if(hGettextLib)
		FreeLibrary(hGettextLib);
	delete [] szuTextBuffer;
}

// the functions below call the matching DLL function if possible ( DLL was found )
char *ClsGettext::m_Gettext(const char *szMsgId){
	if(pGettext) // Function pointer loaded
		return (*pGettext)(szMsgId);
	return (char *) szMsgId; // do nothing
}

// the functions below call the matching DLL function if possible ( DLL was found )
wchar_t *ClsGettext::m_GettextW(const char *szMsgId){
	const char *translatedszMsg = NULL;
	if (pGettext) // Function pointer loaded
		translatedszMsg = (*pGettext)(szMsgId);
	else
		translatedszMsg = szMsgId;

	int ret = ERROR_INSUFFICIENT_BUFFER;
	while (ret == ERROR_INSUFFICIENT_BUFFER) {
		// Convert the returned string to Unicode
		ret = MultiByteToWideChar(CP_UTF8, 0, translatedszMsg, -1, szuTextBuffer, dwTextBufferSize);
		if (ret == ERROR_INVALID_FLAGS || ret == ERROR_INVALID_FLAGS || ret == ERROR_NO_UNICODE_TRANSLATION)
			return L"Failed to get translation";

		if (ret == ERROR_INSUFFICIENT_BUFFER) {
			// Increase the size of our unicode buffer
			dwTextBufferSize += 1024;
			delete [] szuTextBuffer;
			szuTextBuffer = new wchar_t[dwTextBufferSize+1];
		} else {
			break;
		}
	}

	return (wchar_t *)szuTextBuffer;
}

char *ClsGettext::m_dGettext(const char *szDomain,const char *szMsgId){
	if(pDGettext) // Function pointer loaded
		return (*pDGettext)(szDomain,szMsgId);
	return (char *) szMsgId; // do nothing
}

char *ClsGettext::m_dcGettext(const char *szDomain,const char *szMsgId,int iCategory){
	if(pDCGettext) // Function pointer loaded
		return (*pDCGettext)(szDomain,szMsgId,iCategory);
	return (char *) szMsgId; // do nothing
}

char *ClsGettext::m_Textdomain(const char *szDomain){
	if(pTextdomain) // Function pointer loaded
		return (*pTextdomain)(szDomain);
	return (char *) szDomain; // do nothing
}

char *ClsGettext::m_Bindtextdomain(const char *szDomain,const char *szDirectory){
	if(pBindtextdomain) // Function pointer loaded
		return (*pBindtextdomain)(szDomain,szDirectory);
	return (char *) szDirectory; // do nothing
}

int ClsGettext::m_Gettext_putenv(const char *szEnvStr){
	if(pGettext_putenv) // Function pointer loaded
		return (*pGettext_putenv)(szEnvStr);
	return 0; // do nothing
}

bool ClsGettext::m_Gettext_is_loaded(){
	if(hGettextLib) // dll loaded ?
		return true;
	return false;
}

void ClsGettext::UnloadGettext() {
	if (hGettextLib) {
		FreeLibrary(hGettextLib);
		hGettextLib = NULL;
	}
}

// Now the safe gettext dll functions
extern "C"{

void UnloadGettext(void) {
	clsGettext.UnloadGettext();
}
char *gettext(const char *szMsgId){
	return clsGettext.m_Gettext(szMsgId);
}

wchar_t *gettextW(const char *szMsgId){
	return clsGettext.m_GettextW(szMsgId);
}

char *dgettext(const char *szDomain,const char *szMsgId){
	return clsGettext.m_dGettext(szDomain,szMsgId);
}

char *dcgettext(const char *szDomain,const char *szMsgId,int iCategory){
	return clsGettext.m_dcGettext(szDomain,szMsgId,iCategory);
}

char *textdomain(const char *szDomain){
	return clsGettext.m_Textdomain(szDomain);
}

char *bindtextdomain(const char *szDomain,const char *szDirectory){
	return clsGettext.m_Bindtextdomain(szDomain,szDirectory);
}

int gettext_putenv(const char *envstring){	
	return clsGettext.m_Gettext_putenv(envstring);
}

bool gettext_is_loaded(){
	return clsGettext.m_Gettext_is_loaded();
}

} // extern "C"
