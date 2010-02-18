
// 
// Sample crawler capture component. This component registers for .lnk files
// 

#include "stdafx.h"
#include <atlstr.h>
#include <atlsafe.h>
#include <shobjidl.h>
#include <shlguid.h>
#include "IndexMatroska.h"
#include "../common/GoogleDesktopSearchAPI.h"

#include "GDCMatroska_i.c"

// CIndexMatroska

CIndexMatroska::CIndexMatroska() 
{
  
}

CString CIndexMatroska::GetRegistryValue(HKEY hBaseKey, TCHAR *reg_key, TCHAR *value_key, TCHAR *default_value)
{	
  CString ret_value = default_value;
  HKEY key_handle = NULL;
  DWORD lpType = NULL;
  DWORD state = 0;
  TCHAR key_text[MAX_PATH+2];
  DWORD size = MAX_PATH;

  RegCreateKeyEx(hBaseKey, reg_key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, &state);  
  if(state == REG_OPENED_EXISTING_KEY) {
    RegQueryValueEx(key_handle, value_key, 0, &lpType, (BYTE*)key_text, &size);	
    ret_value = key_text;
  }
  RegCloseKey(key_handle);
  return ret_value;
}

STDMETHODIMP CIndexMatroska::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IIndexMatroska
  };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP CIndexMatroska::HandleFile(BSTR full_path_to_file, IDispatch *event_factory) 
{
  USES_CONVERSION;
  CComPtr<IGoogleDesktopSearchEventFactory> spEventFactory;
  CComPtr<IDispatch> spEventDisp;  
  HRESULT hr;
  CString cS;

  cS.Format(_T("CIndexMatroska::HandleFile = %s\n"), W2T(full_path_to_file));
  //MessageBox(NULL, cS, _T("GDCMatroska"), 0);
  OutputDebugStr(cS);

  hr  = event_factory->QueryInterface(&spEventFactory);
  if (FAILED(hr))
    return Error(L"No event factory", IID_IIndexMatroska, hr);

  hr = spEventFactory->CreateEvent(CComBSTR(CLSID_IndexMatroska), CComBSTR(L"Google.Desktop.MediaFile"), &spEventDisp);
  if (FAILED(hr))
    return Error(L"Unable to create event", IID_IIndexMatroska, hr);
    
  CComQIPtr<IGoogleDesktopSearchEvent> spEvent(spEventDisp);
  
  ATLASSERT(spEventDisp && spEvent);
  if (spEvent == NULL)
    return Error(L"Event does not implement IGoogleDesktopSearchEvent", IID_IIndexMatroska, E_UNEXPECTED);

  CStringW strOriginal = full_path_to_file;
  if (PathFileExists(strOriginal)) {
    WIN32_FIND_DATA wfd;
    if (!GetFileAttributesEx(strOriginal, GetFileExInfoStandard, &wfd))
      return HRESULT_FROM_WIN32(::GetLastError());

    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&wfd.ftCreationTime, &systemTime);

    // convert the date to the variant format
    double varDate;
    SystemTimeToVariantTime(&systemTime, &varDate);
    
    hr = spEvent->AddProperty(CComBSTR(L"uri"), CComVariant(full_path_to_file));
    if (SUCCEEDED(hr))
      hr = spEvent->AddProperty(CComBSTR(L"last_modified_time"), CComVariant(varDate, VT_DATE));
    
    CStringW content;
    
    Win32Stream	st;
    MatroskaFile *mf;
    char err_msg[256];
    unsigned int i;

    Win32Stream_Init(&st);  
    Win32Stream_OpenW(&st, strOriginal);

    /* initialize matroska parser */
    mf = mkv_OpenEx(&st.base, /* pointer to I/O object */
      0, /* starting position in the file */
      0, /* flags, you can set MKVF_AVOID_SEEKS if this is a non-seekable stream */
      err_msg, sizeof(err_msg)); /* error message is returned here */
    if (mf == NULL) {
      Win32Stream_Close(&st);
      //fprintf(stderr,"Can't parse Matroska file: %s\n", err_msg);
      return E_FAIL;
    }

    int audio_Channels = -1;
    int video_Height = -1;
    int video_Width = -1;
    double duration = 0.0;
    CStringA s;

    SegmentInfo *segInfo = mkv_GetFileInfo(mf);
    if (segInfo != NULL) {
      duration = (double)segInfo->Duration;

      char szBuffer[256];
      szBuffer[0] = 0;
      for (int i = 0; i < sizeof(segInfo->UID); i++) {
        size_t len = strlen(szBuffer);
        _snprintf(&szBuffer[len], 255-len, " 0x%02x", (unsigned char)segInfo->UID[i]);
      }

      s.Format("<ul>"
        "<li><b>Segment Info</b></li>"
        "<ul>"
        "<li>Segment UID:%s</li>"
        "<li>Muxing App: %s</li>"
        "<li>Writing App: %s</li>"
        "<li>Title: %s</li>"
        "<li>Filename: %s</li>"
        "</ul>"
        "</ul>", 
        szBuffer, 
        segInfo->MuxingApp,
        segInfo->WritingApp,
        segInfo->Title != NULL ? segInfo->Title : "(null)",
        segInfo->Filename != NULL ? segInfo->Filename : "(null)");
      content += CA2W(s);
    }
    s.Format("<ul>"
      "<li><b>Track Info (Count: %u)</b></li>", mkv_GetNumTracks(mf));
    content += CA2W(s);  
    /* if track_num was not specified, then list tracks */
    for (i = 0; i < mkv_GetNumTracks(mf); ++i) {      
      TrackInfo	*ti = mkv_GetTrackInfo(mf, i);
      if (ti->Type == TT_VIDEO) {
        if (video_Width == -1) {
          video_Height = ti->Video.PixelHeight;
          video_Width = ti->Video.PixelWidth;
        }
        s.Format("<ul>"
          "<li><b>Track %u</b></li>"
          "<ul>"
          "<li>Type: Video</li>"
          "<li>CodecID: %s</li>"
          "<li>Language: %s</li>"
          "<li>Pixel Size: %ix%i</li>"
          "<li>Display Size: %ix%i</li>"
          "<li>Enabled: %s</li>"
          "<li>Default: %s</li>"
          "<li>Default Duration: %.2fms (%.3f fps)</li>"        
          "</ul>"
          "</ul>", 
          ti->Number, 
          (ti->CodecID != NULL) ? ti->CodecID : "(null)", 
          (ti->Language != NULL) ? ti->Language : "(null)", 
          ti->Video.PixelWidth, ti->Video.PixelHeight, 
          ti->Video.DisplayWidth, ti->Video.DisplayHeight,
          ti->Enabled ? "True" : "False",
          ti->Default ? "True" : "False",
          (double)ti->DefaultDuration / 1000000.0, 1000000000.0 / (double)ti->DefaultDuration);
        content += CA2W(s);

      } else if (ti->Type == TT_AUDIO) {
        if (audio_Channels == -1) {
          audio_Channels = ti->Audio.Channels;
        }
        s.Format("<ul>"
          "<li><b>Track %u</b></li>"
          "<ul>"
          "<li>Type: Audio</li>"
          "<li>CodecID: %s</li>"
          "<li>Language: %s</li>"
          "<li>Channels: %i</li>"
          "<li>Sampling Rate: %.2f</li>"
          "<li>Enabled: %s</li>"
          "<li>Default: %s</li>"
          "<li>Default Duration: %.2fms (%.3f fps)</li>"    
          "</ul>"
          "</ul>", ti->Number, 
          (ti->CodecID != NULL) ? ti->CodecID : "(null)", 
          (ti->Language != NULL) ? ti->Language : "(null)", 
          ti->Audio.Channels, 
          ti->Audio.SamplingFreq,
          ti->Enabled ? "True" : "False",
          ti->Default ? "True" : "False",
          (double)ti->DefaultDuration / 1000000.0, 1000000000.0 / (double)ti->DefaultDuration);
        content += CA2W(s);

      } else if (ti->Type == TT_SUB) {
        s.Format("<ul>"
          "<li><b>Track %u</b></li>"
          "<ul>"
          "<li>Type: Subtitle</li>"
          "<li>CodecID: %s</li>"
          "<li>Language: %s</li>"
          "<li>Enabled: %s</li>"
          "<li>Default: %s</li>"
          "<li>Default Duration: %.2fms (%.3f fps)</li>"    
          "</ul>"
          "</ul>", 
          ti->Number, 
          (ti->CodecID != NULL) ? ti->CodecID : "(null)", 
          (ti->Language != NULL) ? ti->Language : "(null)",
          ti->Enabled ? "True" : "False",
          ti->Default ? "True" : "False",
          (double)ti->DefaultDuration / 1000000.0, 1000000000.0 / (double)ti->DefaultDuration);
        content += CA2W(s);

      } else {
        s.Format("<ul>"
          "<li><b>Track %u</b></li>"
          "<ul>"
          "<li>Type: %i</li>"
          "<li>CodecID: %s</li>"
          "<li>Language: %s</li>"
          "<li>Enabled: %s</li>"
          "<li>Default: %s</li>"
          "<li>Default Duration: %.2fms (%.3f fps)</li>"    
          "</ul>"
          "</ul>", 
          ti->Number, 
          (ti->CodecID != NULL) ? ti->CodecID : "(null)", 
          ti->Type, (ti->Language != NULL) ? ti->Language : "(null)",
          ti->Enabled ? "True" : "False",
          ti->Default ? "True" : "False",
          (double)ti->DefaultDuration / 1000000.0, 1000000000.0 / (double)ti->DefaultDuration);
        content += CA2W(s);
      }
    }
    s.Format("</ul>");
    content += CA2W(s);
    
    hr = spEvent->AddProperty(CComBSTR(L"content"), CComVariant(content));
    hr = spEvent->AddProperty(CComBSTR(L"format"), CComVariant("text/html"));      

    duration = duration * 1000.0 * 1000.0;
    CComVariant llDuration;
    llDuration = (ULONG)duration;
    llDuration.ChangeType(VT_UI8);
    hr = spEvent->AddProperty(CComBSTR(L"length"), llDuration);

    if (video_Width != -1) {
      hr = spEvent->AddProperty(CComBSTR(L"width"), CComVariant((unsigned int)video_Width, VT_UI4));
      hr = spEvent->AddProperty(CComBSTR(L"height"), CComVariant((unsigned int)video_Height, VT_UI4));
    }

    if (audio_Channels != -1) {
      hr = spEvent->AddProperty(CComBSTR(L"channels"), CComVariant((unsigned int)audio_Channels, VT_UI4));
    }

    /* close matroska parser */
    mkv_Close(mf);

    /* close file */
    Win32Stream_Close(&st);

    if (FAILED(hr))
      return Error(L"AddProperty failed", IID_IIndexMatroska, hr);

    hr = E_INVALID_EVENT_FLAGS;
    hr = spEvent->Send(EventFlagIndexable);
    if (FAILED(hr))
      return Error(L"Send failed", IID_IIndexMatroska, hr);
  }
   
  return hr;
}
