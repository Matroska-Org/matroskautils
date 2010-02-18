/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MP3TagReader.cpp
 *
 *  Copyright (C) Jory Stone - 2003
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
    \file AviTagReader.h
		\version \$Id: MP3TagReader.cpp,v 1.2 2003/10/08 03:30:42 jcsston Exp $
    \brief A MP3 ID3 Tag Importer
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#include "MP3TagReader.h"

using namespace MatroskaUtilsNamespace;
using namespace LIBMATROSKA_NAMESPACE;

MP3TagReader::MP3TagReader(const JString &filename) {	
	ReadFile(filename);
};

void MP3TagReader::ReadFile(const JString &filename) {
#ifdef USING_ID3LIB
	m_MP3Filename = filename;

	m_Tag.Link(m_MP3Filename.mb_str());	

#endif
};
MP3TagReader::~MP3TagReader() {
#ifdef USING_ID3LIB
	
#endif // USING_ID3LIB
};

bool MP3TagReader::HasTags() {
#ifdef USING_ID3LIB
	if (m_Tag.NumFrames() > 0)
		return true;
#endif

	return false;
};

void MP3TagReader::ImportTags(MatroskaTagInfo *target) {
#ifdef USING_ID3LIB
	
	// use an std::auto_ptr here to handle object cleanup automatically
  ID3_Tag::Iterator *iter = m_Tag.CreateIterator();
  ID3_Frame *myFrame = NULL;
  while (NULL != (myFrame = iter->GetNext())) {
		// do something with myFrame
		//myFrame;
		ID3_Field *textField = myFrame->GetField(ID3FN_TEXTENC);
		ID3_Field *intField = myFrame->GetField(ID3FN_COUNTER);
		
		if (myFrame->GetID() == ID3FID_PLAYCOUNTER) {
			target->Tag_PlayCounter = intField->Get();

		} else if (myFrame->GetID() == ID3FID_POPULARIMETER) {
			target->Tag_Popularimeter = intField->Get();

		} else if (myFrame->GetID() == ID3FID_ALBUM) {
			MatroskaTagMultiTitleItem *newTagItem = new MatroskaTagMultiTitleItem();
			newTagItem->Type = KaxTagMultiTitleType_AlbumMovieShowTitle;
			newTagItem->Name = textField->GetRawUnicodeText();
			target->AddMultiTitleItem(newTagItem);

		} else if (myFrame->GetID() == ID3FID_BPM) {
			target->Tag_BPM = intField->Get();

		} else if (myFrame->GetID() == ID3FID_COMPOSER) {
			MatroskaTagMultiEntityItem *newTagItem = new MatroskaTagMultiEntityItem();
			newTagItem->Type = KaxTagMultiEntitiesType_Composer;
			newTagItem->Name = textField->GetRawUnicodeText();
			target->AddMultiEntityItem(newTagItem);

		} else if (myFrame->GetID() == ID3FID_PLAYLISTDELAY) {	
			target->Tag_PlaylistDelay = intField->Get();

		} else if (myFrame->GetID() == ID3FID_ENCODEDBY) {	
			MatroskaTagMultiEntityItem *newTagItem = new MatroskaTagMultiEntityItem();
			newTagItem->Type = KaxTagMultiEntitiesType_EncodedBy;
			newTagItem->Name = textField->GetRawUnicodeText();
			target->AddMultiEntityItem(newTagItem);

		} else if (myFrame->GetID() == ID3FID_LYRICIST) {	
			MatroskaTagMultiEntityItem *newTagItem = new MatroskaTagMultiEntityItem();
			newTagItem->Type = KaxTagMultiEntitiesType_LyricistTextWriter;
			newTagItem->Name = textField->GetRawUnicodeText();
			target->AddMultiEntityItem(newTagItem);

		} else if (myFrame->GetID() == ID3FID_TITLE) {
			MatroskaTagMultiTitleItem *newTagItem = new MatroskaTagMultiTitleItem();
			newTagItem->Type = KaxTagMultiTitleType_TrackTitle;
			newTagItem->Name = textField->GetRawUnicodeText();
			target->AddMultiTitleItem(newTagItem);

		} else if (myFrame->GetID() == ID3FID_SUBTITLE) {
			MatroskaTagMultiTitleItem *newTagItem = new MatroskaTagMultiTitleItem();
			newTagItem->Type = KaxTagMultiTitleType_TrackTitle;
			newTagItem->SubTitle = textField->GetRawUnicodeText();
			target->AddMultiTitleItem(newTagItem);

		} 
		
  }
  delete iter;

#endif
};
