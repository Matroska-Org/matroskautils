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
		\version \$Id: CColumnProvider.cpp,v 1.4 2004/03/06 09:10:05 jcsston Exp $
    \brief The Column Shell Extension
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

using namespace MatroskaUtilsNamespace;

// ************************************************************
// *             CColumnProviderClassFactory                  *
// ************************************************************
CColumnProviderClassFactory::CColumnProviderClassFactory()
{
	ODS("CColumnProviderClassFactory::CColumnProviderClassFactory()");

	m_cRef = 0L;

	g_cRefThisDll++;
}

CColumnProviderClassFactory::~CColumnProviderClassFactory()
{
	g_cRefThisDll--;
}

STDMETHODIMP CColumnProviderClassFactory::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CColumnProviderClassFactory::QueryInterface()");

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

STDMETHODIMP_(ULONG) CColumnProviderClassFactory::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CColumnProviderClassFactory::Release()
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CColumnProviderClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	ODS("CColumnProviderClassFactory::CreateInstance()");

	*ppvObj = NULL;

	// Shell extensions typically don't support aggregation (inheritance)
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.
	LPCCOLUMNPROVIDER pColumns = new CColumnProvider();  //Create the CToolTip object

	if (pColumns == NULL)
		return E_OUTOFMEMORY;

	return pColumns->QueryInterface(riid, ppvObj);
}

STDMETHODIMP CColumnProviderClassFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}

// ************************************************************
// *                     CColumnProvider                           *
// ************************************************************
CColumnProvider::CColumnProvider()
{
	ODS("CColumnProvider::CColumnProvider()");

	m_cRef = 0L;
	m_pDataObj = NULL;

	g_cRefThisDll++;
}

CColumnProvider::~CColumnProvider()
{
	ODS("CColumnProvider::~CColumnProvider()");

	if (m_pDataObj)
		m_pDataObj->Release();

	g_cRefThisDll--;
}

STDMETHODIMP CColumnProvider::QueryInterface(REFIID riid, LPVOID *ppv)
{
	ODS("CColumnProvider::QueryInterface()");

	*ppv = NULL;

	if (IsEqualIID(riid, IID_IColumnProvider) || IsEqualIID(riid, IID_IUnknown))
	{
		ODS("CColumnProvider::QueryInterface()==>IID_IColumnProvider");

		*ppv = this;
	}

	if (*ppv)
	{
		AddRef();

		return NOERROR;
	}

	ODS("CColumnProvider::QueryInterface()==>Unknown Interface!");

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CColumnProvider::AddRef()
{
	TRACE("CColumnProvider::AddRef()");

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CColumnProvider::Release()
{
	TRACE("CColumnProvider::Release()");

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CColumnProvider::Initialize(LPCSHCOLUMNINIT psci)
{
	ODS("CColumnProvider::Initialize()");

	wcscpy(m_szFolder, psci->wszFolder);

	return S_OK;
};

STDMETHODIMP CColumnProvider::GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO *psci)
{
	ODS("CColumnProvider::GetColumnInfo()");
	CRASH_PROTECT_START;

	switch (dwIndex)
	{
		case 0:
		{
			// The Summary Information Property Set
			psci->scid.fmtid = FMTID_SummaryInformation;   
			//Title PIDSI_TITLE 0x00000002 VT_LPSTR 
			psci->scid.pid = PIDSI_TITLE;                   
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Title", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Title of the file", MAX_COLUMN_DESC_LEN);
			break;
		}		
		case 1:
		{
			// The Summary Information Property Set
			psci->scid.fmtid = FMTID_SummaryInformation;   
			//Subject PIDSI_SUBJECT 0x00000003 VT_LPSTR 
			psci->scid.pid = PIDSI_SUBJECT;                   
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Subject", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Subject of the file", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 2:
		{
			// The Summary Information Property Set
			psci->scid.fmtid = FMTID_SummaryInformation;   
			//Author PIDSI_AUTHOR 0x00000004 VT_LPSTR 
			psci->scid.pid = PIDSI_AUTHOR;                   
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Author", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Author of the file", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 3:
		{
			// The Summary Information Property Set
			psci->scid.fmtid = FMTID_SummaryInformation;   
			//Keywords PIDSI_KEYWORDS 0x00000005 VT_LPSTR 
			psci->scid.pid = PIDSI_KEYWORDS;                   
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Keywords", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Keywords of the file", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 4:
		{
			// The Summary Information Property Set
			psci->scid.fmtid = FMTID_SummaryInformation;   
			//Comments PIDSI_COMMENTS 0x00000006 VT_LPSTR 
			psci->scid.pid = PIDSI_COMMENTS;                   
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Comments", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Comments of the file", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 5:
		{
			// The Summary Information Property Set
			psci->scid.fmtid = FMTID_SummaryInformation;   
			//Name of Creating Application PIDSI_APPNAME 0x00000012 VT_LPSTR 
			psci->scid.pid = PIDSI_APPNAME;
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Matroska Writing App", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Program/Lib used to Write the Matroska file", MAX_COLUMN_DESC_LEN);
			break;
		}	
		case 6:
		{
			// This is a handy GUID
			psci->scid.fmtid = CLSID_MatroskaColumnProvider_ShellExtension;   
			// Any ol' ID will do, but using the col # is easiest
			psci->scid.pid = 0;                   
			// We'll return the data as a string
			psci->vt = VT_LPSTR;    
			// Text will be left-aligned in the column
			psci->fmt = LVCFMT_LEFT;
			// Data should be sorted as strings
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Matroska Muxing App", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Program used to Mux the Matroska file", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 7:
		{
			// FMTID_VideoSummaryInformation Property Set
			psci->scid.fmtid = FMTID_VideoSummaryInformation;   			
			psci->scid.pid = PIDVSI_FRAME_WIDTH;			
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 8;                  

			wcsncpy(psci->wszTitle, L"Video Width", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Video Width in pixels", MAX_COLUMN_DESC_LEN);
			break;
		}	
		case 8:
		{
			// FMTID_VideoSummaryInformation Property Set
			psci->scid.fmtid = FMTID_VideoSummaryInformation;   			
			psci->scid.pid = PIDVSI_FRAME_HEIGHT;			
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 8;                  

			wcsncpy(psci->wszTitle, L"Video Height", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Video Height in pixels", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 9:
		{
			// FMTID_VideoSummaryInformation Property Set
			psci->scid.fmtid = FMTID_VideoSummaryInformation;   			
			psci->scid.pid = PIDVSI_TIMELENGTH;			
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 8;                  

			wcsncpy(psci->wszTitle, L"Length", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Length in millseconds", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 10:
		{
			// FMTID_VideoSummaryInformation Property Set
			psci->scid.fmtid = FMTID_VideoSummaryInformation;   	
			// frames/millisecond
			psci->scid.pid = PIDVSI_FRAME_RATE;			
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 8;                  

			wcsncpy(psci->wszTitle, L"Video Framerate", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Avg (for Matroska) Video Framerate", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 11:
		{
			// FMTID_VideoSummaryInformation Property Set
			psci->scid.fmtid = FMTID_VideoSummaryInformation;   			
			psci->scid.pid = PIDVSI_COMPRESSION;			
			psci->vt = VT_LPWSTR;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Video Compression", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Video Compression used", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 12:
		{
			// FMTID_AudioSummaryInformation Property Set
			psci->scid.fmtid = FMTID_AudioSummaryInformation;   	
			psci->scid.pid = PIDASI_TIMELENGTH;			
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 9;                  

			wcsncpy(psci->wszTitle, L"Length", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Length in millseconds", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 13:
		{
			// FMTID_AudioSummaryInformation Property Set
			psci->scid.fmtid = FMTID_AudioSummaryInformation;   	
			psci->scid.pid = PIDASI_SAMPLE_RATE;			 //Hz
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 9;                  

			wcsncpy(psci->wszTitle, L"Sample Rate", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Sample Rate in Hz", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 14:
		{
			// FMTID_AudioSummaryInformation Property Set
			psci->scid.fmtid = FMTID_AudioSummaryInformation;   	
			psci->scid.pid = PIDASI_SAMPLE_SIZE;
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 9;                  

			wcsncpy(psci->wszTitle, L"Bit-Depth", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Bit-depth", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 15:
		{
			// FMTID_AudioSummaryInformation Property Set
			psci->scid.fmtid = FMTID_AudioSummaryInformation;   	
			psci->scid.pid = PIDASI_CHANNEL_COUNT;
			psci->vt = VT_UI4;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_INT; 
			// Default col width in chars
			psci->cChars = 9;                  

			wcsncpy(psci->wszTitle, L"Channels", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Channel count", MAX_COLUMN_DESC_LEN);
			break;
		}
		case 16:
		{
			// FMTID_AudioSummaryInformation Property Set
			psci->scid.fmtid = FMTID_AudioSummaryInformation;   			
			psci->scid.pid = PIDVSI_COMPRESSION;			
			psci->vt = VT_LPWSTR;    
			psci->fmt = LVCFMT_LEFT;
			psci->csFlags = SHCOLSTATE_TYPE_STR; 
			// Default col width in chars
			psci->cChars = 32;                  

			wcsncpy(psci->wszTitle, L"Audio Compression", MAX_COLUMN_NAME_LEN);
			wcsncpy(psci->wszDescription, L"Audio Compression used", MAX_COLUMN_DESC_LEN);
			break;
		}
		default:
			return S_FALSE;
			break;		
	}
	return S_OK;

	CRASH_PROTECT_END;
};

STDMETHODIMP CColumnProvider::GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT *pvarData)
{
	ODS("CColumnProvider::GetItemData()");	
	CRASH_PROTECT_START;

	if (pscd->dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE))
		return S_FALSE;
	
	// Look for the filename in our cache.
	MatroskaInfoParser *parser = g_parserCache.Lookup(pscd->wszFile);

	if (parser == NULL)
	{
		//File not in the cache, we have to create a new one
		parser = new MatroskaInfoParser(pscd->wszFile);
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


	if (pscid->fmtid == CLSID_MatroskaColumnProvider_ShellExtension)
	{
		switch (pscid->pid)
		{
			case 0:
			{
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->m_segmentInfo.m_infoWritingApp.c_str());
				break;
			}			
			default:
				//PID not known
				return S_FALSE;
				break;
		}
		return S_OK;
	}	else if (pscid->fmtid == FMTID_SummaryInformation) {
		switch (pscid->pid)
		{			
			case PIDSI_SUBJECT:
			{				
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->tags.FindSimpleTag("SUBJECT").c_str());
				break;
			}
			case PIDSI_TITLE:
			{
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->GetTitle().c_str());
				break;
			}
			case PIDSI_AUTHOR:
			{
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->GetTagArtist().c_str());
				break;
			}
			case PIDSI_KEYWORDS:
			{
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->tags.FindSimpleTag("KEYWORDS").c_str());
				break;
			}
			case PIDSI_COMMENTS:
			{
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->GetTagComment().c_str());
				break;
			}
			case PIDSI_APPNAME:
			{
				pvarData->vt = VT_BSTR;
				pvarData->bstrVal = SysAllocString(parser->m_segmentInfo.m_infoMuxingApp.c_str());
				break;
			}
			default:
				//PID not known
				return S_FALSE;
				break;
		};
		return S_OK;
	}	else if (pscid->fmtid == FMTID_VideoSummaryInformation) {
		switch (pscid->pid)
		{			
			case PIDVSI_FRAME_WIDTH:
			{			
				MatroskaTrackInfo *videoTrack = parser->GetFirstVideoTrack();
				if (videoTrack != NULL) {
					pvarData->vt = VT_UI4;
					pvarData->ulVal = videoTrack->GetVideoInfo()->video_Width;
				}
				break;
			}
			case PIDVSI_FRAME_HEIGHT:
			{
				MatroskaTrackInfo *videoTrack = parser->GetFirstVideoTrack();
				if (videoTrack != NULL) {
					pvarData->vt = VT_UI4;
					pvarData->ulVal = videoTrack->GetVideoInfo()->video_Height;
				}
				break;
			}
			case PIDVSI_TIMELENGTH:
			{
				pvarData->vt = VT_UI4;
				pvarData->ulVal = (ULONG)parser->m_segmentInfo.m_infoDuration;			
				break;
			}
			case PIDVSI_FRAME_RATE:
			{
				MatroskaTrackInfo *videoTrack = parser->GetFirstVideoTrack();
				if (videoTrack != NULL) {
					pvarData->vt = VT_UI4;
					pvarData->ulVal = (ULONG)videoTrack->GetVideoInfo()->video_FrameRate;
				}
				break;
			}
			case PIDVSI_COMPRESSION:
			{
				MatroskaTrackInfo *videoTrack = parser->GetFirstVideoTrack();
				if (videoTrack != NULL) {
					pvarData->vt = VT_LPWSTR;
					JString codecString = videoTrack->GetCodecID();
					if (videoTrack->m_CodecOldID.length() > 0)
						codecString = codecString + L" " + videoTrack->m_CodecOldID.c_str();
					pvarData->bstrVal = SysAllocString(codecString.c_str());
				}
				break;
			}			
			default:
				//PID not known
				return S_FALSE;
				break;
		};
		return S_OK;
	}	else if (pscid->fmtid == FMTID_AudioSummaryInformation) {
		switch (pscid->pid)
		{			
			case PIDASI_TIMELENGTH:
			{
				pvarData->vt = VT_UI4;
				pvarData->ulVal = (ULONG)parser->m_segmentInfo.m_infoDuration;			
				break;
			}
			case PIDASI_SAMPLE_RATE:
			{
				MatroskaTrackInfo *audioTrack = parser->GetFirstAudioTrack();
				if (audioTrack != NULL) {
					pvarData->vt = VT_UI4;
					pvarData->ulVal = (ULONG)audioTrack->GetAudioInfo()->audio_SampleRate;
				}
				break;
			}
			case PIDASI_SAMPLE_SIZE:
			{
				MatroskaTrackInfo *audioTrack = parser->GetFirstAudioTrack();
				if (audioTrack != NULL) {
					pvarData->vt = VT_UI4;
					pvarData->ulVal = audioTrack->GetAudioInfo()->audio_BitDepth;
				}
				break;
			}			
			case PIDASI_CHANNEL_COUNT:
			{
				MatroskaTrackInfo *audioTrack = parser->GetFirstAudioTrack();
				if (audioTrack != NULL) {
					pvarData->vt = VT_UI4;
					pvarData->ulVal = audioTrack->GetAudioInfo()->audio_Channels;
				}
				break;
			}
			case PIDASI_COMPRESSION:
			{
				MatroskaTrackInfo *audioTrack = parser->GetFirstAudioTrack();
				if (audioTrack != NULL) {
					pvarData->vt = VT_LPWSTR;
					JString codecString = audioTrack->GetCodecID();
					if (audioTrack->m_CodecOldID.length() > 0)
						codecString = codecString + L" " + audioTrack->m_CodecOldID.c_str();
					pvarData->bstrVal = SysAllocString(codecString.c_str());
				}
				break;
			}			
			default:
				//PID not known
				return S_FALSE;
				break;
		};
		return S_OK;
	}

	CRASH_PROTECT_END;

	return S_FALSE;
};


