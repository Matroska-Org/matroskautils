/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MatroskaPagesTagChapter.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
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
    \file MatroskaPagesTagChapter.cpp
		\version \$Id: MatroskaPagesChapter.cpp,v 1.4 2004/03/20 05:08:30 jcsston Exp $
    \brief This is the GUI code for the Chapter Page
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#include "MatroskaPages.h"

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

using namespace MatroskaUtilsNamespace;

extern long g_MyBuildNumber;

void DisplayChapterAtom_TreeView(HWND treeCtrl, HTREEITEM parent, MatroskaChapterEntry *chapter, std::wstring chapterTitle)
{
	assert(chapter != NULL);

	TVINSERTSTRUCT treeItemInfo;
	memset(&treeItemInfo, 0, sizeof(TVINSERTSTRUCT));
	treeItemInfo.hInsertAfter = TVI_LAST;
	treeItemInfo.hParent = parent;
	treeItemInfo.item.mask = TVIF_TEXT;
	treeItemInfo.item.pszText = (LPWSTR)chapterTitle.c_str();

	HTREEITEM chapterItem = TreeView_InsertItem(treeCtrl, &treeItemInfo);

	treeItemInfo.hParent = chapterItem;
	// Insert UID
	{
		SmartStringFormat wBuf;
		wBuf << _W("UID");
		wBuf << L": ";
		wBuf << chapter->chapterUID;
		std::wstring wStr = wBuf.str();
		treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
		TreeView_InsertItem(treeCtrl, &treeItemInfo);
	}
	// Insert Start, End Times
	{
		SmartStringFormat wBuf;
		wBuf << _W("Start Time");
		wBuf << L": ";
		wBuf << chapter->GetTimeStartStr().c_str();
		std::wstring wStr = wBuf.str();
		treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
		TreeView_InsertItem(treeCtrl, &treeItemInfo);
	}	
	if (chapter->timeEnd != 0) {
		SmartStringFormat wBuf;
		wBuf << _W("End Time");
		wBuf << L": ";
		wBuf << chapter->GetTimeEndStr().c_str();
		std::wstring wStr = wBuf.str();
		treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
		TreeView_InsertItem(treeCtrl, &treeItemInfo);
	}	

	// Insert Display Strings
	for (size_t d = 0; d < chapter->display.size(); d++) {
		MatroskaChapterDisplayInfo &displayInfo = chapter->display.at(d);
		
		treeItemInfo.hParent = chapterItem;
		treeItemInfo.item.pszText = _W("Display String");
		treeItemInfo.hParent = TreeView_InsertItem(treeCtrl, &treeItemInfo);

		if (displayInfo.string.length() > 0) {
			SmartStringFormat wBuf;
			wBuf << _W("String");
			wBuf << L": ";
			wBuf << displayInfo.string.c_str();
			std::wstring wStr = wBuf.str();
			treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
			TreeView_InsertItem(treeCtrl, &treeItemInfo);
		}
		if (displayInfo.lang.length() > 0) {
			SmartStringFormat wBuf;
			wBuf << _W("Language");
			wBuf << L": ";
			wBuf << displayInfo.lang.c_str();
			std::wstring wStr = wBuf.str();
			treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
			TreeView_InsertItem(treeCtrl, &treeItemInfo);
		}
		if (displayInfo.country.length() > 0) {			
			SmartStringFormat wBuf;
			wBuf << _W("Country");
			wBuf << L": ";
			wBuf << displayInfo.country.c_str();
			std::wstring wStr = wBuf.str();
			treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
			TreeView_InsertItem(treeCtrl, &treeItemInfo);
		}
	}	
	treeItemInfo.hParent = chapterItem;

	// Insert Track UID
	if (chapter->tracks.size() > 0) {
		treeItemInfo.item.pszText = _W("Track UID(s))");
		treeItemInfo.hParent = TreeView_InsertItem(treeCtrl, &treeItemInfo);
	}
	for (size_t t = 0; t < chapter->tracks.size(); t++) {
		SmartStringFormat wBuf;
		wBuf << _W("Track UID");
		wBuf << L": ";
		wBuf << (uint64)chapter->tracks.at(t);
		std::wstring wStr = wBuf.str();
		treeItemInfo.item.pszText = (LPWSTR)wStr.c_str();
		TreeView_InsertItem(treeCtrl, &treeItemInfo);
	}
	treeItemInfo.hParent = chapterItem;

	for (size_t c = 0; c < chapter->children.size(); c++) {
		MatroskaChapterEntry *childChapter = chapter->children.at(c);
		SmartStringFormat wBuf;
		wBuf << chapterTitle.c_str();
		wBuf << L"-";
		wBuf << (c+1);
		DisplayChapterAtom_TreeView(treeCtrl, chapterItem, childChapter, wBuf.str());
	}
}

void DisplayChapterList(HWND hwndDlg, MatroskaInfoParser *parser)
{
	assert(parser != NULL);
	CRASH_PROTECT_START;

	HWND treeCtrl = GetDlgItem(hwndDlg, IDC_TREE_CHAPTER);

	// Clear existing items
	TreeView_DeleteAllItems(treeCtrl);

	TVINSERTSTRUCT treeItemInfo;
	memset(&treeItemInfo, 0, sizeof(TVINSERTSTRUCT));
	treeItemInfo.hInsertAfter = TVI_ROOT;
	treeItemInfo.hParent = TVI_ROOT;
	treeItemInfo.item.mask = TVIF_TEXT;
	treeItemInfo.item.pszText = _W("Chapters");

	HTREEITEM rootItem = TreeView_InsertItem(treeCtrl, &treeItemInfo);			

	for (size_t i = 0; i < parser->chapters.size(); i++) {
		MatroskaChapterEntry *chapter = parser->chapters.at(i);				

		SmartStringFormat wBuf;
		wBuf << _W("Chapter");
		wBuf << L" ";
		wBuf << (i+1);

		DisplayChapterAtom_TreeView(treeCtrl, rootItem, chapter, wBuf.str());
	}

	TreeView_Expand(treeCtrl, rootItem, TVE_EXPAND);

	CRASH_PROTECT_END;
}

BOOL CALLBACK MediaPropProc_Chapters(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
		{
			ODS("Displaying Chapter Tab");

			INITCOMMONCONTROLSEX common;
			common.dwICC = ICC_TREEVIEW_CLASSES;
			common.dwSize = sizeof(common);
			InitCommonControlsEx(&common);

			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
			pageData = (MatroskaPages *)lParam;

#ifdef MATROSKA_PROP
			MoreData *pm;
			// When the shell creates a dialog box for a property sheet page,
      // it passes the pointer to the PROPSHEETPAGE data structure as
      // lParam. The dialog procedures of extensions typically store it
      // in the DWL_USER of the dialog box window.
      SetWindowLong(hwndDlg, DWL_USER, lParam);
      pm = (MoreData *)lParam;
      pageData = pm->pObjPageData;
			pageData->AddRef();

			//Store the Matroska Infomation dialog handle
			pageData->g_hChapterDialog = hwndDlg;
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);

			if ((pageData->parser == NULL) || (pageData->parser->chapters.size() == 0))
			{
				delete pageData->parser;
				pageData->parser = new MatroskaInfoParser(pm->pObjPageData->szFile);
				//pageData->parser->m_parseSeekHead = true;
				//pageData->parser->m_parseAttachments = true;
				//pageData->parser->m_parseTags = true;
				pageData->parser->ParseFile();
			}

#endif

			SetDlgItemText(hwndDlg, IDC_STATIC_CHAPTER_BORDER, _W("Matroska Chapter Infomation"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_EXPORT_CHAPTERS_XML, _W("Export as XML"));

			DisplayChapterList(hwndDlg, pageData->parser);

			break;
		}
		case WM_DESTROY:
		{
#ifdef MATROSKA_PROP
			pageData->Release();
#endif // MATROSKA_PROP
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON_EXPORT_CHAPTERS_XML:
				{
					ODS("IDC_BUTTON_EXPORT_CHAPTERS_XML button pressed");
#ifdef USING_TINYXML
					JString xmlFilename;
					//Get the XML target filename
					OPENFILENAME saveFileAs;
					TCHAR fileName[1024];
					memset(&saveFileAs, 0, sizeof(OPENFILENAME));
					memset(fileName, 0, sizeof(fileName));
					saveFileAs.lStructSize =  sizeof(OPENFILENAME);
					saveFileAs.hwndOwner = hwndDlg;
					saveFileAs.lpstrFile = fileName;
					saveFileAs.nMaxFile	= sizeof(fileName);
					saveFileAs.lpstrFilter = _T("XML Files(*.xml)\0 *.xml\0\0");
					saveFileAs.lpstrDefExt = _T("xml");
					saveFileAs.lpstrTitle	= _W("Export As XML To...");
					saveFileAs.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
					if (GetSaveFileName(&saveFileAs)) {
						xmlFilename = fileName;
						int ret = pageData->parser->SaveChaptersToXMLFile(xmlFilename);
						if (ret != 0)
							MessageBox(hwndDlg, _W("Failed to save XML file."), _W("Unable to save"), 0);
					}
#endif // USING_TINYXML
					break;
				}
			}
			break;
	}

	CRASH_PROTECT_END;

	return FALSE;
}
