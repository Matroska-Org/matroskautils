/*
 *  Part of the Matroska Shell Extension
 *
 *  DShowTools.cpp
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
    \file CxImageRenderer.cpp
		\version \$Id: DShowTools.cpp,v 1.4 2004/03/20 05:05:33 jcsston Exp $
    \brief A DirectShow Video Renderer that renders the image to a CxImage
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#include "DShowTools.h"

#include "streams.h"
#include "Qedit.h"
#include "Dvdmedia.h"

#include "convert.h"

/// Find a filter by it's CLISD, add it to the graph and return a pointer to the IBaseFilter of the filter
/// \param pGraph Pointer to the Filter Graph Manager.
/// \param clsid CLSID of the filter to create.
/// \param wszName A name for the filter.
/// \param ppF Receives a pointer to the filter.
HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF);

HRESULT FindOutputPin(IBaseFilter *pFilter, IPin **ppPin);
HRESULT FindInputPin(IBaseFilter *pFilter, IPin **ppPin);

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveGraphFromRot(DWORD pdwRegister);

[uuid("5071DDEB-BFE3-48D8-9827-F2D9D6791701")]
class CxImageRenderer : public CBaseRenderer
{
public:
	CxImageRenderer(LPUNKNOWN pUnk, HRESULT* phr);
	HRESULT CheckMediaType(const CMediaType* pmt);
	HRESULT DoRenderSample(IMediaSample *pSample);
	CxImage &GetImage();
	bool GetImageOk();

protected:
	bool m_ImageValid;
	CxImage lastImage;
};

#define CRASH_PROTECT_START
#define CRASH_PROTECT_END

CFactoryTemplate g_Templates[] = 
{
  {	NULL,
	NULL,
	NULL,
	NULL,
	NULL}	
};

// Count of objects listed in g_cTemplates
int g_cTemplates = 0;//sizeof(g_Templates) / sizeof(g_Templates[0]);

extern "C" HRESULT DSHOWTOOLS_DLL_EXPORT GetDShowPreview(LPCWSTR filename, DWORD second_offset, DWORD timeout, DWORD mode, CxImage *image)
{
	DWORD lastLine;

	if (filename == NULL)
		return E_POINTER;
	if (image == NULL)
		return E_POINTER;

	if (mode == 1) {
		//Use DShow (with Internal Renderer) to get a frame of the video
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr))	
			return E_FAIL;
#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		IGraphBuilder *pGraph;
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		IBaseFilter *pInputSource;
		hr = pGraph->AddSourceFilter(filename, filename, &pInputSource);
		if (FAILED(hr))
			return E_FAIL;
#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		// Crashes here on filewalkers system
		IPin *pSourceOutPin;
		hr = FindOutputPin(pInputSource, &pSourceOutPin);
		if (FAILED(hr))
			return E_FAIL;
#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		IBaseFilter *pPictureRender = new CxImageRenderer(NULL, &hr);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		hr = pGraph->AddFilter(pPictureRender, L"Render to Bitmap");
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		IPin *pRenderInPin;
		hr = FindInputPin(pPictureRender, &pRenderInPin);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		hr = pGraph->Connect(pSourceOutPin, pRenderInPin);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		//Now we try to render the graph
		IMediaControl *pControl;
		hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
		if (FAILED(hr))
			return E_FAIL;

		IMediaSeeking *pSeeking;
		hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pSeeking);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		hr = pControl->Pause();
		if (FAILED(hr))
			return E_FAIL;

		OAFilterState fs;
		hr = pControl->GetState(INFINITE, &fs);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		REFERENCE_TIME rtCurrent = 10000000i64;
		rtCurrent = rtCurrent * second_offset;
		hr = pSeeking->SetPositions(&rtCurrent, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		if (FAILED(hr))
			return E_FAIL;

#ifdef _DEBUG
		lastLine = __LINE__;
#endif

		DWORD startTime = GetTickCount();
		DWORD thumbnailWaitTime = timeout * 1000;
		while ( !((CxImageRenderer *)pPictureRender)->GetImageOk() )
		{				
			hr = pControl->Pause();				
			hr = pControl->Run();	
			Sleep(200);
			// If we don't have a thumbnail in 5 minutes I doubt we will ever have one
			if (GetTickCount() - startTime > thumbnailWaitTime)
				break;
		};
#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		if (((CxImageRenderer *)pPictureRender)->GetImageOk())
		{
			CxImage &myImage = ((CxImageRenderer *)pPictureRender)->GetImage();
			*image = myImage;
		}
#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		// Free the DShow interfaces
		ULONG refCount = 0;
		refCount = pControl->Release();
		refCount = pSeeking->Release();
		refCount = pRenderInPin->Release();	
		refCount = pSourceOutPin->Release();
		//refCount = pPictureRender->Release();			
		refCount = pInputSource->Release();
		refCount = pGraph->Release();
		CoUninitialize();
#ifdef _DEBUG
		lastLine = __LINE__;
#endif
		
		return NOERROR;
	} else if (mode == 2) {
		//Use DShow to get a frame of the video, using IMediaDet
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr))	
			return E_FAIL;

		IMediaDet *pMediaDet;
		hr = CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **)&pMediaDet);
		if (FAILED(hr))
			return E_FAIL;
		
		hr = pMediaDet->put_Filename((BSTR)filename);
		if (FAILED(hr))
			return E_FAIL;

		long streamCount = 0;
		hr = pMediaDet->get_OutputStreams(&streamCount);
		if (FAILED(hr))
			return E_FAIL;

		AM_MEDIA_TYPE streamInfo = { 0 };
		for (int s = 0; s < streamCount; s++)
		{
			hr = pMediaDet->put_CurrentStream(s);
			hr = pMediaDet->get_StreamMediaType(&streamInfo);
			if (streamInfo.majortype == MEDIATYPE_Video)
				break;
		}
		// Determine the correct size
		VIDEOINFOHEADER *videoHeader = (VIDEOINFOHEADER *)streamInfo.pbFormat;
		image->Create(videoHeader->bmiHeader.biWidth, videoHeader->bmiHeader.biHeight, 24);

		//Alloc our buffer for the bitmap
		long bitBufferSize = videoHeader->bmiHeader.biWidth * videoHeader->bmiHeader.biHeight * 24;

		hr = pMediaDet->GetBitmapBits(second_offset, &bitBufferSize, (char *)image->GetBits(), videoHeader->bmiHeader.biWidth, videoHeader->bmiHeader.biHeight);
		if (FAILED(hr))
			return E_FAIL;

		// Free the DShow interfaces
		pMediaDet->Release();
		CoUninitialize();

	} else {
		return E_INVALIDARG;
	}
	return S_OK;
}

extern "C" HRESULT DSHOWTOOLS_DLL_EXPORT PlayFile(LPCWSTR filename, DWORD mode)
{
	HRESULT hr;
	IGraphBuilder *graphBuilder;
	DWORD dwRegister;
	IMediaControl *pMediaControl;

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graphBuilder);

	if (FAILED(hr))
		return hr;

	hr = AddGraphToRot(graphBuilder, &dwRegister);

	hr = graphBuilder->RenderFile(filename, NULL);

	hr = graphBuilder->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);

	hr = pMediaControl->Run();

	return hr;
}

HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF)
{
	if (!pGraph || ! ppF) return E_POINTER;
	*ppF = 0;
	IBaseFilter *pF = 0;
	HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pF));
	if (SUCCEEDED(hr))
	{
		hr = pGraph->AddFilter(pF, wszName);
		if (SUCCEEDED(hr))
			*ppF = pF;
		else
			pF->Release();
	}
	return hr;
}

HRESULT FindOutputPin(IBaseFilter *pFilter, IPin **ppPin)
{
  CRASH_PROTECT_START;

	if (!pFilter || ! ppPin)
		return E_POINTER;

	*ppPin = 0;
	HRESULT hr;
	//Find the output pin of the Source Filter
	IEnumPins *pPinEnum;
	hr = pFilter->EnumPins(&pPinEnum);
	if (FAILED(hr))
	  return E_FAIL;
	IPin *pSearchPin;
	while (pPinEnum->Next(1, &pSearchPin, NULL) == S_OK)
	{
		PIN_DIRECTION pPinDir;
		hr = pSearchPin->QueryDirection(&pPinDir);
		if (FAILED(hr))
		  return E_FAIL;
		if (pPinDir == PINDIR_OUTPUT)
		{
			//Found out pin
			*ppPin = pSearchPin;
			break;
		}
	}
	pPinEnum->Release();
	return hr;

  CRASH_PROTECT_END;
}

HRESULT FindInputPin(IBaseFilter *pFilter, IPin **ppPin)
{
  CRASH_PROTECT_START;

	if (!pFilter || ! ppPin)
		return E_POINTER;

	*ppPin = 0;
	HRESULT hr;
	//Find the output pin of the Source Filter
	IEnumPins *pPinEnum;
	hr = pFilter->EnumPins(&pPinEnum);
	if (FAILED(hr))
	  return E_FAIL;

	IPin *pSearchPin;
	while (pPinEnum->Next(1, &pSearchPin, NULL) == S_OK)
	{
		PIN_DIRECTION pPinDir;
		hr = pSearchPin->QueryDirection(&pPinDir);
		if (FAILED(hr))
	    return E_FAIL;
		if (pPinDir == PINDIR_INPUT)
		{
			//Found out pin
			*ppPin = pSearchPin;
			break;
		}
	}
	pPinEnum->Release();
	return hr;

	CRASH_PROTECT_END;
}

// {004EB4C4-3EB3-462a-8626-07A44F4845C4}
static const GUID CxImageRenderer_GUID = 
{ 0x4eb4c4, 0x3eb3, 0x462a, { 0x86, 0x26, 0x7, 0xa4, 0x4f, 0x48, 0x45, 0xc4 } };


CxImageRenderer::CxImageRenderer(LPUNKNOWN pUnk, HRESULT* phr)
	: CBaseRenderer(CxImageRenderer_GUID, NAME("CxImageRenderer"), pUnk, phr)
{
	NOTE("CxImageRenderer::CxImageRenderer()");
	*phr = S_OK;
	m_ImageValid = false;
};

HRESULT CxImageRenderer::CheckMediaType(const CMediaType* pmt)
{
	NOTE("CxImageRenderer::CheckMediaType()");
	//Check the stream type
	bool bIsVideo = (pmt->majortype == MEDIATYPE_Video) 
		&& ((pmt->formattype == FORMAT_VideoInfo) 
		|| (pmt->formattype == FORMAT_VideoInfo2));		
	//Check the colorspace
	bool bColorspaceOk =
		pmt->subtype == MEDIASUBTYPE_RGB32
		|| pmt->subtype == MEDIASUBTYPE_RGB24
		|| pmt->subtype == MEDIASUBTYPE_YUY2
		|| pmt->subtype == MEDIASUBTYPE_YV12 // YV12 is b0rked
		;
	if (bIsVideo && bColorspaceOk)
		return S_OK;
	else
		return E_FAIL;
};

HRESULT CxImageRenderer::DoRenderSample(IMediaSample *pSample)
{
	NOTE("CxImageRenderer::DoRenderSample()");

	CMediaType mt;
	m_pInputPin->ConnectionMediaType(&mt);
	BITMAPINFOHEADER bih;		

	//See if we are using VideoHeader2
	if (mt.formattype == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2 *vih2 = (VIDEOINFOHEADER2*)mt.Format();
		bih = vih2->bmiHeader;
	} else {
		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*)mt.Format();
		bih = vih->bmiHeader;
	}
	if (bih.biSize == 0) {
		// A hack for a strange bug, it's VideoHeader2 but it's not?
		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*)mt.Format();
		bih = vih->bmiHeader;
	}
	BYTE* p;
	pSample->GetPointer(&p);
	long dataSize = pSample->GetSize();

	//Do something with the pic
	if (mt.subtype == MEDIASUBTYPE_RGB32) {
		lastImage.CreateFromARGB(bih.biWidth, bih.biHeight, p);
		lastImage.Flip();
		m_ImageValid = lastImage.IsValid();						
	} else if (mt.subtype == MEDIASUBTYPE_RGB24) {				
		lastImage.Create(bih.biWidth, bih.biHeight, 24);					
		// Do a direct memory copy
		memcpy(lastImage.GetBits(), p, bih.biWidth * bih.biHeight * 3);
		m_ImageValid = lastImage.IsValid();						
	// YV12 is b0rked
	/*} else if (mt.subtype == MEDIASUBTYPE_YV12) {				
		BYTE *yuvBuffer = new BYTE[bih.biHeight * bih.biWidth * 32];
		
		// Call Klaus Post's mmx YV12->YUY2 conversion routine
		BYTE *y = p;
		BYTE *u = y + (bih.biHeight/2 * bih.biWidth/2);
		//BYTE *u = y + (bih.biHeight * bih.biWidth);
		BYTE *v = u + (bih.biHeight/2 * bih.biWidth/2);

		yv12_to_rgb32_mmx(yuvBuffer,
			bih.biWidth*4,
			y,
			u, 
			v, 
			bih.biWidth,
			bih.biWidth/2,
			bih.biWidth,
			bih.biHeight);

		lastImage.CreateFromARGB(bih.biWidth, bih.biHeight, yuvBuffer);		
		//lastImage.Create(bih.biWidth, bih.biHeight, 24);					
		
		//Using alexnoe's YUV2->RGB24 Assembly rountines
		//YUV422toRGB24_MMX(yuvBuffer, lastImage.GetBits(), 0, bih.biWidth, bih.biHeight, bih.biWidth * 2, bih.biWidth * 3);
		
		//memcpy(lastImage.GetBits(), yuvBuffer, bih.biWidth * bih.biHeight * 3);
		
		m_ImageValid = lastImage.IsValid();						
		lastImage.Draw(GetDC(NULL));
		delete [] yuvBuffer;
*/
	}else if (mt.subtype == MEDIASUBTYPE_YUY2) {				
		lastImage.Create(bih.biWidth, bih.biHeight, 24);					
		//Using alexnoe's YUV2->RGB24 Assembly rountines
		YUV422toRGB24_MMX(p, lastImage.GetBits(), 0, bih.biWidth, bih.biHeight, bih.biWidth * 2, bih.biWidth * 3);
		//lastImage.Draw(GetDC(NULL));
		lastImage.Flip();
		m_ImageValid = lastImage.IsValid();						
	}

	//We only need one good sample
	m_bAbort = true;

	return S_OK;
};

CxImage &CxImageRenderer::GetImage()
{
	NOTE("CxImageRenderer::GetImage()");
	return lastImage;
};

bool CxImageRenderer::GetImageOk()
{
	NOTE("CxImageRenderer::GetImageOk()");
	return m_ImageValid;
}

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	if (FAILED(GetRunningObjectTable(0, &pROT))) 
	{
		return E_FAIL;
	}

	WCHAR wsz[128];
	wsprintfW(wsz, L"FilterGraph %08x pid %08x - AudioDShowEncoder", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());

	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr)) 
	{
		// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
		// to the object.  Using this flag will cause the object to remain
		// registered until it is explicitly revoked with the Revoke() method.
		//
		// Not using this flag means that if GraphEdit remotely connects
		// to this graph and then GraphEdit exits, this object registration 
		// will be deleted, causing future attempts by GraphEdit to fail until
		// this application is restarted or until the graph is registered again.
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, pMoniker, pdwRegister);
		pMoniker->Release();
	}

	pROT->Release();
	return hr;
}

void RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}