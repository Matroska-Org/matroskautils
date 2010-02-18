// IndexShortcut.h : Declaration of the CIndexShortcut

#pragma once
#include "resource.h"       // main symbols

#include "GDCMatroska_h.h"
#include "MatroskaParser.h"
#include "Win32Stream.h"

// CIndexShortcut

class ATL_NO_VTABLE CIndexMatroska : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CIndexMatroska, &CLSID_IndexMatroska>,
  public ISupportErrorInfo,
  public IDispatchImpl<IIndexMatroska, &IID_IIndexMatroska, &LIBID_GDCMatroskaLib, /*wMajor =*/ 1, /*wMinor =*/ 0> {
public:
  CIndexMatroska();

  DECLARE_REGISTRY_RESOURCEID(IDR_INDEXMATROSKA)

  BEGIN_COM_MAP(CIndexMatroska)
    COM_INTERFACE_ENTRY(IIndexMatroska)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

  // ISupportsErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  CString GetRegistryValue(HKEY hBaseKey, TCHAR *reg_key, TCHAR *value_key, TCHAR *default_value);

  HRESULT FinalConstruct() {
    return S_OK;
  }
  
  void FinalRelease() {
  }
  
public:
  // IIndexShortcut implementation
  STDMETHOD(HandleFile)(BSTR full_path_to_file, IDispatch *event_factory);
};

OBJECT_ENTRY_AUTO(__uuidof(IndexMatroska), CIndexMatroska)
