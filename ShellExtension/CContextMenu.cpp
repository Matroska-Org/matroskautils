/*
 *  Matroska Shell Extension
 *
 *  CContextMenu.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
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
    \file CContextMenu.cpp
		\version \$Id: CContextMenu.cpp,v 1.4 2004/03/06 09:10:05 jcsston Exp $
    \brief The Context Menu Shell Extension
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <mbstring.h>
#include <shobjidl.h>
#include <Shlwapi.h>
#include "MatroskaProp.h"

//
// Global variables
//
// Reference count of this DLL.
extern UINT g_cRefThisDll;
extern HINSTANCE g_hmodThisDll;	

// Matroska File info Cache
extern MatroskaInfoParserCache g_parserCache;

using namespace MatroskaUtilsNamespace;

// ************************************************************
// *                   CContextMenuFactory                    *
// ************************************************************
CContextMenuClassFactory::CContextMenuClassFactory()
{
	ODS("CContextMenuClassFactory::CContextMenuClassFactory()");

	m_cRef = 0L;
	g_cRefThisDll++;
}

CContextMenuClassFactory::~CContextMenuClassFactory()
{
	g_cRefThisDll--;
}

STDMETHODIMP CContextMenuClassFactory::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CContextMenuClassFactory::QueryInterface()");

	*ppv = NULL;

	// Any interface on this object is the object pointer

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
	{
		*ppv = (LPCLASSFACTORY)this;

		AddRef();

		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CContextMenuClassFactory::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CContextMenuClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CContextMenuClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CContextMenuClassFactory::CreateInstance()");

	*ppvObj = NULL;

	// Shell extensions typically don't support aggregation (inheritance)

	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.

	LPCCONTEXTMENU pContextMenu = new CContextMenu();  //Create the CContextMenu object

	if (NULL == pContextMenu)
		return E_OUTOFMEMORY;

	return pContextMenu->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CContextMenuClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}

// ************************************************************
// *                    CContextMenu                          *
// ************************************************************
CContextMenu::CContextMenu()
{
	ODS("CContextMenu::CContextMenu()");

	m_cRef = 0L;
	m_pDataObj = NULL;

	g_cRefThisDll++;
}

CContextMenu::~CContextMenu()
{
	ODS("CContextMenu::~CContextMenu()");
	if (m_pDataObj)
		m_pDataObj->Release();
	
	for (uint32 i = 0; i < m_Attachments.size(); i++)
		delete m_Attachments.at(i);	

	g_cRefThisDll--;
}

STDMETHODIMP CContextMenu::QueryInterface(REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;

	if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
	{
		ODS("CContextMenu::QueryInterface()==>IID_IShellExtInit");

		*ppv = (LPSHELLEXTINIT)this;
	}
	else if (IsEqualIID(riid, IID_IContextMenu))
	{
		ODS("CContextMenu::QueryInterface()==>IID_IContextMenu");

		*ppv = (LPCONTEXTMENU)this;
	}
	else if (IsEqualIID(riid, IID_IContextMenu2))
	{
		ODS("CContextMenu::QueryInterface()==>IID_IContextMenu2");

		*ppv = (LPCONTEXTMENU2)this;
	}
	else if (IsEqualIID(riid, IID_IContextMenu3))
	{
		ODS("CContextMenu::QueryInterface()==>IID_IContextMenu3");

		*ppv = (LPCONTEXTMENU3)this;
	}

	if (*ppv)
	{
		AddRef();

		return NOERROR;
	}

	ODS("CContextMenu::QueryInterface()==>Unknown Interface!");

	return E_NOINTERFACE;
}

/// \brief Called by the shell when initializing a context menu or property sheet extension.
/// \param pIDFolder	Specifies the parent folder
/// \param pDataObj		Spefifies the set of items selected in that folder.
/// \param hRegKey		Specifies the type of the focused item in the selection.
/// \return						NOERROR in all cases.
/// Note that at the time this function is called, we don't know
/// (or care) what type of shell extension is being initialized.
/// It could be a context menu or a property sheet.\n
/// Extra: Implements the IShellExtInit member function necessary to support
/// the context menu and property sheet portioins of this shell extension.
STDMETHODIMP CContextMenu::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey)
{
	ODS("CContextMenu::Initialize()");

	// Initialize can be called more than once

	if (m_pDataObj)
		m_pDataObj->Release();

	// duplicate the object pointer and registry handle

	if (pDataObj)
	{
		m_pDataObj = pDataObj;
		pDataObj->AddRef();
		
		STGMEDIUM medium;
		FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
		UINT uCount;

		if(SUCCEEDED(m_pDataObj->GetData(&fe, &medium)))
		{
			// Get the file name from the CF_HDROP.
			uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);
			if(uCount)
				DragQueryFile((HDROP)medium.hGlobal, 0, m_szFile, MAX_MATROSKA_PATH-2);

			ReleaseStgMedium(&medium);
		}
	}

	return NOERROR;
}

STDMETHODIMP_(ULONG) CContextMenu::AddRef()
{
	ODS("CContextMenu::AddRef()");

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CContextMenu::Release()
{
	ODS("CContextMenu::Release()");

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	ODS("CContextMenu::GetCommandString()");
	
	return E_NOTIMPL;
};

STDMETHODIMP CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
	ODS("CContextMenu::InvokeCommand()");

	HRESULT hr = E_INVALIDARG;
	
	CRASH_PROTECT_START;
	if (!HIWORD(lpcmi->lpVerb))
	{
		UINT idCmd = LOWORD(lpcmi->lpVerb);

		switch (idCmd)
		{
		/*case 0:
		{
			MatroskaSimpleMuxer muxer(MatroskaMux_DShow);
			hr = muxer.AddInputFile(JString(m_szFile));			
			hr = muxer.MuxToMatroska();
			break;
		}*/
		case 0:
			ConfigureOptions();
			break;
		}

		if (m_AttachmentCmdID.size() > 0) {
			MatroskaAttachmentItem *the_attached_info = NULL;
			for (size_t a = 0; a < m_AttachmentCmdID.size(); a++) {
				MatroskaAttachmentMenuClickItem &attachmentID = m_AttachmentCmdID.at(a);
				if (attachmentID.uItemID == idCmd) {
					the_attached_info = attachmentID.attachmentSource;
					break;
				}
			}

			if (the_attached_info != NULL) {
				int ret = MessageBox(lpcmi->hwnd, JString(JString(_T("This file ")) + the_attached_info->GetAttachmentFilenameOnly() + _T("\nIs not handled by this Extension. Run external program? (equal to double-clicking file)")).t_str(), _T("Warning"), MB_YESNO|MB_ICONWARNING);
				if (ret == IDYES)
					the_attached_info->OpenViaShell(lpcmi->hwnd);
			}
		}
	}

	CRASH_PROTECT_END;

	return hr;
};

STDMETHODIMP CContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	ODS("CContextMenu::QueryContextMenu()");
	
	CRASH_PROTECT_START;

	UINT idCmd = idCmdFirst;
	BOOL bAppendItems = TRUE;
	//Check the flags
	//Normal
	//Note that the administrative snap-ins will 
	//use only the CMF_NORMAL state. 
	//So CMF_EXPLORE will never be set for admin snap-ins.
	if ((uFlags & 0x000F) == CMF_NORMAL)  //Check == here, since CMF_NORMAL=0
	{
		bAppendItems = TRUE;
	}
	//Note that if the context menu is a shell extension,
	//the Windows shell may be displaying the object in Explorer view.
	//This can be handled differently based on what you need/want.
	else if (uFlags & CMF_EXPLORE) {
		bAppendItems = TRUE;
	} else if (uFlags & CMF_DEFAULTONLY) {
		bAppendItems = FALSE;
	} else {
		bAppendItems = FALSE;
	}
  if (m_szFile == NULL)
    return S_FALSE;
   
	//Insert the menu items.
	if (bAppendItems) {					
		// Look for the filename in our cache.
		MatroskaInfoParser *parser = g_parserCache.Lookup(m_szFile);

		if (parser == NULL)
		{
			//File not in the cache, we have to create a new one
			parser = new MatroskaInfoParser(m_szFile);
			parser->m_parseSeekHead = true;
			parser->m_parseAttachments = true;
			parser->m_parseTags = true;
			parser->m_parseWholeFile = false;
			if (!parser->IsValid() || parser->ParseFile() != 0)
			{
				delete parser;
				return S_FALSE;
			}			
			// Add our new parser to the cache
			g_parserCache.AddParser(parser);
		}

		InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

		HMENU popUp = CreatePopupMenu();

		InsertMenu(hMenu, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)popUp, _T("Matroska"));
		
		UINT baseID = idCmd;		
		InsertMenu(popUp, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, _W("Configure MatroskaProp"));
		InsertMenu(popUp, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

		TCHAR track_txt[257];

		HMENU hmTracks = CreatePopupMenu();
		InsertMenu(popUp, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmTracks, _T("Tracks"));
		for (uint16 t = 0; t < parser->trackInfos.size(); t++) {
			MatroskaTrackInfo *currentTrack = parser->trackInfos.at(t);
			if (currentTrack != NULL) {
				HMENU hmTrackItem = CreatePopupMenu();
				
				JString temp(currentTrack->GetTrackTypeStr());
				temp += " ";
				temp += _W("Track");
				_sntprintf(track_txt, 255, _T("%s %u"), temp.t_str(), currentTrack->GetTrackNumber());

				InsertMenu(hmTracks, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmTrackItem, track_txt);

				_sntprintf(track_txt, 255, _T("%s: %u"), _W("UID"), currentTrack->GetTrackUID());
				InsertMenu(hmTrackItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);

				_sntprintf(track_txt, 255, _T("%s: %S"), _W("Codec ID"), currentTrack->GetCodecID());
				InsertMenu(hmTrackItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);
				
				_sntprintf(track_txt, 255, _T("%s: %S"), _W("Language"), currentTrack->GetLanguageName());
				InsertMenu(hmTrackItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);				
			}
		}

		if (parser->fileAttachments.size() > 0) {
			HMENU hmAttachments = CreatePopupMenu();
			InsertMenu(popUp, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmAttachments, _T("Attachments"));
			for (uint16 t = 0; t < parser->fileAttachments.size(); t++) {
				MatroskaAttachmentItem *currentAttachment = parser->fileAttachments.at(t);
				if (currentAttachment != NULL) {					
					HMENU hmAttachmentItem = CreatePopupMenu();
					
					_sntprintf(track_txt, 255, _T("%s %i"), _W("Attachment"), t+1);					
					InsertMenu(hmAttachments, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmAttachmentItem, track_txt);
					
					_sntprintf(track_txt, 255, _T("%s: %u"), _W("UID"), currentAttachment->attachmentUID);
					InsertMenu(hmAttachmentItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);
					_sntprintf(track_txt, 255, _T("%s: %s"), _W("Filename"), currentAttachment->GetAttachmentFilenameOnly());
					InsertMenu(hmAttachmentItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);
					_sntprintf(track_txt, 255, _T("%s: %S"), _W("Mime Type"), currentAttachment->attachmentMimeType.c_str());
					InsertMenu(hmAttachmentItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);
					_sntprintf(track_txt, 255, _T("%s: %s"), _W("Description"), currentAttachment->attachmentDescription.c_str());
					InsertMenu(hmAttachmentItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, track_txt);

					if (currentAttachment->GetAttachmentImageType() != 0) {						
						MatroskaAttachmentMenuItem *trueAttachmentStruct = new MatroskaAttachmentMenuItem();
						
						trueAttachmentStruct->attachmentSource = currentAttachment;
						trueAttachmentStruct->hmAttachmentItem = hmAttachmentItem;
						
						InsertMenu(hmAttachmentItem, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, idCmd++, 0);
						trueAttachmentStruct->uPosition = indexMenu;
						trueAttachmentStruct->uIDNewItem = idCmd;
						InsertMenu(hmAttachmentItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, _W("Attached Image"));
						InsertMenu(hmAttachmentItem, indexMenu++, MF_OWNERDRAW|MF_BYPOSITION, idCmd++, (LPCTSTR)trueAttachmentStruct);					
												
						m_Attachments.push_back(trueAttachmentStruct);
					}

					MatroskaAttachmentMenuClickItem newMenuClickItem;
					newMenuClickItem.uItemID = idCmd - baseID;
					newMenuClickItem.attachmentSource = currentAttachment;
					m_AttachmentCmdID.push_back(newMenuClickItem);

					InsertMenu(hmAttachmentItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, _W("Open with Default App"));
				}
			}
		}

		if (parser->chapters.size() > 0) {
			HMENU hmChapters = CreatePopupMenu();
			InsertMenu(popUp, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmChapters, _T("Chapters"));
			for (uint16 t = 0; t < parser->chapters.size(); t++) {
				MatroskaChapterEntry *currentChapter = parser->chapters.at(t);
				
				SmartStringFormat wBuf;
				wBuf << _W("Chapter");
				wBuf << L" ";
				wBuf << (t+1);

				AddChapterItem_Menu(hmChapters, currentChapter, wBuf.str(), idCmd, indexMenu);				
			}
		}
		//Return number of menu items we added.
		return MAKE_HRESULT(SEVERITY_SUCCESS, 0, idCmd - idCmdFirst + 1);		
	}

	CRASH_PROTECT_END;

	return NOERROR;
};

void CContextMenu::AddChapterItem_Menu(HMENU parent, MatroskaChapterEntry *chapter, std::wstring chapterTitle, UINT &idCmd, UINT &indexMenu)
{
	CRASH_PROTECT_START;

	HMENU hmChapterItem = CreatePopupMenu();				
	InsertMenu(parent, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmChapterItem, chapterTitle.c_str());
	
	// Insert UID
	{
		SmartStringFormat wBuf;
		wBuf << _W("UID");
		wBuf << L": ";
		wBuf << chapter->chapterUID;
		InsertMenu(hmChapterItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
	}
	// Insert Start, End Times
	{
		SmartStringFormat wBuf;
		wBuf << _W("Start Time");
		wBuf << L": ";
		wBuf << chapter->GetTimeStartStr().c_str();
		InsertMenu(hmChapterItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
	}	
	if (chapter->timeEnd != 0) {
		SmartStringFormat wBuf;
		wBuf << _W("End Time");
		wBuf << L": ";
		wBuf << chapter->GetTimeEndStr().c_str();
		InsertMenu(hmChapterItem, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
	}	

	// Insert Display Strings
	for (size_t d = 0; d < chapter->display.size(); d++) {
		MatroskaChapterDisplayInfo &displayInfo = chapter->display.at(d);
		
		HMENU hmDisplayInfo = CreatePopupMenu();				
		InsertMenu(hmChapterItem, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmDisplayInfo, _W("Display String"));

		if (displayInfo.string.length() > 0) {
			SmartStringFormat wBuf;
			wBuf << _W("String");
			wBuf << L": ";
			wBuf << displayInfo.string.c_str();			
			InsertMenu(hmDisplayInfo, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
		}
		if (displayInfo.lang.length() > 0) {
			SmartStringFormat wBuf;
			wBuf << _W("Language");
			wBuf << L": ";
			wBuf << displayInfo.lang.c_str();
			InsertMenu(hmDisplayInfo, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
		}
		if (displayInfo.country.length() > 0) {			
			SmartStringFormat wBuf;
			wBuf << _W("Country");
			wBuf << L": ";
			wBuf << displayInfo.country.c_str();
			InsertMenu(hmDisplayInfo, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
		}
	}	

	// Insert Track UID
	HMENU hmTrackUID; 
	if (chapter->tracks.size() > 0) {
		hmTrackUID = CreatePopupMenu();				
		InsertMenu(hmChapterItem, indexMenu++, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT_PTR)hmTrackUID, _W("Track UID(s))"));
	}
	for (size_t t = 0; t < chapter->tracks.size(); t++) {
		SmartStringFormat wBuf;
		wBuf << _W("Track UID");
		wBuf << L": ";
		wBuf << (uint64)chapter->tracks.at(t);
		InsertMenu(hmTrackUID, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, wBuf.str().c_str());
	}

	for (size_t c = 0; c < chapter->children.size(); c++) {
		MatroskaChapterEntry *childChapter = chapter->children.at(c);
		SmartStringFormat wBuf;
		wBuf << chapterTitle.c_str();
		wBuf << L"-";
		wBuf << (c+1);
		AddChapterItem_Menu(hmChapterItem, childChapter, wBuf.str(), idCmd, indexMenu);
	}

	CRASH_PROTECT_END;
}

STDMETHODIMP CContextMenu::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TRACE("CContextMenu::HandleMenuMsg()");

	LRESULT res;
	return MenuMessageHandler(uMsg, wParam, lParam, &res);	
};

STDMETHODIMP CContextMenu::HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{
	TRACE("CContextMenu::HandleMenuMsg2()");

	if (plResult == NULL) {
		LRESULT res;
		return MenuMessageHandler(uMsg, wParam, lParam, &res);
	} else {
		return MenuMessageHandler(uMsg, wParam, lParam, plResult);
	}
};

STDMETHODIMP CContextMenu::MenuMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{	
	TRACE("CContextMenu::MenuMessageHandler()");
	CRASH_PROTECT_START;

	switch (uMsg)
	{
		case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT itemInfo = (LPMEASUREITEMSTRUCT)lParam;
			MatroskaAttachmentMenuItem *currentAttachmentStruct = (MatroskaAttachmentMenuItem *)itemInfo->itemData;
			MatroskaAttachmentItem *currentAttachment = currentAttachmentStruct->attachmentSource;
			if (currentAttachment != NULL) {
				// An image attachment
				CxImage *attachedImage = currentAttachment->GetCxImage();
				if (attachedImage != NULL) {
					//TCHAR track_txt[256];					
					//_sntprintf(track_txt, 255, _T("%i x %i, %i bits"), attachedImage->GetWidth(), attachedImage->GetHeight(), attachedImage->GetBpp());
					ModifyMenuA(currentAttachmentStruct->hmAttachmentItem, currentAttachmentStruct->uIDNewItem, MF_STRING|MF_BYCOMMAND, currentAttachmentStruct->uIDNewItem, currentAttachment->GetImageInfo().c_str());

					SIZE correctSize = SmartResize(attachedImage->GetWidth(), attachedImage->GetHeight(), 100, 100);
					attachedImage->Resample(correctSize.cx, correctSize.cy, MatroskaShellExt_GetRegistryValue(_T("ThumbnailResizeMethod"), 0));
					
					itemInfo->itemWidth = correctSize.cx;
					itemInfo->itemHeight = correctSize.cy;
				}
			}
			break;
		}
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT itemInfo = (LPDRAWITEMSTRUCT)lParam;
			
			MatroskaAttachmentMenuItem *currentAttachmentStruct = (MatroskaAttachmentMenuItem *)itemInfo->itemData;
			
			MatroskaAttachmentItem *currentAttachment = currentAttachmentStruct->attachmentSource;
			if (currentAttachment != NULL) {
				// An image attachment
				CxImage *attachedImage = currentAttachment->GetCxImage();
				if (attachedImage != NULL) {
					long leftPos = (itemInfo->rcItem.right - itemInfo->rcItem.left - attachedImage->GetWidth()) / 2 + itemInfo->rcItem.left;
					long topPos = itemInfo->rcItem.top;
					attachedImage->Draw(itemInfo->hDC, leftPos, topPos);
				}
			}
			break;
		}
	}	
	CRASH_PROTECT_END;

	return S_OK;
};

