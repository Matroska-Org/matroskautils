/*
 *  Matroska Shell Extension
 *
 *  MatroskaProp.h
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
    \file MatroskaProp.h
		\version \$Id: MatroskaProp.h,v 1.22 2004/03/07 08:08:49 jcsston Exp $
    \brief This is the meat code of the Matroska Shell Extension
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#ifndef _MATROSKAPROP_H
#define _MATROSKAPROP_H

#include "CxImage/ximage.h"

#include "MatroskaPages.h"
#include <list>
#include <memory>
//#include "streams.h"
//#include "Dshow.h"
//#include "Qedit.h"
//#include "Dvdmedia.h"
#include "MatroskaInfoParserCache.h"
#include "MatroskaPropShellExtension.h"

#ifdef USE_MUXER
//Matroska Muxer CLISD
//[uuid("1E1299A2-9D42-4F12-8791-D79E376F4143")]
DEFINE_GUID(CLSID_MatroskaMuxer, 0x1E1299A2, 0x9D42, 0x4F12, 0x87, 0x91, 0xD7, 0x9E, 0x37, 0x6F, 0x41, 0x43);
#endif // USE_MUXER

// The class ID of this Shell extension class:
// {780BCB64-0CAF-473c-A9FC-E08C03D75515}
#define MatroskaPageCLSID _T("{780BCB64-0CAF-473c-A9FC-E08C03D75515}")
DEFINE_GUID(CLSID_MatroskaPage_ShellExtension, 0x780bcb64, 0xcaf, 0x473c, 0xa9, 0xfc, 0xe0, 0x8c, 0x3, 0xd7, 0x55, 0x15);

// {78DC191E-EFC1-4532-9A71-224577A86A7D}
#define MatroskaThumbnailCLSID _T("{78DC191E-EFC1-4532-9A71-224577A86A7D}")
DEFINE_GUID(CLSID_MatroskaThumbnail_ShellExtension, 0x78dc191e, 0xefc1, 0x4532, 0x9a, 0x71, 0x22, 0x45, 0x77, 0xa8, 0x6a, 0x7d);

// {794D04CA-70AC-4020-80EB-FFD59DEF8027}
#define MatroskaTooltipCLSID _T("{794D04CA-70AC-4020-80EB-FFD59DEF8027}")
DEFINE_GUID(CLSID_MatroskaTooltip_ShellExtension, 0x794d04ca, 0x70ac, 0x4020, 0x80, 0xeb, 0xff, 0xd5, 0x9d, 0xef, 0x80, 0x27);

// {781395AF-A127-469f-A06F-59B482AF4F3F}
#define MatroskaColumnProviderCLSID _T("{781395AF-A127-469f-A06F-59B482AF4F3F}")
DEFINE_GUID(CLSID_MatroskaColumnProvider_ShellExtension, 0x781395af, 0xa127, 0x469f, 0xa0, 0x6f, 0x59, 0xb4, 0x82, 0xaf, 0x4f, 0x3f);

// {789111D8-68A3-46a3-9663-145A3FF4C9C9}
#define MatroskaContextMenuCLSID _T("{789111D8-68A3-46a3-9663-145A3FF4C9C9}")
DEFINE_GUID(CLSID_MatroskaContextMenu_ShellExtension, 0x789111d8, 0x68a3, 0x46a3, 0x96, 0x63, 0x14, 0x5a, 0x3f, 0xf4, 0xc9, 0xc9);

// {78678AF0-0E87-4958-A99B-A23FAFC0D92D}
#define MatroskaShellIconCLSID _T("{78678AF0-0E87-4958-A99B-A23FAFC0D92D}")
DEFINE_GUID(CLSID_MatroskaShellIcon_ShellExtension, 
0x78678af0, 0xe87, 0x4958, 0xa9, 0x9b, 0xa2, 0x3f, 0xaf, 0xc0, 0xd9, 0x2d);

STDAPI DllRegisterServer(void);
STDAPI DllUnregisterServer(void);
STDAPI GetMatroskaPropVersion(void);
STDAPI ConfigureOptions(void);
STDAPI VistUnicowsWebsite(void);

class MatroskaProp
{
public:
	MatroskaProp();
	~MatroskaProp();	
	int Install();
	int Uninstall();

	int ChooseIcon();

protected:
	void SetupKeys();
	void LoadKeyList();

	bool m_Installed;
	bool m_Uninstalled;
	TCHAR dllFilename[MAX_MATROSKA_PATH];
	TCHAR *key_list[28][3];
	TCHAR *supported_ext[5];
	TCHAR *ext_key_list[4][3];
	TCHAR *approved_list[7][2];
	std::list<JString> m_keyList;
};

class CShellExtClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CShellExtClassFactory();
	~CShellExtClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;

class CShellExt : public IShellExtInit, IShellPropSheetExt, MatroskaPropShellExtension
{
public:
	TCHAR szFile[MAX_MATROSKA_PATH];
	//VSAP

protected:
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;
	MatroskaPages *pageData;

public:
	CShellExt();
	~CShellExt();

	//IUnknown members
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	//IShellExtInit methods
	STDMETHODIMP            Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

	//IShellPropSheetExt methods
	STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);

	STDMETHODIMP ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam);

};
typedef CShellExt *LPCSHELLEXT;

class CToolTipClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CToolTipClassFactory();
	~CToolTipClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CToolTipClassFactory *LPCTOOLTIPCLASSFACTORY;

class CToolTip : public IQueryInfo, IPersistFile, MatroskaPropShellExtension
{
public:	
	CToolTip();
	~CToolTip();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IPersistFile methods
	STDMETHODIMP GetClassID(CLSID *pCLSID) {ODS("CToolTip::GetClassID()"); return E_NOTIMPL;};
	STDMETHODIMP Load(LPCOLESTR pOleStr, DWORD dwMode);
	STDMETHODIMP IsDirty(void) { ODS("CToolTip::IsDirty()"); return E_NOTIMPL;};        
	/// \param pszFileName [unique][in]
	/// \param fRemember [in]
  STDMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember) { ODS("CToolTip::Save()"); return E_NOTIMPL;};  
	/// \param pszFileName [unique][in]
  STDMETHODIMP SaveCompleted(LPCOLESTR pszFileName) { ODS("CToolTip::SaveCompleted()"); return E_NOTIMPL;};  
	/// \param ppszFileName [out]
  STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName) { ODS("CToolTip::GetCurFile()"); return E_NOTIMPL;};

	//IToolTip methods
	STDMETHODIMP GetInfoFlags(DWORD *pdwFlags);
	STDMETHODIMP GetInfoTip(DWORD dwFlags, LPWSTR *ppwszTip);

	DWORD m_dwMode;	

	wchar_t m_szFileName[MAX_MATROSKA_PATH];

protected:
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;
};
typedef CToolTip *LPCTOOLTIP;

class CThumbnailClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CThumbnailClassFactory();
	~CThumbnailClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CThumbnailClassFactory *LPCTHUMBNAILCLASSFACTORY;

class CThumbnail : public IExtractImage, IPersistFile, IRunnableTask, MatroskaPropShellExtension
{
public:	
	CThumbnail();
	~CThumbnail();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IPersistFile methods
	STDMETHODIMP GetClassID(CLSID *pCLSID) {ODS("CThumbnail::GetClassID()"); return E_NOTIMPL;};
	STDMETHODIMP Load(LPCOLESTR pOleStr, DWORD dwMode);
	STDMETHODIMP IsDirty(void) { ODS("CThumbnail::IsDirty()"); return E_NOTIMPL;};        
	/// \param pszFileName [unique][in]
	/// \param fRemember [in]
  STDMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember) { ODS("CThumbnail::Save()"); return E_NOTIMPL;};  
	/// \param pszFileName [unique][in]
  STDMETHODIMP SaveCompleted(LPCOLESTR pszFileName) { ODS("CThumbnail::SaveCompleted()"); return E_NOTIMPL;};  
	/// \param ppszFileName [out]
  STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName) { ODS("CThumbnail::GetCurFile()"); return E_NOTIMPL;};

	//IRunnableTask methods
	STDMETHODIMP Run() { ODS("CThumbnail::Run()"); return E_NOTIMPL;};        
	STDMETHODIMP Kill(BOOL fWait) { ODS("CThumbnail::Kill()"); return E_NOTIMPL;};
	STDMETHODIMP Suspend() { ODS("CThumbnail::Suspend()"); return E_NOTIMPL;};        
	STDMETHODIMP Resume() { ODS("CThumbnail::Resume()"); return E_NOTIMPL;};
	STDMETHODIMP_(ULONG) IsRunning() { ODS("CThumbnail::IsRunning()"); return m_ThreadState;};

	//IExtractImage methods
	/// \param pszPathBuffer [size_is][out]
	/// \param cch [in] 
	/// \param pdwPriority [unique][out][in]
	/// \param prgSize [in] 
	/// \param dwRecClrDepth [in]
	/// \param pdwFlags [in]
  STDMETHODIMP GetLocation(LPWSTR pszPathBuffer, DWORD cch, DWORD *pdwPriority, const SIZE *prgSize, DWORD dwRecClrDepth, DWORD *pdwFlags);
  /// \param phBmpThumbnail  [out]
  STDMETHODIMP Extract(HBITMAP *phBmpThumbnail);

	DWORD m_dwMode;
	wchar_t m_szFileName[MAX_MATROSKA_PATH];

protected:
	ULONG m_ThreadState;
	DWORD m_height;
	DWORD m_width;
	DWORD m_color_depth;

	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;
};
typedef CThumbnail *LPCTHUMBNAIL;

class CColumnProviderClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CColumnProviderClassFactory();
	~CColumnProviderClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CColumnProviderClassFactory *LPCCOLUMNPROVIDERCLASSFACTORY;

class CColumnProvider : public IColumnProvider, MatroskaPropShellExtension
{
public:
	CColumnProvider();
	~CColumnProvider();

	//IUnknown members
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

  // IColumnProvider methods
  STDMETHODIMP Initialize(LPCSHCOLUMNINIT psci);
  STDMETHODIMP GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO *psci);
  STDMETHODIMP GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT *pvarData);

protected:	
	/// Filename
	TCHAR m_szFile[MAX_MATROSKA_PATH];
	wchar_t m_szFolder[MAX_PATH];
	
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;
};
typedef CColumnProvider *LPCCOLUMNPROVIDER;

class CContextMenuClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CContextMenuClassFactory();
	~CContextMenuClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CContextMenuClassFactory *LPCCONTEXTMENUCLASSFACTORY;

struct MatroskaAttachmentMenuItem {
	HMENU hmAttachmentItem;
	UINT uPosition;
	UINT uIDNewItem;
	MatroskaAttachmentItem *attachmentSource;
};

struct MatroskaAttachmentMenuClickItem {
	UINT uItemID;
	MatroskaAttachmentItem *attachmentSource;
};

class CContextMenu : public IShellExtInit, IContextMenu3, MatroskaPropShellExtension
{
public:
	CContextMenu();
	~CContextMenu();

	//IUnknown members
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	//IShellExtInit methods
	STDMETHODIMP            Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

	// IContextMenu methods
	STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
	STDMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);

	// IContextMenu2 methods
	STDMETHODIMP HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// IContextMenu3 methods
	STDMETHODIMP HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);

	// Msg handler
	STDMETHODIMP MenuMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
protected:
	//Internal Functions
	void AddChapterItem_Menu(HMENU parent, MatroskaChapterEntry *chapter, std::wstring chapterTitle, UINT &idCmd, UINT &indexMenu);
	std::vector<UINT> m_TrackCmdID;
	std::vector<MatroskaAttachmentMenuClickItem> m_AttachmentCmdID;
	
	std::vector<MatroskaAttachmentMenuItem *> m_Attachments;
	/// Filename
	TCHAR m_szFile[MAX_MATROSKA_PATH];
	
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;

};
typedef CContextMenu *LPCCONTEXTMENU;

class CShellIconClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CShellIconClassFactory();
	~CShellIconClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CShellIconClassFactory *LPCSHELLICONCLASSFACTORY;

class CShellIcon : public IPersistFile, IExtractIcon, MatroskaPropShellExtension
{
public:
	CShellIcon();
	~CShellIcon();

	//IUnknown members
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	//IPersistFile methods
	STDMETHODIMP GetClassID(CLSID *pCLSID) {ODS("CShellIcon::GetClassID()"); return E_NOTIMPL;};
	STDMETHODIMP Load(LPCOLESTR pOleStr, DWORD dwMode);
	STDMETHODIMP IsDirty(void) { ODS("CShellIcon::IsDirty()"); return E_NOTIMPL;};        
	/// \param pszFileName [unique][in]
	/// \param fRemember [in]
  STDMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember) { ODS("CShellIcon::Save()"); return E_NOTIMPL;};  
	/// \param pszFileName [unique][in]
  STDMETHODIMP SaveCompleted(LPCOLESTR pszFileName) { ODS("CShellIcon::SaveCompleted()"); return E_NOTIMPL;};  
	/// \param ppszFileName [out]
  STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName) { ODS("CShellIcon::GetCurFile()"); return E_NOTIMPL;};

	// IExtractIcon methods  
	STDMETHODIMP GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags);
  STDMETHODIMP Extract(LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize);

protected:
	DWORD m_dwMode;
	wchar_t m_szFileName[MAX_MATROSKA_PATH];


	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;
};
typedef CShellIcon *LPCSHELLICON;

#ifdef USE_MUXER
enum MatroskaSimpleMuxerModes
{
	MatroskaMux_DShow,
	MatroskaMux_mkvmerge
};
///This is a simple class to mux files into Matroska via DShow, mkvmerge
class MatroskaSimpleMuxer
{
public:
	MatroskaSimpleMuxer(enum MatroskaSimpleMuxerModes type = MatroskaMux_DShow);
	~MatroskaSimpleMuxer();
	HRESULT AddInputFile(JString &inputFilename);
	HRESULT MuxToMatroska();

protected:
	HRESULT MuxViaDShow();
	HRESULT MuxViaMkvmerge();

	enum MatroskaSimpleMuxerModes muxerType;
	std::vector<JString> inputFilenames;
	JString outputFilename;
};
#endif // USE_MUXER

#endif // _MATROSKAPROP_H
