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
		\version \$Id: CThumbnail.cpp,v 1.4 2004/03/06 09:10:05 jcsston Exp $
    \brief The Thumbnail Shell Extension
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <mbstring.h>
#include <shobjidl.h>
#include <Shlwapi.h>
#include "MatroskaProp.h"
#include "DShowTools/DShowTools.h"
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
// *                   CThumbnailClassFactory                   *
// ************************************************************
CThumbnailClassFactory::CThumbnailClassFactory()
{
	ODS("CThumbnailClassFactory::CThumbnailClassFactory()");

	m_cRef = 0L;

	g_cRefThisDll++;
}

CThumbnailClassFactory::~CThumbnailClassFactory()
{
	g_cRefThisDll--;
}

STDMETHODIMP CThumbnailClassFactory::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CThumbnailClassFactory::QueryInterface()");

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

STDMETHODIMP_(ULONG) CThumbnailClassFactory::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CThumbnailClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CThumbnailClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CThumbnailClassFactory::CreateInstance()");

	*ppvObj = NULL;

	// Shell extensions typically don't support aggregation (inheritance)
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.
	LPCTHUMBNAIL pThumbnail = new CThumbnail();  //Create the CToolTip object

	if (pThumbnail == NULL)
		return E_OUTOFMEMORY;

	return pThumbnail->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CThumbnailClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}

// ************************************************************
// *                     CThumbnail                           *
// ************************************************************
CThumbnail::CThumbnail()
{
	ODS("CThumbnail::CThumbnail()");

	m_ThreadState = IRTIR_TASK_NOT_RUNNING;
	m_cRef = 0L;
	m_pDataObj = NULL;

	g_cRefThisDll++;
}

CThumbnail::~CThumbnail()
{
	ODS("CThumbnail::~CThumbnail()");

	if (m_pDataObj)
		m_pDataObj->Release();

	g_cRefThisDll--;
}

STDMETHODIMP CThumbnail::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CThumbnail::QueryInterface()");

	*ppv = NULL;

	if (IsEqualIID(riid, IID_IPersistFile) || IsEqualIID(riid, IID_IUnknown))
	{
		ODS("CToolTip::QueryInterface()==>IID_IPersistFile");

		*ppv = (LPPERSISTFILE)this;
	}
	else if (IsEqualIID(riid, IID_IExtractImage))
	{
		ODS("CThumbnail::QueryInterface()==>IID_IExtractImage");

		*ppv = (LPCTHUMBNAIL)this;
	}
	else if (IsEqualIID(riid, IID_IRunnableTask))
	{
		ODS("CThumbnail::QueryInterface()==>IID_IRunnableTask");

		*ppv = (LPRUNNABLETASK)this;
	}

	if (*ppv)
	{
		AddRef();

		return NOERROR;
	}

	ODS("CThumbnail::QueryInterface()==>Unknown Interface!");

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CThumbnail::AddRef()
{
	ODS("CThumbnail::AddRef()");

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CThumbnail::Release()
{
	ODS("CThumbnail::Release()");

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CThumbnail::Load(LPCOLESTR pOleStr, DWORD dwMode)
{
	ODS("CThumbnail::Load()");

	if (pOleStr != NULL) {
		//Copy the filename to the Unicode string
		lstrcpyW(m_szFileName, (LPWSTR)pOleStr);
	} else {
		ODS("CThumbnail::Load(), pOleStr == NULL !!!, Invalid filename passed!");
		return E_POINTER;
	}
	m_dwMode = dwMode;

	return S_OK;
}

STDMETHODIMP CThumbnail::GetLocation(LPWSTR pszPathBuffer, DWORD cch, DWORD *pdwPriority, const SIZE *prgSize, DWORD dwRecClrDepth, DWORD *pdwFlags)
{
	ODS("CThumbnail::GetLocation()");

	if (prgSize != NULL)
	{
		m_height = prgSize->cx;
		m_width = prgSize->cy;
	}
	m_color_depth = dwRecClrDepth;
	
	if (pdwPriority != NULL)
		*pdwPriority = 1;

	if (pdwFlags != NULL)
		*pdwFlags = IEIFLAG_ASYNC | IEIFLAG_CACHE;
	
	return E_PENDING;
};

STDMETHODIMP CThumbnail::Extract(HBITMAP *phBmpThumbnail)
{
	ODS("CThumbnail::Extract()");
	using namespace MatroskaUtilsNamespace;
#ifdef _DEBUG	
	ULONG lastLine = 0;
#endif
	CRASH_PROTECT_START;

	if ((m_height == MatroskaShellExt_GetRegistryValue(_T("Task Thumbnail Height"), 150)) && (m_width == MatroskaShellExt_GetRegistryValue(_T("Task Thumbnail Width"), 100)) && !MatroskaShellExt_GetRegistryValue(_T("Task Thumbnails"), 0))
		//On my system Thumbnail is called everytime you just click on a file (not in the Thumbnail mode)
		return E_FAIL;

	m_ThreadState = IRTIR_TASK_RUNNING;

	// Look for the filename in our cache.
	MatroskaInfoParser *parser = g_parserCache.Lookup(m_szFileName);

	if (parser == NULL)
	{
		//File not in the cache, we have to create a new one
		parser = new MatroskaInfoParser(m_szFileName);
		parser->m_parseSeekHead = false;
		parser->m_parseAttachments = true;
		parser->m_parseTags = false;
		parser->m_parseWholeFile = false;
		if (!parser->IsValid() || parser->ParseFile() != 0)
		{
			delete parser;
			return S_FALSE;
		}

		// Add our new parser to the cache
		g_parserCache.AddParser(parser);
	}
#ifdef _DEBUG
	lastLine = __LINE__;
#endif
	if (parser != NULL) {				

		MatroskaAttachmentItem *correct_image = NULL;
		for (uint32 i = 0; i < parser->fileAttachments.GetNumberOfAttachments(); i++)
		{
			MatroskaAttachmentItem *current_attach = parser->fileAttachments.GetAttachment(i);
			if (current_attach != NULL)
			{
				if (current_attach->GetAttachmentImageType() != CXIMAGE_FORMAT_UNKNOWN)
				{
					correct_image = current_attach;

					if (correct_image->IsCover())
						break;
				}
			}
		}
#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		if (correct_image != NULL) {
			CxImage *image = correct_image->GetCxImage();

			if (image != NULL) {
				SIZE correctSize = SmartResize(image->GetWidth(), image->GetHeight(), m_width, m_height);
				image->Resample(correctSize.cx, correctSize.cy, MatroskaShellExt_GetRegistryValue(_T("ThumbnailResizeMethod"), 0));

				*phBmpThumbnail = image->MakeBitmap(GetDC(NULL));				

				m_ThreadState = IRTIR_TASK_FINISHED;
				return NOERROR;
			}
		}
	}
	CxImage myImage;
	HRESULT hr = S_OK;
	DWORD mode = 1;
	
	if (MatroskaShellExt_GetRegistryValue(_T("DShow (Internal Render) Thumbnail Fallback"), 1)) {
    mode = 1;
  } else if (MatroskaShellExt_GetRegistryValue(_T("DShow (IMediaDet) Thumbnail Fallback"), 0)) {
    mode = 2;
  }
  
  hr = GetDShowPreview(m_szFileName,
  MatroskaShellExt_GetRegistryValue(_T("ThumbnailFrameTime"), 5),
  MatroskaShellExt_GetRegistryValue(_T("Thumbnail Frame Wait Time"), 60 * 5),
  mode,
  &myImage);
  if (FAILED(hr))
    return hr;
    
	SIZE correctSize = SmartResize(myImage.GetWidth(), myImage.GetHeight(), m_width, m_height);
	myImage.Resample(correctSize.cx, correctSize.cy, MatroskaShellExt_GetRegistryValue(_T("ThumbnailResizeMethod"), 0));

	*phBmpThumbnail = myImage.MakeBitmap(GetDC(NULL));

	if (*phBmpThumbnail != NULL)
		return NOERROR;

	m_ThreadState = IRTIR_TASK_FINISHED;

#ifdef _DEBUG
	CRASH_PROTECT_END_EXTRA(lastLine);
#else
	CRASH_PROTECT_END;
#endif	

	return E_FAIL;
};
