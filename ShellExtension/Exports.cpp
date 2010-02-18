/*
 *  Matroska Shell Extension
 *
 *  Exports.cpp
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
    \file Exports.cpp
		\version \$Id: Exports.cpp,v 1.13 2004/03/07 08:08:49 jcsston Exp $
    \brief This has the exported interface to MatroskaUtils, mainly for VB and Delphi apps
		\author Jory Stone     <jcsston @ toughguy.net>

		\note Look at VB_DLL_Example.frm for a Visual Basic Example
*/

#include <windows.h>
#include <windowsx.h>
#include <mbstring.h>

#include "MatroskaUtils.h"

using namespace MatroskaUtilsNamespace;

#ifdef MATROSKA_INFO

#include "resource.h"

// Handle to this DLL itself.
HINSTANCE g_hmodThisDll = NULL;

// List of allocated parsers
std::vector<MatroskaInfoParser *> g_pParsers;

bool ParserCheck(MatroskaInfoParser *x) {
	if (x == NULL) {
		return -1;
	} else {
		for (size_t p = 0; p < g_pParsers.size(); p++) {
			MatroskaInfoParser *currentParser = g_pParsers.at(p);
			if (currentParser == x)
				break;
		}
		return -1;
	};
}

#define PARSER_CHECK(x) if (!ParserCheck(x)) return -1;

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		ODS("In DLLMain, DLL_PROCESS_ATTACH\n");

		// Extension DLL one-time initialization
		g_hmodThisDll = hInstance;

	} else if (dwReason == DLL_PROCESS_DETACH) {
		ODS("In DLLMain, DLL_PROCESS_DETACH\n");
	}

	return 1;   // ok
}

#else

// The normal interface only checks if the parser is not NULL
#define PARSER_CHECK(x) if (x == NULL) return -1;

#endif

// Export all these functions
extern "C" {
// Prototype for Rundll32
//void WINAPI EntryPoint(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);

// Visual Basic / Delphi DLL functions
LONG WINAPI CreateParserObject(MatroskaInfoParser **objParser, char *filename)
{
	MatroskaInfoParser *new_parser;
	new_parser = new MatroskaInfoParser(filename);
#ifdef MATROSKA_INFO
	g_pParsers.push_back(new_parser);
#endif

	*objParser = new_parser;
	return 0;
}

LONG WINAPI FillParserObject(MatroskaInfoParser *objParser, bool parseTags, bool parseWholeFile)
{
	PARSER_CHECK(objParser);

	objParser->m_parseTags = parseTags;
	objParser->m_parseWholeFile = parseWholeFile;
	return objParser->ParseFile();
}

LONG WINAPI DeleteParserObject(MatroskaInfoParser *objParser)
{
	PARSER_CHECK(objParser);

#ifdef MATROSKA_INFO
	std::vector<MatroskaInfoParser *>::iterator itToDelete = g_pParsers.begin();
	while (itToDelete != g_pParsers.end()){
		if (objParser == *itToDelete) {
			g_pParsers.erase(itToDelete);
			break;
		}
		itToDelete++;
	}
#endif
	delete objParser;
	return 0;
}

LONG WINAPI GetTrackCount(MatroskaInfoParser *objParser)
{
	PARSER_CHECK(objParser);
	return objParser->GetNumberOfTracks();
}

LONG WINAPI GetTrackUID(MatroskaInfoParser *objParser, int track_no)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
		return (LONG)objParser->trackInfos[track_no]->GetTrackUID();

	return -1;
}

LONG WINAPI GetTrackType(MatroskaInfoParser *objParser, int track_no)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
		return objParser->trackInfos[track_no]->GetTrackType();

	return -1;
}

LONG WINAPI GetTrackTypeLen(MatroskaInfoParser *objParser, int track_no)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		JString found_track_type = objParser->trackInfos[track_no]->GetTrackTypeStr();
		if (found_track_type.length() > 0) {
			return found_track_type.length();
		}
	}

	return -1;
}

LONG WINAPI GetTrackTypeStr(MatroskaInfoParser *objParser, int track_no, char *track_type_name)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		JString found_track_type = objParser->trackInfos[track_no]->GetTrackTypeStr();
		if (found_track_type.length() > 0)
		{
			strcpy(track_type_name, found_track_type.mb_str());
		}

		return objParser->trackInfos[track_no]->GetTrackType();
	}

	return -1;
}

LONG WINAPI GetTrackCodecIDLen(MatroskaInfoParser *objParser, int track_no)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		std::string &codec_ID = objParser->trackInfos[track_no]->m_codecID;
		if (codec_ID.length())
			return codec_ID.length() + 1 + objParser->trackInfos[track_no]->m_CodecOldID.length();
	}

	return -1;
}

LONG WINAPI GetTrackCodecIDStr(MatroskaInfoParser *objParser, int track_no, char *track_type_name)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		std::string &found_track_type = objParser->trackInfos[track_no]->m_codecID;
		if (found_track_type.length())
		{
			strcpy(track_type_name, found_track_type.c_str());
			strcpy(track_type_name+found_track_type.length(), " ");
			strcpy(track_type_name+found_track_type.length()+1, objParser->trackInfos[track_no]->m_CodecOldID.c_str());
		}
		return 0;
	}

	return -1;
}

LONG WINAPI GetTrackLength(MatroskaInfoParser *objParser, int track_no)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		return (long)objParser->m_segmentInfo.GetDuration();;
	}

	return -1;
}

LONG WINAPI GetTrackVideoInfo(MatroskaInfoParser *objParser, int track_no, void *video_info_struct)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		MatroskaVideoTrackInfo *track_video = objParser->trackInfos[track_no]->GetVideoInfo();
		memcpy(video_info_struct, (void *)track_video, sizeof(MatroskaVideoTrackInfo));
		return 1;
	}

	return -1;
}

LONG WINAPI GetTrackAudioInfo(MatroskaInfoParser *objParser, int track_no, void *audio_info_struct)
{
	PARSER_CHECK(objParser);
	if ((track_no > 0) && (track_no < objParser->GetNumberOfTracks()))
	{
		MatroskaAudioTrackInfo *track_audio = objParser->trackInfos[track_no]->GetAudioInfo();
		if ((track_audio != NULL) && (audio_info_struct != NULL))
		{
			memcpy(audio_info_struct, (void *)track_audio, sizeof(MatroskaAudioTrackInfo));
			return 1;
		}
	}

	return -1;
}

LONG WINAPI SetAR(MatroskaInfoParser *objParser, int track_no, int display_x, int display_y)
{
	PARSER_CHECK(objParser);
	return objParser->WriteDisplaySize(track_no, display_x, display_y);
}

LONG WINAPI UpdateTrackLanguage(MatroskaInfoParser *objParser, int track_no, char *lang)
{
	PARSER_CHECK(objParser);
	return objParser->WriteLanguage(track_no, lang);
}

};
