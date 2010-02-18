/*
 *  Matroska Shell Icon Extension
 *
 *  CShellIcon.cpp
 *
 *  Copyright (C) Jory Stone - February 2004
 * 
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
    \file CShellIcon.cpp
		\version \$Id: CShellIcon.cpp,v 1.1 2004/03/08 07:56:51 jcsston Exp $
    \brief The Shell Icon Extension
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
// *                  CShellIconClassFactory                   *
// ************************************************************
CShellIconClassFactory::CShellIconClassFactory()
{
	ODS("CShellIconClassFactory::CShellIconClassFactory()");

	m_cRef = 0L;

	g_cRefThisDll++;
}

CShellIconClassFactory::~CShellIconClassFactory()
{
	g_cRefThisDll--;
}

STDMETHODIMP CShellIconClassFactory::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CShellIconClassFactory::QueryInterface()");

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

STDMETHODIMP_(ULONG) CShellIconClassFactory::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellIconClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CShellIconClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CShellIconClassFactory::CreateInstance()");

	*ppvObj = NULL;

	// Shell extensions typically don't support aggregation (inheritance)

	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.

	LPCSHELLICON pShellIcon = new CShellIcon();  //Create the CShellIcon object

	if (NULL == pShellIcon)
		return E_OUTOFMEMORY;

	return pShellIcon->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellIconClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}

// ************************************************************
// *                       CShellIcon                          *
// ************************************************************
CShellIcon::CShellIcon()
{
	ODS("CShellIcon::CShellIcon()");

	m_cRef = 0L;
	m_pDataObj = NULL;

	g_cRefThisDll++;
}

CShellIcon::~CShellIcon()
{
	ODS("CShellIcon::~CShellIcon()");
	if (m_pDataObj)
		m_pDataObj->Release();

	g_cRefThisDll--;
}

STDMETHODIMP CShellIcon::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CShellIcon::QueryInterface()");

	*ppv = NULL;

	if (IsEqualIID(riid, IID_IPersistFile) || IsEqualIID(riid, IID_IUnknown))
	{
		ODS("CToolTip::QueryInterface()==>IID_IPersistFile");

		*ppv = (LPPERSISTFILE)this;
	}
	else if (IsEqualIID(riid, IID_IExtractIcon))
	{
		ODS("CShellIcon::QueryInterface()==>IID_IExtractIcon");

		*ppv = (LPEXTRACTICON)this;
	}

	if (*ppv)
	{
		AddRef();

		return NOERROR;
	}

	ODS("CShellIcon::QueryInterface()==>Unknown Interface!");

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellIcon::AddRef()
{
	ODS("CShellIcon::AddRef()");

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellIcon::Release()
{
	ODS("CShellIcon::Release()");

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CShellIcon::Load(LPCOLESTR pOleStr, DWORD dwMode)
{
	ODS("CShellIcon::Load()");

	if (pOleStr != NULL) {
		//Copy the filename to the Unicode string
		lstrcpyW(m_szFileName, (LPWSTR)pOleStr);
	} else {
		ODS("CShellIcon::Load(), pOleStr == NULL !!!, Invalid filename passed!");
		return E_POINTER;
	}
	m_dwMode = dwMode;

	return S_OK;
}

STDMETHODIMP CShellIcon::GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
{
	CRASH_PROTECT_START;

	NOTE1("CShellIcon::GetIconLocation(uFlags = %i)", uFlags);

	TCHAR szModulePath[MAX_PATH];
  GetModuleFileName(g_hmodThisDll, szModulePath, MAX_PATH);
  lstrcpyn(szIconFile, szModulePath, cchMax);

	// Look for the filename in our cache.
	MatroskaInfoParser *parser = g_parserCache.Lookup(m_szFileName);

	if (parser == NULL)
	{
		//File not in the cache, we have to create a new one
		parser = new MatroskaInfoParser(m_szFileName);
		if (!parser->IsValid()) {
			delete parser;
			return S_FALSE;
		}
		parser->m_parseSeekHead = false;
		parser->m_parseAttachments = true;
		parser->m_parseTags = false;
		parser->m_parseWholeFile = false;
		if (parser->ParseFile() != 0)
		{
			delete parser;
			return S_FALSE;
		}

		// Add our new parser to the cache
		g_parserCache.AddParser(parser);
	}

	*piIndex = 0;
	if (parser->GetFirstVideoTrack() != NULL)
		*piIndex |= 0x01;
	
	if (parser->GetFirstAudioTrack() != NULL)
		*piIndex |= 0x02;
	
	if (parser->GetFirstSubtitleTrack() != NULL)
		*piIndex |= 0x04;

	*piIndex += 100;

	CRASH_PROTECT_END;

	return S_OK;
}

STDMETHODIMP CShellIcon::Extract(LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize)
{
	NOTE3(_T("CShellIcon::Extract(pszFile = %s, nIconIndex = %i, nIconSize = %i)"), pszFile, nIconIndex, nIconSize);

	int smallSize = HIWORD(nIconSize);
	int largeSize = LOWORD(nIconSize);

	ICONINFO iconInfo;
	iconInfo.fIcon = true;
	iconInfo.xHotspot = 0;
	iconInfo.yHotspot = 0;
	CxImage iconImg;
	CxImage iconAlphaImg;
	iconImg.CreateFromHICON((HICON)LoadImage(g_hmodThisDll, MAKEINTRESOURCE(IDI_ICON_MONKEY), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
	iconImg.IncreaseBpp(24);

	nIconIndex = nIconIndex - 100;
	if (nIconIndex == (nIconIndex | 0x01)) {
		CxImage iconVideo;
		iconVideo.CreateFromHICON((HICON)LoadImage(g_hmodThisDll, MAKEINTRESOURCE(IDI_ICON_SUBTITLE), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
		iconVideo.IncreaseBpp(24);
		iconVideo.Resample(16, 16, 0);
		iconImg.Mix(iconVideo, CxImage::ImageOpType::OpDstCopy, 0, 0);		
	}
	
	if (nIconIndex == (nIconIndex | 0x02)) {
		CxImage iconAudio;
		iconAudio.CreateFromHICON((HICON)LoadImage(g_hmodThisDll, MAKEINTRESOURCE(IDI_ICON_AUDIO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
		iconAudio.Resample(16, 16, 0);
		iconImg.Mix(iconAudio, CxImage::ImageOpType::OpDstCopy, 16, 16);
	}

	if (nIconIndex == (nIconIndex | 0x04)) {

	}
	
	iconImg.AlphaSplit(&iconAlphaImg);
	iconAlphaImg.Negative();

#ifdef _DEBUG
	//iconImg.Draw(GetDC(NULL));
	//iconAlphaImg.Draw(GetDC(NULL));
#endif

	iconInfo.hbmColor = iconImg.MakeBitmap();
	iconInfo.hbmMask = iconAlphaImg.MakeBitmap();
	
	*phiconLarge = CreateIconIndirect(&iconInfo);

	return S_OK;
}

