/**
	*  Part of The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
	*
	*  MatroskaReg.h
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
    \file MatroskaReg.h
		\version \$Id: MatroskaReg.h,v 1.4 2004/03/07 08:08:48 jcsston Exp $
    \brief Some settings saving code
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#ifndef _MATROSKA_REG_H_
#define _MATROSKA_REG_H_

#include <windows.h>
#include <string.h>
#include <tchar.h>
#include "JString.h"

/// Gets a uint32 value from the Key under HKEY_CURRENT_USER\\Software\\CoreCodec\\The Core Media Player\\Matroska Info CDL
/// \param value_key The name of the subkey you wish to get
/// \param default_value If this key doesn't exist this value will be returned to you
/// \return The uint32 value of the requested key, default_value is returned if the key doesn't exist
DWORD MatroskaCDL_GetRegistryValue(TCHAR *value_key, DWORD default_value);
DWORD MatroskaShellExt_GetRegistryValue(TCHAR *value_key, DWORD default_value);
/// Sets a uint32 value to a Key under HKEY_CURRENT_USER\\Software\\CoreCodec\\The Core Media Player\\Matroska Info CDL
/// \param value_key The name of the subkey you wish to save
/// \param the_value The value to write to the Registry key
void MatroskaCDL_SetRegistryValue(TCHAR *value_key, DWORD the_value);
void MatroskaShellExt_SetRegistryValue(TCHAR *value_key, DWORD the_value);

JString MatroskaShellExt_GetRegistryValueStr(TCHAR *value_key, JString default_value);
void MatroskaShellExt_SetRegistryValueStr(TCHAR *value_key, JString the_value);

class CRegistryRestore {
public:

	void AddKey(HKEY root, LPCTSTR keyLocation, LPVOID data, DWORD size);

	/// Preserve all changes made since last Preserve() call
	void Preserve(HKEY root, LPCTSTR keyLocation);
	/// Reverse all changes made, supply same root and keyLocation as previously given to Preserve() call
	void Reverse(HKEY root, LPCTSTR keyLocation);
	
protected:

};

#endif // _MATROSKA_REG_H_
