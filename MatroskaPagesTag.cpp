/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MatroskaPagesTag.cpp
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
    \file MatroskaPagesTag.cpp
		\version \$Id: MatroskaPagesTag.cpp,v 1.13 2004/03/20 05:08:30 jcsston Exp $
    \brief This is all the GUI code for the CDL and Shell Ext
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#include "MatroskaPages.h"

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

using namespace MatroskaUtilsNamespace;

extern long g_MyBuildNumber;

BOOL CALLBACK MediaPropProc_AddTag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
		{
      SetWindowText(hwndDlg, _W("Add a new Tag"));
      SetDlgItemText(hwndDlg, IDC_STATIC_ADD_TAG_1, _W("Choose the type of tag to add"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_ADDTAG_OK, _W("OK"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_ADDTAG_CANCEL, _W("Cancel"));

			uint32 item = 1;
			char *tag_item_name = NULL;

			//Loop until we reach the end of the tag name list
			for (item = 1; item <= 44; item++)
			{
				//Get the next tag name
				tag_item_name = MatroskaTagInfo::GetTagListStr(item);
				if (tag_item_name != NULL)
				{
					//Add the current tag name to the list
					SendDlgItemMessageA(hwndDlg, IDC_LIST_ADDTAG, LB_ADDSTRING, 0, (LPARAM)tag_item_name);
				}
			}
			return TRUE;
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
				case IDC_BUTTON_ADDTAG_OK:
				{
					//Add the selected tag :P
					int nItem = 0;

					//Get the selected tag type
					nItem = SendDlgItemMessage(hwndDlg, IDC_LIST_ADDTAG, LB_GETCURSEL, 0, 0);
					char *field_txt = new char[SendDlgItemMessageA(hwndDlg, IDC_LIST_ADDTAG, LB_GETTEXTLEN, nItem, 0)+1];
					SendDlgItemMessageA(hwndDlg, IDC_LIST_ADDTAG, LB_GETTEXT, (nItem), (LPARAM)field_txt);
					uint32 tag_type = 0;
					tag_type = MatroskaTagInfo::GetTagListUInt(field_txt);

					MatroskaTagInfo *current_tag = GetSelectedTagEntry(pageData->g_hTagDialog);

					if (current_tag != NULL) {

						switch (tag_type) {
							case 1: //"MultiComment"
							{
								MatroskaTagMultiCommentItem *added_multi_comment_item = new MatroskaTagMultiCommentItem;
								added_multi_comment_item->Language = "eng"; //Default Langauge
								added_multi_comment_item->Comments = L"A new MultiComment tag";
								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiComments != NULL) {
									current_tag->Tag_MultiComments->AddItem(added_multi_comment_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiComment *new_multi_list = new MatroskaTagMultiComment;
									//Attach this new Multi tag list
									current_tag->Tag_MultiComments = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiComments->AddItem(added_multi_comment_item);
								}
								break;
							}
							case 2: //"MultiCommercial"
							{
								MatroskaTagMultiCommercialItem *added_multi_commercial_item = new MatroskaTagMultiCommercialItem;
								added_multi_commercial_item->Type = KaxTagMultiCommercialType_FilePurchase; //Default Type
								added_multi_commercial_item->PriceAmount = 1.00; //Is 1 unit enough :D

								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiCommercials != NULL) {
									current_tag->Tag_MultiCommercials->AddItem(added_multi_commercial_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiCommercial *new_multi_list = new MatroskaTagMultiCommercial;
									//Attach this new Multi tag list
									current_tag->Tag_MultiCommercials = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiCommercials->AddItem(added_multi_commercial_item);
								}
								break;
							}
							case 3: //"MultiDate"
							{
								MatroskaTagMultiDateItem *added_multi_item = new MatroskaTagMultiDateItem;
								//Set the intial values to be right now
								added_multi_item->Type = KaxTagMultiDateType_TaggingDate;
								time_t ltime = 0;
								time(&ltime);
								added_multi_item->DateBegin = ltime;
								added_multi_item->DateEnd = ltime;
								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiDates != NULL) {
									current_tag->Tag_MultiDates->AddItem(added_multi_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiDate *new_multi_list = new MatroskaTagMultiDate;
									//Attach this new Multi tag list
									current_tag->Tag_MultiDates = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiDates->AddItem(added_multi_item);
								}
								break;
							}
							case 4: //"MultiEntity"
							{
								MatroskaTagMultiEntityItem *added_multi_entity_item = new MatroskaTagMultiEntityItem;
								added_multi_entity_item->Type = KaxTagMultiEntitiesType_EditedBy; //Default Type
								added_multi_entity_item->Name = L"The TCMP Matroska plugin";

								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiEntities != NULL) {
									current_tag->Tag_MultiEntities->AddItem(added_multi_entity_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiEntity *new_multi_list = new MatroskaTagMultiEntity;
									//Attach this new Multi tag list
									current_tag->Tag_MultiEntities = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiEntities->AddItem(added_multi_entity_item);
								}
								break;
							}
							case 5:	//"MultiIdentifier"
							{
								MatroskaTagMultiIdentifierItem *added_multi_identifier_item = new MatroskaTagMultiIdentifierItem;
								added_multi_identifier_item->Type = KaxTagMultiIdentifierType_UniqueFileIdentifier; //Default Type
								added_multi_identifier_item->StringData = L"Blank";

								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiIdentifiers != NULL) {
									current_tag->Tag_MultiIdentifiers->AddItem(added_multi_identifier_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiIdentifier *new_multi_list = new MatroskaTagMultiIdentifier;
									//Attach this new Multi tag list
									current_tag->Tag_MultiIdentifiers = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiIdentifiers->AddItem(added_multi_identifier_item);
								}
								break;
							}
							case 6:	//"MultiLegal"
							{
								MatroskaTagMultiLegalItem *added_multi_legal_item = new MatroskaTagMultiLegalItem;
								added_multi_legal_item->Type = KaxTagMultiLegalType_Copyright; //Default Type
								added_multi_legal_item->URL = "http://";

								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiLegals != NULL) {
									current_tag->Tag_MultiLegals->AddItem(added_multi_legal_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiLegal *new_multi_list = new MatroskaTagMultiLegal;
									//Attach this new Multi tag list
									current_tag->Tag_MultiLegals = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiLegals->AddItem(added_multi_legal_item);
								}
								break;
							}
							case 7:	//"MultiTitle"
							{
								MatroskaTagMultiTitleItem *added_multi_title_item = new MatroskaTagMultiTitleItem;
								added_multi_title_item->Type = KaxTagMultiTitleType_TrackTitle; //Default Type
								added_multi_title_item->Name = L"Blank";

								//Once again we have to check if the tag already exists
								if (current_tag->Tag_MultiTitles != NULL) {
									current_tag->Tag_MultiTitles->AddItem(added_multi_title_item);
								}else {
									//Nope we need to create a new one
									MatroskaTagMultiTitle *new_multi_list = new MatroskaTagMultiTitle;
									//Attach this new Multi tag list
									current_tag->Tag_MultiTitles = new_multi_list;
									//Append our new item
									current_tag->Tag_MultiTitles->AddItem(added_multi_title_item);
								}
								break;
							}
							case 9:	//"Archival Location"
							{
								if (!(current_tag->Tag_ArchivalLocation.length() > 0))
								{
									current_tag->Tag_ArchivalLocation = L"Blank";
								}
								break;
							}
							case 10: //"Audio Gain"
							{
								if (!(current_tag->Tag_AudioGain = 0))
								{
									current_tag->Tag_AudioGain = 1;
								}
								break;
							}
							case 11: //"Audio Genre"
							{
								if (!(current_tag->Tag_AudioGenre.length() > 0))
								{
									current_tag->Tag_AudioGenre = "Blank";
								}
								break;
							}
							case 12: //"Audio Peak"
							{
								if (current_tag->Tag_AudioPeak == 0)
								{
									current_tag->Tag_AudioPeak = 1;
								}
								break;
							}
							case 13: //"Audio Secondary Genre"
							{
								if (!(current_tag->Tag_AudioSecondaryGenre.length() > 0))
								{
									current_tag->Tag_AudioSecondaryGenre = "Blank";
								}

								break;
							}
							case 14: //"Bibliography"
							{
								if (!(current_tag->Tag_Bibliography.length() > 0))
								{
									current_tag->Tag_Bibliography = L"Blank";
								}
								break;
							}
							case 15: //"BPM"
							{
								if (current_tag->Tag_BPM == 0)
								{
									current_tag->Tag_BPM = 1;
								}
								break;
							}
							case 16: //"Capture DPI"
							{
								if (current_tag->Tag_CaptureDPI == 0)
								{
									current_tag->Tag_CaptureDPI = 1;
								}
								break;
							}
							case 17: //"Capture Palette Setting"
							{
								if (current_tag->Tag_CapturePaletteSetting == 0)
								{
									current_tag->Tag_CapturePaletteSetting = 1;
								}
								break;
							}
							case 18: //"Cropped"
							{
								if (!(current_tag->Tag_Cropped.length() > 0))
								{
									current_tag->Tag_Cropped = L"Blank";
								}
								break;
							}
							case 19: //"Disc Track"
							{
								if (current_tag->Tag_DiscTrack == 0)
								{
									current_tag->Tag_DiscTrack = 1;
								}
								break;
							}
							case 20: //"Encoder"
							{
								if (!(current_tag->Tag_Encoder.length() > 0))
								{
									current_tag->Tag_Encoder = L"Blank";
								}
								break;
							}
							case 21: //"Encode Settings"
							{
								if (!(current_tag->Tag_EncodeSettings.length() > 0))
								{
									current_tag->Tag_EncodeSettings = L"Blank";
								}
								break;
							}
							case 22: //"File"
							{
								if (!(current_tag->Tag_File.length() > 0))
								{
									current_tag->Tag_File = L"Blank";
								}
								break;
							}
							case 23: //"Initial Key"
							{
								if (!(current_tag->Tag_InitialKey.length() > 0))
								{
									current_tag->Tag_InitialKey = "Blank";
								}
								break;
							}
							case 24: //"Keywords"
							{
								if (!(current_tag->Tag_Keywords.length() > 0))
								{
									current_tag->Tag_Keywords = L"Blank";
								}
								break;
							}
							case 25: //"Language"
							{
								if (!(current_tag->Tag_Language.length() > 0))
								{
									current_tag->Tag_Language = "Blank"; //Default Language
								}
								break;
							}
							case 26: //"Length"
							{
								if (current_tag->Tag_Length == 0)
								{
									current_tag->Tag_Length = 1;
								}
								break;
							}
							case 27: //"Official Audio File URL"
							{
								if (!(current_tag->Tag_OfficialAudioFileURL.length() > 0))
								{
									current_tag->Tag_OfficialAudioFileURL = "http://";
								}
								break;
							}
							case 28: //"Official Audio Source URL"
							{
								if (!(current_tag->Tag_OfficialAudioSourceURL.length() > 0))
								{
									current_tag->Tag_OfficialAudioSourceURL = "http://";
								}
								break;
							}
							case 29: //"Original Dimensions"
							{
								if (!(current_tag->Tag_OriginalDimensions.length() > 0))
								{
									current_tag->Tag_OriginalDimensions = "Blank";
								}
								break;
							}
							case 30: //"Original Media Type"
							{
								if (!(current_tag->Tag_OriginalMediaType.length() > 0))
								{
									current_tag->Tag_OriginalMediaType = L"Blank";
								}
								break;
							}
							case 31: //"Play Counter"
							{
								if (current_tag->Tag_PlayCounter == 0)
								{
									current_tag->Tag_PlayCounter = 1;
								}
								break;
							}
							case 32: //"Playlist Delay"
							{
								if (current_tag->Tag_PlaylistDelay == 0)
								{
									current_tag->Tag_PlaylistDelay = 1;
								}
								break;
							}
							case 33: //"Popularimeter"
							{
								if (current_tag->Tag_Popularimeter == 0)
								{
									current_tag->Tag_Popularimeter = 1;
								}
								break;
							}
							case 34: //"Product"
							{
								if (!(current_tag->Tag_Product.length() > 0))
								{
									current_tag->Tag_Product = L"Blank";
								}
								break;
							}
							case 35: //"Record Location"
							{
								if (!(current_tag->Tag_RecordLocation.length() > 0))
								{
									current_tag->Tag_RecordLocation = "Blank";
								}
								break;
							}
							case 36: //"Set Part"
							{
								if (current_tag->Tag_SetPart == 0)
								{
									current_tag->Tag_SetPart = 1;
								}
								break;
							}
							case 37: //"Source"
							{
								if (!(current_tag->Tag_Source.length() > 0))
								{
									current_tag->Tag_Source = L"Blank";
								}
								break;
							}
							case 38: //"Source Form"
							{
								if (!(current_tag->Tag_SourceForm.length() > 0))
								{
									current_tag->Tag_SourceForm = L"Blank";
								}
								break;
							}
							case 39: //"Sub Genre"
							{
								if (!(current_tag->Tag_SubGenre.length() > 0))
								{
									current_tag->Tag_SubGenre = "Blank";
								}
								break;
							}
							case 40: //"Subject"
							{
								if (!(current_tag->Tag_Subject.length() > 0))
								{
									current_tag->Tag_Subject = L"Blank";
								}
								break;
							}
							case 41: //"Synchronised Tempo"
							{
								if (!(current_tag->Tag_SynchronisedTempo.length() > 0))
								{
									current_tag->Tag_SynchronisedTempo = "Blank";
								}
								break;
							}
							case 42: //"Synchronised Text"
							{
								if (!(current_tag->Tag_SynchronisedText.length() > 0))
								{
									current_tag->Tag_SynchronisedText = L"Blank";
								}
								break;
							}
							/*case 43: //"Unsynchronised Text"
							{
								if (!(current_tag->Tag_UnsynchronisedText.length() > 0))
								{
									current_tag->Tag_UnsynchronisedText = L"Blank";
								}
								break;
							}
							case 44: //"User Defined URL"
							{
								if (!(current_tag->Tag_UserDefinedURL.length() > 0))
								{
									current_tag->Tag_UserDefinedURL = "http://";
								}
								break;
							}*/
						}
					}
					MediaPropProc_Tag_TagEntryChange(pageData->g_hTagDialog);
					EndDialog(hwndDlg, IDOK);
					break;
				}
				case IDC_BUTTON_ADDTAG_CANCEL:
				{
					//User pressed cancel, so we exit this dialog
					EndDialog(hwndDlg, wParam);
					return TRUE;
					break;
				}
				case IDC_LIST_ADDTAG:
				{
					switch (HIWORD(wParam))
					{
						case LBN_SELCHANGE:
						{
							//Let's display some infomation about the selected tag to add
							int nItem = 0;

							//Get the selected tag type
							nItem = SendDlgItemMessage(hwndDlg, IDC_LIST_ADDTAG, LB_GETCURSEL, 0, 0);
							char *field_txt = new char[SendDlgItemMessageA(hwndDlg, IDC_LIST_ADDTAG, LB_GETTEXTLEN, nItem, 0)+1];
							SendDlgItemMessageA(hwndDlg, IDC_LIST_ADDTAG, LB_GETTEXT, (nItem), (LPARAM)field_txt);
							uint32 tag_type = 0;
							tag_type = MatroskaTagInfo::GetTagListUInt(field_txt);
							//Now display the infomation about the tag,
							SetDlgItemTextA(hwndDlg, IDC_STATIC_TAGADD_INFO, MatroskaTagInfo::GetTagListStrInfo(tag_type));
							delete field_txt;
							break;
						}	//case LBN_SELCHANGE:
					}
				}
			}
		} //case WM_COMMAND:
	}
	CRASH_PROTECT_END;

	return FALSE;
};

BOOL CALLBACK MediaPropProc_AddSimpleTag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CRASH_PROTECT_START;
	static HWND hwndToolTip = NULL;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
		{
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
			pageData = (MatroskaPages *)lParam;

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

			AddTooltip(hwndToolTip, GetDlgItem(hwndDlg,	IDC_COMBO_TAG_NAME), LPSTR_TEXTCALLBACK);

      SetWindowText(hwndDlg, _W("Add Simple Tag"));
			SetDlgItemText(hwndDlg, IDC_STATIC_NAME, _W("Name:"));
			SetDlgItemText(hwndDlg, IDC_STATIC_VALUE, _W("Value:"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_ADD_SIMPLE_TAG_OK, _W("OK"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_ADD_SIMPLE_TAG_CANCEL, _W("Cancel"));

			// This crashes for some reason
#if 0
			int i = 0;
			HWND hNameCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_NAME);
			SendMessage(hNameCombo, CB_ADDSTRING, 0, (LPARAM)_T("test"));
			while (MatroskaSimpleTag::GetOfficalList(i).name != NULL)
			{
				TCHAR *test = MatroskaSimpleTag::GetOfficalList(i).name;
				SendMessage(hNameCombo, CB_ADDSTRING, 0, (LPARAM)test);
				i++;
			}
#endif
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
		case WM_NOTIFY:
		{
			NMHDR *notifyHeader = (NMHDR *)lParam;
			if (notifyHeader->hwndFrom == GetDlgItem(hwndDlg, IDC_COMBO_TAG_NAME))
			{
				switch (notifyHeader->code)
				{
					case TTN_GETDISPINFO:
					{
						LPNMTTDISPINFO notifyHeaderData = (LPNMTTDISPINFO)notifyHeader;

						int nItem = 0;
						nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_NAME, CB_GETCURSEL, 0, 0);

						TCHAR field_txt[1025];
						GetDlgItemText(hwndDlg, IDC_COMBO_TAG_NAME, field_txt, 1024);

						int i = 0;
						while (MatroskaSimpleTag::GetOfficalList(i).name != NULL)
						{
							if (!_tcscmp(MatroskaSimpleTag::GetOfficalList(i).name, field_txt)) {
								notifyHeaderData->lpszText = MatroskaSimpleTag::GetOfficalList(i).desc;
								break;
							}
							i++;
						}
						break;
					}
				}
			}
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{ // Find which control the message applies to
				case IDC_BUTTON_ADD_SIMPLE_TAG_OK:
				{
					//Add the new simple tag :P
					int nItem = 0;

					int name_txt_len = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_NAME, WM_GETTEXTLENGTH, 0, 0);
					TCHAR *name_txt = new TCHAR[name_txt_len+1];
					SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_NAME, WM_GETTEXT, name_txt_len+1, (LPARAM)name_txt);

					int value_txt_len = SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_VALUE, WM_GETTEXTLENGTH, 0, 0);
					TCHAR *value_txt = new TCHAR[value_txt_len+1];
					SendDlgItemMessage(hwndDlg, IDC_EDIT_TAG_VALUE, WM_GETTEXT, value_txt_len+1, (LPARAM)value_txt);

					//First find which tag is selected
					MatroskaTagInfo *current_tag = GetSelectedTagEntry(pageData->g_hTagDialog);
					if (current_tag != NULL) {
						// Lets add the tag now
						current_tag->Tag_Simple_Tags.SetSimpleTagValue(name_txt, value_txt);
					}

					delete name_txt;
					delete value_txt;

					MediaPropProc_Tag_TagEntryChange(pageData->g_hTagDialog);
					EndDialog(hwndDlg, IDOK);
					break;
				}
				case IDC_BUTTON_ADD_SIMPLE_TAG_CANCEL:
				{
					//User pressed cancel, so we exit this dialog
					EndDialog(hwndDlg, wParam);
					return TRUE;
					break;
				}
				case IDC_COMBO_TAG_NAME:
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
/*
							//Let's display some infomation about the selected tag to add
							int nItem = 0;

							//Get the selected tag type
							nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_NAME, CB_GETCURSEL, 0, 0);

							char field_txt[1025];
							GetDlgItemTextA(hwndDlg, IDC_COMBO_TAG_NAME, field_txt, 1024);

							int i = 0;
							while (MatroskaSimpleTagOfficalList[i].name != NULL)
							{
								if (!strcmp(field_txt, MatroskaSimpleTagOfficalList[i].name)) {


								}
								i++;
							}
*/

							break;
						}	//case LBN_SELCHANGE:
					}
				}
			}
		} //case WM_COMMAND:
	}
	CRASH_PROTECT_END;

	return FALSE;
};

BOOL CALLBACK MediaPropProc_AddUID(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
		{
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
			pageData = (MatroskaPages *)lParam;

      SetWindowText(hwndDlg, _W("Add UID"));
			SetDlgItemText(hwndDlg, IDC_STATIC_ADD_UID_1, _W("Select or type in the UID to add:"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_ADD_UID_OK, _W("OK"));
      SetDlgItemText(hwndDlg, IDC_BUTTON_ADD_UID_CANCEL, _W("Cancel"));

			size_t i;
			for (i = 0; i < pageData->parser->chapters.size(); i++) {
				if (pageData->parser->chapters.at(i)->chapterUID != 0) {
					SmartStringFormat wBuf;
					wBuf << _W("Chapter");
					wBuf << L": ";
					wBuf << pageData->parser->chapters.at(i)->chapterUID;
					SendDlgItemMessage(hwndDlg, IDC_LIST_ADD_UID, LB_ADDSTRING, 0, (LPARAM)wBuf.str().c_str());
				}
			}

			for (i = 0; i < pageData->parser->trackInfos.size(); i++) {		
				if (pageData->parser->trackInfos.at(i)->m_trackUID != 0) {
					SmartStringFormat wBuf;
					wBuf << _W("Track");
					wBuf << L": ";
					wBuf << pageData->parser->trackInfos.at(i)->m_trackUID;
					SendDlgItemMessage(hwndDlg, IDC_LIST_ADD_UID, LB_ADDSTRING, 0, (LPARAM)wBuf.str().c_str());
				}
			}

			for (i = 0; i < pageData->parser->fileAttachments.size(); i++) {
				if (pageData->parser->fileAttachments.at(i)->attachmentUID != 0) {
					SmartStringFormat wBuf;
					wBuf << _W("Attachment");
					wBuf << L": ";
					wBuf << pageData->parser->fileAttachments.at(i)->attachmentUID;
					SendDlgItemMessage(hwndDlg, IDC_LIST_ADD_UID, LB_ADDSTRING, 0, (LPARAM)wBuf.str().c_str());				
				}
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
			{ // Find which control the message applies to
				case IDC_BUTTON_ADD_UID_OK:
				{
					UTFstring selectedUID = GetSelectedListItem(hwndDlg, IDC_LIST_ADD_UID);
					if (selectedUID.length() > 0) {
						MatroskaTagInfo *selected_tag = GetSelectedTagEntry(pageData->g_hTagDialog);

						if (selected_tag != NULL)
						{
							if (!_tcsncmp(selectedUID.c_str(), _W("Track"), _tcslen(_W("Track")))) {
								const char *uidStart = selectedUID.GetUTF8().c_str()+strlen(_("Track"))+2;
								selected_tag->Tag_TrackUID.Add(atoi64(uidStart));

							} else if (!_tcsncmp(selectedUID.c_str(), _W("Chapter"), _tcslen(_W("Chapter")))) {
								const char *uidStart = selectedUID.GetUTF8().c_str()+strlen(_("Chapter"))+2;
								selected_tag->Tag_ChapterUID.Add(atoi64(uidStart));

							} else if (!_tcsncmp(selectedUID.c_str(), _W("Attachment"), _tcslen(_W("Attachment")))) {
								const char *uidStart = selectedUID.GetUTF8().c_str()+strlen(_("Attachment"))+2;
								selected_tag->Tag_AttachmentUID.Add(atoi64(uidStart));
							}

							DisplayTagUIDList(pageData->g_hTagDialog, selected_tag);
						}

					} else {
						MessageBox(hwndDlg, _W("No UID Selected"), NULL, 0);
						break;
					}

					EndDialog(hwndDlg, IDOK);
					break;
				}
				case IDC_BUTTON_ADD_UID_CANCEL:
				{
					//User pressed cancel, so we exit this dialog
					EndDialog(hwndDlg, wParam);
					return TRUE;
					break;
				}
				case IDC_LIST_ADD_UID:
				{
					switch (HIWORD(wParam))
					{
						case LBN_SELCHANGE:
						{
							

							break;
						}	//case LBN_SELCHANGE:
						case LBN_DBLCLK:
						{
							MediaPropProc_AddUID(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD_UID_OK, BN_CLICKED), 0);
							break;
						}
					}
				}
			}
		} //case WM_COMMAND:
	}
	CRASH_PROTECT_END;

	return FALSE;
};

BOOL CALLBACK MediaPropProc_Tag(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	//TRACE("Tag Tab Msg: %X wParam: %i lParam: %i \n", uMsg, wParam, lParam);
	CRASH_PROTECT_START;
	//Get the dialog data
	MatroskaPages *pageData = (MatroskaPages *)GetWindowLong(hwndDlg, DWL_USER);
	switch(uMsg) {
		case WM_INITDIALOG:
		{
			ODS("Displaying Tag Tab");

			INITCOMMONCONTROLSEX common;
			common.dwICC = ICC_DATE_CLASSES;
			common.dwSize = sizeof(common);
			InitCommonControlsEx(&common);

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
			pageData->g_hTagDialog = hwndDlg;
			//Store the MatroskaPages struct in the DWL_USER
			SetWindowLong(hwndDlg, DWL_USER, (LONG)pageData);

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

			/*DisplayTagEntryList(pageData->g_hTagDialog, pageData->parser);

			// Update the tabs
			if (pageData->g_hInfoDialog != NULL)
				DisplayMainTrackList(pageData->g_hInfoDialog, pageData->parser);
			if (pageData->g_hSimpleTagDialog != NULL)
				DisplaySimpleTagTrackList(pageData->g_hSimpleTagDialog, pageData->parser);
			if (pageData->g_hAttachmentDialog != NULL)
				DisplayAttachmentList(pageData->g_hAttachmentDialog, pageData->parser);
*/
#endif

			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_FRAME_BORDER, _W("The Matroska Tagger"));
      //SetDlgItemText(hwndDlg, IDC_STATIC_TAG_1, _W("Track"));
			//SetDlgItemText(hwndDlg, IDC_STATIC_TRACK_UID, _W("Track UID"));
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_ENTRIES, _W("Tag Entries:"));
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_ASSIGNED_UID, _W("Assigned UIDs:"));
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_2, _W("Tag Type:"));
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_3, _W("Tag Field:"));
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_4, _W("Field Text:"));
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_5, _W("Tag Entry Tags"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_TAG_ENTRY_ADD, _W("Add"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_TAG_ENTRY_DELETE, _W("Delete"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_TAG_UID_ADD, _W("Add"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_TAG_UID_DELETE, _W("Delete"));

			SetDlgItemText(hwndDlg, IDC_BUTTON_ADDTAG, _W("Add"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_DELETETAG, _W("Delete"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_EXPORT_XML, _W("Export as XML"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_TAG_IMPORT, _W("Import"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_TAGS_RELOAD, _W("Reload"));
			SetDlgItemText(hwndDlg, IDC_BUTTON_SAVE, _W("Save"));

#ifdef MATROSKA_PROP
		if (!MatroskaShellExt_GetRegistryValue(_T("Auto-Reload Tags after Save"), 1))
			ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_TAGS_RELOAD), SW_HIDE);
#else
		if (!MatroskaCDL_GetRegistryValue(_T("Auto-Reload Tags after Save"), 1))
			ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_TAGS_RELOAD), SW_HIDE);

#endif

#ifndef USING_TINYXML
			ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON_EXPORT_XML), SW_HIDE);
#endif
			break;
		}
		case WM_DESTROY:
		{
#ifdef MATROSKA_PROP
			pageData->Release();
#endif // MATROSKA_PROP
			break;
		}
		case WM_NOTIFY:
		{
			NMHDR *notifyHeader = (NMHDR *)lParam;
			if ((notifyHeader->hwndFrom == GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE))
					|| (notifyHeader->hwndFrom == GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME)))
			{
				LPNMDATETIMECHANGE notifyHeaderData = (LPNMDATETIMECHANGE)notifyHeader;
				switch (notifyHeaderData->nmhdr.code)
				{
					case DTN_DATETIMECHANGE:
					{
						HWND hwndTagFieldCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_FIELD);
						LRESULT nItem = SendMessage(hwndTagFieldCombo, CB_GETCURSEL, 0, 0); ;
						if (nItem != CB_ERR)
						{
							int64 *date_field = reinterpret_cast<int64 *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
							if (date_field != NULL) {
								SYSTEMTIME theTime;
								tm new_date;

								DateTime_GetSystemtime(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), &theTime);
								new_date.tm_hour = theTime.wHour;
								new_date.tm_min = theTime.wMinute;
								new_date.tm_sec = theTime.wSecond;

								DateTime_GetSystemtime(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), &theTime);
								new_date.tm_mon = theTime.wMonth-1;
								new_date.tm_mday = theTime.wDay;
								new_date.tm_year = theTime.wYear-1900;
								new_date.tm_isdst = -1;

								*date_field = _mktime64(&new_date);
							}
						}
						break;
					};
				}
			}
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))	{ /* Find which control the message applies to */
				case IDC_BUTTON_SAVE:
				{
					ODS("MediaPropProc_Tag::Matroska Tag Save button pressed.");
          if (MatroskaShellExt_GetRegistryValue(_T("Display Tag Warning"), 1) == 1) {
            int ret = MessageBox(hwndDlg, _T("Please note the tag writing functionalalty is outdated and should not be used. Are you sure you wish to write tags?"), _T("Warning"), MB_YESNO);
            if (ret == IDNO) {
              ODS("MediaPropProc_Tag::Tag Save aborted");
              break;
            }
          }
					try {
						int ret = pageData->parser->WriteTags();
						if (ret == 0)
							MessageBox(hwndDlg, _W("Tags Written."), _W("Success"), 0);

						MediaPropProc_Tag(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_TAGS_RELOAD, BN_CLICKED), BN_CLICKED);
					} catch (std::exception &ex) {
						UTFstring errMsg;
						errMsg.SetUTF8(ex.what());

						MessageBox(hwndDlg, errMsg.c_str(), _W("Failed to write tags"), 0);
					}
					break;
				}
				case IDC_BUTTON_ADDTAG:
				{
					ODS("MediaPropProc_Tag::Matroska Add tag button pressed.");

					HWND hwndAddTagDialog = CreateDialogParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_SIMPLE_TAG), hwndDlg, MediaPropProc_AddSimpleTag, (LPARAM)pageData);
					ShowWindow(hwndAddTagDialog, SW_SHOW);
					break;
				}
				case IDC_BUTTON_DELETETAG:
				{
					MediaPropProc_Tag_DeleteTag(hwndDlg);
					break;
				}
				case IDC_BUTTON_EXPORT_XML:
				{
					ODS("MediaPropProc_Tag::Export Tags as XML button pressed");
#ifdef USING_TINYXML
					JString xmlFilename;
					//Get the XML target filename
					OPENFILENAME saveFileAs;
					TCHAR fileName[1024];
					memset(&saveFileAs, 0, sizeof(OPENFILENAME));
					memset(fileName, 0, sizeof(fileName));
					saveFileAs.lStructSize =  sizeof(OPENFILENAME);
					saveFileAs.hwndOwner = hwndDlg;
					saveFileAs.lpstrFile = fileName;
					saveFileAs.nMaxFile	= sizeof(fileName);
					saveFileAs.lpstrFilter = _T("XML Files(*.xml)\0 *.xml\0\0");
					saveFileAs.lpstrDefExt = _T("xml");
					saveFileAs.lpstrTitle	= _W("Export As XML To...");
					saveFileAs.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
					if (GetSaveFileName(&saveFileAs)) {
						xmlFilename = fileName;
						int ret = pageData->parser->SaveTagsToXMLFile(xmlFilename);
						if (ret != 0)
							MessageBox(hwndDlg, _W("Failed to save XML file."), _W("Unable to save"), 0);
					}
#endif // USING_TINYXML
					break;
				}
				case IDC_BUTTON_TAG_IMPORT:
				{
					MatroskaTagInfo *selected_tag = GetSelectedTagEntry(hwndDlg);
					if (selected_tag != NULL) {
						JString aviFilename;
						//Get the AVI filename
						OPENFILENAME openFile;
						TCHAR fileName[1024];
						memset(&openFile, 0, sizeof(OPENFILENAME));
						memset(fileName, 0, sizeof(fileName));
						openFile.lStructSize =  sizeof(OPENFILENAME);
						openFile.hwndOwner = hwndDlg;
						openFile.lpstrFile = fileName;
						openFile.nMaxFile	= sizeof(fileName);
						openFile.lpstrFilter = _T("AVI Files(*.avi)\0 *.avi\0MP3 Files(*.mp3)\0 *.mp3\0\0");
						openFile.lpstrTitle	= _W("Open file to import tags...");
						openFile.Flags = OFN_EXPLORER;
						if (GetOpenFileName(&openFile)) {
							aviFilename = fileName;
							if (!wcsicmp((aviFilename.c_str()+aviFilename.length()-3), L"avi")) {
								AviTagReader aviTags(aviFilename);
								if (aviTags.HasTags()) {
									aviTags.ImportTags(selected_tag);
								}
							} else if (!wcsicmp((aviFilename.c_str()+aviFilename.length()-3), L"mp3")) {
								MP3TagReader mp3Tags(aviFilename);
								if (mp3Tags.HasTags()) {
									mp3Tags.ImportTags(selected_tag);
								}
							} else {

							}
						}
					}
					break;
				}
				case IDC_BUTTON_TAG_ENTRY_ADD:
				{
					/*
					MatroskaInfoParser *parser;
					int64 parserStartMemorySize;
					int64 parserEndMemorySize;
					int i;
					int f;
					clock_t c;
					wchar_t *filenames[] =
					{					
					L"I:\\videos\\3_videos.mkv",
					L"I:\\Kim - Two to Tutor.mkv",
					L"I:\\The Princess Diaries.avs.ogg.mkv",
					L"I:\\videos\\105-Videos_105\\Kim Possible - The Ron Factor.mkv", 
					L"I:\\videos\\PBS\\Western Tradition - Program 8.mkv", 
					L"I:\\videos\\105-Videos_105\\The Princess Diaries.mkv", 
					NULL};

					f = 0;
					while (filenames[f] != NULL) {
						c = clock();
						YourAllocHook(1000, &parserStartMemorySize, 0, 0, 0, 0, 0);								
						for (i = 0; i < 3; i++) {
							parser = new MatroskaInfoParser(filenames[f]);
							parser->m_parseSeekHead = true;
							parser->m_parseAttachments = false;
							parser->m_parseTags = true;
							parser->ParseFile();
						}
						YourAllocHook(1000, &parserEndMemorySize, 0, 0, 0, 0, 0);
						c = clock() - c;
						NOTE3("Avg Memory Size for %ws, %i bytes %i ticks", filenames[f], (int)(parserEndMemorySize - parserStartMemorySize / 3), c/3);
						f++;
					}
					*/

					pageData->parser->tags.push_back(new MatroskaTagInfo());
					DisplayTagEntryList(hwndDlg, pageData->parser);
					break;
				}
				case IDC_BUTTON_TAG_ENTRY_DELETE:
				{
					MatroskaTagInfo *selected_tag = GetSelectedTagEntry(hwndDlg);

					if (selected_tag != NULL) {
						pageData->parser->tags.Remove(selected_tag);
						DisplayTagEntryList(hwndDlg, pageData->parser);
					}
					break;
				}
				case IDC_BUTTON_TAG_UID_ADD:
				{
					HWND hwndAddUIDDialog = CreateDialogParam(pageData->g_hAppInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_UID), hwndDlg, MediaPropProc_AddUID, (LPARAM)pageData);
					ShowWindow(hwndAddUIDDialog, SW_SHOW);
					break;
				}
				case IDC_BUTTON_TAG_UID_DELETE:
				{
					UTFstring selectedUID = GetSelectedListItem(hwndDlg, IDC_LIST_TAG_ASSIGNED_UID);

					if (selectedUID.length() > 0) {
						MatroskaTagInfo *selected_tag = NULL;
						selected_tag = GetSelectedTagEntry(hwndDlg);

						if (selected_tag != NULL)
						{
							if (!_tcsncmp(selectedUID.c_str(), _W("Track"), _tcslen(_W("Track")))) {
								const char *uidStart = selectedUID.GetUTF8().c_str()+strlen(_("Track"))+2;
								selected_tag->Tag_TrackUID.Remove(atoi64(uidStart));

							} else if (!_tcsncmp(selectedUID.c_str(), _W("Chapter"), _tcslen(_W("Chapter")))) {
								const char *uidStart = selectedUID.GetUTF8().c_str()+strlen(_("Chapter"))+2;
								selected_tag->Tag_ChapterUID.Remove(atoi64(uidStart));

							} else if (!_tcsncmp(selectedUID.c_str(), _W("Attachment"), _tcslen(_W("Attachment")))) {
								const char *uidStart = selectedUID.GetUTF8().c_str()+strlen(_("Attachment"))+2;
								selected_tag->Tag_AttachmentUID.Remove(atoi64(uidStart));
							}

							DisplayTagUIDList(hwndDlg, selected_tag);
						}
					}
					break;
				}
				case IDC_LIST_TAG_ENTRIES:
				{
					switch (HIWORD(wParam))
          {
						case LBN_SELCHANGE:
						{
							MediaPropProc_Tag_TagEntryChange(hwndDlg);

							break;
						}
					}
					break;
				}
				case IDC_LIST_TAGS:
				{
					switch (HIWORD(wParam))
          {
						case LBN_SELCHANGE:
						{
							MediaPropProc_Tag_TagListChange(hwndDlg);
							break;
						}
					}
					break;
				}
				case IDC_COMBO_TAG_TYPE:
				{
					switch (HIWORD(wParam))
          {
						case CBN_SELCHANGE:
						{
							MediaPropProc_Tag_TagTypeChange(hwndDlg);
						}
					}
					break;
				}
				case IDC_COMBO_TAG_FIELD:
				{
					switch (HIWORD(wParam))
          {
						case CBN_SELCHANGE:
						{
							MediaPropProc_Tag_TagFieldChange(hwndDlg, pageData);
							break;
						}
					}
					break;
				}

				case IDC_EDIT_SELECTED_TAG_TEXT: //The ID of our main text edit box
				{
					switch (HIWORD(wParam))
					{
						case EN_CHANGE: //The user changed the text
						{
							MediaPropProc_Tag_TagTextChange(hwndDlg, pageData);
							break;
						}
					}
					break;
				}
				case IDC_BUTTON_TAGS_RELOAD:
				{
					switch (HIWORD(wParam))
          {
						case BN_CLICKED:
						{
							//Reload the tags
							//Clean-up the fields
							SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_RESETCONTENT, 0, 0);
							SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ENTRIES, CB_RESETCONTENT, 0, 0);
							SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_RESETCONTENT, 0, 0);
							SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_RESETCONTENT, 0, 0);
							SendDlgItemMessage(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, 0);

							//Free memory used by parser
							if (pageData->parser != NULL) {
								ODS("Reloading Tags per Users request");

								delete pageData->parser;
								pageData->parser = new MatroskaInfoParser(pageData->szFile);
								pageData->parser->m_parseSeekHead = true;
								pageData->parser->m_parseAttachments = true;
								pageData->parser->m_parseTags = true;
								ParseFileMT(pageData, hwndDlg);
							}
							break;
						}
					}
				}
				default:
				{
					//
				}
			}
			break;
		}
		case WM_CLOSE:
		{
			//Clean-up the fields
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ENTRIES, LB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, 0);
			pageData->g_hTagDialog = NULL;
			return FALSE;
		}
		default:
			return FALSE;
	}
	CRASH_PROTECT_END;

	return FALSE;
}



int DisplayTagEntryList(HWND hwndDlg, MatroskaInfoParser *parser)
{
	ODS("DisplayTagEntryList()");

	if ((hwndDlg == NULL) || (parser == NULL))
		return -1;

#if 0
	if (!parser->really_found_tags)
		SetDlgItemText(hwndDlg, IDC_STATIC_TAG_FOUND, _W("No tag element found"));
	else
		SetDlgItemText(hwndDlg, IDC_STATIC_TAG_FOUND, _W("Tag element found"));
#endif

	MatroskaTagInfo *current_tag = NULL;
	int nItem = 0;

	// Clear the Tag UID List
	SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ASSIGNED_UID, LB_RESETCONTENT, 0, 0);

	// Clear the Tag List
	SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_RESETCONTENT, 0, 0);

	//Clear the tag entries list
	SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ENTRIES, LB_RESETCONTENT, 0, 0);

 	for (size_t i = 0; i < parser->tags.size(); i++)
	{
		current_tag = parser->tags.at(i);

		JString tag_txt = _W("Tag");
		if (parser->tags.size() < 9)
		{
			tag_txt += JString::Format(L" %.1u", i+1);
		}
		else if (parser->tags.size() < 99)
		{
			tag_txt += JString::Format(L" %.2u", i+1);
		}
		else if (parser->tags.size() < 999)
		{
			tag_txt += JString::Format(L" %.3u", i+1);
		}

		nItem = SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ENTRIES, LB_ADDSTRING, 0, (LPARAM)tag_txt.t_str());
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ENTRIES, LB_SETITEMDATA, nItem, reinterpret_cast<LPARAM>(current_tag));
	}

	//Select the first item, for the user
	SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ENTRIES, LB_SETCURSEL, 0, 0);
	MediaPropProc_Tag(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_LIST_TAG_ENTRIES, LBN_SELCHANGE), LBN_SELCHANGE);

	return 0;
};

int DisplayTagUIDList(HWND hwndDlg, MatroskaTagInfo *tag)
{
	CRASH_PROTECT_START;

	// Clear the Tag UID List
	SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ASSIGNED_UID, LB_RESETCONTENT, 0, 0);

	// Add the UIDs to the list
	if (tag->Tag_TrackUID != 0)
	{
		for (size_t u = 0; u < tag->Tag_TrackUID.size(); u++) {
			SmartStringFormat wBuf;
			wBuf << _W("Track");
			wBuf << L": ";
			wBuf << tag->Tag_TrackUID.at(u);
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ASSIGNED_UID, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
		}
	}

	if (tag->Tag_ChapterUID != 0)
	{
		for (size_t u = 0; u < tag->Tag_ChapterUID.size(); u++) {
			SmartStringFormat wBuf;
			wBuf << _W("Chapter");
			wBuf << L": ";
			wBuf << tag->Tag_ChapterUID.at(u);
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ASSIGNED_UID, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
		}
	}

	if (tag->Tag_AttachmentUID != 0)
	{
		for (size_t u = 0; u < tag->Tag_AttachmentUID.size(); u++) {
			SmartStringFormat wBuf;
			wBuf << _W("Attachment");
			wBuf << L": ";
			wBuf << tag->Tag_AttachmentUID.at(u);
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAG_ASSIGNED_UID, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
		}
	}

	CRASH_PROTECT_END;

	return 0;
}

MatroskaTagInfo *GetSelectedTagEntry(HWND hTagDlg)
{
	int nItem = 0;
	HWND hwndTagEntryList = GetDlgItem(hTagDlg, IDC_LIST_TAG_ENTRIES);
	nItem = SendMessage(hwndTagEntryList, LB_GETCURSEL, 0, 0);
	if (nItem != -1)
		return reinterpret_cast<MatroskaTagInfo *>(SendMessage(hwndTagEntryList, LB_GETITEMDATA, (nItem), 0));

	return NULL;
};

void MediaPropProc_Tag_DeleteTag(HWND hTagDlg)
{
	CRASH_PROTECT_START;

	HWND hwndList;
	int nItem = 0;
	int last_item = 0;
	wchar_t tag_txt[256];

	//Get the selected tag list index
	hwndList = GetDlgItem(hTagDlg, IDC_LIST_TAGS);
	nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (nItem == -1) {
		//No item selected, exit
		return;
	}
	//Get the selected tag text in the listbox
	wchar_t *field_txt = new wchar_t[SendMessage(hwndList, LB_GETTEXTLEN, nItem, 0)+1];
	SendMessage(hwndList, LB_GETTEXT, (nItem), (LPARAM)field_txt);

	//We need this do found out what type of Tag this is

	wcscpy(tag_txt, JString(field_txt).BeforeFirst(L':').c_str());
	//tag_txt = wcsncpy(tag_txt, field_txt, (wcschr(field_txt, ':')-field_txt));
	if (!wcsicmp(tag_txt, _W("MultiComment"))) {
		MatroskaTagMultiCommentItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiCommentItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}
	}else if (!wcsicmp(tag_txt, _W("MultiCommercial"))) {
		MatroskaTagMultiCommercialItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiCommercialItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}
	}else if (!wcsicmp(tag_txt, _W("MultiDate"))) {
		MatroskaTagMultiDateItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiDateItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}
	}else if (!wcsicmp(tag_txt, _W("MultiEntity"))) {
		MatroskaTagMultiEntityItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiEntityItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}
	}else if (!wcsicmp(tag_txt, _W("MultiIdentifier"))) {
		MatroskaTagMultiIdentifierItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiIdentifierItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}
	}else if (!wcsicmp(tag_txt, _W("MultiLegal"))) {
		MatroskaTagMultiLegalItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiLegalItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}
	}else if (!wcsicmp(tag_txt, _W("MultiTitle"))) {
		MatroskaTagMultiTitleItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiTitleItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->Invalidate();
		}

	}else if (!wcsicmp(tag_txt, _W("Simple Tag"))) {
		MatroskaSimpleTag *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaSimpleTag *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			selected_tag->bDelete = true;
		}

	}else if (!wcsicmp(tag_txt, _W("Abstract"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Archival Location"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Gain"))) {
		double *selected_tag = NULL;
		selected_tag = reinterpret_cast<double *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Genre"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Peak"))) {
		double *selected_tag = NULL;
		selected_tag = reinterpret_cast<double *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Secondary Genre"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Bibliography"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("BPM"))) {
		double *selected_tag = NULL;
		selected_tag = reinterpret_cast<double *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Capture DPI"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Capture Palette Setting"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Cropped"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Disc Track"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Encoder"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Encode Settings"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("File"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Initial Key"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Keywords"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Language"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Length"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Official Audio File URL"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Official Audio Source URL"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Original Dimensions"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Original Media Type"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Play Counter"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Playlist Delay"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Popularimeter"))) {
		int32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<int32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Record Location"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Set Part"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = 0;
		}

	}else if (!wcsicmp(tag_txt, _W("Source"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Source Form"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}

	}else if (!wcsicmp(tag_txt, _W("Sub Genre"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Subject"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}
	}else if (!wcsicmp(tag_txt, _W("User Defined URL"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = "";
		}

	}else if (!wcsicmp(tag_txt, _W("Muxing App"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}
	}else if (!wcsicmp(tag_txt, _W("Writing App"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			*selected_tag = L"";
		}
	}

	//Select the first field, we deleted a field !
	SendDlgItemMessage(hTagDlg, IDC_COMBO_TAG_FIELD, CB_SETCURSEL, 0, 0);
	MediaPropProc_Tag(hTagDlg, WM_COMMAND, MAKEWPARAM(IDC_COMBO_TAG_FIELD, CBN_SELCHANGE), CBN_SELCHANGE);

	delete field_txt;
	field_txt = NULL;

	CRASH_PROTECT_END;
};

void MediaPropProc_Tag_TagEntryChange(HWND hwndDlg)
{
	CRASH_PROTECT_START;

	ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT), SW_NORMAL);

	MatroskaTagInfo *selected_tag = GetSelectedTagEntry(hwndDlg);
	if (selected_tag != NULL)
	{
		int tag_items = 0;
		//Clear the existing items
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, 0);

		DisplayTagUIDList(hwndDlg, selected_tag);

		//Fill out the Tag fields
		if (selected_tag->Tag_ArchivalLocation.length() > 1)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Archival Location");
			wBuf << L": ";
			wBuf << selected_tag->Tag_ArchivalLocation.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_ArchivalLocation));
		}
		if (selected_tag->Tag_AudioGain != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Audio Gain");
			wBuf << L": ";
			wBuf << selected_tag->Tag_AudioGain;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_AudioGain));
		}
		if (selected_tag->Tag_AudioGenre.length() > 1)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Audio Genre");
			wBuf << L": ";
			wBuf << selected_tag->Tag_AudioGenre.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_AudioGenre));
		}
		if (selected_tag->Tag_AudioPeak != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Audio Peak");
			wBuf << L": ";
			wBuf << selected_tag->Tag_AudioPeak;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_AudioPeak));
		}
		if (selected_tag->Tag_AudioSecondaryGenre.length() > 1)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Audio Secondary Genre");
			wBuf << L": ";
			wBuf << selected_tag->Tag_AudioSecondaryGenre.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_AudioSecondaryGenre));
		}
		if (selected_tag->Tag_Bibliography.length() > 1)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Bibliography");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Bibliography.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Bibliography));
		}
		if (selected_tag->Tag_BPM != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("BPM");
			wBuf << L": ";
			wBuf << selected_tag->Tag_BPM;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_BPM));
		}
		if (selected_tag->Tag_CaptureDPI != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Capture DPI");
			wBuf << L": ";
			wBuf << selected_tag->Tag_CaptureDPI;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_CaptureDPI));
		}
		if (selected_tag->Tag_CapturePaletteSetting != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Capture Palette Setting");
			wBuf << L": ";
			wBuf << selected_tag->Tag_CapturePaletteSetting;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_CapturePaletteSetting));
		}
		if (selected_tag->Tag_Cropped.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Cropped");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Cropped.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Cropped));
		}
		if (selected_tag->Tag_DiscTrack != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Disc Track");
			wBuf << L": ";
			wBuf << selected_tag->Tag_DiscTrack;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_DiscTrack));
		}
		if (selected_tag->Tag_Encoder.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Encoder");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Encoder.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Encoder));
		}

		if (selected_tag->Tag_EncodeSettings.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Encode Settings");
			wBuf << L": ";
			wBuf << selected_tag->Tag_EncodeSettings.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_EncodeSettings));
		}
		if (selected_tag->Tag_File.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("File");
			wBuf << L": ";
			wBuf << selected_tag->Tag_File.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_File));
		}
		if (selected_tag->Tag_InitialKey.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Initial Key");
			wBuf << L": ";
			wBuf << selected_tag->Tag_InitialKey.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_InitialKey));
		}
		if (selected_tag->Tag_Keywords.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Keywords");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Keywords.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Keywords));
		}
		if (selected_tag->Tag_Language.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Language");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Language.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Language));
		}
		if (selected_tag->Tag_Length != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Length");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Length;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Length));
		}
		if (selected_tag->Tag_Mood.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Mood");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Mood.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Mood));
		}
		if (selected_tag->Tag_OfficialAudioFileURL.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Official Audio File URL");
			wBuf << L": ";
			wBuf << selected_tag->Tag_OfficialAudioFileURL.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_OfficialAudioFileURL));
		}
		if (selected_tag->Tag_OfficialAudioSourceURL.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Official Audio Source URL");
			wBuf << L": ";
			wBuf << selected_tag->Tag_OfficialAudioSourceURL.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_OfficialAudioSourceURL));
		}
		if (selected_tag->Tag_OriginalDimensions.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Original Dimensions");
			wBuf << L": ";
			wBuf << selected_tag->Tag_OriginalDimensions.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_OriginalDimensions));
		}
		if (selected_tag->Tag_OriginalMediaType.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Original Media Type");
			wBuf << L": ";
			wBuf << selected_tag->Tag_OriginalMediaType.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_OriginalMediaType));
		}
		if (selected_tag->Tag_PlayCounter != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Play Counter");
			wBuf << L": ";
			wBuf << selected_tag->Tag_PlayCounter;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_PlayCounter));
		}
		if (selected_tag->Tag_PlaylistDelay != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Playlist Delay");
			wBuf << L": ";
			wBuf << selected_tag->Tag_PlaylistDelay;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_PlaylistDelay));
		}
		if (selected_tag->Tag_Popularimeter != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Popularimeter");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Popularimeter;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Popularimeter));
		}
		if (selected_tag->Tag_Product.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Product");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Product.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Product));
		}
		//How should I read & display the Rating tag?
		/*if (selected_tag->Tag_Rating.length() > 1)
		{
		snwprintf(wide_tag_txt, 1024, L"Rating: %ws", selected_tag->Tag_Rating);
		tag_items = SendDlgItemMessageW(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wide_tag_txt);
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Rating));
		}*/
		if (selected_tag->Tag_RecordLocation.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Record Location");
			wBuf << L": ";
			wBuf << selected_tag->Tag_RecordLocation.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_RecordLocation));
		}
		if (selected_tag->Tag_SetPart != 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Set Part");
			wBuf << L": ";
			wBuf << selected_tag->Tag_SetPart;
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_SetPart));
		}
		if (selected_tag->Tag_Source.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Source");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Source.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Source));
		}
		if (selected_tag->Tag_SourceForm.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Source Form");
			wBuf << L": ";
			wBuf << selected_tag->Tag_SourceForm.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_SourceForm));
		}
		if (selected_tag->Tag_SubGenre.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Sub Genre");
			wBuf << L": ";
			wBuf << selected_tag->Tag_SubGenre.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_SubGenre));
		}
		if (selected_tag->Tag_Subject.length() > 0)
		{
			SmartStringFormat wBuf;
			wBuf << _W("Subject");
			wBuf << L": ";
			wBuf << selected_tag->Tag_Subject.c_str();
			tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
			SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Subject));
		}
		/*if (selected_tag->Tag_SynchronisedTempo.length() > 0)
		{
		_sntprintf(tag_txt, 1024, _W("Synchronised Tempo: %S"), selected_tag->Tag_SynchronisedTempo.c_str());
		tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) tag_txt);
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_SynchronisedTempo));
		}
		if (selected_tag->Tag_SynchronisedText.length() > 0)
		{
		_sntprintf(tag_txt, 1024, _W("Synchronised Text: %s"), selected_tag->Tag_SynchronisedText.c_str());
		tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) tag_txt);
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_SynchronisedText));
		}
		if (selected_tag->Tag_UnsynchronisedText.length() > 0)
		{
		_sntprintf(tag_txt, 1024, _W("Unsynchronised Text: %s"), selected_tag->Tag_UnsynchronisedText.c_str());
		tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) tag_txt);
		}
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_UnsynchronisedText));
		}
		if (selected_tag->Tag_UserDefinedURL.length() > 0)
		{
		_sntprintf(tag_txt, 1024, _W("User Defined URL: %S"), selected_tag->Tag_UserDefinedURL.c_str());
		tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) tag_txt);
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_UserDefinedURL));
		}*/
		//Another binary data tag
		/*if (selected_tag->Tag_VideoGenre.length() > 1)
		{
		snwprintf(wide_tag_txt, 1024, L"Subject: %ws", selected_tag->Tag_Subject.c_str());
		tag_items = SendDlgItemMessageW(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) tag_txt);
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Subject));
		}
		if (selected_tag->Tag_VideoSecondaryGenre.length() > 1)
		{
		snwprintf(wide_tag_txt, 1024, L"Subject: %ws", selected_tag->Tag_Subject.c_str());
		tag_items = SendDlgItemMessageW(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) tag_txt);
		SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(&selected_tag->Tag_Subject));
		}*/
		if (selected_tag->Tag_Simple_Tags.size() > 0)
		{
			MatroskaSimpleTag *simpleTag = NULL;
			for (size_t t = 0; t < selected_tag->Tag_Simple_Tags.size(); t++)
			{
				simpleTag = selected_tag->Tag_Simple_Tags.at(t);
				if (simpleTag != NULL)
				{
					if (simpleTag->bDelete) {
						selected_tag->Tag_Simple_Tags.Cleanup();
						t--;
						continue;
					}
					SmartStringFormat wBuf;
					wBuf << _W("Simple Tag");
					wBuf << L": ";
					wBuf << simpleTag->name.c_str();
					wBuf << L"=";
					wBuf << simpleTag->value.c_str();
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(simpleTag));
				}
			}
		}
		if (selected_tag->Tag_MultiComments != NULL)
		{
			MatroskaTagMultiCommentItem *comment_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiComments->GetMultiCommentCount(); c++)
			{
				comment_item =  selected_tag->Tag_MultiComments->GetItem(c);
				if (comment_item != NULL && comment_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiComment");
					wBuf << L": ";
					wBuf << _W("Language");
					wBuf << L": ";
					wBuf << comment_item->Language.c_str();
					wBuf << L" | ";
					wBuf << _W("Comment");
					wBuf << L": ";
					wBuf << comment_item->Comments.c_str();
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(comment_item));
				}
			}
		}
		if (selected_tag->Tag_MultiCommercials != NULL)
		{
			MatroskaTagMultiCommercialItem *commercial_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiCommercials->GetMultiCommercialCount(); c++)
			{
				commercial_item = selected_tag->Tag_MultiCommercials->GetItem(c);;
				if (commercial_item != NULL && commercial_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiCommercial");
					wBuf << L": ";
					wBuf << _W("Type");
					wBuf << L": ";
					wBuf << commercial_item->Type;
					wBuf << L" | ";
					wBuf << _W("Currency");
					wBuf << L": ";
					wBuf << commercial_item->PriceCurrency.c_str();
					wBuf << L" | ";
					wBuf << _W("Price");
					wBuf << L": ";
					wBuf << commercial_item->PriceAmount;
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(commercial_item));
				}
			}
		}
		if (selected_tag->Tag_MultiDates != NULL)
		{
			MatroskaTagMultiDateItem *date_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiDates->GetMultiDateCount(); c++)
			{
				date_item = selected_tag->Tag_MultiDates->GetItem(c);;
				if (date_item != NULL && date_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiDate");
					wBuf << L": ";
					wBuf << _W("Type");
					wBuf << L": ";
					wBuf << date_item->Type;
					wBuf << L" | ";
					wBuf << _W("Date Begin");
					wBuf << L": ";
					wBuf << date_item->DateBegin;
					wBuf << L" | ";
					wBuf << _W("Date End");
					wBuf << L": ";
					wBuf << date_item->DateEnd;
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(date_item));
				}
			}
		}
		if (selected_tag->Tag_MultiEntities != NULL)
		{
			MatroskaTagMultiEntityItem  *entity_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiEntities->GetMultiEntityCount(); c++)
			{
				entity_item = selected_tag->Tag_MultiEntities->GetItem(c);;
				if (entity_item != NULL && entity_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiEntity");
					wBuf << L": ";
					wBuf << _W("Type");
					wBuf << L": ";
					wBuf << entity_item->Type;
					wBuf << L" | ";
					wBuf << _W("Name");
					wBuf << L": ";
					wBuf << entity_item->Name.c_str();
					wBuf << L" | ";
					wBuf << _W("URL");
					wBuf << L": ";
					wBuf << entity_item->URL.c_str();
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(entity_item));
				}
			}
		}
		if (selected_tag->Tag_MultiIdentifiers != NULL)
		{
			MatroskaTagMultiIdentifierItem *title_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiIdentifiers->GetMultiIdentifierCount(); c++)
			{
				title_item = selected_tag->Tag_MultiIdentifiers->GetItem(c);;
				if (title_item != NULL && title_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiIdentifier");
					wBuf << L": ";
					wBuf << _W("Type");
					wBuf << L": ";
					wBuf << title_item->Type;
					wBuf << L" | ";
					wBuf << _W("String Data");
					wBuf << L": ";
					wBuf << title_item->StringData.c_str();
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(title_item));
				}
			}
		}
		if (selected_tag->Tag_MultiLegals != NULL)
		{
			MatroskaTagMultiLegalItem *title_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiLegals->GetMultiLegalCount(); c++)
			{
				title_item = selected_tag->Tag_MultiLegals->GetItem(c);;
				if (title_item != NULL && title_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiLegal");
					wBuf << L": ";
					wBuf << _W("Type");
					wBuf << L": ";
					wBuf << title_item->Type;
					wBuf << L" | ";
					/*wBuf << _W("Text");
					wBuf << L": ";
					wBuf << title_item->Text;
					wBuf << L" | ";*/
					wBuf << _W("URL");
					wBuf << L": ";
					wBuf << title_item->URL.c_str();
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(title_item));
				}
			}
		}

		if (selected_tag->Tag_MultiTitles != NULL)
		{
			MatroskaTagMultiTitleItem *title_item = NULL;
			for (int c = 0; c < selected_tag->Tag_MultiTitles->GetMultiTitleCount(); c++)
			{
				title_item = selected_tag->Tag_MultiTitles->GetItem(c);;
				if (title_item != NULL && title_item->IsValid())
				{
					SmartStringFormat wBuf;
					wBuf << _W("MultiTitle");
					wBuf << L": ";
					wBuf << _W("Type");
					wBuf << L": ";
					wBuf << title_item->Type;
					wBuf << L" | ";
					wBuf << _W("Name");
					wBuf << L": ";
					wBuf << title_item->Name.c_str();
					if (title_item->SubTitle.length() > 0) {
						wBuf << L" | ";
						wBuf << _W("Subtitle");
						wBuf << L": ";
						wBuf << title_item->SubTitle.c_str();
					}
					if (title_item->URL.length() > 0) {
						wBuf << L" | ";
						wBuf << _W("URL");
						wBuf << L": ";
						wBuf << title_item->URL.c_str();
					}
					tag_items = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_ADDSTRING, 0, (LPARAM) wBuf.str().c_str());
					SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_SETITEMDATA, tag_items, reinterpret_cast<LPARAM>(title_item));
				}
			}
		}
	}

	CRASH_PROTECT_END;
};

void MediaPropProc_Tag_TagTextChange(HWND hwndDlg, MatroskaPages *pageData)
{
	CRASH_PROTECT_START;

	//Selected field
	int nItem = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETCURSEL, 0, 0);
	if (nItem != -1)
	{
		wchar_t *field_txt = new wchar_t[SendDlgItemMessageW(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETLBTEXTLEN, nItem, 0)+3];
		SendDlgItemMessageW(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETLBTEXT, (nItem), (LPARAM)field_txt);

		if (!wcsicmp(field_txt, _W("Unicode String"))) {
			//Should have a pointer to the Comments UTFstring in the ItemData
			UTFstring *string_field = reinterpret_cast<UTFstring *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (string_field != NULL) {
				wchar_t *tag_txt = NULL;
				tag_txt = new wchar_t[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				if (pageData->g_bUseUnicodeWinAPI)
				{
					//Unicode is supported
					GetDlgItemText(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				}else {
					char *ansi_tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
					GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, ansi_tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
					snwprintf(tag_txt, 1024, L"%ws", ansi_tag_txt);
					delete ansi_tag_txt;
				}
				*string_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("ASCII String"))) {
			//Should have a pointer to a std::string in the ItemData
			std::string *ascii_string_field = reinterpret_cast<std::string *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (ascii_string_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				ascii_string_field->erase();
				ascii_string_field->append(tag_txt);
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Floating Point Number"))) {
			//Should have a pointer to a double in the ItemData
			double *float_field = reinterpret_cast<double *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (float_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				*float_field = atof(tag_txt);
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Positive Integer"))) {
			//Should have a pointer to a uint32 in the ItemData
			uint32 *uint32_field = reinterpret_cast<uint32 *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (uint32_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				*uint32_field = atoi64(tag_txt);
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Integer"))) {
			//Should have a pointer to a int32 in the ItemData
			int32 *int32_field = reinterpret_cast<int32 *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (int32_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				*int32_field = atoi(tag_txt);
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Comments"))) {
			//Should have a pointer to the Comments UTFstring in the ItemData
			UTFstring *comment_field = reinterpret_cast<UTFstring *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (comment_field != NULL) {
				wchar_t *tag_txt = new wchar_t[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				if (pageData->g_bUseUnicodeWinAPI)
				{
					//Unicode is supported
					GetDlgItemTextW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				}else {
					char *ansi_tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
					GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, ansi_tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
					snwprintf(tag_txt, 1024, L"%ws", ansi_tag_txt);
					delete ansi_tag_txt;
				}
				*comment_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Language"))) {
			//Should have a pointer to the Language std::string in the ItemData
			std::string *language_field = reinterpret_cast<std::string *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (language_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				*language_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Name"))) {
			//Should have a pointer to the Names UTFstring in the ItemData
			UTFstring *name_field = reinterpret_cast<UTFstring *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (name_field != NULL) {
				wchar_t *tag_txt = new wchar_t[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				if (pageData->g_bUseUnicodeWinAPI)
				{
					//Unicode is supported
					GetDlgItemTextW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				}else {
					char *ansi_tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
					GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, ansi_tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
					snwprintf(tag_txt, 1024, L"%ws", ansi_tag_txt);
					delete ansi_tag_txt;
				}
				*name_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Value"))) {
			//Should have a pointer to the Values' UTFstring in the ItemData
			UTFstring *value_field = reinterpret_cast<UTFstring *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (value_field != NULL) {
				wchar_t *tag_txt = new wchar_t[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				if (pageData->g_bUseUnicodeWinAPI)
				{
					//Unicode is supported
					GetDlgItemTextW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				}else {
					char *ansi_tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
					GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, ansi_tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
					snwprintf(tag_txt, 1024, L"%ws", ansi_tag_txt);
					delete ansi_tag_txt;
				}
				*value_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("SubTitle"))) {
			//Should have a pointer to the SubTitle UTFstring in the ItemData
			UTFstring *subtitle_field = reinterpret_cast<UTFstring *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (subtitle_field != NULL) {
				wchar_t *tag_txt = new wchar_t[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				if (pageData->g_bUseUnicodeWinAPI)
				{
					//Unicode is supported
					GetDlgItemTextW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				}else {
					char *ansi_tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
					GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, ansi_tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
					snwprintf(tag_txt, 1024, L"%ws", ansi_tag_txt);
					delete ansi_tag_txt;
				}
				*subtitle_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Edition"))) {
			//Should have a pointer to the Edition UTFstring in the ItemData
			UTFstring *edition_field = reinterpret_cast<UTFstring *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (edition_field != NULL) {
				wchar_t *tag_txt = new wchar_t[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				if (pageData->g_bUseUnicodeWinAPI)
				{
					//Unicode is supported
					GetDlgItemTextW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				}else {
					char *ansi_tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
					GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, ansi_tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
					snwprintf(tag_txt, 1024, L"%ws", ansi_tag_txt);
					delete ansi_tag_txt;
				}
				*edition_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("URL"))) {
			//Should have a pointer to the url std::string in the ItemData
			std::string *url_field = reinterpret_cast<std::string *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (url_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				url_field->erase();
				url_field->append(tag_txt);
				delete tag_txt;
				tag_txt = NULL;
			}

		}else if (!wcsicmp(field_txt, _W("Email"))) {
			//Should have a pointer to the Email std::string in the ItemData
			std::string *email_field = reinterpret_cast<std::string *>(SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_GETITEMDATA, (nItem), 0));
			if (email_field != NULL) {
				char *tag_txt = new char[GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT)+1];
				GetDlgItemTextA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, tag_txt, GetEditControlTextLength(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT));
				*email_field = tag_txt;
				delete tag_txt;
				tag_txt = NULL;
			}

		}

		delete field_txt;
		field_txt = NULL;
	}

	CRASH_PROTECT_END;
};

void MediaPropProc_Tag_TagListChange(HWND hwndDlg)
{
	CRASH_PROTECT_START;

	HWND hwndList;
	int selectedItem = 0;
	int nItem = 0;
	int last_item = 0;
	wchar_t tag_txt[256];

	//Clean-up the fields
	//Clear the Field Combo box
	SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, 0);
	ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), SW_HIDE);


	//Get the selected tag list index
	hwndList = GetDlgItem(hwndDlg, IDC_LIST_TAGS);
	nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (nItem == -1) {
		//No item selected, exit
		return;
	}
	//Get the selected tag text in the listbox
	wchar_t *field_txt = new wchar_t[SendMessage(hwndList, LB_GETTEXTLEN, nItem, 0)+1];
	SendMessage(hwndList, LB_GETTEXT, (nItem), (LPARAM)field_txt);

	//We need this do found out what type of Tag this is

	wcscpy(tag_txt, JString(field_txt).BeforeFirst(L':').c_str());
	//tag_txt = wcsncpy(tag_txt, field_txt, (wcschr(field_txt, ':')-field_txt));
	if (!wcsicmp(tag_txt, _W("MultiComment"))) {
		MatroskaTagMultiCommentItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiCommentItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiComment"));
			//Fill out the Type combo box
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("Comments"));
			//Fill out the Tag fields for the Multi-Comment items
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Comments"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, 0, reinterpret_cast<LPARAM>(&selected_tag->Comments));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 1, (LPARAM)_W("Language"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, 1, reinterpret_cast<LPARAM>(&selected_tag->Language));
		}
	}else if (!wcsicmp(tag_txt, _W("MultiCommercial"))) {
		MatroskaTagMultiCommercialItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiCommercialItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiCommercial"));
			//Fill out the Type combo box
			for (uint16 i = 1; i <= 3; i++)
			{
				SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)MatroskaTagMultiCommercial::ConvertTypeUIntToStr(i));
			}
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_SETCURSEL, (selected_tag->Type - 1), 0);
			//Fill out the Tag fields for the Multi-Commercial items
			int last_item = 0;
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Price Currency"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->PriceCurrency));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Price Date"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->PriceDate));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Price Amount"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->PriceAmount));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->URL));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Email"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Email));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Address"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Address));
		}
	}else if (!wcsicmp(tag_txt, _W("MultiDate"))) {
		MatroskaTagMultiDateItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiDateItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiDate"));
			//Fill out the Type combo box
			for (uint16 i = 1; i <= 6; i++)
			{
				SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)MatroskaTagMultiDate::ConvertTypeUIntToStr(i));
			}
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_SETCURSEL, (selected_tag->Type - 1), 0);
			//Fill out the Tag fields for the Multi-Date items
			int last_item = 0;
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Date Begin"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->DateBegin));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Date End"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->DateEnd));
		}
	}else if (!wcsicmp(tag_txt, _W("MultiEntity"))) {
		MatroskaTagMultiEntityItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiEntityItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiEntity"));
			//Fill out the Type combo box
			for (uint16 i = 1; i <= 24; i++)
			{
				SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)MatroskaTagMultiEntity::ConvertTypeUIntToStr(i));
			}
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_SETCURSEL, (selected_tag->Type - 1), 0);
			//Fill out the Tag fields for the Multi-Entity items
			int last_item = 0;
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Name"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Name));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->URL));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Email"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Email));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Address"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Address));
			//last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)"Language");
			//SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->));
		}
	}else if (!wcsicmp(tag_txt, _W("MultiIdentifier"))) {
		MatroskaTagMultiIdentifierItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiIdentifierItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiIdentifier"));
			//Fill out the Type combo box
			for (uint16 i = 1; i <= 6; i++)
			{
				SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)MatroskaTagMultiIdentifier::ConvertTypeUIntToStr(i));
			}
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_SETCURSEL, (selected_tag->Type - 1), 0);
			//Fill out the Tag fields for the Multi-Identifier items
			int last_item = 0;
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->StringData));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Binary"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->BinaryData));
		}
	}else if (!wcsicmp(tag_txt, _W("MultiLegal"))) {
		MatroskaTagMultiLegalItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiLegalItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiLegal"));
			//Fill out the Type combo box
			for (uint16 i = 1; i <= 3; i++)
			{
				SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)MatroskaTagMultiLegal::ConvertTypeUIntToStr(i));
			}
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_SETCURSEL, (selected_tag->Type - 1), 0);
			//Fill out the Tag fields for the Multi-Legal items
			int last_item = 0;
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Address"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Address));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->URL));
		}
	}else if (!wcsicmp(tag_txt, _W("MultiTitle"))) {
		MatroskaTagMultiTitleItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiTitleItem *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("MultiTitle"));
			//Fill out the Type combo box
			for (uint16 i = 1; i <= 4; i++)
			{
				SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)MatroskaTagMultiTitle::ConvertTypeUIntToStr(i));
			}
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_SETCURSEL, (selected_tag->Type - 1), 0);
			//Fill out the Tag fields for the Multi-Title items
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Name"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Name));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("SubTitle"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->SubTitle));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Edition"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Edition));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->URL));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Email"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Email));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Address"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Address));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Language"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->Language));
		}
	}else if (!wcsicmp(tag_txt, _W("Simple Tag"))) {
		MatroskaSimpleTag *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaSimpleTag *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Simple Tag"));

			//Fill out the Tag fields for the Simple Tags items
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Name"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->name));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Value"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(&selected_tag->value));

			//Auto-Select the value field
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETCURSEL, last_item, 0);
			MediaPropProc_Tag(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_COMBO_TAG_FIELD, CBN_SELCHANGE), CBN_SELCHANGE);

			// We already selected the correct item
			selectedItem = 1;
		}

	}else if (!wcsicmp(tag_txt, _W("Abstract"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Abstract"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Archival Location"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Archival Location"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Gain"))) {
		double *selected_tag = NULL;
		selected_tag = reinterpret_cast<double *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Audio Gain"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Floating Point Number"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Genre"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Audio Genre"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Peak"))) {
		double *selected_tag = NULL;
		selected_tag = reinterpret_cast<double *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Audio Peak"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Floating Point Number"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Audio Secondary Genre"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Audio Secondary Genre"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Bibliography"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Bibliography"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("BPM"))) {
		double *selected_tag = NULL;
		selected_tag = reinterpret_cast<double *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("BPM"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Floating Point Number"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Capture DPI"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Capture DPI"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Capture Palette Setting"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Capture Palette Setting"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Cropped"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Cropped"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Disc Track"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Disc Track"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Encoder"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Encoder"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Encode Settings"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Encode Settings"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("File"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("File"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Initial Key"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Initial Key"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Keywords"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Keywords"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Language"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Language"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Length"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Length"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Official Audio File URL"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Official Audio File URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Official Audio Source URL"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Official Audio Source URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Original Dimensions"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Original Dimensions"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Original Media Type"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Original Media Type"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Play Counter"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Play Counter"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Playlist Delay"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Playlist Delay"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Popularimeter"))) {
		int32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<int32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Popularimeter"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Record Location"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Record Location"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Set Part"))) {
		uint32 *selected_tag = NULL;
		selected_tag = reinterpret_cast<uint32 *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Set Part"));
			SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)"N/A");
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Positive Integer"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Source"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Source"));
			SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)"N/A");
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Source Form"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Source Form"));
			SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)"N/A");
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Sub Genre"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Sub Genre"));
			SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)"N/A");
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Subject"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Subject"));
			SendDlgItemMessageA(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)"N/A");
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

		/*							}else if (!strcmpi(tag_txt, "Synchronised Tempo")) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
		SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Synchronised Tempo"));
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
		last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)(_W("ASCII String"));
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

		}else if (!strcmpi(tag_txt, "Synchronised Text")) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
		SetDlgItemTextA(hwndDlg, IDC_STATIC_TAG_NAME, _W("Synchronised Text"));
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
		last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

		}else if (!strcmpi(tag_txt, "Unsynchronised Text")) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
		SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Unsynchronised Text"));
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
		last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}
		*/
	}else if (!wcsicmp(tag_txt, _W("User Defined URL"))) {
		std::string *selected_tag = NULL;
		selected_tag = reinterpret_cast<std::string *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("User Defined URL"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("ASCII String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}

	}else if (!wcsicmp(tag_txt, _W("Muxing App"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Muxing App"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}
	}else if (!wcsicmp(tag_txt, _W("Writing App"))) {
		UTFstring *selected_tag = NULL;
		selected_tag = reinterpret_cast<UTFstring *>(SendMessage(hwndList, LB_GETITEMDATA, (nItem), 0));
		if (selected_tag != NULL)
		{
			SetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, _W("Writing App"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_TYPE, CB_ADDSTRING, 0, (LPARAM)_W("N/A"));
			last_item = SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_ADDSTRING, 0, (LPARAM)_W("Unicode String"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETITEMDATA, last_item, reinterpret_cast<LPARAM>(selected_tag));
		}
	}

	if (selectedItem == 0) {
		//Auto-Select the first field
		SendDlgItemMessage(hwndDlg, IDC_COMBO_TAG_FIELD, CB_SETCURSEL, 0, 0);
		MediaPropProc_Tag(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_COMBO_TAG_FIELD, CBN_SELCHANGE), CBN_SELCHANGE);
	}

	delete field_txt;
	field_txt = NULL;

	CRASH_PROTECT_END;
}

void MediaPropProc_Tag_TagFieldChange(HWND hwndDlg, MatroskaPages *pageData)
{
	CRASH_PROTECT_START;

	ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT), SW_NORMAL);

	HWND hwndTagFieldCombo;
	int nItem;

	// Show the selected player's statistics.
	hwndTagFieldCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_FIELD);
	nItem = SendMessage(hwndTagFieldCombo, CB_GETCURSEL, 0, 0);
	wchar_t *field_txt = new wchar_t[SendMessage(hwndTagFieldCombo, CB_GETLBTEXTLEN, nItem, 0)+3];
	char tag_txt[1025];
	SendMessage(hwndTagFieldCombo, CB_GETLBTEXT, (nItem), (LPARAM)field_txt);

	if (!wcsicmp(field_txt, _W("Unicode String"))) {
		//Should have a pointer to the String UTFstring in the ItemData
		UTFstring *unicode_string_field = reinterpret_cast<UTFstring *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (unicode_string_field != NULL) {
			if (pageData->g_bUseUnicodeWinAPI)
			{
				//Unicode is supported
				SendDlgItemMessageW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)unicode_string_field->c_str());
			}else {
				snprintf(tag_txt, 1024, "%ws", unicode_string_field->c_str());
				SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
			}
		}

	}else if (!wcsicmp(field_txt, _W("ASCII String"))) {
		//Should have a pointer to a std::string in the ItemData
		std::string *ascii_string_field = reinterpret_cast<std::string *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (ascii_string_field != NULL) {
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)ascii_string_field->c_str());
		}

	}else if (!wcsicmp(field_txt, _W("Floating Point Number"))) {
		//Should have a pointer to a double in the ItemData
		double *float_field = reinterpret_cast<double *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (float_field != NULL) {
			snprintf(tag_txt, 1024, "%f", *float_field);
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
		}

	}else if (!wcsicmp(field_txt, _W("Positive Integer"))) {
		//Should have a pointer to a uint32 in the ItemData
		uint32 *uint32_field = reinterpret_cast<uint32 *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (uint32_field != NULL) {
			snprintf(tag_txt, 1024, "%u", *uint32_field);
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
		}

	}else if (!wcsicmp(field_txt, _W("Integer"))) {
		//Should have a pointer to a int32 in the ItemData
		int32 *int32_field = reinterpret_cast<int32 *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (int32_field != NULL) {
			snprintf(tag_txt, 1024, "%i", *int32_field);
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
		}

	}else if (!wcsicmp(field_txt, _W("Comments"))) {
		//Should have a pointer to the Comments UTFstring in the ItemData
		UTFstring *comment_field = reinterpret_cast<UTFstring *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (comment_field != NULL) {
			if (pageData->g_bUseUnicodeWinAPI)
			{
				//Unicode is supported
				SendDlgItemMessageW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)comment_field->c_str());
			}else {
				snprintf(tag_txt, 1024, "%ws", comment_field->c_str());
				SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
			}
		}

	}else if (!wcsicmp(field_txt, _W("Language"))) {
		//Should have a pointer to the Language std::string in the ItemData
		std::string *language_field = reinterpret_cast<std::string *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (language_field != NULL) {
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)language_field->c_str());
		}

	}else if (!wcsicmp(field_txt, _W("Name"))) {
		//Should have a pointer to the Name UTFstring in the ItemData
		UTFstring *name_field = reinterpret_cast<UTFstring *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (name_field != NULL) {
			if (pageData->g_bUseUnicodeWinAPI)
			{
				//Unicode is supported
				SendDlgItemMessageW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)name_field->c_str());
			}else {
				snprintf(tag_txt, 1024, "%ws", name_field->c_str());
				SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
			}
		}

	}else if (!wcsicmp(field_txt, _W("Value"))) {
		//Should have a pointer to the Value UTFstring in the ItemData
		UTFstring *value_field = reinterpret_cast<UTFstring *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (value_field != NULL) {
			if (pageData->g_bUseUnicodeWinAPI)
			{
				//Unicode is supported
				SendDlgItemMessageW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)value_field->c_str());
			}else {
				snprintf(tag_txt, 1024, "%ws", value_field->c_str());
				SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
			}
		}

	}else if (!wcsicmp(field_txt, _W("SubTitle"))) {
		//Should have a pointer to the SubTitle UTFstring in the ItemData
		UTFstring *subtitle_field = reinterpret_cast<UTFstring *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (subtitle_field != NULL) {
			if (pageData->g_bUseUnicodeWinAPI)
			{
				//Unicode is supported
				SendDlgItemMessageW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)subtitle_field->c_str());
			}else {
				snprintf(tag_txt, 1024, "%ws", subtitle_field->c_str());
				SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
			}
		}

	}else if (!wcsicmp(field_txt, _W("Edition"))) {
		//Should have a pointer to the Edition UTFstring in the ItemData
		UTFstring *edition_field = reinterpret_cast<UTFstring *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (edition_field != NULL) {
			if (pageData->g_bUseUnicodeWinAPI)
			{
				//Unicode is supported
				SendDlgItemMessageW(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)edition_field->c_str());
			}else {
				snprintf(tag_txt, 1024, "%ws", edition_field->c_str());
				SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)tag_txt);
			}
		}

	}else if (!wcsicmp(field_txt, _W("URL"))) {
		//Should have a pointer to the url std::string in the ItemData
		std::string *url_field = reinterpret_cast<std::string *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (url_field != NULL) {
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)url_field->c_str());
		}

	}else if (!wcsicmp(field_txt, _W("Email"))) {
		//Should have a pointer to the Email std::string in the ItemData
		std::string *email_field = reinterpret_cast<std::string *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (email_field != NULL) {
			SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)email_field->c_str());
		}

	}else if (!wcsicmp(field_txt, _W("Date Begin"))) {
		//Should have a pointer to the date uint64 in the ItemData
		int64 *date_begin_field = reinterpret_cast<int64 *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (date_begin_field != NULL) {
			ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), SW_NORMAL);
			ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), SW_NORMAL);
			ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT), SW_HIDE);

			tm *theTimeStruct = _localtime64(date_begin_field);
			if (theTimeStruct != NULL)
			{
				SYSTEMTIME theTime;
				theTime.wDayOfWeek = theTimeStruct->tm_wday;
				theTime.wDay = theTimeStruct->tm_mday;
				theTime.wMonth = theTimeStruct->tm_mon+1;
				theTime.wYear = theTimeStruct->tm_year+1900;
				theTime.wHour = theTimeStruct->tm_hour;
				theTime.wMinute = theTimeStruct->tm_min;
				theTime.wSecond = theTimeStruct->tm_sec;
				theTime.wMilliseconds = 0;

				DateTime_SetSystemtime(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), GDT_VALID, &theTime);
				DateTime_SetSystemtime(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), GDT_VALID, &theTime);
			}
		}

	}else if (!wcsicmp(field_txt, _W("Date End"))) {
		//Should have a pointer to the date uint64 in the ItemData
		int64 *date_end_field = reinterpret_cast<int64 *>(SendMessage(hwndTagFieldCombo, CB_GETITEMDATA, (nItem), 0));
		if (date_end_field != NULL) {
			ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), SW_NORMAL);
			ShowWindow(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), SW_NORMAL);
			ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT), SW_HIDE);

			if (*date_end_field == 0)
				//No date set, setting to current time
				*date_end_field = _time64(NULL);

			tm *theTimeStruct = _localtime64(date_end_field);
			SYSTEMTIME theTime;
			theTime.wDayOfWeek = theTimeStruct->tm_wday;
			theTime.wDay = theTimeStruct->tm_mday;
			theTime.wMonth = theTimeStruct->tm_mon+1;
			theTime.wYear = theTimeStruct->tm_year+1900;
			theTime.wHour = theTimeStruct->tm_hour;
			theTime.wMinute = theTimeStruct->tm_min;
			theTime.wSecond = theTimeStruct->tm_sec;
			theTime.wMilliseconds = 0;

			DateTime_SetSystemtime(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_TIME), GDT_VALID, &theTime);
			DateTime_SetSystemtime(GetDlgItem(hwndDlg, IDC_TAG_DATETIMEPICKER_DATE), GDT_VALID, &theTime);

			//IDC_COMBO_TAG_FIELD
			//SendDlgItemMessageA(hwndDlg, IDC_EDIT_SELECTED_TAG_TEXT, WM_SETTEXT, 0, (LPARAM)email_field->c_str());
		}
	}

	delete field_txt;
	field_txt = NULL;

	CRASH_PROTECT_END;
}

void MediaPropProc_Tag_TagTypeChange(HWND hwndDlg)
{
	CRASH_PROTECT_START;

	HWND hwndTagTypeCombo;
	int nItem;

	// Show the selected player's statistics.
	hwndTagTypeCombo = GetDlgItem(hwndDlg, IDC_COMBO_TAG_TYPE);
	nItem = SendMessage(hwndTagTypeCombo, CB_GETCURSEL, 0, 0);
	char *field_txt = new char[SendMessageA(hwndTagTypeCombo, CB_GETLBTEXTLEN, nItem, 0)+3];
	SendMessageA(hwndTagTypeCombo, CB_GETLBTEXT, (nItem), (LPARAM)field_txt);

	wchar_t tag_txt[65];
	GetDlgItemText(hwndDlg, IDC_STATIC_TAG_NAME, tag_txt, 64);

	int nListItem = SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETCURSEL, 0, 0);
	if (nListItem == -1) {
		//No item selected, exit
		return;
	}

	if (!wcsicmp(tag_txt, _W("MultiComment"))) {
		//MatroskaTagMultiCommentItem *selected_tag = NULL;
		//selected_tag = reinterpret_cast<MatroskaTagMultiCommentItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));

	}else if (!wcsicmp(tag_txt, _W("MultiCommercial"))) {
		MatroskaTagMultiCommercialItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiCommercialItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));
		if (selected_tag != NULL) {
			selected_tag->Type = MatroskaTagMultiCommercial::ConvertTypeStrToUInt(field_txt);
		}

	}else if (!wcsicmp(tag_txt, _W("MultiDate"))) {
		MatroskaTagMultiDateItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiDateItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));
		if (selected_tag != NULL) {
			selected_tag->Type = MatroskaTagMultiDate::ConvertTypeStrToUInt(field_txt);
		}

	}else if (!wcsicmp(tag_txt, _W("MultiEntity"))) {
		MatroskaTagMultiEntityItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiEntityItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));
		if (selected_tag != NULL) {
			selected_tag->Type = MatroskaTagMultiEntity::ConvertTypeStrToUInt(field_txt);
		}

	}else if (!wcsicmp(tag_txt, _W("MultiIdentifier"))) {
		MatroskaTagMultiIdentifierItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiIdentifierItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));
		if (selected_tag != NULL) {
			selected_tag->Type = MatroskaTagMultiIdentifier::ConvertTypeStrToUInt(field_txt);
		}

	}else if (!wcsicmp(tag_txt, _W("MultiLegal"))) {
		MatroskaTagMultiLegalItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiLegalItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));
		if (selected_tag != NULL) {
			selected_tag->Type = MatroskaTagMultiLegal::ConvertTypeStrToUInt(field_txt);
		}

	}else if (!wcsicmp(tag_txt, _W("MultiTitle"))) {
		MatroskaTagMultiTitleItem *selected_tag = NULL;
		selected_tag = reinterpret_cast<MatroskaTagMultiTitleItem *>(SendDlgItemMessage(hwndDlg, IDC_LIST_TAGS, LB_GETITEMDATA, (nListItem), 0));
		if (selected_tag != NULL) {
			selected_tag->Type = MatroskaTagMultiTitle::ConvertTypeStrToUInt(field_txt);
		}
	}

	delete field_txt;
	field_txt = NULL;

	CRASH_PROTECT_END;
};

UTFstring GetSelectedListItem(HWND hwndDlg, int ctrl)
{
	UTFstring txt(L"");
	
	HWND hwndList = GetDlgItem(hwndDlg, ctrl);
	int nItem = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	if (nItem == -1) {
		//No item selected, exit
		return txt;
	}
	//Get the selected tag text in the listbox
	wchar_t *field_txt = new wchar_t[SendMessage(hwndList, LB_GETTEXTLEN, nItem, 0)+1];
	field_txt[0] = 0;
	SendMessage(hwndList, LB_GETTEXT, (nItem), (LPARAM)field_txt);
	
	txt = field_txt;
	delete [] field_txt;

	return txt;
}