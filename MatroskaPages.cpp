/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MatroskaPages.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
 *  Copyright (C) John Cannon - 2003
 *
 *  CDL API Copyright (C) Blacksun & Toff - 2003
 *
 *  The TCMP Matroska CDL is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  The TCMP Matroska CDL is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The TCMP Matroska CDL; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*!
    \file MatroskaPages.cpp
		\version \$Id: MatroskaPages.cpp,v 1.61 2004/03/08 07:56:51 jcsston Exp $
    \brief This is all the GUI code for the CDL and Shell Ext
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#include "MatroskaPages.h"
#include <Shlwapi.h>

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

using namespace MatroskaUtilsNamespace;

extern long g_MyVersionMajor;
extern long g_MyVersionMinor;
extern long g_MyBuildNumber;
//long g_MyBuildNumber = RESOURCE_VERSION_BUILD;

// Handle to this DLL itself.
extern HINSTANCE g_hmodThisDll;	

MatroskaPages::MatroskaPages()
{
	ODS("MatroskaPages::MatroskaPages()");
	parser = NULL;
	g_bUseUnicodeWinAPI = (LONG)GetVersion() >= 0;
	
	g_hAppInst = NULL;
	g_hTreeDialog = NULL;
	g_hInfoDialog = NULL;
	g_hTagDialog = NULL;
	g_hSimpleTagDialog = NULL;
	g_hAttachmentDialog = NULL;
	g_hChapterDialog = NULL;
	attachedImage = NULL;
	m_RefCount = 0;
}

MatroskaPages::~MatroskaPages()
{
	ODS("MatroskaPages::~MatroskaPages()");
	delete parser;
	if (attachedImage != NULL)
		delete attachedImage;
}

uint16 MatroskaPages::AddRef() {
  return ++m_RefCount;
};

uint16 MatroskaPages::Release() {
	if (--m_RefCount)
		return m_RefCount;

	delete this;
	return 0;
};

MatroskaAboutDialog::MatroskaAboutDialog() {
	//Set the box infomation text
	about_txt.clear();

#ifdef MATROSKA_PROP
	about_txt.append(MATROSKA_SHELL_EXT_APP_NAME);
#ifdef _DEBUG
	about_txt.append(" Debug");
#endif
	about_txt.append(_W(" Build "));
	about_txt.append(JString::Format(L"%i", g_MyBuildNumber));

	about_txt.append("\r\n");
	about_txt.append(_W("Developed by"));
	about_txt.append(" Jory Stone (jcsston@jory.info)\r\n\r\n");
	
	about_txt.append(_W("Reads infomation about a Matroska file and R/W the Tags of a Matroska file"
		"via a Property sheet, also allows Tooltips, Thumbnails, and more for Matroska files in Windows Explorer."));
	about_txt.append("\r\n");
	about_txt.append(_W("Thanks to Pamel for working so hard on the Matroska tagging system."));
	about_txt.append("\r\n");
	about_txt.append(_W("YUV2->RGB32 Assembly Routines by alexnoe."));
#else
	about_txt.append(MATROSKA_CDL_APP_NAME);

#ifdef _DEBUG
	about_txt.append(" Debug");
#endif
	about_txt.append(_W(" Build "));
	about_txt.append(JString::Format(L"%i", g_MyBuildNumber));

	about_txt.append("\r\nDeveloped by Jory Stone (jcsston@toughguy.net) and Inspired by John Cannon (spyder482@yahoo.com)\r\n\r\n"
		"R/W the Tags of a Matroska file and allows auto-setting the AR from the Matroska file."
    "\r\nThanks to Pamel for working so hard on the Matroska tagging system.");
#endif
	about_txt.append("\r\n\n");
	about_txt.append(_W("Compiled on"));
	about_txt.append(" " __TIMESTAMP__);
	about_txt.append("\r\n");
	about_txt.append(_W("Using ..."));
	about_txt.append("\r\nlibebml ");
	about_txt.append(LIBEBML_NAMESPACE::EbmlCodeVersion.c_str());
	about_txt.append("\r\nlibmatroska ");
	about_txt.append(LIBMATROSKA_NAMESPACE::KaxCodeVersion.c_str());
#ifdef USING_CXIMAGE
	about_txt.append("\r\n");
	about_txt.append(CxImage(0).GetVersion());
#endif
#ifdef USING_TINYXML
	about_txt.append("\r\nTinyXml 2.1.5 (with UTF-8 patch)");
#endif

#ifdef USING_CXIMAGE
	about_txt.append("\r\nCFireRoutine by dswigger");
	//Setup the fire area stuff
	fireArea.top = 2;
	fireArea.left = 2;
	fireArea.right = fireWidth+2;
	fireArea.bottom = fireHeight+2;

	myFireBitmap = new DWORD[fireHeight * fireWidth + 10];
	memset(myFireBitmap, 0, fireHeight * fireWidth + 10);

	//Setup the fire object
	myFire.m_iHeight = fireHeight;
	myFire.m_iWidth = fireWidth;
	myFire.InitFire();							

	//Setup our images
#ifdef MATROSKA_PROP
	HMODULE myModule = GetModuleHandle(_T("MatroskaProp.dll"));	
	HRSRC hres = FindResource(myModule, MAKEINTRESOURCE(IDR_PNG_MATROSKA_INFO), _T("PNG"));
#else
	HMODULE myModule = GetModuleHandle(_T("MatroskaCDL.cdl"));
	HRSRC hres = FindResource(myModule, MAKEINTRESOURCE(IDR_PNG_MATROSKA_CDL), _T("PNG"));
#endif	
	HGLOBAL hgImage = LoadResource(myModule, hres);
	myTextImage.Decode((BYTE *)LockResource(hgImage), SizeofResource(myModule, hres), CXIMAGE_FORMAT_PNG);
#endif // USING_CXIMAGE
};

#ifdef USING_CXIMAGE
MatroskaAboutDialog::~MatroskaAboutDialog() {
	delete myFireBitmap;
};

void MatroskaAboutDialog::RenderFire(HWND hAboutDialog) {
	myFire.Render((DWORD*)myFireBitmap, fireWidth, fireHeight);	
	InvalidateRect(hAboutDialog, &fireArea, false);
	myFireImage.CreateFromARGB(fireWidth, fireHeight, (BYTE *)myFireBitmap);
	myFireImage.Mix(myTextImage, CxImage::OpDstCopy);		
};

void MatroskaAboutDialog::PaintFire(HWND hAboutDialog) {
	myFireImage.Draw(GetDC(hAboutDialog), fireArea);
};
#endif // USING_CXIMAGE

BOOL CALLBACK AboutDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	MatroskaAboutDialog *aboutData = (MatroskaAboutDialog *)GetWindowLong(hwndDlg, DWL_USER);	
  switch (uMsg)
  {
		case WM_INITDIALOG:
		{
			try {
				aboutData = new MatroskaAboutDialog();
				// Store tha about dialog data
				SetWindowLong(hwndDlg, DWL_USER, (LONG)aboutData);

				// Display the text
				SetDlgItemText(hwndDlg, IDC_STATIC_ABOUT, aboutData->about_txt.t_str());
				
				SetWindowText(hwndDlg, _W("About"));
				SetDlgItemText(hwndDlg, IDC_BUTTON_ABOUT_OK, _W("OK"));
	#ifdef USING_CXIMAGE
				// Start our fire timer
				SetTimer(hwndDlg, 555, 50, NULL);
	#endif // USING_CXIMAGE
			} catch (...) {
				SetDlgItemText(hwndDlg, IDC_STATIC_ABOUT, _T("Failed to init about dialog"));
				aboutData = NULL;
			}
			break;
		}		
		case WM_TIMER:
		{
#ifdef USING_CXIMAGE
			if (wParam == 555) {
				if (aboutData != NULL)
					aboutData->RenderFire(hwndDlg);
			}
#endif // USING_CXIMAGE
			break;
		}
		case WM_PAINT:
		{			
#ifdef USING_CXIMAGE
			if (aboutData != NULL)
				aboutData->PaintFire(hwndDlg);			
#endif // USING_CXIMAGE			
			break;
		}
		case WM_DESTROY:
		{
			KillTimer(hwndDlg, 555);
			if (aboutData != NULL)
				delete aboutData;
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
		case WM_COMMAND:
			// Process button
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON_ABOUT_LISTEN:
				{
					/*
					UTFstring tmp;
					FILE *temp;
					char *temp_file;
					HRSRC hFile;
					HGLOBAL hgFile;
					void *pFile;
					DWORD dwFile;
						
					hFile = FindResource(g_hmodThisDll, MAKEINTRESOURCE(IDR_MP3_MATROSKA_JORY), _T("MP3"));
					hgFile = LoadResource(g_hmodThisDll, hFile);
					pFile = LockResource(hgFile);
					dwFile = SizeofResource(g_hmodThisDll, hFile);

					temp_file = _tempnam("C:\\", "Matroska_MP3");		
					if ((temp_file != NULL) && (dwFile > 0)) {
						temp = fopen(temp_file, "wb");
						if (temp != NULL) {
							fwrite(pFile, dwFile, 1, temp);
							fclose(temp);
							temp = NULL;
						}
							
						tmp.SetUTF8(temp_file);
						PlayFile(tmp.c_str(), 0);
						unlink(temp_file);
					}
					*/
					break;
				}
				case IDC_BUTTON_ABOUT_OK:
					EndDialog(hwndDlg, IDOK);
					break;
				case IDCANCEL:
					EndDialog(hwndDlg, IDCANCEL);
					break;
			}
			break;
		default:
			break;
			//Nothing for now
	}
  return FALSE;
}

// ----------------------------------------------------------------------------

BOOL CALLBACK TreeDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//TRACE("Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	switch (uMsg)
	{		
		case WM_INITDIALOG:
		{
			//Load the last settings
			CheckDlgButton(hwndDlg, IDC_CHECK_DAR, MatroskaCDL_GetRegistryValue(_T("DAR"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_TAG_TAB, MatroskaCDL_GetRegistryValue(_T("Advanced Tag Tab"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_TAG_TAB_SIMPLE, MatroskaCDL_GetRegistryValue(_T("Simple Tag Tab"), 0));
			break;
		}
		case WM_DESTROY:
		{
			//Save the current settings
			MatroskaCDL_SetRegistryValue(_T("Advanced Tag Tab"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_TAG_TAB));
			MatroskaCDL_SetRegistryValue(_T("Simple Tag Tab"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_TAG_TAB_SIMPLE));
			MatroskaCDL_SetRegistryValue(_T("DAR"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_DAR));
			break;
		}
		case WM_COMMAND:
			break;
	}
	return FALSE;
}

BOOL CALLBACK ConfigurationDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//TRACE("Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	static HWND hwndToolTip = NULL;
	switch (uMsg)
	{				
		case WM_INITDIALOG:
		{
			INITCOMMONCONTROLSEX common;
			common.dwICC = ICC_UPDOWN_CLASS|ICC_WIN95_CLASSES;
			common.dwSize = sizeof(common);
			InitCommonControlsEx(&common);
static HWND hwndToolTip = NULL;
			// Create the tooltip control
			DWORD balloonTips = 0;
			if (MatroskaShellExt_GetRegistryValue(_T("Use Balloon Tooltips"), 1))
				balloonTips = TTS_BALLOON;

			hwndToolTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | balloonTips,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							hwndDlg, NULL, GetModuleHandle(_T("MatroskaProp")), NULL);

			// Have it cover the whole window
			SetWindowPos(hwndToolTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

			// Set our own delays
			SendMessage(hwndToolTip, TTM_SETDELAYTIME, (WPARAM)(DWORD)TTDT_INITIAL, (LPARAM)50);
			SendMessage(hwndToolTip, TTM_SETDELAYTIME, (WPARAM)(DWORD)TTDT_AUTOPOP, (LPARAM)30*1000);
			SendMessage(hwndToolTip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)160);
			
			//Get the filename+path of this dll
			TCHAR dllFilename[MAX_MATROSKA_PATH];
			GetModuleFileName(GetModuleHandle(_T("MatroskaProp")), dllFilename, MAX_MATROSKA_PATH/sizeof(TCHAR));
      SetDlgItemText(hwndDlg, IDC_REG_FILENAME, dllFilename);
#ifdef USING_GETTTEXT			
			JString dllFilenameStr = dllFilename;
			dllFilenameStr = dllFilenameStr.BeforeLast(L'\\');
			dllFilenameStr += "\\lang\\*.*";

			SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)_T("English (Internal)"));

			WIN32_FIND_DATA searchFolders;
			HANDLE searchHandle = FindFirstFile(dllFilenameStr.t_str(), &searchFolders);
			do {
				if (searchFolders.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (searchFolders.cFileName[0] != _T('.'))
						SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)searchFolders.cFileName);
				}
			} while (FindNextFile(searchHandle, &searchFolders));
			FindClose(searchHandle);
			
			JString selectedLanguageTranslation = MatroskaShellExt_GetRegistryValueStr(_T("Translation"), "LANGUAGE=English");
			int selectedIndex = SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_FINDSTRING, -1, (LPARAM)selectedLanguageTranslation.AfterFirst(L'=').t_str());
			SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_SETCURSEL, selectedIndex, 0);
			
			gettext_putenv(selectedLanguageTranslation.mb_str());
#endif
			ConfigurationDlgProc(hwndDlg, WM_LOADTRANSLATIONS, 0, 0);

			//Load the last settings			
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_CONTEXT_MENU, MatroskaShellExt_GetRegistryValue(_T("Context Menus"), 0));
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_COLUMN, MatroskaShellExt_GetRegistryValue(_T("Column Provider"), 1));						
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_TOOLTIPS, MatroskaShellExt_GetRegistryValue(_T("Tooltips"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_USE_SHORT_TOOLTIPS, MatroskaShellExt_GetRegistryValue(_T("UseShortTooltips"), 0));			
			
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_THUMBNAILS, MatroskaShellExt_GetRegistryValue(_T("Thumbnails"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_TASK_THUMBNAILS, MatroskaShellExt_GetRegistryValue(_T("Task Thumbnails"), 0));
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_DSHOW_FALLBACK, MatroskaShellExt_GetRegistryValue(_T("DShow (Internal Render) Thumbnail Fallback"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_IMEDIADET_DSHOW_FALLBACK, MatroskaShellExt_GetRegistryValue(_T("DShow (IMediaDet) Thumbnail Fallback"), 0));

			SendDlgItemMessage(hwndDlg, IDC_SPIN_FRAMETIME, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)300, (short)0));			
			SetDlgItemInt(hwndDlg, IDC_EDIT_FRAMETIME, MatroskaShellExt_GetRegistryValue(_T("ThumbnailFrameTime"), 0), false);
			
			SendDlgItemMessage(hwndDlg, IDC_EDIT_THUMBNAIL_TIMEOUT, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)15, (short)1));			
			SetDlgItemInt(hwndDlg, IDC_EDIT_THUMBNAIL_TIMEOUT, MatroskaShellExt_GetRegistryValue(_T("Thumbnail Frame Wait Time"), 5 * 60) / 60, false);			

			SendDlgItemMessage(hwndDlg, IDC_SPIN_CACHE_LIMIT, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)255, (short)1));			
			SetDlgItemInt(hwndDlg, IDC_EDIT_CACHE_LIMIT, MatroskaShellExt_GetRegistryValue(_T("Parser Cache Limit"), 5), false);

			CheckDlgButton(hwndDlg, IDC_CHECK_ENABLE_CRASH_CATCHER, MatroskaShellExt_GetRegistryValue(_T("Crash Catcher"), 0));

			CheckDlgButton(hwndDlg, IDC_CHECK_DISPLAY_ATTACHMENTS, MatroskaShellExt_GetRegistryValue(_T("Attachments Tab"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_DISPLAY_INFO, MatroskaShellExt_GetRegistryValue(_T("Info Tab"), 1));
			CheckDlgButton(hwndDlg, IDC_CHECK_DISPLAY_TAG, MatroskaShellExt_GetRegistryValue(_T("Tag Tab"), 1));

			CheckDlgButton(hwndDlg, IDC_CHECK_AUTO_RELOAD_TAGS, MatroskaShellExt_GetRegistryValue(_T("Auto-Reload Tags after Save"), 1));
			break;
		}
		case WM_LOADTRANSLATIONS:
		{
			SetWindowText(hwndDlg, _W("Matroska Shell Extension Options"));

			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_1, 
				_W("Matroska Shell Ext Options Setup\nChoose which features you want enabled and configure any options."));
			//SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_2, );
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_3, _W("Cache Size:"));
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_CRASH_CATCHER, _W("Enable Crash Catcher"));
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_5, _W("Which Property Pages to Show"));
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_6, _W("Thumbnail Video Frame Time"));
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_7, _W("Thumbnail Resize Method"));
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_8, _W("Thumbnails"));
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_9, _W("Thumbnail Generation Timeout"));
			//SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_10, );
			SetDlgItemText(hwndDlg, IDC_STATIC_CONFIG_11, _W("Language:"));
      SetDlgItemText(hwndDlg, IDC_STATIC_TRANSLATION_BY, _W("Translation by <translator name>"));
			
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_CONTEXT_MENU, _W("Enable Matroska Context Menus"));
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_COLUMN, _W("Enable Matroska File Columns"));
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_TOOLTIPS, _W("Enable Matroska File Tooltips"));
			SetDlgItemText(hwndDlg, IDC_CHECK_USE_SHORT_TOOLTIPS, _W("Use short Matroska Tooltips"));			

			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_THUMBNAILS, _W("Enable Matroska File Thumbnails"));
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_TASK_THUMBNAILS, 
				_W("Show thumbnails on click (slows browsing down and only shows in the tasks view)"));
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_DSHOW_FALLBACK, _W("Enable DShow (Internal Render) thumbnail generation (prefered)"));
			SetDlgItemText(hwndDlg, IDC_CHECK_ENABLE_IMEDIADET_DSHOW_FALLBACK, _W("Enable DShow (IMediaDet) thumbnail generation"));
			SetDlgItemText(hwndDlg, IDC_CHECK_DISPLAY_INFO, _W("Matroska Info"));
			SetDlgItemText(hwndDlg, IDC_CHECK_DISPLAY_TAG, _W("Matroska Tags"));
			SetDlgItemText(hwndDlg, IDC_CHECK_DISPLAY_ATTACHMENTS, _W("Matroska Attachments"));
			
			HWND cbThumbnail = GetDlgItem(hwndDlg, IDC_COMBO_THUMBNAIL_RESIZE);
			SendMessage(cbThumbnail, CB_RESETCONTENT, 0, 0);
			SendMessage(cbThumbnail, CB_ADDSTRING, 0, (LPARAM)_W("Bilinear"));
			SendMessage(cbThumbnail, CB_ADDSTRING, 0, (LPARAM)_W("Nearest Pixel"));
			SendMessage(cbThumbnail, CB_ADDSTRING, 0, (LPARAM)_W("Bicubic"));
			SendMessage(cbThumbnail, CB_SETCURSEL, (LPARAM)MatroskaShellExt_GetRegistryValue(_T("ThumbnailResizeMethod"), 1), 0);

			SetDlgItemText(hwndDlg, IDC_BUTTON_CONFIG_SET_TRANSLATION, _W("Change"));
			SetDlgItemText(hwndDlg, IDC_CONFIG_OK, _W("OK"));
			SetDlgItemText(hwndDlg, IDC_CONFIG_CANCEL, _W("Cancel"));

			SetDlgItemText(hwndDlg, IDC_CHECK_AUTO_RELOAD_TAGS, _W("Auto-Reload Tags"));

			AddTooltip(hwndToolTip, NULL, NULL);
      AddTooltip(hwndToolTip, GetDlgItem(hwndDlg,	IDC_EDIT_FRAMETIME), _W("If the MKV doesn't have an attached cover. The video frame at this time will be displayed via DShow"));
      AddTooltip(hwndToolTip, GetDlgItem(hwndDlg,	IDC_EDIT_CACHE_LIMIT), _W("The Column, Context Menu, Tooltip, and Thumbnail handers all share a cache of the last few files read. Each one is 4KB-10KB in memory."));
			AddTooltip(hwndToolTip, GetDlgItem(hwndDlg,	IDC_EDIT_THUMBNAIL_TIMEOUT), _W("The number of minutes to wait before canceling thumbnail generation."));
			AddTooltip(hwndToolTip, GetDlgItem(hwndDlg,	IDC_CHECK_AUTO_RELOAD_TAGS), _W("Auto-Reload Tags after Writing."));

			break;
		}
		case WM_DESTROY:
		{
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{ /* Find which control the message applies to */	
			case IDC_CONFIG_CANCEL:
				{					
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							EndDialog(hwndDlg, IDCANCEL);
							break;
						}
					}
					break;
				}				
				/*			
				case IDC_COMBO_CONFIG_LANGUAGE:
				{					
					switch (HIWORD(wParam)) 
          { 
						case LB_SELECTSTRING: 
						{
							// Get the selected language
							LONG nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_GETCURSEL, 0, 0);
							if (nItem == -1)
								break;
							UINT lang_txt_len = SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_GETLBTEXTLEN, nItem, 0);

							TCHAR *lang_txt = new TCHAR[lang_txt_len+1];
							SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_GETLBTEXT, (nItem), (LPARAM)lang_txt);
							delete [] lang_txt;
							
							JString langSelection = "LANGUAGE=";
							langSelection += lang_txt;
							
							gettext_putenv(langSelection.mb_str());
							break;
						}
					}
					break;
				}			
				*/
				case IDC_BUTTON_CONFIG_SET_TRANSLATION:
				{					
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
#ifdef USING_GETTTEXT			
							if (!gettext_is_loaded()) {
								MessageBox(hwndDlg, _W("Translation dll, 'gnu_gettext.dll' was not loaded."), _W("Error"), 0);
								return TRUE;
							}
							// Get the selected language
							LONG nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_GETCURSEL, 0, 0);
							if (nItem == -1)
								break;
							UINT lang_txt_len = SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_GETLBTEXTLEN, nItem, 0);

							TCHAR *lang_txt = new TCHAR[lang_txt_len+1];
							SendDlgItemMessage(hwndDlg, IDC_COMBO_CONFIG_LANGUAGE, CB_GETLBTEXT, (nItem), (LPARAM)lang_txt);							
							
							JString langSelection = "LANGUAGE=";
							langSelection += lang_txt;							
							delete [] lang_txt;

							gettext_putenv(langSelection.mb_str());
							
							MatroskaShellExt_SetRegistryValueStr(_T("Translation"), langSelection);

							ConfigurationDlgProc(hwndDlg, WM_LOADTRANSLATIONS, 0, 0);
#endif // USING_GETTTEXT			
							break;
						}
					}
					break;
				}				
				case IDC_CONFIG_OK:
				{					
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							//Save the current settings
							MatroskaShellExt_SetRegistryValue(_T("Context Menus"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_CONTEXT_MENU));
							
							MatroskaShellExt_SetRegistryValue(_T("Column Provider"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_COLUMN));			
							//1 to enable, 0 to disable
							MatroskaShellExt_SetRegistryValue(_T("Tooltips"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_TOOLTIPS));
							MatroskaShellExt_SetRegistryValue(_T("UseShortTooltips"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_USE_SHORT_TOOLTIPS));
							//1 to enable, 0 to disable
							MatroskaShellExt_SetRegistryValue(_T("Thumbnails"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_THUMBNAILS));
							//0 for Bilinear method , 1 for Nearest Pixel method, or 2 for Bicubic
							MatroskaShellExt_SetRegistryValue(_T("ThumbnailResizeMethod"), SendDlgItemMessage(hwndDlg, IDC_COMBO_THUMBNAIL_RESIZE, CB_GETCURSEL, 0, 0));
							MatroskaShellExt_SetRegistryValue(_T("Task Thumbnails"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_TASK_THUMBNAILS));

							MatroskaShellExt_SetRegistryValue(_T("DShow (Internal Render) Thumbnail Fallback"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_DSHOW_FALLBACK));
							MatroskaShellExt_SetRegistryValue(_T("DShow (IMediaDet) Thumbnail Fallback"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_IMEDIADET_DSHOW_FALLBACK));
							MatroskaShellExt_SetRegistryValue(_T("ThumbnailFrameTime"), GetDlgItemInt(hwndDlg, IDC_EDIT_FRAMETIME, NULL, false));
							
							MatroskaShellExt_SetRegistryValue(_T("Thumbnail Frame Wait Time"), 60 * GetDlgItemInt(hwndDlg, IDC_EDIT_THUMBNAIL_TIMEOUT, NULL, false));

							UINT parserCacheSize = GetDlgItemInt(hwndDlg, IDC_EDIT_CACHE_LIMIT, NULL, false);
							if ((parserCacheSize > 1) && (parserCacheSize < 255))
								MatroskaShellExt_SetRegistryValue(_T("Parser Cache Limit"), parserCacheSize);
							

							MatroskaShellExt_SetRegistryValue(_T("Attachments Tab"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_DISPLAY_ATTACHMENTS));
							MatroskaShellExt_SetRegistryValue(_T("Info Tab"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_DISPLAY_INFO));
							MatroskaShellExt_SetRegistryValue(_T("Tag Tab"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_DISPLAY_TAG));

							MatroskaShellExt_SetRegistryValue(_T("Crash Catcher"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_ENABLE_CRASH_CATCHER));

							MatroskaShellExt_SetRegistryValue(_T("Auto-Reload Tags after Save"), IsDlgButtonChecked(hwndDlg, IDC_CHECK_AUTO_RELOAD_TAGS));

							EndDialog(hwndDlg, IDOK);
							break;
						}
					}
					break;
				}
			}			
			break;
	}
	return FALSE;	
};

BOOL CALLBACK IconDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{	
	CRASH_PROTECT_START;

	JString *iconName = (JString *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			iconName = (JString *)lParam;
			SetWindowLong(hwndDlg, DWL_USER, (LONG)iconName);
#ifdef MATROSKA_PROP			
			// Setup the common controls
			INITCOMMONCONTROLSEX common;			
			common.dwICC = ICC_LISTVIEW_CLASSES; 
			common.dwSize = sizeof(common);
			InitCommonControlsEx(&common);

			HWND hWndListView = GetDlgItem(hwndDlg, IDC_LIST_VIEW_ICON_CHOICE);

			ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_LABELTIP);

			// Create the full-sized icon image lists. 
			HIMAGELIST hLarge = ImageList_Create(32, 32, ILC_COLOR32|ILC_MASK, 1, 1);
			HIMAGELIST hSmall = ImageList_Create(16, 16, ILC_COLOR32|ILC_MASK, 1, 1);
		
			HMODULE myModule = GetModuleHandle(_T("MatroskaProp.dll"));	

			// icon for list-view items 
			HICON hiconItem;
			// Add an icon to each image list.  
			hiconItem = LoadIcon(myModule, MAKEINTRESOURCE(IDI_ICON_WRENCH)); 
			ImageList_AddIcon(hLarge, hiconItem); 				
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem); 
			hiconItem = LoadIcon(myModule, MAKEINTRESOURCE(IDI_ICON_MKV1)); 
			ImageList_AddIcon(hLarge, hiconItem); 
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem); 
			hiconItem = LoadIcon(myModule, MAKEINTRESOURCE(IDI_ICON_MKV_BLUE)); 
			ImageList_AddIcon(hLarge, hiconItem); 
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem);
			hiconItem = (HICON)LoadImage(myModule, MAKEINTRESOURCE(IDI_ICON_MONKEY), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR); 
			ImageList_AddIcon(hLarge, hiconItem); 
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem);
			hiconItem = LoadIcon(myModule, MAKEINTRESOURCE(IDI_ICON_MKV6)); 
			ImageList_AddIcon(hLarge, hiconItem); 	
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem);
			hiconItem = LoadIcon(myModule, MAKEINTRESOURCE(IDI_ICON_MKV_DOLL_4)); 
			ImageList_AddIcon(hLarge, hiconItem); 
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem);
			hiconItem = LoadIcon(myModule, MAKEINTRESOURCE(IDI_ICON_MKV_DOLL_5)); 
			ImageList_AddIcon(hLarge, hiconItem); 	
			ImageList_AddIcon(hSmall, hiconItem); 				
			DestroyIcon(hiconItem);

			// Assign the image lists to the list-view control. 
			ListView_SetImageList(hWndListView, hLarge, LVSIL_NORMAL); 
			ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL); 

			LVITEM trackItem;
			memset(&trackItem, 0, sizeof(LVITEM));
			trackItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			trackItem.pszText = _W("Leave Icon");
			trackItem.iItem = 0;
			trackItem.iImage = 0;			
			ListView_InsertItem(hWndListView, &trackItem);
			
			trackItem.pszText = _W("Use Pamel's Logo with Jory colors");
			trackItem.iItem = 1;
			trackItem.iImage = 1;			
			ListView_InsertItem(hWndListView, &trackItem);

			trackItem.pszText = _W("Use Pamel's Logo with Blue colors");
			trackItem.iItem = 2;
			trackItem.iImage = 2;			
			ListView_InsertItem(hWndListView, &trackItem);

			trackItem.pszText = _W("Use Jory's silly Monkey");
			trackItem.iItem = 3;
			trackItem.iImage = 3;			
			ListView_InsertItem(hWndListView, &trackItem);

			trackItem.pszText = _W("MKV Document");
			trackItem.iItem = 4;
			trackItem.iImage = 4;			
			ListView_InsertItem(hWndListView, &trackItem);

			trackItem.pszText = _W("Doll 1");
			trackItem.iItem = 5;
			trackItem.iImage = 5;			
			ListView_InsertItem(hWndListView, &trackItem);

			trackItem.pszText = _W("Doll 2");
			trackItem.iItem = 6;
			trackItem.iImage = 6;			
			ListView_InsertItem(hWndListView, &trackItem);

			ListView_SetItemState(hWndListView, -1, 0, LVIS_SELECTED); // deselect all items
			int nItem = atoi(iconName->mb_str());
			ListView_EnsureVisible(hWndListView, nItem, TRUE); // if item is far, scroll to it
			ListView_SetItemState(hWndListView, nItem, LVIS_SELECTED, LVIS_SELECTED); // select item
			ListView_SetItemState(hWndListView, nItem, LVIS_FOCUSED, LVIS_FOCUSED); // optional


			SetWindowText(hwndDlg, _W("Select your Matroska Icon"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_SELECT_ICON, _W("OK"));
#endif	
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				/* Find which control the message applies to */
				case IDC_BUTTON_SELECT_ICON:
				{					
					switch (HIWORD(wParam)) 
					{ 
						case BN_CLICKED: 
						{
							int nItem;
							nItem = ListView_GetNextItem(GetDlgItem(hwndDlg, IDC_LIST_VIEW_ICON_CHOICE), -1, LVNI_SELECTED);
							if (nItem > 1)
								*iconName = JString::Format(",%i", nItem);
							/*if (IsDlgButtonChecked(hwndDlg, IDC_RADIO_NO_ICON) == BST_CHECKED) {
								*iconName = "";
							} else if (IsDlgButtonChecked(hwndDlg, IDC_RADIO_MKV_BLUE) == BST_CHECKED) { 
								*iconName = ",3";

							} else if (IsDlgButtonChecked(hwndDlg, IDC_RADIO_MKV_JORY) == BST_CHECKED) { 
								*iconName = ",0";

							} else if (IsDlgButtonChecked(hwndDlg, IDC_RADIO_MONKEY) == BST_CHECKED) { 
								*iconName = ",1";
							}*/
							EndDialog(hwndDlg, IDOK);
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
	CRASH_PROTECT_END;
	return FALSE;

};

BOOL CALLBACK ProgressDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CRASH_PROTECT_START;
	//TRACE("Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch (uMsg)
	{		
		case WM_INITDIALOG:
		{			
			pageData = (MatroskaPages *)lParam;
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);
			SetTimer(hwndDlg, 321, 500, NULL);
			SendDlgItemMessage(hwndDlg, IDC_PROGRESS_BAR, PBM_SETPOS, 0, 0);
			
			SetWindowText(hwndDlg, _W("Matroska Parser"));			
			SetDlgItemText(hwndDlg, IDC_STATIC_PROGRESS_TITLE, _W("Parsing Matroska File"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_ABORT, _W("Abort"));

			
			break;
		}
		case WM_TIMER:
		{
			if ((wParam == 321)
					&& (pageData != NULL)
					&& (pageData->parser != NULL))
			{
				uint8 new_amount = pageData->parser->GetPercentComplete();

				SendDlgItemMessage(hwndDlg, IDC_PROGRESS_BAR, PBM_SETPOS, new_amount, 0);
				// Update text
				TCHAR progress_txt[128];
				_sntprintf(progress_txt, 127, _T("%u %%"), new_amount);
				SetDlgItemText(hwndDlg, IDC_STATIC_PROGRESS, progress_txt);

				if (new_amount == 100)
				{
					KillTimer(hwndDlg, 321);
					EndDialog(hwndDlg, IDCANCEL);
				}
			}
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				/* Find which control the message applies to */
				case IDC_BUTTON_ABORT:
				{					
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							pageData->parser->keepProcessing = false;
							//KillTimer(hwndDlg, 321);
							//EndDialog(hwndDlg, IDCANCEL);
							break;
						}
					}
				}
			}			
			break;
	}
	CRASH_PROTECT_END;

	return FALSE;	
};

BOOL CALLBACK MediaPropProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//TRACE("MediaPropProc Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);

	//Get the dialog data
	CRASH_PROTECT_START;
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);

	switch(uMsg) {
	case WM_INITDIALOG:
		{
			INITCOMMONCONTROLSEX common;			
			common.dwICC = ICC_LISTVIEW_CLASSES|ICC_TAB_CLASSES|ICC_DATE_CLASSES; 
			common.dwSize = sizeof(common);
			InitCommonControlsEx(&common);
			HWND hWndListView = GetDlgItem(hwndDlg, IDC_LIST_INFOMATION);
			LV_COLUMN lvC;

			//Set the default values for the columns.
			lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvC.fmt = LVCFMT_LEFT;			
			lvC.iSubItem = 0;			

			//Set the column labels and add the columns
			lvC.cx = MatroskaShellExt_GetRegistryValue(_T("Info Tab List View Column 1"), 80);
			lvC.pszText = _W("Name");
			ListView_InsertColumn(hWndListView, 0, &lvC);
			lvC.cx = MatroskaShellExt_GetRegistryValue(_T("Info Tab List View Column 2"), 140);
			lvC.pszText = _W("Value");
			ListView_InsertColumn(hWndListView, 1, &lvC);

			ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_LABELTIP);

      ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_CRASH_ME), SW_HIDE);
			ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_CONFIG), SW_HIDE);			

			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
			pageData = (MatroskaPages *)lParam;
#ifdef MATROSKA_PROP      
			MoreData *pm;
			// When the shell creates a dialog box for a property sheet page,
			// it passes the pointer to the PROPSHEETPAGE data structure as
			// lParam. The dialog procedures of extensions typically store it
			// in the DWL_USER of the dialog box window.
			SetWindowLong(hwndDlg, DWL_USER, lParam);
			pm = (MoreData *)lParam;      
			pageData = pm->pObjPageData;
			pageData->AddRef();

			//Store the Matroska Infomation dialog handle
			pageData->g_hInfoDialog = hwndDlg;
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);
#endif			

			HWND hWndTrackListView = GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS);						 
			// Create the full-sized icon image lists. 
			HIMAGELIST hLarge = ImageList_Create(32, 32, ILC_MASK, 1, 1);
		
			// icon for list-view items 
			HICON hiconItem;
			// Add an icon to each image list.  
			hiconItem = LoadIcon(pageData->g_hAppInst, MAKEINTRESOURCE(IDI_ICON_AUDIO)); 
			ImageList_AddIcon(hLarge, hiconItem); 				
			DestroyIcon(hiconItem); 
			hiconItem = LoadIcon(pageData->g_hAppInst, MAKEINTRESOURCE(IDI_ICON_SUBTITLE)); 
			ImageList_AddIcon(hLarge, hiconItem); 				
			DestroyIcon(hiconItem);
			hiconItem = LoadIcon(pageData->g_hAppInst, MAKEINTRESOURCE(IDI_ICON_VIDEO)); 
			ImageList_AddIcon(hLarge, hiconItem); 				
			DestroyIcon(hiconItem);

			// Assign the image lists to the list-view control. 
			ListView_SetImageList(hWndTrackListView, hLarge, LVSIL_NORMAL); 
			
			//ListView_SetIconSpacing(hWndTrackListView, GetSystemMetrics(SM_CXICONSPACING), GetSystemMetrics(SM_CYICONSPACING));
			//ListView_SetExtendedListViewStyle(hWndTrackListView, LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT|LVS_EX_UNDERLINEHOT);

#ifdef MATROSKA_PROP      
			//Create a new MatroskaInfoParser object with the filename
			delete pageData->parser;
			pageData->parser = NULL;
			pageData->parser = new MatroskaInfoParser(pm->pObjPageData->szFile);
			pageData->parser->m_parseSeekHead = true;
			pageData->parser->m_parseAttachments = false;
			pageData->parser->m_parseTags = true;
			pageData->parser->ParseFile();		

			//Fill out the Matroska Info dialog/tag fields
			DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
			ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_CONFIG), SW_NORMAL);
			
#ifdef _DEBUG
			if (MatroskaShellExt_GetRegistryValue(_T("Crash button"), 0))
			 ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_CRASH_ME), SW_NORMAL);
#endif
#endif

#ifndef USING_BITRATE_INFO
			ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_BITRATE), SW_HIDE);
#endif // USING_BITRATE_INFO

			SetDlgItemText(hwndDlg, IDC_STATIC_INFO_BORDER, _W("Matroska File Infomation"));
      SetDlgItemText(hwndDlg, IDC_STATIC_INFO_TRACK_BORDER, _W("Track Infomation"));
      SetDlgItemText(hwndDlg, IDC_STATIC_INFO_TRACKS, _W("Tracks"));
			SetDlgItemText(hwndDlg, IDC_RADIO_BASIC_INFOMATION, _W("Basic Infomation"));
			SetDlgItemText(hwndDlg, IDC_RADIO_ADVANCED_INFO, _W("Advanced Infomation"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_PROP_ABOUT, _W("About"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_CONFIG, _W("Options"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_PROP_CHANGE, _W("Change"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_BITRATE, _W("Bitrate"));

			break;
		}
	case WM_DESTROY:
	{
#ifdef MATROSKA_PROP		
		pageData->Release();
#endif
		break;
	}
	case WM_NOTIFY:
	{
		LPNMHDR notifyInfo = (LPNMHDR)lParam;
		HWND hWndTrackListView = GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS);						 
		if (notifyInfo->hwndFrom == hWndTrackListView) {
			// Process notification messages.            
			switch (notifyInfo->code) 
			{ 
				// select a track					 
				case LVN_ITEMACTIVATE: 
				{
					if(BST_CHECKED == SendDlgItemMessage(hwndDlg, IDC_RADIO_ADVANCED_INFO, BM_GETSTATE, 0, 0))
						SendDlgItemMessage(hwndDlg, IDC_RADIO_ADVANCED_INFO, BM_CLICK, 0, 0);
					else
						SendDlgItemMessage(hwndDlg, IDC_RADIO_BASIC_INFOMATION, BM_CLICK, 0, 0);
					break; 
				}
				case NM_KILLFOCUS:
				{					
					// Save the column widths
					//MatroskaShellExt_SetRegistryValue(_T("Info Tab List View Column 1"), ListView_GetColumnWidth(hWndTrackListView, 0));
					//MatroskaShellExt_SetRegistryValue(_T("Info Tab List View Column 2"), ListView_GetColumnWidth(hWndTrackListView, 1));
					break;
				}
				case NM_CLICK: 
				{
					if(BST_CHECKED == SendDlgItemMessage(hwndDlg, IDC_RADIO_ADVANCED_INFO, BM_GETSTATE, 0, 0))
						SendDlgItemMessage(hwndDlg, IDC_RADIO_ADVANCED_INFO, BM_CLICK, 0, 0);
					else
						SendDlgItemMessage(hwndDlg, IDC_RADIO_BASIC_INFOMATION, BM_CLICK, 0, 0);
					break; 
				}
			}
		}
		break;
	}
	/*This code was the attempt to have a right-click pop-up menu
	static int xPos;
	static int yPos;
	case WM_MOUSEMOVE:
		{
			xPos = GET_X_LPARAM(lParam); 
			yPos = GET_Y_LPARAM(lParam); 
			break;
		}
		case WM_NOTIFY:		
		{
			
			switch (notifyInfo->idFrom)
			{
			case IDC_LIST_INFOMATION: 				
				{        						
					switch (notifyInfo->code) 
					{ 
					case NM_RCLICK: 
						{
							HMENU popupMenu = LoadMenu(pageData->g_hAppInst, MAKEINTRESOURCE(IDR_MENU_CLIPBOARD));
							BOOL result = TrackPopupMenu(popupMenu, TPM_RIGHTALIGN | TPM_LEFTBUTTON, xPos, yPos, 0, hwndDlg, 0);

							break;
						}
					}
					break;
				}
			}
			break;
		}*/
	case WM_COMMAND:		
		{
			switch (LOWORD(wParam))
			{
			case IDC_LIST_TRACKS: 
				{        
					switch (HIWORD(wParam)) 
					{ 
					case LBN_SELCHANGE: 
						{
							if(BST_CHECKED == SendDlgItemMessage(hwndDlg, IDC_RADIO_ADVANCED_INFO, BM_GETSTATE, 0, 0))
								SendDlgItemMessage(hwndDlg, IDC_RADIO_ADVANCED_INFO, BM_CLICK, 0, 0);
							else
								SendDlgItemMessage(hwndDlg, IDC_RADIO_BASIC_INFOMATION, BM_CLICK, 0, 0);

							break;
						}
					}
					break;
				}
			case IDC_RADIO_BASIC_INFOMATION:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
							//Display Basic Infomation about the selected track
							DisplayBasicTrackInfo(hwndDlg, pageData->parser);
							SetFocus(GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS));
							break;
						}
					}
					break;
				}				
			case IDC_RADIO_ADVANCED_INFO:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
							//Display Advanced Infomation About the selected track
							DisplayAdvancedTrackInfo(hwndDlg, pageData->parser);
							SetFocus(GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS));
							break;
						}
					}
					break;
				}				
			case IDC_BUTTON_BITRATE:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
#ifdef USING_BITRATE_INFO
							MatroskaBitrateInfo bitrate;
							bitrate.Open(pageData->parser);
							DialogBoxParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_BITRATE), hwndDlg, MediaPropProc_BitrateDisplay, (LPARAM)&bitrate);
#endif // USING_BITRATE_INFO
							break;
						}
					}
					break;
				}								
			case IDC_BUTTON_CRASH_ME:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
							CrashMe();								
							break;
						}
					}
					break;
				}								
			case IDC_BUTTON_PROP_CHANGE:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
							MatroskaTrackInfo *selected_track = GetMainSelectedTrack(hwndDlg, pageData->parser);
							if (selected_track != NULL) {
								//Ok we have the selected track, now we get the select infomation to change
								HWND hwndList = GetDlgItem(hwndDlg, IDC_COMBO_PROP_CHANGE); 
								int nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
								if (nItem != -1) {
									//A item is selected									
									//Get the selected item text in the listbox
									TCHAR *list_txt = new TCHAR[SendMessage(hwndList, CB_GETLBTEXTLEN, nItem, 0)+1];
									SendMessage(hwndList, CB_GETLBTEXT, (nItem), (LPARAM)list_txt);

									if (!_tcsicmp(_W("Track Language"), list_txt)) {
										pageData->parser->lastSelectedTrack = selected_track;
										HWND hwndLanguageSelectionDialog = CreateDialogParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_SELECT_LANGUAGE), hwndDlg, MediaPropProc_TrackLanguage, (LONG)pageData);
										ShowWindow(hwndLanguageSelectionDialog, SW_SHOW);
									} else  if (!_tcsicmp(_W("Display Size/AR"), list_txt)) {
										pageData->parser->lastSelectedTrack = selected_track;
										HWND hwndDisplaySizeDialog = CreateDialogParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_DISPLAY_SIZE), hwndDlg, MediaPropProc_DisplaySize, (LONG)pageData);
										ShowWindow(hwndDisplaySizeDialog, SW_SHOW);
									}
									delete list_txt;
								} else {
									MessageBox(hwndDlg, _W("You have to select something to change."), _W("Doh!"), 0);
									SendMessage(GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS), WM_SETFOCUS, 0, 0);
								}
							} else {
									MessageBox(hwndDlg, _W("You have to select a track to change."), _W("Doh!"), 0);
									SendMessage(GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS), WM_SETFOCUS, 0, 0);
							}
							break;
						}
					}
					break;
				}
			case IDC_BUTTON_CONFIG:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
#ifdef MATROSKA_PROP														
							DialogBox(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_SHELL_OPTIONS), hwndDlg, ConfigurationDlgProc);
#endif
							break;
						}
					}
					break;
				}				
			case IDC_BUTTON_PROP_ABOUT:
				{
					switch (HIWORD(wParam)) 
					{ 
					case BN_CLICKED: 
						{
							//pageData->parser->CopyFile("C:\\testt.mkv");
							//Show the Matroska CDL about dialog
							DialogBox(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwndDlg, AboutDlgProc);
							break;
						}
					}
					break;
				}
			}
			break;
		}
	default:
		return FALSE;
	}
	CRASH_PROTECT_END;
	return FALSE;
}

BOOL CALLBACK MediaPropProc_Attachments(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//TRACE("MediaPropProc_Attachments Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	CRASH_PROTECT_START;
	// Left, Top, Right, Bottom
	static RECT preview_area = {10, 165, 336, 342};

	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
			pageData = (MatroskaPages *)lParam;
#ifdef MATROSKA_PROP
			MoreData *pm;
			// When the shell creates a dialog box for a property sheet page,
      // it passes the pointer to the PROPSHEETPAGE data structure as
      // lParam. The dialog procedures of extensions typically store it
      // in the DWL_USER of the dialog box window.
      SetWindowLong(hwndDlg, DWL_USER, lParam);
      pm = (MoreData *)lParam;      
      pageData = pm->pObjPageData;

			pageData->AddRef();
			//Store the Matroska Infomation dialog handle
			pageData->g_hAttachmentDialog = hwndDlg;
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);

			ODS("Displaying Attachment Tab\n");			
			if ((pageData->parser == NULL) || (pageData->parser->fileAttachments.GetNumberOfAttachments() < 1))
			{
				delete pageData->parser;
				pageData->parser = new MatroskaInfoParser(pm->pObjPageData->szFile);
				pageData->parser->m_parseSeekHead = false;
				pageData->parser->m_parseAttachments = true;
				pageData->parser->m_parseTags = false;
				pageData->parser->ParseFile();
			}
#endif
			//Checkout the file Attachments
			DisplayAttachmentList(pageData->g_hAttachmentDialog, pageData->parser);

			// Update the tabs
			if (pageData->g_hInfoDialog != NULL)
				DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
			if (pageData->g_hSimpleTagDialog != NULL)
				DisplaySimpleTagTrackList(pageData->g_hSimpleTagDialog, pageData->parser);
			if (pageData->g_hTagDialog != NULL)
				DisplayTagEntryList(pageData->g_hTagDialog, pageData->parser);


      SetDlgItemText(hwndDlg, IDC_STATIC_ATTACHMENT_BORDER, _W("Matroska Attachment Infomation"));
      SetDlgItemText(hwndDlg, IDC_STATIC_ATTACHMENT_FRAME, _W("Attachment Infomation"));
      SetDlgItemText(hwndDlg, IDC_STATIC_ATTACHMENT_PREVIEW, _W("Attachment Preview"));
			SetDlgItemText(hwndDlg, IDC_STATIC_ATTACHMENT_PREVIEW, _W("Attachment Preview"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_VIEW_ATTACHMENT, _W("View Attachment"));
			SetDlgItemText(hwndDlg, IDC_SAVE_ATTACHMENT, _W("Save As"));

			break;
		case WM_PAINT:
		{
#ifdef USING_CXIMAGE
			if (pageData->attachedImage != NULL && pageData->attachedImage->IsValid())
			{
				// Calling BeginPaint clears the update region that was set by calls
				// to InvalidateRect(). Once the update region is cleared no more
				// WM_PAINT messages will be sent to the window until InvalidateRect
				// is called again.
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwndDlg, &ps);
			
				RECT attachedImagePos;				
				attachedImagePos.left = preview_area.left;
				attachedImagePos.right = preview_area.left + pageData->attachedImage->GetWidth();
				attachedImagePos.top = preview_area.top;
				attachedImagePos.bottom = preview_area.top + pageData->attachedImage->GetHeight();

				pageData->attachedImage->Draw(hdc, attachedImagePos);
				
				// EndPaint balances off the BeginPaint call.
				EndPaint(hwndDlg, &ps);
			}
#endif
			break;
		}
		case WM_TIMER:
		{
			break;
		}
		case WM_DESTROY:
		{
			//DeleteObject(pageData->attachedImage);
			//pageData->attachedImage = NULL;
#ifdef MATROSKA_PROP
			pageData->Release();
#endif // MATROSKA_PROP
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{ /* Find which control the message applies to */
				case IDC_BUTTON_LOAD_ATTACHMENTS:
				{
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							ODS("Loading Attachments per Users request\n");
							//Free memory used by parser
							if (pageData->parser != NULL) {								
								delete pageData->parser;							
								pageData->parser = new MatroskaInfoParser(pageData->szFile);															
								pageData->parser->m_parseSeekHead = true;
								pageData->parser->m_parseAttachments = true;
								pageData->parser->m_parseTags = true;
								pageData->parser->m_parseWholeFile = false;								
								ParseFileMT(pageData, hwndDlg);
								TRACE("Attachments Loaded per Users request\n");								
							}
							break;
						}
					}
					break;
				}
				case IDC_LIST_ATTACHMENT: 
				{        
					switch (HIWORD(wParam)) 
					{ 
						case LBN_DBLCLK:
							{
								MediaPropProc_Attachments(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_VIEW_ATTACHMENT, BN_CLICKED), 0);
								break;
							}
						case LBN_SELCHANGE: 
						{
							HWND hwndList;
							int nItem;							

							MatroskaAttachmentItem *selected_track = NULL;
							// Show the selected player's statistics. 
							hwndList = GetDlgItem(hwndDlg, IDC_LIST_ATTACHMENT); 
							nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
							if (nItem != LB_ERR) {
								selected_track = reinterpret_cast<MatroskaAttachmentItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
								//selected_track = parser->GetTrackInfo(nItem+1);							
								if (selected_track != NULL)
								{
									//Display the Attachment infomation									
									SmartStringFormat wBuf;
									wBuf << _W("Filename");
									wBuf << L": ";
									wBuf << selected_track->GetAttachmentFilename();
									wBuf << L"\n";
									wBuf << _W("Mime Type");
									wBuf << L": ";
									if (selected_track->attachmentMimeType.length() > 0) {
										wBuf << selected_track->attachmentMimeType.c_str();
									} else {
										wBuf << _W("None");
									}
									wBuf << L"\n";
									wBuf << _W("Size");
									wBuf << L": ";
									wBuf << selected_track->GetNiceSize();
									wBuf << L"\n";									
									wBuf << _W("Description");
									wBuf << L": "; 
									if (selected_track->attachmentDescription.length() > 0) {
										wBuf << selected_track->attachmentDescription.c_str();
									} else {
										wBuf << _W("None");
									}
									//Send it to the Static Text control
									SetDlgItemTextW(hwndDlg, IDC_STATIC_ATTACHMENT_INFO, wBuf.str().c_str());
									ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT_ATTACHMENT), SW_HIDE);
#ifdef USING_CXIMAGE					
									InvalidateRect(hwndDlg, &preview_area, true);						
									if (pageData->attachedImage != NULL)
										pageData->attachedImage->Destroy();

									//Display a waiting text message
									if (selected_track->GetAttachmentImageType() != 0)
									{
										HDC dcText = GetDC(hwndDlg);
										SetBkMode(dcText, TRANSPARENT);
										//SetTextColor(dcText, RGB(0, 0, 0));
										DrawText(dcText, _W("Please wait decoding..."), -1, &preview_area, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
									
										CxImage *image = selected_track->GetCxImage();
										if (image != NULL)
										{
											if (!selected_track->bResized)
											{
												SIZE correctSize = SmartResize(image->GetWidth(), image->GetHeight(), preview_area.right - preview_area.left, preview_area.bottom - preview_area.top);
												image->Resample(correctSize.cx, correctSize.cy, 0);

												selected_track->bResized = true;
											}
											if (pageData->attachedImage == NULL)												
												pageData->attachedImage = new CxImage(*image);
											else												
												*pageData->attachedImage = *image;

											InvalidateRect(hwndDlg, &preview_area, true);
										}			
									}
#endif
									if (!stricmp(selected_track->attachmentMimeType.c_str(), "text/avs")
												|| !stricmp(selected_track->attachmentMimeType.c_str(), "text/plain")
												|| !stricmp(selected_track->attachmentMimeType.c_str(), "text/unicode"))
									{
										ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT_ATTACHMENT), SW_NORMAL);
										
										JString stringData;
										stringData.clear();										
										stringData = selected_track->GetAttachmentData();
										/*if ((selected_track->GetAttachmentSize() > 3) && data[0] == 0xff && data[1] == 0xfe) { //Unicode
											stringData = (wchar_t *)data;
										} else if ((selected_track->GetAttachmentSize() > 4)  && data[0] == 0xef && data[1] == 0xbb && data[2] == 0xbf) { //UTF-8
											stringData = "UTF-8 encoded text";
										} else { //Asuming ANSI
											stringData = (char *)data;
										}*/
										SetDlgItemText(hwndDlg, IDC_EDIT_ATTACHMENT, stringData.t_str());
									}
								}																														
							}
							break;
						}
					}
					break;
				}				
				case IDC_SAVE_ATTACHMENT:
				{
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							//Save the Attachment as a file
							HWND hwndList;
							int nItem;							

							MatroskaAttachmentItem *the_attached_info = NULL;							
							hwndList = GetDlgItem(hwndDlg, IDC_LIST_ATTACHMENT); 
							nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
							if (nItem != LB_ERR) {
								the_attached_info = reinterpret_cast<MatroskaAttachmentItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));

								if (the_attached_info != NULL)
								{
									//Get the Attachment infomation
									OPENFILENAME saveFileAs;
									TCHAR fileName[1024];
									memset(&saveFileAs, 0, sizeof(OPENFILENAME));
									memset(fileName, 0, sizeof(fileName));
									wcscpy(fileName, the_attached_info->GetAttachmentFilenameOnly());
									saveFileAs.lStructSize =  sizeof(OPENFILENAME);
									saveFileAs.hwndOwner = hwndDlg;									
									saveFileAs.lpstrFile = fileName;
									saveFileAs.nMaxFile	= sizeof(fileName);
									saveFileAs.lpstrTitle	= _W("Save Attachment As...");
									saveFileAs.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

									if (GetSaveFileName(&saveFileAs))
									{
										if (!the_attached_info->SaveTo(fileName))
											MessageBox(hwndDlg, JString::Format(_W("Saved to: %s"), fileName).t_str(), _W("Saved"), 0);
										else
											MessageBox(hwndDlg, JString::Format(_W("Failed to save: %s"), fileName).t_str(), _W("Failed "), 0);
									}
								}
							}
							break;
						}
					}
					break;
				};
				case IDC_BUTTON_VIEW_ATTACHMENT:
				{
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							//Display the Attachment
							HWND hwndList;
							int nItem;							

							MatroskaAttachmentItem *the_attached_info = NULL;							
							hwndList = GetDlgItem(hwndDlg, IDC_LIST_ATTACHMENT); 
							nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
							if (nItem != LB_ERR) {
								the_attached_info = reinterpret_cast<MatroskaAttachmentItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));

								if (the_attached_info != NULL)
								{
									JString msgStr = JString::Format(
										_W("This file %s \nIs not handled by this Extension. Run external program? (equal to double-clicking file)"), 
										the_attached_info->GetAttachmentFilenameOnly());

									int ret;
									ret = MessageBox(hwndDlg, msgStr.t_str(), _W("Warning"), MB_YESNO|MB_ICONQUESTION);
									if (ret == IDYES)
										the_attached_info->OpenViaShell(hwndDlg);
								
								} //if (the_attached_info != NULL)																														
							}
							break;
						}
					}
					break;
				}				
			}
		} //case WM_COMMAND:
	}
	CRASH_PROTECT_END;
	return FALSE;	
};

/// This dialog will be taken out in a later version, and a dynamic text box inside the attachment page will replace it
BOOL CALLBACK MediaPropProc_AttachedFile(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//TRACE("Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	CRASH_PROTECT_START;
	switch(uMsg)
	{
		case WM_INITDIALOG:
			// For now I'm assuming ASCII for the text, as all .avs (the only use for this dialog) are in ASCII AFAIK
			SetDlgItemTextA(hwndDlg, IDC_EDIT_ATTACHED_TEXT, (char *)lParam);
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg, IDOK);
			break;
	}
	CRASH_PROTECT_END;
	return FALSE;
}

BOOL CALLBACK MediaPropProc_DisplaySize(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			pageData = (MatroskaPages *)lParam;
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);			

      SetWindowText(hwndDlg, _W("Add a new Tag"));
      SetDlgItemText(hwndDlg, IDC_STATIC_DISPLAY_SIZE_1, _W("Change the Display Size or AR"));
      SetDlgItemText(hwndDlg, IDC_STATIC_DISPLAY_SIZE_2, _W("Width"));
      SetDlgItemText(hwndDlg, IDC_STATIC_DISPLAY_SIZE_3, _W("Height"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_DISPLAY_SIZE_OK, _W("OK"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_DISPLAY_SIZE_CANCEL, _W("Cancel"));
      
      
			SendDlgItemMessage(hwndDlg, IDC_SPIN_DISPLAY_WIDTH, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)8192, (short)0));
			SendDlgItemMessage(hwndDlg, IDC_SPIN_DISPLAY_HEIGHT, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)8192, (short)0));
			if (pageData != NULL) {
				if (pageData->parser != NULL) {
					MatroskaTrackInfo *selected_track = pageData->parser->lastSelectedTrack;
					if (selected_track != NULL)
					{
						if (selected_track->GetVideoInfo() == NULL)
						{
							MessageBox(hwndDlg, _W("Video track not selected!"), _W("Umm..."), 0);
							EndDialog(hwndDlg, IDCANCEL);
						}								
						//Okay, fill out the current display size
						SetDlgItemInt(hwndDlg, IDC_EDIT_WIDTH, selected_track->GetVideoInfo()->video_DisplayWidth, false);
						SetDlgItemInt(hwndDlg, IDC_EDIT_HEIGHT, selected_track->GetVideoInfo()->video_DisplayHeight, false);

					}else {
						MessageBox(hwndDlg, _W("Unable to find selected track."), _W("Fatal Error"), 0);
					}
				}else {
					MessageBoxA(hwndDlg, "pageData->parser == NULL !!!\n" __FILE__  ":" __FUNCTION__, "Fatal Error", 0);
				}
			}else {
				MessageBoxA(hwndDlg, "pageData == NULL !!!\n" __FILE__  ":" __FUNCTION__, "Fatal Error", 0);
			}
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{ /* Find which control the message applies to */
				case IDC_BUTTON_DISPLAY_SIZE_OK:
				{					
					switch (HIWORD(wParam)) 
					{ 
						case BN_CLICKED: 
						{
							if (pageData != NULL) {
								if (pageData->parser != NULL) {
									MatroskaTrackInfo *selected_track = pageData->parser->lastSelectedTrack;
									if (selected_track != NULL)
									{
										int track_number = selected_track->GetTrackNumber();
										uint16 new_height = GetDlgItemInt(hwndDlg, IDC_EDIT_HEIGHT, NULL, false);
										uint16 new_width = GetDlgItemInt(hwndDlg, IDC_EDIT_WIDTH, NULL, false);
										try {
											int display_write_return = pageData->parser->WriteDisplaySize(track_number, new_width, new_height); 
											if (display_write_return == 0) {
													MessageBox(hwndDlg, _W("Display size updated"), _W("Success"), 0);
													selected_track->GetVideoInfo()->video_DisplayWidth = new_width;
													selected_track->GetVideoInfo()->video_DisplayHeight = new_height;
													DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
											}
										} catch (std::exception &ex) {
											UTFstring errMsg;
											errMsg.SetUTF8(ex.what());

											MessageBox(hwndDlg, errMsg.c_str(), _W("Unable to update display size"), 0);
										}															
									}else {
										MessageBox(hwndDlg, _W("Unable to find selected track."), _W("Fatal Error"), 0);
									}
								}
							}else {
								MessageBoxA(hwndDlg, "Unable to write display size\n" __FILE__ ":" __FUNCTION__, "Fatal Error", 0);
							}
							// Close this dialog down
							EndDialog(hwndDlg, IDOK);
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_DISPLAY_SIZE_CANCEL:
				{
					switch (HIWORD(wParam)) 
					{ 
						case BN_CLICKED: 
						{
							//User pressed cancel, so we exit this dialog
							EndDialog(hwndDlg, wParam); 
							return TRUE;							
							break;
						}
					break;
					}
				}				
			}
		} //case WM_COMMAND:
	}
	CRASH_PROTECT_END;
	return FALSE;
}

BOOL CALLBACK MediaPropProc_TrackLanguage(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			pageData = (MatroskaPages *)lParam;
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);			

      SetWindowText(hwndDlg, _W("Select ISO-639 Language"));
      SetDlgItemText(hwndDlg, IDC_STATIC_LANGUAGE_1, _W("Select the Language from the list"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_LANGUAGE_OK, _W("OK"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_LANGUAGE_CANCEL, _W("Cancel"));
      
			if (pageData != NULL) {
				if (pageData->parser != NULL) {
					MatroskaTrackInfo *selected_track = pageData->parser->lastSelectedTrack;
					if (selected_track != NULL)
					{
						if (selected_track->m_trackLanguage.length() == 0)
						{
							MessageBox(hwndDlg, _W("Track doesn't have an existing track language set!"), _W("Umm..."), 0);
							EndDialog(hwndDlg, IDCANCEL);
						}								
						//Okay, fill out the language list box
						std::string languageString;
						languageString.resize(64);
						uint16 i = 0;
						while (iso639_2B_codes[i] != NULL) {
							languageString = iso639_2B_codes[i];
							languageString += " : ";
							languageString += MatroskaTrackInfo::GetLanguageNameFromISO639(iso639_2B_codes[i]);
							int nItem = SendDlgItemMessageA(hwndDlg, IDC_COMBO_LANGUAGE_SELECTION, CB_ADDSTRING, 0, (LPARAM)languageString.c_str());	
							if (!stricmp(iso639_2B_codes[i], selected_track->m_trackLanguage.c_str())) {
								SendDlgItemMessageA(hwndDlg, IDC_COMBO_LANGUAGE_SELECTION, CB_SETCURSEL, nItem, 0);	
							}
							i++;
						}				
					}else {
						MessageBox(hwndDlg, _W("Unable to find selected track."), _W("Fatal Error"), 0);
					}
				}else {
					MessageBoxA(hwndDlg, "pageData->parser == NULL !!!\n"  __FILE__  ":" __FUNCTION__, _("Fatal Error"), 0);
				}
			}else {
				MessageBoxA(hwndDlg, "pageData == NULL !!!\n" __FILE__  ":" __FUNCTION__, _("Fatal Error"), 0);
			}
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{ /* Find which control the message applies to */
				case IDC_BUTTON_LANGUAGE_OK:
				{					
					switch (HIWORD(wParam)) 
					{ 
						case BN_CLICKED: 
						{
							if (pageData != NULL) {
								if (pageData->parser != NULL) {
									MatroskaTrackInfo *selected_track = pageData->parser->lastSelectedTrack;
									if (selected_track != NULL)
									{
										int nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_LANGUAGE_SELECTION, CB_GETCURSEL, 0, 0);
										int lang_txt_len = SendDlgItemMessageA(hwndDlg, IDC_COMBO_LANGUAGE_SELECTION, CB_GETLBTEXTLEN, nItem, 0);
										char *lang_txt = new char[lang_txt_len+1];
										SendDlgItemMessageA(hwndDlg, IDC_COMBO_LANGUAGE_SELECTION, CB_GETLBTEXT, (nItem), (LPARAM)lang_txt);																																											
										
										lang_txt[strchr(lang_txt, ':') - lang_txt - 1] = 0;
										std::string new_language = lang_txt;
										delete lang_txt;

										uint8 track_number = (uint8)selected_track->GetTrackNumber();											
										try {
											int language_write_return = pageData->parser->WriteLanguage(track_number, new_language); 
											if (language_write_return == 0) {
													MessageBox(hwndDlg, _W("Track language updated"), _W("Success"), 0);
													selected_track->m_trackLanguage = new_language;
													DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
											}
										} catch (std::exception &ex) {
											UTFstring errMsg;
											errMsg.SetUTF8(ex.what());

											MessageBox(hwndDlg, errMsg.c_str(), _W("Unable to update track language"), 0);
										}					
									}else {
										MessageBox(hwndDlg, _W("Unable to find selected track."), _W("Fatal Error"), 0);
									}
								}else {
					       MessageBoxA(hwndDlg, "pageData->parser == NULL !!!\n"  __FILE__  ":" __FUNCTION__, _("Fatal Error"), 0);
				        }
							}else {
								MessageBoxA(hwndDlg, "pageData == NULL !!!\n"  __FILE__  ":" __FUNCTION__, _("Fatal Error"), 0);
							}
							// Close this dialog down
							EndDialog(hwndDlg, IDOK);
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_LANGUAGE_CANCEL:
				{
					switch (HIWORD(wParam)) 
					{ 
						case BN_CLICKED: 
						{
							//User pressed cancel, so we exit this dialog
							EndDialog(hwndDlg, wParam); 
							return TRUE;							
							break;
						}
					break;
					}
				}				
			}
		} //case WM_COMMAND:
	}	
	CRASH_PROTECT_END;
	return FALSE;
}

BOOL CALLBACK MediaPropProc_BitrateDisplay(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CRASH_PROTECT_START;
	MatroskaBitrateInfo *bitrateInfo = (MatroskaBitrateInfo *)GetWindowLong(hwndDlg, DWL_USER);
	switch (uMsg)
	{		
		case WM_INITDIALOG:
		{			
			bitrateInfo = (MatroskaBitrateInfo *)lParam;
			SetWindowLong(hwndDlg, DWL_USER, (LONG)bitrateInfo);
			SetTimer(hwndDlg, 321, 500, NULL);
			SendDlgItemMessage(hwndDlg, IDC_PROGRESS_BAR_BITRATE, PBM_SETPOS, 0, 0);
			
			bitrateInfo->SetReportTextCtrl(GetDlgItem(hwndDlg, IDC_EDIT_BITRATE_DISPLAY));
			SetWindowText(hwndDlg, _W("Bitrate Display"));			
			SetDlgItemText(hwndDlg, IDC_STATIC_BITRATE_USAGE, _W("Bitrate Usage:"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_BITRATE_OK, _W("Abort"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_BITRATE_SAVE, _W("Save"));
			
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_BITRATE_SAVE), false);
			break;
		}
		case WM_TIMER:
		{
			if ((wParam == 321)
					&& (bitrateInfo != NULL))
			{
				uint8 new_amount = bitrateInfo->GetCurrentPercent();

				SendDlgItemMessage(hwndDlg, IDC_PROGRESS_BAR_BITRATE, PBM_SETPOS, new_amount, 0);
				// Update text
				/*TCHAR progress_txt[128];
				_sntprintf(progress_txt, 127, _T("%u %%"), new_amount);
				SetDlgItemText(hwndDlg, IDC_STATIC_PROGRESS, progress_txt);
				*/
				if (new_amount == 100) {
					SetDlgItemText(hwndDlg, IDC_BUTTON_BITRATE_OK, _W("OK"));
					EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_BITRATE_SAVE), true);
					if (SendDlgItemMessage(hwndDlg, IDC_EDIT_BITRATE_DISPLAY, WM_GETTEXTLENGTH, 0, 0) == 0)
						SetDlgItemText(hwndDlg, IDC_EDIT_BITRATE_DISPLAY, bitrateInfo->GetReport().c_str());
				}
			}
			break;
		}
		case WM_CLOSE:
		{
			TCHAR button_txt[256];
			GetDlgItemText(hwndDlg, IDC_BUTTON_BITRATE_OK, button_txt, 255);
			
			if (!_tcsicmp(button_txt, _W("Abort"))) {
				bitrateInfo->Stop();
			}
			KillTimer(hwndDlg, 321);
			EndDialog(hwndDlg, IDCANCEL);
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON_BITRATE_OK:
				{					
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							TCHAR button_txt[256];
							GetDlgItemText(hwndDlg, IDC_BUTTON_BITRATE_OK, button_txt, 255);
							
							if (!_tcsicmp(button_txt, _W("OK"))) {
								KillTimer(hwndDlg, 321);
								EndDialog(hwndDlg, IDCANCEL);
							} else if (!_tcsicmp(button_txt, _W("Abort"))) {
								bitrateInfo->Stop();
							}
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_BITRATE_SAVE:
				{					
					switch (HIWORD(wParam)) 
          { 
						case BN_CLICKED: 
						{
							try {
								int ret = bitrateInfo->Save();
								if (ret == 0)
									MessageBox(hwndDlg, _W("Tags Written."), _W("Success"), 0);
								
							} catch (std::exception &ex) {
								UTFstring errMsg;
								errMsg.SetUTF8(ex.what());

								MessageBox(hwndDlg, errMsg.c_str(), _W("Failed to write tags"), 0);
							}							
							break;
						}
					}
					break;
				}				
			}			
			break;
	}
	CRASH_PROTECT_END;

	return FALSE;	
};

int DisplayMainTrackList(HWND hwndDlg, MatroskaInfoParser *parser)
{
	ODS("DisplayMainTrackList()");

	if ((hwndDlg == NULL) || (parser == NULL))
		return -1;

	HWND hWndTrackListView = GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS);

	//We need to clear the track list so we don't get doubles
	//SendDlgItemMessage(hwndDlg, IDC_LIST_TRACKS, LB_RESETCONTENT, 0, 0);
	ListView_DeleteAllItems(hWndTrackListView);

	MatroskaTrackInfo *current_track = NULL;	

	for (int i = 0; i < parser->GetNumberOfTracks(); i++)
	{
		current_track = parser->GetTrackInfo(i);			
		
		if (current_track != NULL)
		{			
			JString tag_txt = _W("Track");
			if (parser->GetNumberOfTracks()+1 < 9)
			{
				tag_txt += JString::Format(L" %.1u", current_track->GetTrackNumber());
			}
			else if (parser->GetNumberOfTracks()+1 < 99)
			{
				tag_txt += JString::Format(L" %.2u", current_track->GetTrackNumber());
			}
			else if (parser->GetNumberOfTracks()+1 < 999)
			{
				tag_txt += JString::Format(L" %.3u", current_track->GetTrackNumber());
			}

			LVITEM trackItem;
			memset(&trackItem, 0, sizeof(LVITEM));
			trackItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			trackItem.pszText = (wchar_t *)tag_txt.t_str();
			trackItem.lParam = i;
			if (current_track->GetTrackType() == track_audio) {
				trackItem.iImage = 0;
			} else if (current_track->GetTrackType() == track_subtitle) {
				trackItem.iImage = 1;
			} else if (current_track->GetTrackType() == track_video) {
				trackItem.iImage = 2;
			}
			ListView_InsertItem(hWndTrackListView, &trackItem);
		}
	}
	// Sort the items
	ListView_SortItemsEx(hWndTrackListView, ListViewCompareFunc, hWndTrackListView);

	ListView_SetItemState(hWndTrackListView, -1, 0, LVIS_SELECTED); // deselect all items
	ListView_EnsureVisible(hWndTrackListView, 0, TRUE); // if item is far, scroll to it
	ListView_SetItemState(hWndTrackListView, 0, LVIS_SELECTED, LVIS_SELECTED); // select item
	ListView_SetItemState(hWndTrackListView, 0, LVIS_FOCUSED, LVIS_FOCUSED); // optional

	//ListView_SetItemState(hWndTrackListView, 0, LVIS_SELECTED|LVIS_FOCUSED, 0);
	NMHDR msgHeader;
	msgHeader.code = NM_CLICK;
	msgHeader.hwndFrom = hWndTrackListView;
	MediaPropProc(hwndDlg, WM_NOTIFY, 0, (LPARAM)&msgHeader);
	//SendMessage(hWndTrackListView, LB_SETCURSEL, 0, 0);
	//ListView_SetSelectionMark(hWndTrackListView, 0);
	//SendMessage(hWndTrackListView, WM_CHAR, _T('t'), 0);
	//MediaPropProc(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_LIST_TRACKS, LBN_SELCHANGE), LBN_SELCHANGE);

	return 0;
};

MatroskaTrackInfo *GetMainSelectedTrack(HWND hwndDlg, MatroskaInfoParser *parser) {
	TRACE("GetMainSelectedTrack()");

	//MatroskaTrackInfo *selected_track;
	//HWND hwndList;
	int nItem;

	HWND hwndTrackListView = GetDlgItem(hwndDlg, IDC_LIST_VIEW_TRACKS);	
	nItem = ListView_GetNextItem(hwndTrackListView, -1, LVNI_SELECTED);
	if (nItem == -1)
		return NULL;

	LVITEM itemData;
	memset(&itemData, 0, sizeof(LVITEM));
	itemData.mask = LVIF_PARAM;
	itemData.iItem = nItem;

	ListView_GetItem(hwndTrackListView, &itemData);
	
	return parser->trackInfos[(int)itemData.lParam];
};

int DisplayBasicTrackInfo(HWND hwndDlg, MatroskaInfoParser *parser)
{
	ODS("DisplayBasicTrackInfo()");

	if ((hwndDlg == NULL) || (parser == NULL))
		return -1;

	TCHAR tag_txt[1025];
	//MatroskaTagInfo *temp = NULL;
		
	MatroskaTrackInfo *selected_track = GetMainSelectedTrack(hwndDlg, parser);

	if (selected_track != NULL)
	{
		// Clear out the change combobox
		SendDlgItemMessage(hwndDlg, IDC_COMBO_PROP_CHANGE, CB_RESETCONTENT, 0, 0);

		HWND hWndListView = GetDlgItem(hwndDlg, IDC_LIST_INFOMATION);
		int index = -1;
		//First clear the previous items
		ListView_DeleteAllItems(hWndListView);
		LVITEM lvI;
		//Setup the default item values
		lvI.mask = LVIF_TEXT | LVIF_STATE;
		lvI.state = 0;
		lvI.stateMask = 0;								

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Track Number");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), selected_track->GetTrackNumber());
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);
		
		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Track Type");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%s"), selected_track->GetTrackTypeStr());
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		//Display the codec ID
		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Codec ID");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%S"), selected_track->GetCodecID());
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		if (selected_track->m_CodecOldID.length() > 0)
		{
			//Display extra codec info
			//A new item
			lvI.iItem = ++index;
			lvI.iSubItem = 0;
			lvI.pszText = _W("Extra Codec Info");
			//Insert the item	
			ListView_InsertItem(hWndListView, &lvI);
			//Do the subitem
			lvI.iItem = index; //This is the index of this item
			lvI.iSubItem = 1;
			_sntprintf(tag_txt, 1024, _T("%S"), selected_track->m_CodecOldID.c_str());
			lvI.pszText = tag_txt;
			ListView_SetItem(hWndListView, &lvI);
		}

		//Display Language
		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Language");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		JString languageString(selected_track->GetLanguageName());
		lvI.pszText = (TCHAR *)languageString.t_str();
		ListView_SetItem(hWndListView, &lvI);
		if (selected_track->m_trackLanguage.length() > 0) {
			SendDlgItemMessage(hwndDlg, IDC_COMBO_PROP_CHANGE, CB_ADDSTRING, 0, (LPARAM)_W("Track Language"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_PROP_CHANGE, CB_SETCURSEL, 0, 0);
		}

		//We hide the Display Size button for everything but video tracks
		//ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_CHANGE_DISPLAY_SIZE), SW_HIDE);		
		if (selected_track->GetTrackType() == track_audio)
		{
			//This is likely a Audio Track
			MatroskaAudioTrackInfo *audio_track = selected_track->GetAudioInfo();
			
			if (audio_track != NULL)
			{
				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Sampling Rate");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				_sntprintf(tag_txt, 1024, _T("%.2fHz"), audio_track->audio_SampleRate);
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);										


				if (audio_track->audio_OutputSampleRate != 0)
				{
					//A new item
					lvI.iItem = ++index;
					lvI.iSubItem = 0;
					lvI.pszText = _W("Output Sampling Rate");
					//Insert the item	
					ListView_InsertItem(hWndListView, &lvI);
					//Do the subitem
					lvI.iItem = index; //This is the index of this item
					lvI.iSubItem = 1;
					_sntprintf(tag_txt, 1024, _T("%.2fHz"), audio_track->audio_OutputSampleRate);
					lvI.pszText = tag_txt;
					ListView_SetItem(hWndListView, &lvI);		
				}

				if (audio_track->audio_BitDepth != 0)
				{
					//A new item
					lvI.iItem = ++index;
					lvI.iSubItem = 0;
					lvI.pszText = _W("Bit-Depth");
					//Insert the item	
					ListView_InsertItem(hWndListView, &lvI);
					//Do the subitem
					lvI.iItem = index; //This is the index of this item
					lvI.iSubItem = 1;
					_sntprintf(tag_txt, 1024, _T("%u bits"), audio_track->audio_BitDepth);
					lvI.pszText = tag_txt;
					ListView_SetItem(hWndListView, &lvI);
				}

				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Channel Count");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				_sntprintf(tag_txt, 1024, _T("%u "), audio_track->audio_Channels);
				_tcscat(tag_txt, _W("channel(s)"));
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);
			}
			
		}else if (selected_track->GetTrackType() == track_video) {
			//This is likely a Video Track
			MatroskaVideoTrackInfo *video_track = selected_track->GetVideoInfo();																	
			// Add the Display Size/AR choice
			SendDlgItemMessage(hwndDlg, IDC_COMBO_PROP_CHANGE, CB_ADDSTRING, 0, (LPARAM)_W("Display Size/AR"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_PROP_CHANGE, CB_SETCURSEL, 0, 0);

			if (video_track != NULL)
			{
				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Pixel Size");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				_sntprintf(tag_txt, 1024, _T("%u x %u"), video_track->video_Width, video_track->video_Height);
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);										

				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Display Size");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				if ((video_track->video_DisplayWidth == 0) && (video_track->video_DisplayHeight == 0))
				{
					_sntprintf(tag_txt, 1024, _W("N/A"));
				}else {
					_sntprintf(tag_txt, 1024, _T("%u x %u"), video_track->video_DisplayWidth, video_track->video_DisplayHeight);
				}
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);

				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Framerate");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				_sntprintf(tag_txt, 1024, _T("%f"), video_track->video_FrameRate);
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);
			}
		}								
	}																														
	return 0;	
};

int DisplayAdvancedTrackInfo(HWND hwndDlg, MatroskaInfoParser *parser)
{
	ODS("DisplayAdvancedTrackInfo()");

	if ((hwndDlg == NULL) || (parser == NULL))
		return -1;

	//Display Advanced Infomation About the selected track
	TCHAR tag_txt[1025];
	//MatroskaTagInfo *temp = NULL;

	MatroskaTrackInfo *selected_track = GetMainSelectedTrack(hwndDlg, parser);

	if (selected_track != NULL)
	{
		//First load the basic info
		DisplayBasicTrackInfo(hwndDlg, parser);

		HWND hWndListView = GetDlgItem(hwndDlg, IDC_LIST_INFOMATION);
		int index = ListView_GetItemCount(hWndListView)-1;

		LVITEM lvI;
		//Setup the default item values
		lvI.mask = LVIF_TEXT | LVIF_STATE;
		lvI.state = 0;
		lvI.stateMask = 0;
		
		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Track UID");
		//Insert the item
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), selected_track->GetTrackUID());
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		if (selected_track->GetTrackType() == track_video) {
			//This is likely a Video Track
			MatroskaVideoTrackInfo *video_track = selected_track->GetVideoInfo();
			
			if (video_track != NULL)
			{			
				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Interlaced");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				_sntprintf(tag_txt, 1024, _T("%u"), video_track->video_Interlaced);
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);

				//A new item
				lvI.iItem = ++index;
				lvI.iSubItem = 0;
				lvI.pszText = _W("Colordepth");
				//Insert the item	
				ListView_InsertItem(hWndListView, &lvI);				
				//Do the subitem
				lvI.iItem = index; //This is the index of this item
				lvI.iSubItem = 1;
				_sntprintf(tag_txt, 1024, _T("%u"), video_track->video_ColorDepth);
				lvI.pszText = tag_txt;
				ListView_SetItem(hWndListView, &lvI);
			}
		}

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Lacing Flag");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), selected_track->m_lacing_flag);
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Default Flag");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), selected_track->m_default_flag);
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Timecode Scale");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), parser->m_segmentInfo.m_infoTimecodeScale);
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Max Cache");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), selected_track->m_max_cache);
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Min Cache");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%u"), selected_track->m_min_cache);
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Duration");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		// Copy the duration str
		_tcscpy(tag_txt, parser->m_segmentInfo.GetNiceDurationW().c_str());
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);																

		if (parser->m_segmentInfo.m_infoDate.length() > 0) {
			//A new item
			lvI.iItem = ++index;
			lvI.iSubItem = 0;
			lvI.pszText = _W("Date");
			//Insert the item	
			ListView_InsertItem(hWndListView, &lvI);
			//Do the subitem
			lvI.iItem = index; //This is the index of this item
			lvI.iSubItem = 1;
			_sntprintf(tag_txt, 1024, _T("%S"), parser->m_segmentInfo.m_infoDate.c_str());
			lvI.pszText = tag_txt;
			ListView_SetItem(hWndListView, &lvI);
		}

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Muxing App");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%S"), parser->GetMuxingApp());
		lvI.pszText = tag_txt;		
		ListView_SetItem(hWndListView, &lvI);

		//A new item
		lvI.iItem = ++index;
		lvI.iSubItem = 0;
		lvI.pszText = _W("Writing App");
		//Insert the item	
		ListView_InsertItem(hWndListView, &lvI);
		//Do the subitem
		lvI.iItem = index; //This is the index of this item
		lvI.iSubItem = 1;
		_sntprintf(tag_txt, 1024, _T("%S"), parser->GetWritingApp());
		lvI.pszText = tag_txt;
		ListView_SetItem(hWndListView, &lvI);

		if (parser->GetMetaSeekSize() > 0) {
			//A new item
			lvI.iItem = ++index;
			lvI.iSubItem = 0;
			lvI.pszText = _W("Total MetaSeek Elements Size");
			//Insert the item	
			ListView_InsertItem(hWndListView, &lvI);
			//Do the subitem
			lvI.iItem = index; //This is the index of this item
			lvI.iSubItem = 1;
			StrFormatByteSize(parser->GetMetaSeekSize(), tag_txt, 1024);
			lvI.pszText = tag_txt;
			ListView_SetItem(hWndListView, &lvI);
		}
		if (parser->GetCuesSize() > 0) {
			//A new item
			lvI.iItem = ++index;
			lvI.iSubItem = 0;
			lvI.pszText = _W("Total Cue Elements Size");
			//Insert the item	
			ListView_InsertItem(hWndListView, &lvI);
			//Do the subitem
			lvI.iItem = index; //This is the index of this item
			lvI.iSubItem = 1;
			StrFormatByteSize(parser->GetMetaSeekSize(), tag_txt, 1024);
			lvI.pszText = tag_txt;
			ListView_SetItem(hWndListView, &lvI);
		}
		if (parser->cluster_count > 0) {
			//A new item
			lvI.iItem = ++index;
			lvI.iSubItem = 0;
			lvI.pszText = _W("Cluster Count");
			//Insert the item	
			ListView_InsertItem(hWndListView, &lvI);
			//Do the subitem
			lvI.iItem = index; //This is the index of this item
			lvI.iSubItem = 1;
			_sntprintf(tag_txt, 1024, _T("%u "), parser->cluster_count);
			_tcscat(tag_txt, _W("clusters"));
			lvI.pszText = tag_txt;
			ListView_SetItem(hWndListView, &lvI);
		}
	}																														
	//delete tag_txt;
	//tag_txt = NULL;

	return 0;
};

void ParseFileMT(MatroskaPages *pageData, HWND parentDialog)
{
	ODS("ParseFileMT()");
	CRASH_PROTECT_START;	

	_beginthread(MatroskaParserThread, 0, (void *)pageData);

	if (parentDialog != NULL) {
		DialogBoxParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_PROGRESS), parentDialog, ProgressDlgProc, (LPARAM)pageData);
	} else {
		HWND hwndProgressDialog = CreateDialogParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_PROGRESS), parentDialog, ProgressDlgProc, (LPARAM)pageData);
		ShowWindow(hwndProgressDialog, SW_SHOW);
	}

	CRASH_PROTECT_END;
};

void __cdecl MatroskaParserThread(void *pages)
{
	ODS("MatroskaParserThread()");
	CRASH_PROTECT_START;	
	int ret = 0;

	MatroskaPages *pageData = (MatroskaPages *)pages;
	if (pageData != NULL)
		ret = pageData->parser->ParseFile();
	
	// Update the tabs
	if (pageData->g_hInfoDialog != NULL)
		DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
	if (pageData->g_hAttachmentDialog != NULL)
		DisplayAttachmentList(pageData->g_hAttachmentDialog, pageData->parser);
	if (pageData->g_hSimpleTagDialog != NULL)
		DisplaySimpleTagTrackList(pageData->g_hSimpleTagDialog, pageData->parser);
	if (pageData->g_hTagDialog != NULL)
		DisplayTagEntryList(pageData->g_hTagDialog, pageData->parser);						
	if (pageData->g_hChapterDialog != NULL)
		DisplayChapterList(pageData->g_hChapterDialog, pageData->parser);						


	_endthread();
	CRASH_PROTECT_END;
};

int DisplayAttachmentList(HWND hwndDlg, MatroskaInfoParser *parser)
{
	ODS("DisplayAttachmentList()");

	if ((hwndDlg == NULL) || (parser == NULL))
		return -1;

	//Clear the attachment list
	SendDlgItemMessage(hwndDlg, IDC_LIST_ATTACHMENT, LB_RESETCONTENT, 0, 0);
	if (parser->fileAttachments.GetNumberOfAttachments() > 0) {
		SetDlgItemText(hwndDlg, IDC_STATIC_ATTACHMENT_FRAME, _W("Attachment Infomation"));
		MatroskaAttachmentItem *current_attach = NULL;
		for (uint32 i = 0; i < parser->fileAttachments.GetNumberOfAttachments(); i++)
		{
			current_attach = parser->fileAttachments.GetAttachment(i);
			
			if (current_attach != NULL) {

				int nItem = SendDlgItemMessage(hwndDlg, IDC_LIST_ATTACHMENT, LB_ADDSTRING, 0, (LPARAM)(current_attach->GetAttachmentFilenameOnly()));
				SendDlgItemMessage(hwndDlg, IDC_LIST_ATTACHMENT, LB_SETITEMDATA, nItem, reinterpret_cast<LPARAM>(current_attach));
			}
		}									
	}else {
		SetDlgItemText(hwndDlg, IDC_STATIC_ATTACHMENT_FRAME, _W("No Attachments Found"));
	}

	return 0;
};

/// I created this little function to get the total length of the text in a multi-line text control
/// \param theDialog The Owner window of the text control
/// \param edit_control The ID of the text control
int GetEditControlTextLength(HWND theDialog, int edit_control)
{
	TRACE("GetEditControlTextLength()");

	int current_line = 0;
	int start_current_line = 0;
	int total_count = 0;

	do
	{
		total_count += SendDlgItemMessage(theDialog, edit_control, EM_LINELENGTH,  start_current_line, 0);

		total_count += 2; //Extra char for line break chars
		current_line++;
		start_current_line = SendDlgItemMessage(theDialog, edit_control, EM_LINEINDEX, current_line, 0);
	}
	while (start_current_line != -1);

	return total_count;
}

SIZE SmartResize(long sourceX, long sourceY, long maxX, long maxY)
{
	CRASH_PROTECT_START;
	//TRACE("SmartResize(sourceX = %i, sourceY = %i, maxX = %i, maxY = %i)", sourceX, sourceY, maxX, maxY);

	double dAspect;

	if (((double)sourceX / maxX) > ((double)sourceY / maxY))
		//aspect ratio is determined by width
		dAspect = (double)maxX / sourceX;
	else
		//aspect ratio is determined by height	
		dAspect = (double)maxY / sourceY;

	SIZE finalSize;
	finalSize.cx = (LONG)(sourceX * dAspect);
	finalSize.cy = (LONG)(sourceY * dAspect);

	return finalSize;

	CRASH_PROTECT_END;
};

int CALLBACK ListViewCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TCHAR text1[65] = { 0 };	
	TCHAR text2[65] = { 0 };
	
	ListView_GetItemText((HWND)lParamSort, lParam1, 0, text1, 64);
	ListView_GetItemText((HWND)lParamSort, lParam2, 0, text2, 64);

	return _tcscmp(text1, text2);
};

WORD AddTooltip(HWND hwndTooltip, HWND hwndClient, LPTSTR strText)
{
	static WORD id = 0;
	TOOLINFO ti;
	RECT rect;

  memset(&ti, 0, sizeof(TOOLINFO));
  memset(&rect, 0, sizeof(RECT));

	if (hwndClient == NULL && strText == NULL) {
		// Remove all the tooltips from the control
		
		// Setup the toolinfo structure
		ti.cbSize = sizeof(TOOLINFO);
		ti.hwnd = (HWND)-1;
		while (ti.hwnd != (HWND)-1) {
			ti.hwnd = (HWND)-1;
			SendMessage(hwndTooltip, TTM_ENUMTOOLS, 0, (LPARAM)(LPTOOLINFO)&ti);
			SendMessage(hwndTooltip, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);		
		}
  } else {    
	  // Get the coordinates of the client control
	  GetClientRect(hwndClient, &rect);

    // Setup the toolinfo structure
	  ti.cbSize = sizeof(TOOLINFO);
	  ti.uFlags = TTF_SUBCLASS;
	  ti.hwnd = hwndClient;
  #ifdef MATROSKA_PROP
	  ti.hinst = GetModuleHandle(_T("MatroskaProp"));
  #else
	  ti.hinst = GetModuleHandle(_T("MatroskaCDL"));
  #endif
	  ti.uId = id;
	  ti.lpszText = strText;

	  // Tooltip control will cover the whole window
	  ti.rect.left	= rect.left;
	  ti.rect.top		= rect.top;
	  ti.rect.right	= rect.right;
	  ti.rect.bottom	= rect.bottom;

	  // Add the tooltip
	  SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

    // Return the id of the tooltip
    return id++;
  }

  return 0;
};

void CrashMe()
{
	CRASH_PROTECT_START;

  char *dummy = _("Dummy string");
  
	ODS("CrashMe()");
	//CRASH CODE!!!!
	int dummyCrash = 3;
	// Do a crash
	switch (dummyCrash)
	{
		case 0:
		{
			MessageBoxA(NULL, "Writing to a bad memory address...", "Crashing on purpose.", 0);
			sprintf((char *)0xbadf00d, "test %i", 5);
			break;
		}
		case 1:
		{
			MessageBoxA(NULL, "Running SSE2 instruction (crash on non-SSE2 capable cpus).", "Crashing on purpose.", 0);
			//__asm { cmove eax, ebx };
			//__asm { pf2iw  mm0, mm1 };
			__asm { MULPD  xmm0, xmm1 };										
			//__asm { punpckq2dq xmm0, xmm1 };
			break;
		}
		case 2:
		{
			MessageBoxA(NULL, "Divide by zero...", "Crashing on purpose.", 0);
			float test = 0;
			float badDivide = 100 / test;
			test = badDivide + 5 * 45 / test;
			break;
		}
		case 3:
		{
			MessageBoxA(NULL, "Reading from a bad memory address...", "Crashing on purpose.", 0);
			char dummyCrashData[1025];
			memcpy(dummyCrashData, (void *)0xaddf00d, 1024);
			break;
		}
		case 4:
		{
			MessageBoxA(NULL, "Deleting a read-only string...", "Crashing on purpose.", 0);
			delete dummy;
			break;
		}
		case 5:
		{
			MessageBoxA(NULL, "Overflowing stack", "Crashing on purpose.", 0);
			__asm {
stack_start:
				push ebx;
				jmp stack_start;
			};
			break;
		}
	};

	//CRASH_PROTECT_END;
	} catch (...) {
		std::ostringstream buf;
		buf << ("Unhandled Exception,\nFile: " __FILE__ "\nFunction: " __FUNCTION__ "\nLine:");
		buf << __LINE__; 
		buf << "\nTry to generate bug report?"; 
		int msgBoxRet = MessageBoxA(NULL, buf.str().c_str(), "Error", MB_YESNO|MB_TASKMODAL); 
		if (msgBoxRet == IDYES) { 
				throw; 
		} 
	};
};


