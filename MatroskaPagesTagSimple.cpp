/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MatroskaPagesTagSimple.cpp
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
    \file MatroskaPagesTagSimple.cpp
		\version \$Id: MatroskaPagesTagSimple.cpp,v 1.1 2004/02/16 08:07:09 jcsston Exp $
    \brief This is the GUI code for the Simple Tag Edit Page
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#include "MatroskaPages.h"

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

using namespace MatroskaUtilsNamespace;

extern long g_MyBuildNumber;

BOOL CALLBACK MediaPropProc_SimpleTag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
#if 0
	//TRACE("Simple Tag Tab Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
		{
			INITCOMMONCONTROLSEX common;			
			common.dwICC = ICC_DATE_CLASSES; 
			common.dwSize = sizeof(common);
			InitCommonControlsEx(&common);
			
			SendDlgItemMessage(hwndDlg, IDC_SPIN_TAG_RATING, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)10, (short)-10));

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
			pageData->g_hSimpleTagDialog = hwndDlg;
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);

			ODS("Displaying Tag Tab");			
			if ((pageData->parser == NULL) || !pageData->parser->found_tags)
			{
				delete pageData->parser;
				pageData->parser = new MatroskaInfoParser(pm->pObjPageData->szFile);
				pageData->parser->m_parseSeekHead = true;
				pageData->parser->m_parseAttachments = true;
				pageData->parser->m_parseTags = true;
				ParseFileMT(pageData, hwndDlg);
				//pageData->parser->ParseFile();
			}			

			DisplaySimpleTagTrackList(pageData->g_hSimpleTagDialog, pageData->parser);			

			// Update the tabs
			if (pageData->g_hInfoDialog != NULL)
				DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
			if (pageData->g_hAttachmentDialog != NULL)
				DisplayAttachmentList(pageData->g_hAttachmentDialog, pageData->parser);
			if (pageData->g_hTagDialog != NULL)
				DisplayTagEntryList(pageData->g_hTagDialog, pageData->parser);
#endif
			return TRUE;
			break;
		}
		case WM_DESTROY:
		{
#ifdef MATROSKA_PROP
			pageData->Release();
#endif // MATROSKA_PROP
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))	{ /* Find which control the message applies to */
			case IDC_COMBO_TAG_SIMPLE_TRACKS:
			{
				switch (HIWORD(wParam)) 
				{ 
					case CBN_SELCHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{
							JString tagString;
							tagString.clear();

							//Fill out the Track Info fields								
							tagString = JString(_W("Track UID")) + JString::Format(L": %u", selected_track->GetTrackUID());
							SetDlgItemText(hwndDlg, IDC_STATIC_TRACK_UID,	tagString.t_str());						

							tagString = pageData->parser->GetTagAlbum(selected_track->GetTagInfo());
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_ALBUM, tagString.t_str());
							
							tagString = pageData->parser->GetTagArtist(selected_track->GetTagInfo());
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_ARTIST, tagString.t_str());
							
							tagString = JString::Format("%f", pageData->parser->GetTagBPM(selected_track->GetTagInfo()));
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_BPM, tagString.t_str());
							
							tagString = JString::Format("%u", pageData->parser->GetTagDiscTrack(selected_track->GetTagInfo()));
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_TRACKNO, tagString.t_str());
							
							tagString = pageData->parser->GetTagGenre(selected_track->GetTagInfo()).c_str();
							SetDlgItemText(hwndDlg, IDC_COMBO_TAG_GENRE, tagString.t_str());
							
							tagString = JString::Format("%i", pageData->parser->GetTagRating(selected_track->GetTagInfo()));
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_RATING, tagString.t_str());
							
							tagString = pageData->parser->GetTagTitle(selected_track->GetTagInfo());
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_TITLE, tagString.t_str());

							tagString = JString::Format("%i", pageData->parser->GetTagYear(selected_track->GetTagInfo()));
							SetDlgItemText(hwndDlg, IDC_EDIT_TAG_YEAR, tagString.t_str());
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_TITLE:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_TITLE, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_TITLE, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;
							pageData->parser->SetTagTitle(tag_string.c_str(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_ARTIST:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_ARTIST, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_ARTIST, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;
							pageData->parser->SetTagArtist(tag_string.c_str(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_ALBUM:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_ALBUM, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_ALBUM, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;
							pageData->parser->SetTagAlbum(tag_string.c_str(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_BPM:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{							
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_BPM, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_BPM, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;

							pageData->parser->SetTagBPM(tag_string.toDouble(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_RATING:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{							
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_RATING, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_RATING, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;

							
							pageData->parser->SetTagRating(tag_string.toLong(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_YEAR:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{							
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_YEAR, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_YEAR, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;

							
							pageData->parser->SetTagYear(tag_string.toLong(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_EDIT_TAG_TRACKNO:
			{
				switch (HIWORD(wParam)) 
				{ 
					case EN_CHANGE: 
					{
						HWND hwndTrackCombo;
						int nItem = 0;
						MatroskaTrackInfo *selected_track = NULL;

						hwndTrackCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS); 
						nItem = SendMessage(hwndTrackCombo, CB_GETCURSEL, 0, 0); 
						selected_track = reinterpret_cast<MatroskaTrackInfo *>(SendMessage(hwndTrackCombo, CB_GETITEMDATA, (nItem), 0));

						if ((selected_track != NULL) && (nItem != -1))
						{							
							int txt_length = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_TRACKNO, WM_GETTEXTLENGTH, 0, 0)+1;
							TCHAR *tag_txt = new TCHAR[txt_length+sizeof(TCHAR)];
							memset(tag_txt, 0, txt_length+sizeof(TCHAR));
							SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_TRACKNO, WM_GETTEXT, txt_length, (LPARAM)tag_txt);
														
							JString tag_string(tag_txt);
							delete tag_txt;

							
							pageData->parser->SetTagDiscTrack(tag_string.toLong(), selected_track->GetTagInfo());							
						}
						break;
					}
				}
				break;
			}
			case IDC_BUTTON_SIMPLE_TAG_SAVE:
			{
				switch (HIWORD(wParam)) 
				{ 
					case BN_CLICKED: 
					{
						/*EDITBALLOONTIP bTip;
						bTip.cbStruct = sizeof(bTip);
						bTip.pszTitle = L"Test";
						bTip.pszText = L"This is only a simple test.";
						bTip.ttiIcon = TTI_INFO;
						Edit_ShowBalloonTip(GetDlgItem(hwndDlg, IDC_EDIT_TAG_ALBUM), &bTip);*/

						if (pageData->parser->WriteTags() == 2)
							MessageBox(hwndDlg, _W("Unable to write tags. Please check that the file isn't open in another program. If you are watching the file please stop it and try again"), _W("Unable to write tags"), 0);
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

#endif
	return FALSE;
}

int DisplaySimpleTagTrackList(HWND hwndDlg, MatroskaInfoParser *parser)
{
#if 0
	ODS("DisplaySimpleTagTrackList()");

	if ((hwndDlg == NULL) || (parser == NULL))
		return -1;

	MatroskaTrackInfo *current_track = NULL;	
	int nItem = 0;
	current_track = parser->GetGlobalTrackInfo();

	nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS, CB_ADDSTRING, 0, (LPARAM)_T("Global Tags"));
	SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS, CB_SETITEMDATA, nItem, reinterpret_cast<LPARAM>(current_track));
	
	//Select the first item, for the user
	SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS, CB_SETCURSEL, 0, 0);
	MediaPropProc_SimpleTag(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_COMBO_TAG_SIMPLE_TRACKS, CBN_SELCHANGE), CBN_SELCHANGE);

 	for (int i = 1; i < parser->GetNumberOfTracks(); i++)
	{
		current_track = parser->GetTrackInfo(i);			
		
		if (current_track != NULL)
		{
			JString tag_txt = _W("Track");
			if (parser->GetNumberOfTracks() < 9)
			{
				tag_txt += JString::Format(L" %.1u", current_track->GetTrackNumber());
			}
			else if (parser->GetNumberOfTracks() < 99)
			{
				tag_txt += JString::Format(L" %.2u", current_track->GetTrackNumber());
			}
			else if (parser->GetNumberOfTracks() < 999)
			{
				tag_txt += JString::Format(L" %.3u", current_track->GetTrackNumber());
			}			
			nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS, CB_ADDSTRING, 0, (LPARAM)tag_txt.t_str());
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_SIMPLE_TRACKS, CB_SETITEMDATA, nItem, reinterpret_cast<LPARAM>(current_track));
		}
	}
#endif
	return 0;
};

