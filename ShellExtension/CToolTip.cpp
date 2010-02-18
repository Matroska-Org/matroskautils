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
		\version \$Id: CToolTip.cpp,v 1.6 2004/03/20 05:08:31 jcsston Exp $
    \brief The Tooltip Shell Extension
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
// *                   CToolTipClassFactory                   *
// ************************************************************
CToolTipClassFactory::CToolTipClassFactory()
{
	ODS("CToolTipClassFactory::CToolTipClassFactory()");

	m_cRef = 0L;

	g_cRefThisDll++;
}

CToolTipClassFactory::~CToolTipClassFactory()
{
	g_cRefThisDll--;
}

STDMETHODIMP CToolTipClassFactory::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CToolTipClassFactory::QueryInterface()");

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

STDMETHODIMP_(ULONG) CToolTipClassFactory::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CToolTipClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CToolTipClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CToolTipClassFactory::CreateInstance()");

	*ppvObj = NULL;

	// Shell extensions typically don't support aggregation (inheritance)

	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.

	LPCTOOLTIP pToolTip = new CToolTip();  //Create the CToolTip object

	if (pToolTip == NULL)
		return E_OUTOFMEMORY;

	return pToolTip->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CToolTipClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}

// ************************************************************
// *                       CToolTip                           *
// ************************************************************
CToolTip::CToolTip()
{
	ODS("CToolTip::CToolTip()");

	m_cRef = 0L;
	m_pDataObj = NULL;

	g_cRefThisDll++;
}

CToolTip::~CToolTip()
{
	ODS("CToolTip::~CToolTip()");

	if (m_pDataObj)
		m_pDataObj->Release();

	g_cRefThisDll--;
}

STDMETHODIMP CToolTip::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CToolTip::QueryInterface()");

	*ppv = NULL;

	if (IsEqualIID(riid, IID_IPersistFile) || IsEqualIID(riid, IID_IUnknown))
	{
		ODS("CToolTip::QueryInterface()==>IID_IPersistFile");

		*ppv = (LPPERSISTFILE)this;
	}
	else if (IsEqualIID(riid, IID_IQueryInfo))
	{
		ODS("CToolTip::QueryInterface()==>IQueryInfo");

		*ppv = (LPCTOOLTIP)this;
	}

	if (*ppv)
	{
		AddRef();

		return NOERROR;
	}

	ODS("CToolTip::QueryInterface()==>Unknown Interface!");

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CToolTip::AddRef()
{
	ODS("CToolTip::AddRef()");

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CToolTip::Release()
{
	ODS("CToolTip::Release()");

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CToolTip::Load(LPCOLESTR pOleStr, DWORD dwMode)
{
	ODS("CToolTip::Load()");

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

STDMETHODIMP CToolTip::GetInfoFlags(DWORD *pdwFlags)
{
	ODS("CToolTip::GetInfoFlags()");

	return E_NOTIMPL;
};

STDMETHODIMP CToolTip::GetInfoTip(DWORD dwFlags, LPWSTR *ppwszTip)
{
	CRASH_PROTECT_START;

	ODS("CToolTip::GetInfoTip()");
	using namespace MatroskaUtilsNamespace;

	//Lets create a nice tooltip
	std::wstring tool_tip;	
	UTFstring tmp;
	
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

	//A temp formating buffer
	wchar_t buffer_txt[128];
	if (!MatroskaShellExt_GetRegistryValue(_T("UseShortTooltips"), 0))
	{
 		for (int i = 0; i < parser->GetNumberOfTracks(); i++)
		{
			if (i > 0)
				//After the first track we insert line breaks before each track line
				tool_tip += L"\n";

			MatroskaTrackInfo *current_track = parser->GetTrackInfo(i);			
			
			if (current_track != NULL)
			{
				tool_tip += _W("Track");
				snwprintf(buffer_txt, 128, L" %u: %s %S", current_track->GetTrackNumber(), current_track->GetTrackTypeStr(), current_track->GetCodecID());
				tool_tip += buffer_txt;
				if (current_track->m_CodecOldID.length() > 0)
				{
					tool_tip += L" ";
					tmp.SetUTF8(current_track->m_CodecOldID);
					tool_tip += tmp.c_str();
				}
				const char *langName = current_track->GetLanguageName();
				if (langName != NULL)
				{
					tool_tip += L" ";
					tool_tip += _W("Language:");
					tool_tip += L" ";
					tmp.SetUTF8(langName);
					tool_tip += tmp.c_str();
				}
				tool_tip += L",";					
			}
		}
		tool_tip += L"\n";
	}else {
		//Ok we display short tooltips
		uint8 video_track_count = 0;
		uint8 audio_track_count = 0;
		uint8 subtitle_track_count = 0;
		for (int i = 0; i < parser->GetNumberOfTracks(); i++)
		{
			MatroskaTrackInfo *current_track = parser->GetTrackInfo(i);			

			if (current_track != NULL)
			{
				switch (current_track->GetTrackType())
				{
					case track_audio:
						audio_track_count++;
						break;
					case track_video:
						video_track_count++;
						break;
					case track_subtitle:
						subtitle_track_count++;
						break;
				}
			}
		}
		if (video_track_count > 0)
		{
			tool_tip += _W("Video Tracks:");
			snwprintf(buffer_txt, 128, L" %u\n", video_track_count);
			tool_tip += buffer_txt;
		}
		if (audio_track_count > 0)
		{
			tool_tip += _W("Audio Tracks:");
			snwprintf(buffer_txt, 128, L" %u\n", audio_track_count);
			tool_tip += buffer_txt;
		}
		if (subtitle_track_count > 0)
		{
			tool_tip += _W("Subtitle Tracks:");
			snwprintf(buffer_txt, 128, L" %u\n", subtitle_track_count);
			tool_tip += buffer_txt;
		}			
	}
	if (parser->file_size != 0)
	{
		tool_tip += _W("Filesize:");
		tool_tip += L" ";
		tmp.SetUTF8(parser->GetNiceFileSize());
		tool_tip += tmp.c_str();
		if (parser->m_segmentInfo.GetDuration() != 0)
		{				
			tool_tip += L" ";
			tool_tip += _W("Avg Bitrate:");			
			snwprintf(buffer_txt, 128, L" %.2fKb/s", (double)parser->file_size / 1024 / parser->m_segmentInfo.GetDuration() * 8);
			tool_tip += buffer_txt;				
		}

		tool_tip += L" ";
		tool_tip += _W("Length:");
		tool_tip += L" ";
		tool_tip += parser->m_segmentInfo.GetNiceDurationW().c_str();			
	}
	tool_tip += L"\n" _T(MATROSKA_SHELL_EXT_APP_NAME);

	LPMALLOC pMalloc = NULL;
	if (SHGetMalloc(&pMalloc))
		return E_OUTOFMEMORY;

	*ppwszTip = (LPWSTR)pMalloc->Alloc(tool_tip.length() * sizeof(wchar_t) + 3);
	if (NULL == *ppwszTip)
	{
		pMalloc->Release();
		return E_OUTOFMEMORY;
	}	
	// Use the Unicode string copy function to put the tooltip text in the buffer.	
	wcscpy(*ppwszTip, tool_tip.c_str());

	//The last thing to do is release the IMalloc interface we got earlier.
	pMalloc->Release();

	CRASH_PROTECT_END;

	return S_OK;
};

