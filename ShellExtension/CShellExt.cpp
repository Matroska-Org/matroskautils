/*
 *  Matroska Shell Extension
 *
 *  MatroskaProp.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
 * 
 *  The idea was based from ImgSize by Victor Sapojnikov
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
    \file MatroskaProp.cpp
		\version \$Id: CShellExt.cpp,v 1.5 2004/03/06 09:10:05 jcsston Exp $
    \brief The Property Page Shell Extension
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

extern long g_MyBuildNumber;

// Matroska File info Cache
extern MatroskaInfoParserCache g_parserCache;

// ************************************************************
// *                  CShellExtClassFactory                   *
// ************************************************************
CShellExtClassFactory::CShellExtClassFactory()
{
	ODS("CShellExtClassFactory::CShellExtClassFactory()");

	m_cRef = 0L;

	g_cRefThisDll++;
}

CShellExtClassFactory::~CShellExtClassFactory()
{
	g_cRefThisDll--;
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CShellExtClassFactory::QueryInterface()");

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

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CShellExtClassFactory::CreateInstance()");

	*ppvObj = NULL;

	// Shell extensions typically don't support aggregation (inheritance)

	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.

	LPCSHELLEXT pShellExt = new CShellExt();  //Create the CShellExt object

	if (NULL == pShellExt)
		return E_OUTOFMEMORY;

	return pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}

// ************************************************************
// *                       CShellExt                          *
// ************************************************************
CShellExt::CShellExt()
{
	ODS("CShellExt::CShellExt()");

	m_cRef = 0L;
	m_pDataObj = NULL;

	g_cRefThisDll++;
}

CShellExt::~CShellExt()
{
	ODS("CShellExt::~CShellExt()");
	if (m_pDataObj)
		m_pDataObj->Release();

	g_cRefThisDll--;
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;

	if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
	{
		ODS("CShellExt::QueryInterface()==>IID_IShellExtInit");

		*ppv = (LPSHELLEXTINIT)this;
	}
	else if (IsEqualIID(riid, IID_IContextMenu))
	{
		ODS("CShellExt::QueryInterface()==>IID_IContextMenu");

		*ppv = (LPCONTEXTMENU)this;
	}
	else if (IsEqualIID(riid, IID_IExtractIcon))
	{
		ODS("CShellExt::QueryInterface()==>IID_IExtractIcon");

		*ppv = (LPEXTRACTICON)this;
	}
	else if (IsEqualIID(riid, IID_IPersistFile))
	{
		ODS("CShellExt::QueryInterface()==>IPersistFile");

		*ppv = (LPPERSISTFILE)this;
	}
	else if (IsEqualIID(riid, IID_IShellPropSheetExt))
	{
		ODS("CShellExt::QueryInterface()==>IShellPropSheetExt");

		*ppv = (LPSHELLPROPSHEETEXT)this;
	}
	else if (IsEqualIID(riid, IID_IShellCopyHook))
	{
		ODS("CShellExt::QueryInterface()==>ICopyHook");

		*ppv = (LPCOPYHOOK)this;
	}

	if (*ppv)
	{
		AddRef();

		return NOERROR;
	}

	ODS("CShellExt::QueryInterface()==>Unknown Interface!");

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
STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder,LPDATAOBJECT pDataObj,HKEY hRegKey)
{
	ODS("CShellExt::Initialize()");

	// Initialize can be called more than once

	if (m_pDataObj)
		m_pDataObj->Release();

	// duplicate the object pointer and registry handle

	if (pDataObj)
	{
		m_pDataObj = pDataObj;
		pDataObj->AddRef();
	}

	return NOERROR;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
	ODS("CShellExt::AddRef()");

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{
	ODS("CShellExt::Release()");

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

/// \brief Called by the shell just before the property sheet is displayed.
/// \param lpfnAddPage Pointer to the Shell's AddPage function
/// \param lParam Passed as second parameter to lpfnAddPage
/// \return NOERROR in all cases.  If for some reason our pages don't get added, the Shell still needs to bring up the Properties... sheet.
STDMETHODIMP CShellExt::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	CRASH_PROTECT_START;

	using namespace MatroskaUtilsNamespace;

	MoreData m;
	HPROPSHEETPAGE hpage;

	ODS("CShellExt::AddPages()");

	FORMATETC fmte = {CF_HDROP, (DVTARGETDEVICE *)NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

	STGMEDIUM medium = { 0 };
	HRESULT hres = 0;

	if (m_pDataObj)  //Paranoid check, m_pDataObj should have something by now...
		hres = m_pDataObj->GetData(&fmte, &medium);

	if (SUCCEEDED(hres))
	{
		//Find out how many files the user has selected...
		UINT cbFiles = 0;

		if (medium.hGlobal)
			cbFiles = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

		if (cbFiles < 2)
		{
			//OK, the user has only selected a single file, so lets go ahead
			//and add the property sheet.  Note that it doesn't HAVE to be this
			//way, but for simplicity and educational reasons, I'll only add
			//the property sheet if a single file is selected.

			//Get the name of the file the user has clicked on
			//In Windows98 this causes an error
			if (cbFiles)
			{
				DragQueryFile((HDROP)medium.hGlobal, 0, szFile, sizeof(szFile)/sizeof(TCHAR));
			}
			if (szFile == NULL)
				return S_FALSE;

			int i = _tcslen(szFile);
			if (i < 5)
				return NOERROR;

			// Look for the filename in our cache.
			MatroskaInfoParser *parser = g_parserCache.Lookup(szFile);

			if (parser == NULL)
			{
				//File not in the cache, we have to create a new one
				parser = new MatroskaInfoParser(szFile);
				if (!parser->IsValid()) {
					delete parser;
					return S_FALSE;
				}
				// Delete the parser obj anyway
				delete parser;
				parser = NULL;
			}

			if (true) {
				MatroskaPages *pageData = new MatroskaPages();
				//put the new MatroskaPages in the vector/list
				//g_pageList.push_back(pageData);

				//Store the app instance
				pageData->g_hAppInst = g_hmodThisDll;

				// Create a property sheet page object from a dialog box.
				//
				// We store a pointer to our class in the m.psp.lParam, so we
				// can access our class members from within the PropPageDlgProc
				//
				// If the page needs more instance data, you can append
				// arbitrary size of data at the end of this structure,
				// and pass it to the CreatePropSheetPage. In such a case,
				// the size of entire data structure (including page specific
				// data) must be stored in the dwSize field.   Note that in
				// general you should NOT need to do this, as you can simply
				// store a pointer to data in the lParam member.
				if (MatroskaShellExt_GetRegistryValue(_T("Info Tab"), 1))
				{
					m.psp.dwSize      = sizeof(m);
					m.psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE;
					m.psp.hInstance   = g_hmodThisDll;
					m.psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE);
					m.psp.hIcon       = 0;
					m.psp.pszTitle    = _W("Matroska Info");
					m.psp.pfnDlgProc  = MediaPropProc;
					m.psp.pcRefParent = &g_cRefThisDll;
					m.psp.lParam      = (LPARAM)pageData;
					m.pObjPageData    = pageData;
					_tcscpy(m.pObjPageData->szFile, szFile);

					hpage = CreatePropertySheetPage(&m.psp);

					if (hpage)
					{
						if (!lpfnAddPage(hpage, lParam))
							DestroyPropertySheetPage(hpage);
					}
				}

				if (((parser != NULL) && (parser->fileAttachments.size() > 0)) || MatroskaShellExt_GetRegistryValue(_T("Attachments Tab"), 1))
				{
					//Add the Attachements tab
					MoreData mAttachments;
					mAttachments.psp.dwSize      = sizeof(mAttachments);
					mAttachments.psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE;
					mAttachments.psp.hInstance   = g_hmodThisDll;
					mAttachments.psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_ATTACHMENT);
					mAttachments.psp.hIcon       = 0;
					mAttachments.psp.pszTitle    = _W("Matroska Attachments");
					mAttachments.psp.pfnDlgProc  = MediaPropProc_Attachments;
					mAttachments.psp.pcRefParent = &g_cRefThisDll;
					mAttachments.psp.lParam      = (LPARAM)pageData;
					mAttachments.pObjPageData    = pageData;
					_tcscpy(mAttachments.pObjPageData->szFile, szFile);

					hpage = CreatePropertySheetPage(&mAttachments.psp);

					if (hpage)
					{
						if (!lpfnAddPage(hpage, lParam))
							DestroyPropertySheetPage(hpage);
					}
				}

				if (((parser != NULL) && (parser->chapters.size() > 0)) || MatroskaShellExt_GetRegistryValue(_T("Chapters Tab"), 1))
				{
					//Add the Attachements tab
					MoreData mChapters;
					mChapters.psp.dwSize      = sizeof(mChapters);
					mChapters.psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE;
					mChapters.psp.hInstance   = g_hmodThisDll;
					mChapters.psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_CHAPTER);
					mChapters.psp.hIcon       = 0;
					mChapters.psp.pszTitle    = _W("Matroska Chapters");
					mChapters.psp.pfnDlgProc  = MediaPropProc_Chapters;
					mChapters.psp.pcRefParent = &g_cRefThisDll;
					mChapters.psp.lParam      = (LPARAM)pageData;
					mChapters.pObjPageData    = pageData;
					_tcscpy(mChapters.pObjPageData->szFile, szFile);

					hpage = CreatePropertySheetPage(&mChapters.psp);

					if (hpage)
					{
						if (!lpfnAddPage(hpage, lParam))
							DestroyPropertySheetPage(hpage);
					}
				}
				//This is the Tags tab
				if (MatroskaShellExt_GetRegistryValue(_T("Tag Tab"), 1))
				{
					MoreData mTag;
					mTag.psp.dwSize      = sizeof(mTag);
					mTag.psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE;
					mTag.psp.hInstance   = g_hmodThisDll;
					mTag.psp.pszTemplate = MAKEINTRESOURCE(IDD_TAG_TAB);
					mTag.psp.hIcon       = 0;
					mTag.psp.pszTitle    = _W("Matroska Tags");
					mTag.psp.pfnDlgProc  = MediaPropProc_Tag;
					mTag.psp.pcRefParent = &g_cRefThisDll;
					mTag.psp.lParam      = (LPARAM)pageData;
					mTag.pObjPageData    = pageData;
					_tcscpy(mTag.pObjPageData->szFile, szFile);

					hpage = CreatePropertySheetPage(&mTag.psp);

					if (hpage)
					{
						if (!lpfnAddPage(hpage, lParam))
							DestroyPropertySheetPage(hpage);
					}
				}
			}

		}
	}

	CRASH_PROTECT_END;

	return NOERROR;
}

/// Called by the shell only for Control Panel property sheet extensions
STDMETHODIMP CShellExt::ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam)
{
	ODS("CShellExt::ReplacePage()");
	// We don't support this function.  It should never be called.
	return E_FAIL;
}


