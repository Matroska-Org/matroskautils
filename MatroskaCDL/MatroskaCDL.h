/*
 *  The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
 *
 *  MatroskaCDL.h
 *
 *  Copyright (C) Jory Stone - June 2003
 *  Copyright (C) John Cannon - 2003
 *
 *  CDL API
 *   Copyright (C) Ludovic 'BlackSun' Vialle, CoreCodec Lead-developer.
 *   Copyright (C) C/C++ Conversion by Christophe 'Toff' PARIS
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
    \file MatroskaCDL.h
		\version \$Id: MatroskaCDL.h,v 1.8 2003/11/19 08:13:47 jcsston Exp $
    \brief The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
		\author Jory Stone     <jcsston @ toughguy.net>
		\author John Cannon    <spyder482 @ yahoo.com>
		
		Inspired by ;) John Cannon (spyder482@yahoo.com) (c) 2003
		Thanks to Toff & Blacksun for the TCMP Plugin API
*/

#if !defined(AFX_MATROSKACDL_H__8CD7AF71_FBA1_40FE_8E19_390AE5021AD1__INCLUDED_)
#define AFX_MATROSKACDL_H__8CD7AF71_FBA1_40FE_8E19_390AE5021AD1__INCLUDED_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//Include the TCMP CDL API
#include "CDLApi.h"

#include "MatroskaPages.h"

int CDL_API_CALL CDL_Load(int version);
void CDL_API_CALL CDL_Config(void);
void CDL_API_CALL CDL_Unload(void);
void CDL_API_CALL CDL_EventHandler(int Event, char* Param1, int Param2);
char* CDL_API_CALL CDL_MediaPropExec(char *Filename);
void CDL_API_CALL CDL_ReadTags(char *Filename, CDL_Metadata &MetaData);
void CDL_API_CALL CDL_WriteTags(char *Filename, CDL_Metadata Metadata);

#define CDL_NAME "Matroska Info"
#define CDL_VERSION "1.7"

MatroskaPages *global_pagedata = NULL;

// ----------------------------------------------------------------------------

CDL_PluginHeader MyCDL =
{
    CDL_VERSION,            // Our version number
    CDL_NAME,				// Name of our plugin
    CDL_Load,               // Point to our load function
    CDL_Config,             // Point to our config procedure
    CDL_Unload,             // Point to our unload procedure
    NULL,
	CORE_NOFLAG
	| CORE_EVENTHANDLER 
	| CORE_MEDIAPROPEXEC
	| CORE_READTAGS
	| CORE_WRITETAGS
	,
    NULL,
    NULL,
    CDL_MediaPropExec,
		CDL_EventHandler,
		CDL_ReadTags,
		CDL_WriteTags
};

PCDL_CoreHeader pCoreCDL = NULL;

// ----------------------------------------------------------------------------

#endif // !defined(AFX_MATROSKACDL_H__8CD7AF71_FBA1_40FE_8E19_390AE5021AD1__INCLUDED_)
