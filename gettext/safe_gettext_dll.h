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

/*
   if you choose to use this interface 
   it is tried to dynamically load gnu_gettext.dll
   if this fails your program will NOT issue a message, but will continue 
   normally, just no translation will take place
   
   if you use C++ just include safe_gettext_dll.cpp in your project, and use
   #define USE_SAFE_GETTEXT_DLL before  #include <libintl.h>
   
   if you use plain C you can use this interface as well,  use
   #define USE_SAFE_GETTEXT_DLL before  #include <libintl.h>,
   compile safe_gettext_dll.cpp with C++ and then link your 
   application to safe_gettext_dll.o and libstdc++
*/ 

#ifndef CLS_GETTEXT_H_INCLUDED
#define CLS_GETTEXT_H_INCLUDED

#include <windows.h>
#include <string>
#include <string.h>

#if defined(__cplusplus)
extern "C"{ // pointer types to gettext DLL functions

  typedef char *(*pfGettext)(const char *); 			// pfGettext;
  typedef char *(*pfDGettext)(const char *,const char *);	// pfDGettext;
  typedef char *(*pfDCGettext)(const char *,const char *, int);	// pfDCGettext;
  typedef char *(*pfTextdomain)(const char *);			// pfTextdomain;
  typedef char *(*pfBindtextdomain)(const char *,const char *);	// pfBindtextdomain;
  // calling _putenv from within the DLL
  typedef int (*pfGettext_putenv)(const char *);                // pfGettext_putenv 

#endif // defined(__cplusplus)
  
  // The safe gettext functions	
  char *gettext(const char *szMsgId);
	wchar_t *gettextW(const char *szMsgId);
  char *dgettext(const char *szDomain,const char *szMsgId);
  char *dcgettext(const char *szDomain,const char *szMsgId,int iCategory);
  char *textdomain(const char *szDomain);
  char *bindtextdomain(const char *szDomain,const char *szDirectory);
  // calling _putenv from within the DLL
  int gettext_putenv(const char *envstring);
	// Extra
	bool gettext_is_loaded();

#if defined(__cplusplus)
} // extern "C"

// the dynamic Library class
class ClsGettext{
private:
	HINSTANCE        hGettextLib;		// DLL Instance Handle
	pfGettext        pGettext;		// pointers to
	pfDGettext       pDGettext;		// gnu_gettext.dll
	pfDCGettext      pDCGettext;		// functions
	pfTextdomain     pTextdomain;
	pfBindtextdomain pBindtextdomain;
	pfGettext_putenv pGettext_putenv;
	wchar_t *szuTextBuffer;
	DWORD dwTextBufferSize;
public:
	ClsGettext();	// Loads Library - and gets the function pointers if possible
	~ClsGettext();	// Free Library
	// the functions below call the matching DLL function if possible ( DLL was found )
	char *m_Gettext(const char *szMsgId); 
	wchar_t *m_GettextW(const char *szMsgId);
	char *m_dGettext(const char *szDomain,const char *szMsgId); 
	char *m_dcGettext(const char *szDomain,const char *szMsgId,int iCategory); 
	char *m_Textdomain(const char *szDomain); 
	char *m_Bindtextdomain(const char *szDomain,const char *szDirectory); 
	int  m_Gettext_putenv(const char *szEnvStr);
	bool m_Gettext_is_loaded();
	void UnloadGettext();
};

#endif // defined(__cplusplus)

#endif // CLS_GETTEXT_H_INCLUDED
