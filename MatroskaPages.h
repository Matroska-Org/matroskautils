/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MatroskaPages.h
 *
 *  Copyright (C) Jory Stone - June 2003
 *  Copyright (C) John Cannon - 2003
 *
 *  CDL API Copyright (C) Blacksun & Toff - 2003
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
    \file MatroskaPages.h
		\version \$Id: MatroskaPages.h,v 1.36 2004/03/07 08:08:48 jcsston Exp $
    \brief This is all the GUI code for the CDL and Shell Extension
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#define MATROSKA_CDL_APP_NAME "The Matroska Infomation CDL v1.8"
#define MATROSKA_SHELL_EXT_APP_NAME "Matroska Shell Extension v2.8"

#ifndef _MATROSKA_PAGES_H_
#define _MATROSKA_PAGES_H_

#define MANIFEST_RESOURCE_ID 2

#include "MatroskaUtilsDebug.h"
#include "AviTagReader.h"
#include "MP3TagReader.h"
#include "MatroskaReg.h"
#include "MatroskaUtils.h"

#include "CFireRoutine.h"
#include "resource.h"

#include <windows.h>
#include <commctrl.h>

#define WM_LOADTRANSLATIONS WM_USER+0x12

using namespace MatroskaUtilsNamespace;

class MatroskaPages {
public:
	MatroskaPages();
	~MatroskaPages();
	uint16 AddRef();
	uint16 Release();

	MatroskaInfoParser *parser;  // The main workhorse of the CDL
	bool g_bUseUnicodeWinAPI;    // Flag used to use Unicode or ANSI
	
	HINSTANCE g_hAppInst; // Application instance handle (needed to create dialog)
	HWND g_hTreeDialog;
	HWND g_hInfoDialog;
	HWND g_hTagDialog;
	HWND g_hSimpleTagDialog;
	HWND g_hAttachmentDialog;
	HWND g_hChapterDialog;
#ifdef USING_CXIMAGE
	CxImage *attachedImage;
#endif
	TCHAR szFile[MAX_MATROSKA_PATH];

protected:
	uint16 m_RefCount;
};

#ifdef MATROSKA_PROP
#pragma pack(push, pp1)
#pragma pack(1)
typedef struct tagMoreData
{
	PROPSHEETPAGE psp;
	MatroskaPages *pObjPageData;
} MoreData;
#pragma pack(pop, pp1)
#endif

#define fireWidth 322
#define fireHeight 64

class MatroskaAboutDialog {
public:
	MatroskaAboutDialog();
	/// About dialog text string
	JString about_txt;

#ifdef USING_CXIMAGE
	~MatroskaAboutDialog();
	void RenderFire(HWND hAboutDialog);
	void PaintFire(HWND hAboutDialog);

	CFireRoutine myFire;
	RECT fireArea;
	DWORD *myFireBitmap;
	CxImage myFireImage;
	CxImage myTextImage;
	//CxImage *fireLayer;
#endif // USING_CXIMAGE
};

BOOL CALLBACK AboutDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK TreeDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ConfigurationDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK IconDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ProgressDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_DisplaySize(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_TrackLanguage(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MediaPropProc_Attachments(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_AttachedFile(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_Chapters(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_Tag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_SimpleTag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_AddTag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_AddSimpleTag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_AddUID(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MediaPropProc_BitrateDisplay(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

//the parsing thread procedure
void ParseFileMT(MatroskaPages *pageData, HWND parentDialog = NULL);
void __cdecl MatroskaParserThread(void *pages);

//Various GUI rountines
MatroskaTrackInfo *GetMainSelectedTrack(HWND hwndDlg, MatroskaInfoParser *parser);
int DisplayMainTrackList(HWND hwndDlg, MatroskaInfoParser *parser);
int DisplayBasicTrackInfo(HWND hwndDlg, MatroskaInfoParser *parser);
int DisplayAdvancedTrackInfo(HWND hwndDlg, MatroskaInfoParser *parser);
int DisplayTagEntryList(HWND hwndDlg, MatroskaInfoParser *parser);
int DisplayTagUIDList(HWND hwndDlg, MatroskaTagInfo *tag);
int DisplaySimpleTagTrackList(HWND hwndDlg, MatroskaInfoParser *parser);
int DisplayAttachmentList(HWND hwndDlg, MatroskaInfoParser *parser);
int GetEditControlTextLength(HWND theDialog, int edit_control);
SIZE SmartResize(long sourceX, long sourceY, long maxX, long maxY);
int CALLBACK ListViewCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
WORD AddTooltip(HWND hwndTooltip, HWND hwndClient, LPTSTR strText);
MatroskaTagInfo *GetSelectedTagEntry(HWND hTagDlg);
void MediaPropProc_Tag_DeleteTag(HWND hTagDlg);
void MediaPropProc_Tag_TagEntryChange(HWND hwndDlg);
void MediaPropProc_Tag_TagTextChange(HWND hwndDlg, MatroskaPages *pageData);
void MediaPropProc_Tag_TagListChange(HWND hwndDlg);
void MediaPropProc_Tag_TagFieldChange(HWND hwndDlg, MatroskaPages *pageData);
void MediaPropProc_Tag_TagTypeChange(HWND hwndDlg);
UTFstring GetSelectedListItem(HWND hwndDlg, int ctrl);
void DisplayChapterList(HWND hwndDlg, MatroskaInfoParser *parser);
void DisplayChapterAtom_TreeView(HWND treeCtrl, HTREEITEM parent, MatroskaChapterEntry *chapter, std::wstring chapterTitle);

// Crash test function
void CrashMe();

#endif // _MATROSKA_PAGES_H_

