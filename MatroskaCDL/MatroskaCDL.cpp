/*
 *  The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
 *
 *  MatroskaCDL.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
 *  Copyright (C) John Cannon - 2003
 *
 *  CDL API
 *   Copyright (C) Ludovic 'BlackSun' Vialle, CoreCodec Lead-developer.
 *   Copyright (C) C/C++ Conversion by Christophe 'Toff' PARIS
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
    \file MatroskaCDL.cpp
		\version \$Id: MatroskaCDL.cpp,v 1.23 2004/04/27 12:35:14 toff Exp $
    \brief The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
		\author Jory Stone     <jcsston @ toughguy.net>
		\author John Cannon    <spyder482 @ yahoo.com>

		Inspired by ;) John Cannon (spyder482@yahoo.com)
		Thanks to Toff & Blacksun for the TCMP Plugin API
*/

#include "resource.h"
#include "MatroskaCDL.h"

#ifdef _DEBUG
//Memory Debugging includes
#include <stdlib.h>
#include <crtdbg.h>
#endif

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

#ifdef _DEBUG
#include "mdump.h"
CrashCatcher myCrasher(_T(MATROSKA_CDL_APP_NAME));
#endif

JList<char *> g_tagStrList;
extern long g_MyBuildNumber;

// Handle to this DLL itself.
HINSTANCE g_hmodThisDll = NULL;	

/// I suppose this is called when TCMP is trying to find out if this DLL is a CDL plugin
CDL_API_EXPORT PCDL_PluginHeader CDL_GetPluginHeader(PCDL_CoreHeader pCoreHeader)
{
	ODS("PCDL_PluginHeader()");

	pCoreCDL = pCoreHeader;
	return &MyCDL;
}

///The DLL starting point
BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	ODS("DllMain()");
	if (reason == DLL_PROCESS_ATTACH)
	{
#ifdef _DEBUG
	// Setup the debug options
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF 
								| _CRTDBG_LEAK_CHECK_DF //Check for memory leaks on app exit
								| _CRTDBG_CHECK_ALWAYS_DF);
	_CrtSetAllocHook(YourAllocHook);
#endif
		// Extension DLL one-time initialization
		g_hmodThisDll = hInst;

		global_pagedata = new MatroskaPages();
		global_pagedata->g_hAppInst = hInst;
		
		//Check if the API supports Unicode
		global_pagedata->g_bUseUnicodeWinAPI = (LONG)GetVersion() >= 0;

		// Fill in the build number
		HMODULE myModule;
		//Get the dll filename
		TCHAR dllFilename[MAX_MATROSKA_PATH];
		myModule = GetModuleHandle(_T("MatroskaCDL.cdl"));
		GetModuleFileName(myModule, dllFilename, MAX_MATROSKA_PATH/sizeof(TCHAR));
		DWORD verSize = GetFileVersionInfoSize(dllFilename, NULL);
		if (verSize > 0) {
			binary *verData = new binary[verSize+1];
			GetFileVersionInfo(dllFilename, NULL, verSize, verData);
			void *verBuffer = NULL;
			UINT verBufferSize = 0;
			VerQueryValue(verData, _T("\\StringFileInfo\\040904b0\\FileVersion"), &verBuffer, &verBufferSize);
			if (verBuffer != NULL) {
				JString version((wchar_t *)verBuffer);					
				g_MyBuildNumber = atol(version.AfterLast(L',').mb_str());
			}
			delete verData;
		}

	} else if (reason == DLL_PROCESS_DETACH) {
		global_pagedata->g_hAppInst = NULL;
		delete global_pagedata;
		global_pagedata = NULL;
	}
	return TRUE;
}

///This is called by TCMP when TCMP loads this plugin
int CDL_API_CALL CDL_Load(int version)
{
	ODS("CDL_Load()");
	if(version < 400)
	{
		return CORE_NOT_OK;
	}
	global_pagedata->g_hTreeDialog = CreateDialog(global_pagedata->g_hAppInst, MAKEINTRESOURCE(IDD_TREEDIALOG), NULL, TreeDlgProc);
	pCoreCDL->CDL_HijackMe(global_pagedata->g_hTreeDialog, CDL_NAME);
	ShowWindow(global_pagedata->g_hTreeDialog,SW_NORMAL);
	return CORE_OK;
}

/// This is called when the user clicks the 'Configure' button at the Plugin List in the TCMP Options
void CDL_API_CALL CDL_Config(void)
{
	ODS("CDL_Config()");
	// config dialog box
	int result = DialogBox(global_pagedata->g_hAppInst,MAKEINTRESOURCE(IDD_DIALOG_ABOUT), MyCDL.hwndParent, AboutDlgProc);
	if(result == IDOK)
	{
		// TODO
	}
	else if(result == IDCANCEL)
	{
		// TODO
	}
}

#define SAFE_DESTROY_WINDOW(x) if(x) { DestroyWindow(x); x = NULL; }

/// This is called when TCMP is exiting and unloading all the plugins
void CDL_API_CALL CDL_Unload(void)
{
	ODS("CDL_Unload()");
	SAFE_DESTROY_WINDOW(global_pagedata->g_hTreeDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hInfoDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hTagDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hSimpleTagDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hAttachmentDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hChapterDialog);	
}

/// We get event messages from TCMP here
void CDL_API_CALL CDL_EventHandler(int Event, char* Param1, int Param2)
{
	NOTE1("CDL_EventHandler(Event = %i)", Event);

	// The player send you some importants events
	switch(Event)
	{
		case CORE_FILELOADING:
			if((Param1 != NULL)) {				
				//Go ahead and parse the file
				memset(global_pagedata->szFile, 0, sizeof(global_pagedata->szFile));
				mbstowcs(global_pagedata->szFile, Param1, strlen(Param1));
				global_pagedata->parser = new MatroskaInfoParser(Param1);
				if (global_pagedata->parser->IsValid()) {
					pCoreCDL->CDL_WriteToConsole("Matroska CDL: Found Matroska File");
					global_pagedata->parser->m_parseSeekHead = true;
					global_pagedata->parser->m_parseAttachments = true;
					global_pagedata->parser->m_parseTags = true;
					global_pagedata->parser->m_parseWholeFile = false;
					global_pagedata->parser->ParseFile();		
				} else {
					delete global_pagedata->parser;
					global_pagedata->parser = NULL;
				}
			}
			break;
		case CORE_FILELOADED:
		{
			//Let's check if we already loaded a Matroska file and that Auto-DAR is enabled
			if ((global_pagedata->parser != NULL) && (IsDlgButtonChecked(global_pagedata->g_hTreeDialog, IDC_CHECK_DAR) == BST_CHECKED))
			{
				//Change the DAR here
#ifdef _DEBUG
				pCoreCDL->CDL_WriteToConsole("Matroska CDL: A Matroska file is starting playback");
#endif //_DEBUG
				char dar_txt[128];
				for (int trackno = 0; trackno < global_pagedata->parser->GetNumberOfTracks(); trackno++)
				{		
					pCoreCDL->CDL_WriteToConsole("Matroska CDL: Searching tracks for a video track");
					
					if(global_pagedata->parser->GetTrackInfo(trackno)->GetTrackType() == track_video)
					{
						pCoreCDL->CDL_WriteToConsole("Matroska CDL: Found video track");
						MatroskaVideoTrackInfo *size_video = global_pagedata->parser->GetTrackInfo(trackno)->GetVideoInfo();
						if (size_video != NULL) {
							if((size_video ->video_DisplayHeight != 0) && (size_video->video_DisplayWidth != 0))
							{														
								snprintf(dar_txt, 128, "Matroska CDL: Found Display Size: %u x %u", size_video->video_DisplayWidth , size_video->video_DisplayHeight); 
								pCoreCDL->CDL_WriteToConsole(dar_txt);
								pCoreCDL->CDL_SetDAR(size_video->video_DisplayWidth, size_video->video_DisplayHeight);
#ifdef _DEBUG
								pCoreCDL->CDL_WriteToConsole("Matroska CDL: Set DAR");
#endif
							}
						}
					}
				}
			}
			break;
		}
		case CORE_FILECLOSED:
			if (global_pagedata->parser != NULL)
			{
				//Previously parsed file still in memory
				pCoreCDL->CDL_WriteToConsole("Matroska CDL: Parser memory freed\n");
				delete global_pagedata->parser;
				global_pagedata->parser = NULL;
			}
			break;
		case CORE_FILEPLAY:
#ifdef _DEBUG
			if (global_pagedata->parser != NULL)
			{
				pCoreCDL->CDL_WriteToOSD(MATROSKA_CDL_APP_NAME " in use");
			}
#endif
			break;
		case CORE_FILEPAUSE:
			break;          
		case CORE_FILESTOP:
			break;
		case CORE_FULLSCREEN:
		{
			//Let's check if we already loaded a Matroska file and that Auto-DAR is enabled
			if ((global_pagedata->parser != NULL) && (IsDlgButtonChecked(global_pagedata->g_hTreeDialog, IDC_CHECK_DAR) == BST_CHECKED))
			{
				//Change the DAR here
#ifdef _DEBUG
				pCoreCDL->CDL_WriteToConsole("Matroska CDL: A Matroska file is starting playback");
#endif //_DEBUG
				char dar_txt[128];
				for (int trackno = 0; trackno < global_pagedata->parser->GetNumberOfTracks(); trackno++)
				{		
					pCoreCDL->CDL_WriteToConsole("Matroska CDL: Searching tracks for a video track");
					
					if(global_pagedata->parser->GetTrackInfo(trackno)->GetTrackType() == track_video)
					{
						pCoreCDL->CDL_WriteToConsole("Matroska CDL: Found video track");
						MatroskaVideoTrackInfo *size_video = global_pagedata->parser->GetTrackInfo(trackno)->GetVideoInfo();
						if (size_video != NULL) {
							if((size_video ->video_DisplayHeight != 0) && (size_video->video_DisplayWidth != 0))
							{														
								snprintf(dar_txt, 128, "Matroska CDL: Found Display Size: %u x %u", size_video->video_DisplayWidth , size_video->video_DisplayHeight); 
								pCoreCDL->CDL_WriteToConsole(dar_txt);
								pCoreCDL->CDL_SetDAR(size_video->video_DisplayWidth, size_video->video_DisplayHeight);
#ifdef _DEBUG
								pCoreCDL->CDL_WriteToConsole("Matroska CDL: Set DAR");
#endif
							}
						}
					}
				}
			}
			break;
		}
		case CORE_FILESEEK:
			break;
		case CORE_VIDEOSIZECHANGE:
			break;
	}
}

// ----------------------------------------------------------------------------
/// This is called... I have no idea, it's just called often, so I cannot have long delays in here
char* CDL_API_CALL CDL_MediaPropExec(char *Filename) {	
	ODS("CDL_MediaPropExec()");
		
	SAFE_DESTROY_WINDOW(global_pagedata->g_hTreeDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hInfoDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hTagDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hSimpleTagDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hAttachmentDialog);
	SAFE_DESTROY_WINDOW(global_pagedata->g_hChapterDialog);

	//First we check if there is a parsed file data in memory already
	if (global_pagedata->parser != NULL) {
		//Ok now we check that the filenames exist
		if ((global_pagedata->parser->GetFilename_ASCII() != NULL) && (Filename  != NULL)) {
			//Now we check if the filenames are different
			if(!stricmp(global_pagedata->parser->GetFilename_ASCII(), Filename)) {
				//We can leave it be, same file
			}else {
				//We need to re-parse the file
				//Go ahead and parse the file, (cleanup needed)
				delete global_pagedata->parser;
				memset(global_pagedata->szFile, 0, sizeof(global_pagedata->szFile));
				mbstowcs(global_pagedata->szFile, Filename, strlen(Filename));
				global_pagedata->parser = new MatroskaInfoParser(Filename);
				if (global_pagedata->parser->IsValid()) {
					global_pagedata->parser->m_parseSeekHead = true;
					global_pagedata->parser->m_parseTags = true;
					global_pagedata->parser->m_parseAttachments = true;
					global_pagedata->parser->m_parseWholeFile = false;
					global_pagedata->parser->ParseFile();			
				}else {
					return "";
				}
			}
		}
	} else {
		//We need to parse the file
		//File type check here
		if(Filename == NULL) {
			return "";
		}
		//Go ahead and parse the file for the first time (clean start)
		memset(global_pagedata->szFile, 0, sizeof(global_pagedata->szFile));
		mbstowcs(global_pagedata->szFile, Filename, strlen(Filename));
		global_pagedata->parser = new MatroskaInfoParser(Filename);
		if (global_pagedata->parser->IsValid()) {
			global_pagedata->parser->m_parseSeekHead = true;
			global_pagedata->parser->m_parseTags = true;
			global_pagedata->parser->m_parseAttachments = true;
			global_pagedata->parser->m_parseWholeFile = false;
			global_pagedata->parser->ParseFile();			
		}else {
			delete global_pagedata->parser;
			global_pagedata->parser = NULL;
			return "";
		}	
	}
	
	if (!global_pagedata->parser->IsValid())
		return "";

	// Display our tab if this is an OK file :P
	HWND hDialog = CreateDialogParam(global_pagedata->g_hAppInst, MAKEINTRESOURCE(IDD_PROPPAGE), MyCDL.hwndParent, MediaPropProc, (LPARAM)global_pagedata);
	if(hDialog)
	{
		//Our dialog was created :D
		ODS("Created Matroska Tab");
		char *tag_txt;
		int nItem = 0;

		global_pagedata->g_hInfoDialog = hDialog;
				
		//Fill out the track list		
		DisplayMainTrackList(global_pagedata->g_hInfoDialog, global_pagedata->parser);
		//Add to the Pages
		pCoreCDL->CDL_AddMediaTab("Matroska Info", hDialog);		
		//Show
		ShowWindow(hDialog, SW_SHOWNA);

		if (global_pagedata->parser->fileAttachments.GetNumberOfAttachments() > 0) {
			global_pagedata->g_hAttachmentDialog = CreateDialogParam(global_pagedata->g_hAppInst, MAKEINTRESOURCE(IDD_PROPPAGE_ATTACHMENT), MyCDL.hwndParent, MediaPropProc_Attachments, (LPARAM)global_pagedata);
			
			// Fill out the attachment list
			DisplayAttachmentList(global_pagedata->g_hAttachmentDialog, global_pagedata->parser);
			
			pCoreCDL->CDL_AddMediaTab("Attachments", global_pagedata->g_hAttachmentDialog);
			ShowWindow(global_pagedata->g_hAttachmentDialog, SW_SHOWNA);
		}

		if (global_pagedata->parser->chapters.size() > 0) {
			global_pagedata->g_hChapterDialog = CreateDialogParam(global_pagedata->g_hAppInst, MAKEINTRESOURCE(IDD_PROPPAGE_CHAPTER), MyCDL.hwndParent, MediaPropProc_Chapters, (LPARAM)global_pagedata);
			
			// Fill out the chapter list
			DisplayChapterList(global_pagedata->g_hChapterDialog, global_pagedata->parser);
			
			pCoreCDL->CDL_AddMediaTab("Chapters", global_pagedata->g_hChapterDialog);
			ShowWindow(global_pagedata->g_hChapterDialog, SW_SHOWNA);
		}
	
		//The Simple Tag Editor
		//if (IsDlgButtonChecked(global_pagedata->g_hTreeDialog, IDC_CHECK_TAG_TAB_SIMPLE) == BST_CHECKED) {
		//	global_pagedata->g_hSimpleTagDialog = CreateDialogParam(global_pagedata->g_hAppInst, MAKEINTRESOURCE(IDD_TAG_SIMPLE_TAB), MyCDL.hwndParent, MediaPropProc_SimpleTag, (LPARAM)global_pagedata);
		//	
		//	//Fill out the track list
		//	DisplaySimpleTagTrackList(global_pagedata->g_hSimpleTagDialog, global_pagedata->parser);

		//	pCoreCDL->CDL_AddMediaTab("Simple Editor", global_pagedata->g_hSimpleTagDialog);			
		//	ShowWindow(global_pagedata->g_hSimpleTagDialog, SW_SHOWNA);
		//};

		//See if the Matroska Tag Tab is enabled
		if (IsDlgButtonChecked(global_pagedata->g_hTreeDialog, IDC_CHECK_TAG_TAB) == BST_CHECKED) {
			//Store the dialog handle
			global_pagedata->g_hTagDialog = CreateDialogParam(global_pagedata->g_hAppInst, MAKEINTRESOURCE(IDD_TAG_TAB), MyCDL.hwndParent, MediaPropProc_Tag, (LPARAM)global_pagedata);
			//Fill out the track list
			DisplayTagEntryList(global_pagedata->g_hTagDialog, global_pagedata->parser);		
			//Add to the Pages
			pCoreCDL->CDL_AddMediaTab("Advanced Editor", global_pagedata->g_hTagDialog);
			//Show
			ShowWindow(global_pagedata->g_hTagDialog, SW_SHOWNA);
		};		

		
		UTFstring artist = global_pagedata->parser->GetTagArtist();
		UTFstring title = global_pagedata->parser->GetTagTitle();
		if ((artist.length() > 0) && (title.length() > 0))
		{
			tag_txt = new char[artist.length() + title.length() + 5];
			sprintf(tag_txt, "%ws - %ws", artist.c_str(), title.c_str());
			//Now we can return the scrolling text in classic
			//Artist - Title style :p
			g_tagStrList.AddItem(tag_txt);
			return tag_txt;
		}
	}

	UTFstring the_title = global_pagedata->parser->GetTagTitle();
	if (the_title.length() > 0)
	{
		char *tag_txt = new char[the_title.length()+1];
		sprintf(tag_txt, "%ws", the_title.c_str());
		g_tagStrList.AddItem(tag_txt);
		return tag_txt;
	}
	
	//We couldn't get the Artist and Title, but this is a Matroska File :D
	return Filename;
}

/// This is for the version 2 of the API
void CDL_API_CALL CDL_ReadTags(char *Filename, CDL_Metadata &MetaData)
{
	ODS("CDL_ReadTags()");
	
	if(Filename != NULL)
	{
		MatroskaInfoParser parser(Filename);
		if (!parser.IsValid())
			return;
		parser.m_parseSeekHead = true;
		parser.m_parseTags = true;
		parser.ParseFile();
		
		char *tag_txt = NULL;
		UTFstring artist = parser.GetTagArtist();
		if (artist.length() > 1)
		{
			tag_txt = new char[artist.length()+1];
			snprintf(tag_txt, artist.length()+1, "%ws", artist.c_str());
			MetaData.Artist = tag_txt;
			g_tagStrList.AddItem(tag_txt);
			tag_txt = NULL;
		}
		UTFstring title = parser.GetTagTitle();
		if (title.length() > 1)
		{
			tag_txt = new char[title.length()+1];
			snprintf(tag_txt, title.length()+1, "%ws", title.c_str());
			MetaData.Title = tag_txt;
			g_tagStrList.AddItem(tag_txt);
			tag_txt = NULL;
		}
		UTFstring album = parser.GetTagAlbum();
		if (album.length() > 1)
		{
			tag_txt = new char[album.length()+1];
			snprintf(tag_txt, album.length()+1, "%ws", album.c_str());
			MetaData.Album = tag_txt;
			g_tagStrList.AddItem(tag_txt);
			tag_txt = NULL;
		}		
		UTFstring comment = parser.GetTagComment();
		if (comment.length() > 1)
		{
			tag_txt = new char[comment.length()+1];
			snprintf(tag_txt, comment.length()+1, "%ws", comment.c_str());
			MetaData.Comment = tag_txt;
			g_tagStrList.AddItem(tag_txt);
			tag_txt = NULL;
		}
		std::string genre = parser.GetTagGenre();
		if (genre.length() > 1)
		{
			tag_txt = new char[genre.length()+1];
			snprintf(tag_txt, genre.length()+1, "%s", genre.c_str());
			MetaData.Genre = tag_txt;
			g_tagStrList.AddItem(tag_txt);
			tag_txt = NULL;
		}

		MetaData.Year = parser.GetTagYear();
		MetaData.Rating = parser.GetTagRating();
		MetaData.Track = parser.GetTagDiscTrack();
			

		char *file_ext = Filename+strlen(Filename)-3;
		if (!stricmp(file_ext, "mka"))
			MetaData.MediaType = "Audio";
		else if (!stricmp(file_ext, "mkv"))
			MetaData.MediaType = "Video";
		else
			MetaData.MediaType = "Unknown";

		MetaData.Extension = file_ext;
	}

	return;
}

void CDL_API_CALL CDL_WriteTags(char *Filename, CDL_Metadata Metadata)
{	
	ODS("CDL_WriteTags()");
	
	if(Filename != NULL)
	{
		MatroskaInfoParser parser(Filename);
		if (parser.IsValid())
		{			
			parser.m_parseSeekHead = true;
			parser.m_parseTags = true;
			parser.ParseFile();
			
			JString tagData = Metadata.Album;
			parser.SetTagAlbum(tagData.c_str());

			tagData = Metadata.Artist;
			parser.SetTagArtist(tagData.c_str());

			tagData = Metadata.Comment;
			parser.SetTagComment(tagData.c_str());

			tagData = Metadata.Genre;
			parser.SetTagGenre(tagData.mb_str());

			tagData = Metadata.Title;
			parser.SetTagTitle(tagData.c_str());

			parser.SetTagDiscTrack(Metadata.Track);
			parser.SetTagYear(Metadata.Year);
			parser.SetTagRating(Metadata.Rating);
			
			parser.WriteTags();
		}
	}
};
