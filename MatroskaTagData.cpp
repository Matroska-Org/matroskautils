/*
 *  Part of The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
 *
 *  MatroskaTagData.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
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
    \file MatroskaTagData.cpp
		\version \$Id: MatroskaTagData.cpp,v 1.18 2004/03/08 07:56:51 jcsston Exp $
    \brief This file deals mostly with the tag stuctures ;)
		\author Jory Stone     <jcsston @ toughguy.net>
		
		Inspired by ;) John Cannon (spyder482@yahoo.com) (c) 2003
*/

#include "MatroskaTagData.h"

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

using namespace MatroskaUtilsNamespace;
namespace MatroskaUtilsNamespace {

static MatroskaSimpleTagOfficalListEntry MatroskaSimpleTagOfficalList1[] =
{
	{_T("ARCHIVAL_LOCATION"), _T("Indicates where the subject of the file is archived (same as the IARL tag in RIFF)")},
	{_T("BIBLIOGRAPHY"), _T("")},
	{_T("BITSPS"), _T("The average bits per second of the specified item. This is only the data in the Blocks, and excludes headers and any container overhead.")},
	{_T("ENCODER"), _T("The software or hardware used to encode this item. (VirtualDubv2.1)")},
	{_T("ENCODE_SETTINGS"), _T("A list of the settings used for encoding this item. No specific format.")},
	{_T("FILE"), _T("The file that the item originated from. For example this track was encoded from the file 3_dt.vob.")},
	{_T("FRAMESPS"), _T("The average frames per second of the specified item. This is typically the average number of Blocks per second. In the event that lacing is used, each laced chunk is to be counted as a seperate frame.")},
	{_T("LANGUAGE"), _T("Specify the language of the track, in the ISO-639-2 form. If this is for a single track, the language must be set the same and the Track Language.")},
	{_T("KEYWORDS"), _T("Keyword to the item, used for searching.")},
	{_T("MOOD"), _T("A set of words indicating the mood of the item.")},
	{_T("ORIGINAL_MEDIA_TYPE"), _T("Describes the original subject of the file, such as, \"computer image,\" \"drawing,\" \"lithograph,\" and so forth.")},
	{_T("PLAY_COUNTER"), _T("The number of time the item has been played.")},
	{_T("POPULARIMETER"), _T("A numeric value defining the how much a person likes the song. This is completey subjective.")},
	{_T("PRODUCT"), _T("Specifies the name of the title the file was originally intended for, such as \"Encyclopedia of Pacific Northwest Geography.\"")},
	{_T("RECORD_LOCATION"), _T("The countries corresponding to the string, same 2 octets as in Internet domains, or possibly ISO 3166.")},
	{_T("SOURCE"), _T("Identifies the name of the person or organization who supplied the original subject of the file. For example, \"Trey Research.\"")},
	{_T("SOURCE_FORM"), _T("Identifies the original form of the material that was digitized, such as \"slide,\" \"paper,\" \"map,\" and so forth. This is not necessarily the same as IMED.")},
	{_T("SUBJECT"), _T("Describes the conbittents of the file, such as \"Aerial view of Seattle.\"")},
	{_T("RATING"), _T("The ICRA rating. (Previously RSACi)")},
	{_T("UNSYNCHRONISED_TEXT"), _T("Lyrics or subtitles that have no synchronization data.")},
	{_T("GENRE"), _T("The genre.")},
	{_T("AUDIO_GAIN"), _T("The gain to apply to reach 89db SPL on playback.")},
	{_T("AUDIO_PEAK"), _T("The maximum absolute peak value of the item.")},
	{_T("BPM"), _T("Contains the number of beats per minute in the main part of the audio. This is general.")},
	{_T("DISC_TRACK"), _T("Track number in the original CD.")},
	{_T("SET_PART"), _T("The total number of tracks on a disc.")},
	{_T("INSTRUMENT"), _T("The instrument that is being used/played.")},
	{_T("INITIAL_KEY"), _T("The initial key that a musical track starts in. The format is identical to ID3.")},
	{_T("CAPTURE_DPI"), _T("Stores dots per inch setting of the digitizer used to produce the file, such as \"300.\"")},
	{_T("CAPTURE_LIGHTNESS"), _T("Describes the changes in lightness settings on the digitizer required to produce the file. Note that the format of this information depends on hardware used.")},
	{_T("CAPTURE_PALETTE_SETTING"), _T("Specifies the number of colors requested when digitizing an image, such as \"256.\"")},
	{_T("CAPTURE_SHARPNESS"), _T("Identifies the changes in sharpness for the digitizer required to produce the file (the format depends on the hardware used).")},
	{_T("CROPPED"), _T("Describes whether an image has been cropped and, if so, how it was cropped. For example, \"30 pixels lower right corner\".")},
	{_T("ORIGINAL_DIMENSIONS"), _T("Dimensions. Specifies the size of the original subject of the file. For example, \"8.5 in h, 11 in w.\"")},
	{_T("COMMENTS"), _T("Any comment related to the content.")},
	{_T("FILE_PURCHASE"), _T("Information on where to purchase this file. This is akin to the WPAY tag in ID3.")},
	{_T("ITEM_PURCHASE"), _T("Information on where to purchase this album. This is akin to the WCOM tag in ID3.")},
	{_T("OWNER"), _T("Information on the purchase that occurred for this file. This is akin to the OWNE tag in ID3.")},
	{_T("ADDRESS"), _T("The physical address of the entity. The address should include a country code.")},
	{_T("URL"), _T("URL of the entity.")},
	{_T("EMAIL"), _T("Email address of the entity.")},
	{_T("CURRENCY"), _T("The currency type used to pay for the entity. Use ISO 4217 for the 3 letter currency code.")},
	{_T("AMOUNT"), _T("The amount paid for entity.")},
	{_T("DATE_ENCODING"), _T("The time that the encoding of this item was completed. This is akin to the TDEN tag in ID3.")},
	{_T("DATE_RECORDING"), _T("The time that the recording began, and finished. This is akin to the TDRC tag in ID3.")},
	{_T("DATE_RELEASE"), _T("The time that the item was originaly released. This is akin to the TDRL tag in ID3.")},
	{_T("DATE_RELEASE_ORIGINAL"), _T("The time that the item was originaly released if it is a remake. This is akin to the TDOR tag in ID3.")},
	{_T("DATE_TAGGING"), _T("The time that the tags were done for this item. This is akin to the TDTG tag in ID3.")},
	{_T("DATE_DIGITIZING"), _T("The time that the item was tranfered to a digital medium. This is akin to the IDIT tag in RIFF.")},
	{_T("DATE"), _T("The date the entity ocurred on or began.")},
	{_T("DATE_END"), _T("The date the entity ended.")},
	{_T("LYRICIST"), _T("The person that wrote the words/script for this item. This is akin to the TEXT tag in ID3.")},
	{_T("COMPOSER"), _T("The name of the composer of this item. This is akin to the TCOM tag in ID3.")},
	{_T("LEAD_PERFORMER"), _T("Lead Performer/Soloist(s). This is akin to the TPE1 tag in ID3.")},
	{_T("BAND"), _T("Band/orchestra/accompaniment. This is akin to the TPE2 tag in ID3.")},
	{_T("ORIGINAL_LYRICIST"), _T("Original lyricist(s)/text writer(s). This is akin to the TOLY tag in ID3.")},
	{_T("ORIGINAL_PERFORMER"), _T("Original artist(s)/performer(s). This is akin to the TOPE tag in ID3.")},
	{_T("ORIGINAL_TITLE"), _T("Original album/movie/show title. This is akin to the TOAL tag in ID3.")},
	{_T("CONDUCTOR"), _T("Conductor/performer refinement. This is akin to the TPE3 tag in ID3.")},
	{_T("MODIFIED_BY"), _T("Interpreted, remixed, or otherwise modified by. This is akin to the TPE4 tag in ID3.")},
	{_T("DIRECTOR"), _T("This is akin to the IART tag in RIFF.")},
	{_T("PRODUCED_BY"), _T("Produced by. This is akin to the IPRO tag in Extended RIFF.")},
	{_T("CINEMATOGRAPHER"), _T("Cinematographer. This is akin to the ICNM tag in Extended RIFF.")},
	{_T("PRODUCTION_DESIGNER"), _T("This is akin to the IPDS tag in Extended RIFF.")},
	{_T("COSTUME_DESIGNER"), _T("This is akin to the ICDS tag in Extended RIFF.")},
	{_T("PRODUCTION_STUDIO"), _T("This is akin to the ISTD tag in Extended RIFF.")},
	{_T("DISTRIBUTED_BY"), _T("This is akin to the IDST tag in Extended RIFF.")},
	{_T("COMMISSIONED_BY"), _T("This is akin to the ICMS tag in RIFF.")},
	{_T("ENGINEER"), _T("This is akin to the IENG tag in RIFF.")},
	{_T("EDITED_BY"), _T("This is akin to the IEDT tag in Extended RIFF.")},
	{_T("ENCODED_BY"), _T("This is akin to the TENC tag in ID3.")},
	{_T("RIPPED_BY"), _T("This is akin to the IRIP tag in Extended RIFF.")},
	{_T("INVOLVED_PERSON"), _T("A very general tag for everyone else that wants to be listed. This is akin to the TMCL tag in ID3v2.4.")},
	{_T("INTERNET_RADIO_STATION"), _T("This is akin to the TSRN tag in ID3v2.4.")},
	{_T("PUBLISHER"), _T("This is akin to the TPUB tag in ID3.")},
	{_T("ISRC"), _T("The International Standard Recording Code")},
	{_T("ISBN"), _T("International Standard Book Number")},
	{_T("CATALOG"), _T("Sometimes the EAN/UPC, often some letters followed by some numbers.")},
	{_T("EAN"), _T("EAN-13 bar code identifier")},
	{_T("UPC"), _T("UPC-A bar code identifier")},
	{_T("LABEL_CODE"), _T("Typically printed as ________ (LC) xxxx) ~~~~~~~~ or _________ (LC) 0xxxx) ~~~~~~~~~ on CDs medias or covers, where xxxx is a 4-digit number.")},
	{_T("LCCN"), _T("Library of Congress Control Number")},
	{_T("COPYRIGHT"), _T("The copyright information as per the copyright holder. This is akin to the TCOP tag in ID3.")},
	{_T("PRODUCTION_COPYRIGHT"), _T("The copyright information as per the production copyright holder. This is akin to the TPRO tag in ID3.")},
	{_T("TERMS_OF_USE"), _T("The terms of use for this item. This is akin to the USER tag in ID3.")},
	{_T("TITLE"), _T("The title of this item. In the case of a track, the MultiName element should be identical to the Name element. For example, for music you might label this \"Canon in D\", or for video's audio track you might use \"English 5.1\" This is akin to the TIT2 tag in ID3.")},
	{_T("ALBUM"), _T("This is the name given to a grouping of tracks and/or chapters. For example, all video, audio, and subtitle tracks for a movie would be grouped under this and be given the name of the movie. All tracks for a particular CD would be grouped together under the title of the CD, or if all tracks for a CD were recorded as a single track, seperated by chapters, the same would apply. You could use this to label episode 3 of The Simpsons. This is akin to the TALB tag in ID3.")},
	{_T("SET_TITLE"), _T("This would be used to label a set of ID 2. For example, season 13 of The Simpsons.")},
	{_T("SERIES"), _T("This would be used to label a set of ID 3. For example, The Simpsons.")},
	{_T("SUBTITLE"), _T("Sub Title of the entity.")},
	{_T("EDITION"), _T("Edition name of the entity (\"director's cut\", \"original edit\", etc).")},
	{NULL, NULL}
};

/*******************************************
 *           Begin Tag classes             *
 *******************************************/

MatroskaTagUID::MatroskaTagUID() 
	: std::vector<uint64>(1)
{

}

void MatroskaTagUID::Add(uint64 newUID)
{
	if (at(0) == 0) {
		at(0) = newUID;
		return;
	}
	
	if (Find(newUID) != 0)
		return;

	push_back(newUID);
}

void MatroskaTagUID::Remove(uint64 deleteUID)
{
	MatroskaTagUID::iterator itToDelete = begin();
	while (itToDelete != end()){			
		if (deleteUID == *itToDelete) {
			if (size() > 1)
				erase(itToDelete);
			else
				at(0) = 0;
			return;
		}
		itToDelete++;
	}
}

uint64 &MatroskaTagUID::Find(uint64 uid)
{
	static uint64 zeroValue = 0;

	for (size_t u = 0; u < size(); u++)
		if (at(u) == uid)
			return at(u);

	return zeroValue;
}
uint64 &MatroskaTagUID::operator = (uint64 newUID)
{
	return at(0) = newUID;
}

MatroskaTagUID::operator uint64()
{
	return at(0);
}

MatroskaTagMultiItemBase::MatroskaTagMultiItemBase() {
	Type = 0;
};

bool MatroskaTagMultiItemBase::IsValid() {
	if (Type == -1)
		return false;

	return true;
};	

void MatroskaTagMultiItemBase::Invalidate() {
	Type = -1;
};	

MatroskaTagMultiCommentItem::MatroskaTagMultiCommentItem()
{
	ODS("MatroskaTagMultiCommentItem::MatroskaTagMultiCommentItem()");
	this->Comments = L"";
	this->Language = "";
	this->next_item = NULL;
};

MatroskaTagMultiCommercialItem::MatroskaTagMultiCommercialItem()
{
	ODS("MatroskaTagMultiCommercialItem::MatroskaTagMultiCommercialItem()");
	this->Address = L"";
	this->Email = "";
	this->URL = "";
	this->PriceAmount = 0;
	this->PriceCurrency = "";
	this->PriceDate = 0;
	this->next_item = NULL;
};

MatroskaTagMultiDateItem::MatroskaTagMultiDateItem()
{
	ODS("MatroskaTagMultiDateItem::MatroskaTagMultiDateItem()");
	this->DateBegin = 0;
	this->DateEnd = 0;
	this->next_item = NULL;
};

/*
void MatroskaTagMultiDateItem::SetBeginDate(const char *dateStr)
{
	if (dateStr == NULL)
		return;

	objDateBegin = dateStr;

	tm *new_date = new tm;
	new_date->tm_hour = objDateBegin.GetHour();
	new_date->tm_min = objDateBegin.GetMin();
	new_date->tm_sec = objDateBegin.GetSeconds();
	new_date->tm_mon = objDateBegin.GetMonth();
	new_date->tm_mday = objDateBegin.GetDay();
	new_date->tm_year = objDateBegin.GetYear();

	DateBegin = _mktime64(new_date);
	delete new_date;
};*/

MatroskaTagMultiEntityItem::MatroskaTagMultiEntityItem()
{
	ODS("MatroskaTagMultiEntityItem::MatroskaTagMultiEntityItem()");
	this->Address = L"";
	this->Email = "";
	this->URL = "";
	this->Name = L"";
	this->next_item = NULL;
};

MatroskaTagMultiIdentifierItem::MatroskaTagMultiIdentifierItem()
{
	ODS("MatroskaTagMultiIdentifierItem::MatroskaTagMultiIdentifierItem()");
	this->BinaryData = NULL;
	this->BinaryData_Length = 0;
	this->StringData = L"";
	this->next_item = NULL;

};

MatroskaTagMultiLegalItem::MatroskaTagMultiLegalItem()
{
	ODS("MatroskaTagMultiLegalItem::MatroskaTagMultiLegalItem()");
	this->Address = L"";
	this->URL = "";
	this->next_item = NULL;
};

MatroskaTagMultiTitleItem::MatroskaTagMultiTitleItem()
{
	ODS("MatroskaTagMultiTitleItem::MatroskaTagMultiTitleItem()");
	this->Address = L"";
	this->Email = "";
	this->URL = "";
	this->Name = L"";
	this->Edition = L"";
	this->SubTitle = L"";
	this->Language = "";
	this->next_item = NULL;
};

/************** MatroskaTagMultiComment Class **************/
MatroskaTagMultiComment::MatroskaTagMultiComment()
{
	TRACE("MatroskaTagMultiComment::MatroskaTagMultiComment()");
	first_item = NULL;
};

MatroskaTagMultiComment::~MatroskaTagMultiComment()
{
	TRACE("MatroskaTagMultiComment::~MatroskaTagMultiComment()");
	//I should free memory here
	MatroskaTagMultiCommentItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiComment::AddItem(MatroskaTagMultiCommentItem *new_item)
{	
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiCommentItem *MatroskaTagMultiComment::FindTrackItemWithLang(char *language_id)
{
	MatroskaTagMultiCommentItem *current = first_item;
	while (current != NULL)
	{
		if (current->Language == language_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiComment::GetMultiCommentCount()
{
	MatroskaTagMultiCommentItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};

MatroskaTagMultiCommentItem *MatroskaTagMultiComment::GetItem(int requested_index)
{
	MatroskaTagMultiCommentItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};

/************** MatroskaTagMultiCommercial Class **************/

MatroskaTagMultiCommercial::MatroskaTagMultiCommercial()
{
	TRACE("MatroskaTagMultiCommercial::MatroskaTagMultiCommercial()");
	first_item = NULL;
};

MatroskaTagMultiCommercial::~MatroskaTagMultiCommercial()
{
	TRACE("MatroskaTagMultiCommercial::~MatroskaTagMultiCommercial()");
	//I should free memory here
	MatroskaTagMultiCommercialItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiCommercial::AddItem(MatroskaTagMultiCommercialItem *new_item)
{	
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiCommercialItem *MatroskaTagMultiCommercial::FindTrackItemWithType(int type_id)
{
	MatroskaTagMultiCommercialItem *current = first_item;
	while (current != NULL)
	{
		if (current->Type == type_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiCommercial::GetMultiCommercialCount()
{
	MatroskaTagMultiCommercialItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};

MatroskaTagMultiCommercialItem *MatroskaTagMultiCommercial::GetItem(int requested_index)
{
	MatroskaTagMultiCommercialItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{		
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};
char *MatroskaTagMultiCommercial::ConvertTypeUIntToStr(uint16 type_id)
{
  static char unknown_type_txt[32];
	switch(type_id)
	{
		case KaxTagMultiCommercialType_FilePurchase:
			return "File Purchase";
			break;
		case KaxTagMultiCommercialType_ItemPurchase:
			return "Item Purchase";
			break;
		case KaxTagMultiCommercialType_Owner:
			return "Owner";
			break;
		default:
			snprintf(unknown_type_txt, 31, "Unknown Type (%u)", type_id);
			return unknown_type_txt;
	}
};

uint16 MatroskaTagMultiCommercial::ConvertTypeStrToUInt(char *type_str)
{
	if (!stricmp("File Purchase", type_str)) {
		return KaxTagMultiCommercialType_FilePurchase;

	}else if (!stricmp("Item Purchase", type_str)) {
		return KaxTagMultiCommercialType_ItemPurchase;

	}else if (!stricmp("Owner", type_str)) {
		return KaxTagMultiCommercialType_Owner;
	}
	return 0;
};

/************** MatroskaTagMultiDate Class **************/

MatroskaTagMultiDate::MatroskaTagMultiDate()
{
	TRACE("MatroskaTagMultiDate::MatroskaTagMultiDate()");
	first_item = NULL;
};

MatroskaTagMultiDate::~MatroskaTagMultiDate()
{
	TRACE("MatroskaTagMultiDate::~MatroskaTagMultiDate()");
	//I should free memory here
	MatroskaTagMultiDateItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiDate::AddItem(MatroskaTagMultiDateItem *new_item)
{
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiDateItem *MatroskaTagMultiDate::FindTrackItemWithType(int type_id)
{
	MatroskaTagMultiDateItem *current = first_item;
	while (current != NULL)
	{
		if (current->Type == type_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiDate::GetMultiDateCount()
{
	MatroskaTagMultiDateItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};


MatroskaTagMultiDateItem *MatroskaTagMultiDate::GetItem(int requested_index)
{
	MatroskaTagMultiDateItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{		
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};

char *MatroskaTagMultiDate::ConvertTypeUIntToStr(uint16 type_id)
{
	switch(type_id)
	{
		case KaxTagMultiDateType_DigitizingDate:
			return "Digitizing Date";
			break;
		case KaxTagMultiDateType_EncodingDate:
			return "Encoding Date";
			break;
		case KaxTagMultiDateType_OriginalReleaseDate:
			return "Original Release Date";
			break;
		case KaxTagMultiDateType_RecordingDate:
			return "Recording Date";
			break;
		case KaxTagMultiDateType_ReleaseDate:
			return "Release Date";
			break;
		case KaxTagMultiDateType_TaggingDate:
			return "Tagging Date";
			break;
		default:
			static char type_txt[65] = { 0 };//new char[strlen("Unknown Type (65000)")+1];
			snprintf(type_txt, 64, "Unknown Type (%u)", type_id);
			return type_txt;
	}
};

uint16 MatroskaTagMultiDate::ConvertTypeStrToUInt(char *type_str)
{
	if (!stricmp("Digitizing Date", type_str)) {
		return KaxTagMultiDateType_DigitizingDate;

	}else if (!stricmp("Encoding Date", type_str)) {
		return KaxTagMultiDateType_EncodingDate;

	}else if (!stricmp("Original Release Date", type_str)) {
		return KaxTagMultiDateType_OriginalReleaseDate;

	}else if (!stricmp("Recording Date", type_str)) {
		return KaxTagMultiDateType_RecordingDate;

	}else if (!stricmp("Release Date", type_str)) {
		return KaxTagMultiDateType_ReleaseDate;

	}else if (!stricmp("Tagging Date", type_str)) {
		return KaxTagMultiDateType_TaggingDate;

	}
	return 0;
};
/************** MatroskaTagMultiEntity Class **************/

MatroskaTagMultiEntity::MatroskaTagMultiEntity()
{
	TRACE("Multi-Entity List Created");
	first_item = NULL;
};

MatroskaTagMultiEntity::~MatroskaTagMultiEntity()
{
	TRACE("Multi-Entity List Deleted");
	//I should free memory here
	MatroskaTagMultiEntityItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiEntity::AddItem(MatroskaTagMultiEntityItem *new_item)
{
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiEntityItem *MatroskaTagMultiEntity::FindTrackItemWithType(int type_id)
{
	MatroskaTagMultiEntityItem *current = first_item;
	while (current != NULL)
	{
		if (current->Type == type_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiEntity::GetMultiEntityCount()
{
	MatroskaTagMultiEntityItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};

MatroskaTagMultiEntityItem *MatroskaTagMultiEntity::GetItem(int requested_index)
{
	MatroskaTagMultiEntityItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};

char *MatroskaTagMultiEntity::ConvertTypeUIntToStr(uint16 type_id)
{
	switch(type_id)
	{
		case KaxTagMultiEntitiesType_BandOrchestraAccompaniment:
			return "Band/Orchestra/Accompaniment";
			break;
		case KaxTagMultiEntitiesType_Cinematographer:
			return "Cinematographer";
			break;
		case KaxTagMultiEntitiesType_CommissionedBy:
			return "Commissioned By";
			break;
		case KaxTagMultiEntitiesType_Composer:
			return "Composer";
			break;
		case KaxTagMultiEntitiesType_ConductorPerformerRefinement:
			return "Conductor/Performer/Refinement";
			break;
		case KaxTagMultiEntitiesType_CostumeDesigner:
			return "Costume Designer";
			break;
		case KaxTagMultiEntitiesType_Director:
			return "Director";
			break;
		case KaxTagMultiEntitiesType_DistributedBy:
			return "Distributed By";
			break;
		case KaxTagMultiEntitiesType_EditedBy:
			return "Edited By";
			break;
		case KaxTagMultiEntitiesType_EncodedBy:
			return "Encoded By";
			break;
		case KaxTagMultiEntitiesType_Engineer:
			return "Engineer";
			break;
		case KaxTagMultiEntitiesType_InternetRadioStationName:
			return "Internet Radio Station Name";
			break;
		case KaxTagMultiEntitiesType_InterpretedRemixedBy:
			return "Interpreted/Remixed By";
			break;
		case KaxTagMultiEntitiesType_InvolvedPeopleList:
			return "Involved People List";
			break;
		case KaxTagMultiEntitiesType_LeadPerformerSoloist:
			return "Lead Performer/Soloist";
			break;
		case KaxTagMultiEntitiesType_LyricistTextWriter:
			return "Lyricist Text Writer";
			break;
		case KaxTagMultiEntitiesType_OriginalAlbumMovieShowTitle:
			return "Original Album/Movie/Show Title";
			break;
		case KaxTagMultiEntitiesType_OriginalArtistPerformer:
			return "Original Artist/Performer";
			break;
		case KaxTagMultiEntitiesType_OriginalLyricistTextWriter:
			return "Original Lyricist Text Writer";
			break;
		case KaxTagMultiEntitiesType_ProducedBy:
			return "Produced By";
			break;
		case KaxTagMultiEntitiesType_ProductionDesigner:
			return "Production Designer";
			break;
		case KaxTagMultiEntitiesType_ProductionStudio:
			return "Production Studio";
			break;
		case KaxTagMultiEntitiesType_Publisher:
			return "Publisher";
			break;
		case KaxTagMultiEntitiesType_RippedBy:
			return "Ripped By";
			break;
		default:
			char *type_txt = new char[strlen("Unknown Type (65000)")+1];
			sprintf(type_txt, "Unknown Type (%u)", type_id);
			return type_txt;
	}
};

uint16 MatroskaTagMultiEntity::ConvertTypeStrToUInt(char *type_str)
{
	if (!stricmp("Ripped By", type_str)) {
		return KaxTagMultiEntitiesType_RippedBy;

	}else if (!stricmp("Band/Orchestra/Accompaniment", type_str)) {
		return KaxTagMultiEntitiesType_BandOrchestraAccompaniment;

	}else if (!stricmp("Cinematographer", type_str)) {
		return KaxTagMultiEntitiesType_Cinematographer;

	}else if (!stricmp("Commissioned By", type_str)) {
		return KaxTagMultiEntitiesType_CommissionedBy;

	}else if (!stricmp("Composer", type_str)) {
		return KaxTagMultiEntitiesType_Composer;

	}else if (!stricmp("Conductor/Performer/Refinement", type_str)) {
		return KaxTagMultiEntitiesType_ConductorPerformerRefinement;

	}else if (!stricmp("Costume Designer", type_str)) {
		return KaxTagMultiEntitiesType_CostumeDesigner;

	}else if (!stricmp("Director", type_str)) {
		return KaxTagMultiEntitiesType_Director;

	}else if (!stricmp("Distributed By", type_str)) {
		return KaxTagMultiEntitiesType_DistributedBy;

	}else if (!stricmp("Edited By", type_str)) {
		return KaxTagMultiEntitiesType_EditedBy;

	}else if (!stricmp("Encoded By", type_str)) {
		return KaxTagMultiEntitiesType_EncodedBy;

	}else if (!stricmp("Engineer", type_str)) {
		return KaxTagMultiEntitiesType_Engineer;

	}else if (!stricmp("Internet Radio Station Name", type_str)) {
		return KaxTagMultiEntitiesType_InternetRadioStationName;

	}else if (!stricmp("Interpreted/Remixed By", type_str)) {
		return KaxTagMultiEntitiesType_InterpretedRemixedBy;

	}else if (!stricmp("Involved People List", type_str)) {
		return KaxTagMultiEntitiesType_InvolvedPeopleList;

	}else if (!stricmp("Lead Performer/Soloist", type_str)) {
		return KaxTagMultiEntitiesType_LeadPerformerSoloist;

	}else if (!stricmp("Lyricist Text Writer", type_str)) {
		return KaxTagMultiEntitiesType_LyricistTextWriter;

	}else if (!stricmp("Original Album/Movie/Show Title", type_str)) {
		return KaxTagMultiEntitiesType_OriginalAlbumMovieShowTitle;

	}else if (!stricmp("Original Artist/Performer", type_str)) {
		return KaxTagMultiEntitiesType_OriginalArtistPerformer;

	}else if (!stricmp("Original Lyricist Text Writer", type_str)) {
		return KaxTagMultiEntitiesType_OriginalLyricistTextWriter;

	}else if (!stricmp("Produced By", type_str)) {
		return KaxTagMultiEntitiesType_ProducedBy;

	}else if (!stricmp("Production Designer", type_str)) {
		return KaxTagMultiEntitiesType_ProductionDesigner;

	}else if (!stricmp("Production Studio", type_str)) {
		return KaxTagMultiEntitiesType_ProductionStudio;

	}else if (!stricmp("Publisher", type_str)) {
		return KaxTagMultiEntitiesType_Publisher;

	}else if (!stricmp("Ripped By", type_str)) {
		return KaxTagMultiEntitiesType_RippedBy;

	}
	return 0;
};

/************** MatroskaTagMultiIdentifier Class **************/

MatroskaTagMultiIdentifier::MatroskaTagMultiIdentifier()
{
	TRACE("Multi-Identifier List Created");
	first_item = NULL;
};

MatroskaTagMultiIdentifier::~MatroskaTagMultiIdentifier()
{
	TRACE("Multi-Identifier List Deleted");
	//I should free memory here
	MatroskaTagMultiIdentifierItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiIdentifier::AddItem(MatroskaTagMultiIdentifierItem *new_item)
{
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiIdentifierItem *MatroskaTagMultiIdentifier::FindTrackItemWithType(int type_id)
{
	MatroskaTagMultiIdentifierItem *current = first_item;
	while (current != NULL)
	{
		if (current->Type == type_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiIdentifier::GetMultiIdentifierCount()
{
	MatroskaTagMultiIdentifierItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};

MatroskaTagMultiIdentifierItem *MatroskaTagMultiIdentifier::GetItem(int requested_index)
{
	MatroskaTagMultiIdentifierItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};

char *MatroskaTagMultiIdentifier::ConvertTypeUIntToStr(uint16 type_id)
{
	switch(type_id)
	{
		case KaxTagMultiIdentifierType_Catalog:
			return "Catalog";
			break;
		case KaxTagMultiIdentifierType_CDIdentifier:
			return "CD Identifier";
			break;
		case KaxTagMultiIdentifierType_EAN:
			return "EAN";
			break;
		case KaxTagMultiIdentifierType_ISBN:
			return "ISBN";
			break;
		case KaxTagMultiIdentifierType_ISRC:
			return "ISRC";
			break;
		case KaxTagMultiIdentifierType_LabelCode:
			return "Label Code";
			break;
		case KaxTagMultiIdentifierType_LCCN:
			return "LCCN";
			break;
		case KaxTagMultiIdentifierType_UniqueFileIdentifier:
			return "Unique File Identifier";
			break;
		case KaxTagMultiIdentifierType_UPC:
			return "UPC";
			break;
		default:
			char *type_txt = new char[strlen("Unknown Type (65000)")+1];
			sprintf(type_txt, "Unknown Type (%u)", type_id);
			return type_txt;
	}
};

uint16 MatroskaTagMultiIdentifier::ConvertTypeStrToUInt(char *type_str)
{
	if (!stricmp("Catalog", type_str)) {
		return KaxTagMultiIdentifierType_Catalog;

	}else if (!stricmp("CD Identifier", type_str)) {
		return KaxTagMultiIdentifierType_CDIdentifier;

	}else if (!stricmp("EAN", type_str)) {
		return KaxTagMultiIdentifierType_EAN;

	}else if (!stricmp("ISBN", type_str)) {
		return KaxTagMultiIdentifierType_ISBN;

	}else if (!stricmp("ISRC", type_str)) {
		return KaxTagMultiIdentifierType_ISRC;

	}else if (!stricmp("Label Code", type_str)) {
		return KaxTagMultiIdentifierType_LabelCode;

	}else if (!stricmp("LCCN", type_str)) {
		return KaxTagMultiIdentifierType_LCCN;

	}else if (!stricmp("Unique File Identifier", type_str)) {
		return KaxTagMultiIdentifierType_UniqueFileIdentifier;

	}else if (!stricmp("UPC", type_str)) {
		return KaxTagMultiIdentifierType_UPC;

	}
	return 0;
};
/************** MatroskaTagMultiLegal Class **************/

MatroskaTagMultiLegal::MatroskaTagMultiLegal()
{
	TRACE("Multi-Legal List Created");
	first_item = NULL;
};

MatroskaTagMultiLegal::~MatroskaTagMultiLegal()
{
	TRACE("Multi-Legal List Deleted");
	//I should free memory here
	MatroskaTagMultiLegalItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiLegal::AddItem(MatroskaTagMultiLegalItem *new_item)
{
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiLegalItem *MatroskaTagMultiLegal::FindTrackItemWithType(int type_id)
{
	MatroskaTagMultiLegalItem *current = first_item;
	while (current != NULL)
	{
		if (current->Type == type_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiLegal::GetMultiLegalCount()
{
	MatroskaTagMultiLegalItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};

MatroskaTagMultiLegalItem *MatroskaTagMultiLegal::GetItem(int requested_index)
{
	MatroskaTagMultiLegalItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};

char *MatroskaTagMultiLegal::ConvertTypeUIntToStr(uint16 type_id)
{
	switch(type_id)
	{
		case KaxTagMultiLegalType_Copyright:
			return "Copyright";
			break;
		case KaxTagMultiLegalType_ProductionCopyright:
			return "Production Copyright";
			break;
		case KaxTagMultiLegalType_TermsOfUse:
			return "Terms Of Use";
			break;
		default:
			char *type_txt = new char[strlen("Unknown Type (65000)")+1];
			sprintf(type_txt, "Unknown Type (%u)", type_id);
			return type_txt;
	}
};

uint16 MatroskaTagMultiLegal::ConvertTypeStrToUInt(char *type_str)
{
	if (!stricmp("Copyright", type_str)) {
		return KaxTagMultiLegalType_Copyright;

	}else if (!stricmp("Production Copyright", type_str)) {
		return KaxTagMultiLegalType_ProductionCopyright;

	}else if (!stricmp("Terms Of Use", type_str)) {
		return KaxTagMultiLegalType_TermsOfUse;

	}
	return 0;
};

/************** MatroskaTagMultiIdentifier Class **************/

MatroskaTagMultiTitle::MatroskaTagMultiTitle()
{
	TRACE("Multi-Title List Created");
	first_item = NULL;
};

MatroskaTagMultiTitle::~MatroskaTagMultiTitle()
{
	TRACE("Multi-Title List Deleted");
	//I should free memory here
	MatroskaTagMultiTitleItem *temp = NULL;

	temp = first_item;
	while (temp != NULL)
	{		
		if (first_item != NULL)
		{
			first_item = first_item->next_item;
		}
		delete temp;
		temp = first_item;
	}
};

void MatroskaTagMultiTitle::AddItem(MatroskaTagMultiTitleItem *new_item)
{
	if (new_item != NULL) {
		new_item->next_item = first_item;
		first_item = new_item;
	}
};

MatroskaTagMultiTitleItem *MatroskaTagMultiTitle::FindTrackItemWithType(int type_id)
{
	MatroskaTagMultiTitleItem *current = first_item;
	while (current != NULL)
	{
		if (current->Type == type_id) {
			return current;
		}
		//Keep searching
		current = current->next_item;
	}
	return NULL;
};

int MatroskaTagMultiTitle::GetMultiTitleCount()
{
	MatroskaTagMultiTitleItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		item_count++;
		//Keep going
		current = current->next_item;
	}
	return item_count;
};

MatroskaTagMultiTitleItem *MatroskaTagMultiTitle::GetItem(int requested_index)
{
	MatroskaTagMultiTitleItem *current = first_item;
	int item_count = 0;
	while (current != NULL)
	{
		if (requested_index == item_count)
			return current;
		//Keep searching
		item_count++;
		current = current->next_item;
	}
	return NULL;
};

char *MatroskaTagMultiTitle::ConvertTypeUIntToStr(uint16 type_id)
{
	switch(type_id)
	{
		case KaxTagMultiTitleType_AlbumMovieShowTitle:
			return "Album/Movie/Show Title";
			break;
		case KaxTagMultiTitleType_Series:
			return "Series";
			break;
		case KaxTagMultiTitleType_SetTitle:
			return "Set Title";
			break;
		case KaxTagMultiTitleType_TrackTitle:
			return "Title";
			break;
		default:
			char *type_txt = new char[strlen("Unknown Type (65000)")+1];
			sprintf(type_txt, "Unknown Type (%u)", type_id);
			return type_txt;
	}
};

uint16 MatroskaTagMultiTitle::ConvertTypeStrToUInt(char *type_str)
{
	if (!stricmp("Album/Movie/Show Title", type_str)) {
		return KaxTagMultiTitleType_AlbumMovieShowTitle;

	}else if (!stricmp("Series", type_str)) {
		return KaxTagMultiTitleType_Series;

	}else if (!stricmp("Set Title", type_str)) {
		return KaxTagMultiTitleType_SetTitle;

	}else if (!stricmp("Title", type_str)) {
		return KaxTagMultiTitleType_TrackTitle;
	}
	return 0;
};

MatroskaSimpleTag::MatroskaSimpleTag()
{
	name = L"";
	value = L"";
	bDelete = false;
};

MatroskaSimpleTagOfficalListEntry &MatroskaSimpleTag::GetOfficalList(size_t i)
{
	return MatroskaSimpleTagOfficalList1[i];
}

TCHAR *MatroskaSimpleTag::GetOfficalList_Name(size_t i)
{
	return MatroskaSimpleTagOfficalList1[i].name;
}

TCHAR *MatroskaSimpleTag::GetOfficalList_Desc(size_t i)
{
	return MatroskaSimpleTagOfficalList1[i].desc;
}

MatroskaSimpleTagList::MatroskaSimpleTagList()
{

}

void MatroskaSimpleTagList::Remove(size_t index)
{
	delete at(index);
	for (size_t t = index; index < size()-1; t++)
	{
		at(t) = at(t+1);
	}
	pop_back();
}

void MatroskaSimpleTagList::Remove(MatroskaSimpleTag *deleteTag)
{
	size_t index = Lookup(deleteTag);
	if (index != -1)
		Remove(index);
}

size_t MatroskaSimpleTagList::Lookup(MatroskaSimpleTag *tagLookup)
{
	for (size_t t = 0; t < size(); t++)
		if (at(t) == tagLookup)
			return t;

	return -1;
}

void MatroskaSimpleTagList::Cleanup()
{
	for (size_t s = 0; s < size(); s++) {
		MatroskaSimpleTag *currentSimpleTag = at(s);
		if (currentSimpleTag->bDelete) {
			Remove(s);
			// Since the index values are different we need to start over
			Cleanup();
			break;
		}
	}
}

UTFstring &MatroskaSimpleTagList::GetSimpleTag(const char *name)
{
	static UTFstring dummy(L"");

	assert(name != NULL);
	for (size_t s = 0; s < size(); s++) {
		MatroskaSimpleTag *currentSimpleTag = at(s);
		if (!strcmpi(currentSimpleTag->name.GetUTF8().c_str(), name)) {
			return currentSimpleTag->value;
		}
	}

	return dummy;
}
void MatroskaSimpleTagList::SetSimpleTagValue(const char *name, const char *value)
{
	assert(name != NULL);
	assert(value != NULL);
	for (size_t s = 0; s < size(); s++) {
		MatroskaSimpleTag *currentSimpleTag = at(s);
		if (!strcmpi(currentSimpleTag->name.GetUTF8().c_str(), name)) {
			currentSimpleTag->value.SetUTF8(value);
			return;
		}
	}
	// If we are here then we didn't find this tag in the vector already
	MatroskaSimpleTag *newSimpleTag = new MatroskaSimpleTag();;
	newSimpleTag->name.SetUTF8(name);
	newSimpleTag->value.SetUTF8(value);
	push_back(newSimpleTag);
};

void MatroskaSimpleTagList::SetSimpleTagValue(const wchar_t *name, const wchar_t *value)
{
	assert(name != NULL);
	assert(value != NULL);
	for (size_t s = 0; s < size(); s++) {
		MatroskaSimpleTag *currentSimpleTag = at(s);
		if (!wcsicmp(currentSimpleTag->name.c_str(), name)) {
			currentSimpleTag->value = value;
			return;
		}
	}
	// If we are here then we didn't find this tag in the vector already
	MatroskaSimpleTag *newSimpleTag = new MatroskaSimpleTag();;
	newSimpleTag->name = name;
	newSimpleTag->value = value;
	push_back(newSimpleTag);
};

MatroskaSimpleTagList::~MatroskaSimpleTagList()
{
	for (size_t t = 0; t < size(); t++)
		delete at(t);
}

/*******************************************
 *      Begin MatroskaTagInfo class        *
 *******************************************/

MatroskaTagInfo::MatroskaTagInfo()
{
	ODS("MatroskaTagInfo::MatroskaTagInfo()");

	Tag_TrackUID = 0;
	Tag_ChapterUID = 0;
	Tag_AttachmentUID = 0;

	Tag_AudioGain = 0;
	Tag_AudioPeak = 0;
	Tag_BPM = 0;
	Tag_CaptureDPI = 0;
	Tag_CapturePaletteSetting = 0;
	Tag_DiscTrack = 0;
	Tag_Length = 0;
	Tag_PlayCounter = 0;
	Tag_PlaylistDelay = 0;
	Tag_Popularimeter = 0;
	Tag_SetPart = 0;

	Tag_MultiComments = new MatroskaTagMultiComment();
	Tag_MultiCommercials = new MatroskaTagMultiCommercial();
	Tag_MultiDates = new MatroskaTagMultiDate();
	Tag_MultiEntities = new MatroskaTagMultiEntity();
	Tag_MultiIdentifiers = new MatroskaTagMultiIdentifier();
	Tag_MultiLegals = new MatroskaTagMultiLegal();
	Tag_MultiTitles = new MatroskaTagMultiTitle();
	Tag_Simple_Title = NULL;
	Tag_Simple_Artist = NULL;
	Tag_Simple_Album = NULL;
};

MatroskaTagInfo::~MatroskaTagInfo()
{
	ODS("MatroskaTagInfo::~MatroskaTagInfo()");
	//Let's free tons of memory here :p
	if (this->Tag_MultiComments != NULL) {
		delete Tag_MultiComments;
		Tag_MultiComments = NULL;
	}
	if (this->Tag_MultiCommercials != NULL) {
		delete Tag_MultiCommercials;
		Tag_MultiCommercials = NULL;
	}
	if (this->Tag_MultiDates != NULL) {
		delete Tag_MultiDates;
		Tag_MultiDates = NULL;
	}
	if (this->Tag_MultiEntities != NULL) {
		delete Tag_MultiEntities;
		Tag_MultiEntities = NULL;
	}
	if (this->Tag_MultiIdentifiers != NULL) {
		delete Tag_MultiIdentifiers;
		Tag_MultiIdentifiers = NULL;
	}
	if (this->Tag_MultiLegals != NULL) {
		delete Tag_MultiLegals;
		Tag_MultiLegals = NULL;
	}
	if (this->Tag_MultiTitles != NULL) {
		delete Tag_MultiTitles;
		Tag_MultiTitles = NULL;
	}
};

void MatroskaTagInfo::MergeWithThisTagInfo(MatroskaTagInfo *source_tags)
{
	Tag_ChapterUID = source_tags->Tag_ChapterUID;
	Tag_AudioEncryption = source_tags->Tag_AudioEncryption;
	Tag_AudioGain = source_tags->Tag_AudioGain;
	Tag_AudioPeak = source_tags->Tag_AudioPeak;
	Tag_BPM = source_tags->Tag_BPM;
	Tag_CaptureDPI = source_tags->Tag_CaptureDPI;
	Tag_CapturePaletteSetting = source_tags->Tag_CapturePaletteSetting;
	Tag_DiscTrack = source_tags->Tag_DiscTrack;
	Tag_Length = source_tags->Tag_Length;
	Tag_PlayCounter = source_tags->Tag_PlayCounter;
	Tag_PlaylistDelay = source_tags->Tag_PlaylistDelay;
	Tag_Popularimeter = source_tags->Tag_Popularimeter;
	Tag_Rating = source_tags->Tag_Rating;
	Tag_SetPart = source_tags->Tag_SetPart;
	Tag_VideoGenre = source_tags->Tag_VideoGenre;
	Tag_VideoSecondaryGenre = source_tags->Tag_VideoSecondaryGenre;

	delete source_tags;
};

char *MatroskaTagInfo::GetTagListStr(uint32 tag_index)
{
	switch (tag_index)
	{
		case 1:
      return "MultiComment";
		case 2:
      return "MultiCommercial";
		case 3:
			return "MultiDate";
		case 4:
			return "MultiEntity";
		case 5:
			return "MultiIdentifier";
		case 6:
			return "MultiLegal";
		case 7:
			return "MultiTitle";		
		case 9:
			return "Archival Location";
		case 10:
			return "Audio Gain";
		case 11:
			return "Audio Genre";
		case 12:
			return "Audio Peak";
		case 14:
			return "Bibliography";
		case 15:
			return "BPM";
		case 16:
			return "Capture DPI";
		case 17:
			return "Capture Palette Setting";
		case 18:
			return "Cropped";
		case 19:
			return "Disc Track";
		case 20:
			return "Encoder";
		case 21:
			return "Encode Settings";
		case 22:
			return "File";
		case 23:
			return "Initial Key";
		case 24:
			return "Keywords";
		case 25:
			return "Language";
		case 26:
			return "Length";
		case 27:
			return "Official Audio File URL";
		case 28:
			return "Official Audio Source URL";
		case 29:
			return "Original Dimensions";
		case 30:
			return "Original Media Type";
		case 31:
			return "Play Counter";
		case 32:
			return "Playlist Delay";
		case 33:
			return "Popularimeter";
		case 34:
			return "Product";
		case 35:
			return "Record Location";
		case 36:
			return "Set Part";
		case 37:
			return "Source";
		case 38:
			return "Source Form";
		case 39:
			return "Sub Genre";
		case 40:
			return "Subject";
		//case 43:
			//return "Unsynchronised Text";
		//case 44:
			//return "User Defined URL";	
		/* Binary data tags
		case 44:
			return "Audio Encryption";
		case 45:
			return "Video Genre";
		case 46:
			return "Video Secondary Genre"
		*/
	};
	return NULL;
};

uint32 MatroskaTagInfo::GetTagListUInt(char* tag_txt)
{
	if (!stricmp(tag_txt, "MultiComment")) {
		return 1;

	}else if (!stricmp(tag_txt, "MultiCommercial")) {
		return 2;

	}else if (!stricmp(tag_txt, "MultiDate")) {
		return 3;

	}else if (!stricmp(tag_txt, "MultiEntity")) {
		return 4;

	}else if (!stricmp(tag_txt, "MultiIdentifier")) {
		return 5;

	}else if (!stricmp(tag_txt, "MultiLegal")) {
		return 6;

	}else if (!stricmp(tag_txt, "MultiTitle")) {
		return 7;

	}else if (!stricmp(tag_txt, "Abstract")) {
		return 8;

	}else if (!stricmp(tag_txt, "Archival Location")) {
		return 9;

	}else if (!stricmp(tag_txt, "Audio Gain")) {
		return 10;

	}else if (!stricmp(tag_txt, "Audio Genre")) {
		return 11;

	}else if (!stricmp(tag_txt, "Audio Peak")) {
		return 12;

	}else if (!stricmp(tag_txt, "Audio Secondary Genre")) {
		return 13;

	}else if (!stricmp(tag_txt, "Bibliography")) {
		return 14;

	}else if (!stricmp(tag_txt, "BPM")) {
		return 15;

	}else if (!stricmp(tag_txt, "Capture DPI")) {
		return 16;

	}else if (!stricmp(tag_txt, "Capture Palette Setting")) {
		return 17;

	}else if (!stricmp(tag_txt, "Cropped")) {
		return 18;

	}else if (!stricmp(tag_txt, "Disc Track")) {
		return 19;

	}else if (!stricmp(tag_txt, "Encoder")) {
		return 20;

	}else if (!stricmp(tag_txt, "Encode Settings")) {
		return 21;

	}else if (!stricmp(tag_txt, "File")) {
		return 22;

	}else if (!stricmp(tag_txt, "Initial Key")) {
		return 23;

	}else if (!stricmp(tag_txt, "Keywords")) {
		return 24;

	}else if (!stricmp(tag_txt, "Language")) {
		return 25;

	}else if (!stricmp(tag_txt, "Length")) {
		return 26;

	}else if (!stricmp(tag_txt, "Official Audio File URL")) {
		return 27;

	}else if (!stricmp(tag_txt, "Official Audio Source URL")) {
		return 28;

	}else if (!stricmp(tag_txt, "Original Dimensions")) {
		return 29;

	}else if (!stricmp(tag_txt, "Original Media Type")) {
		return 30;

	}else if (!stricmp(tag_txt, "Play Counter")) {
		return 31;

	}else if (!stricmp(tag_txt, "Playlist Delay")) {
		return 32;

	}else if (!stricmp(tag_txt, "Popularimeter")) {
		return 33;

	}else if (!stricmp(tag_txt, "Product")) {
		return 34;

	}else if (!stricmp(tag_txt, "Record Location")) {
		return 35;

	}else if (!stricmp(tag_txt, "Set Part")) {
		return 36;

	}else if (!stricmp(tag_txt, "Source")) {
		return 37;

	}else if (!stricmp(tag_txt, "Source Form")) {
		return 38;

	}else if (!stricmp(tag_txt, "Sub Genre")) {
		return 39;

	}else if (!stricmp(tag_txt, "Subject")) {
		return 40;

	}else if (!stricmp(tag_txt, "Synchronised Tempo")) {
		return 41;

	}else if (!stricmp(tag_txt, "Synchronised Text")) {
		return 42;

	}else if (!stricmp(tag_txt, "Unsynchronised Text")) {
		return 43;

	}else if (!stricmp(tag_txt, "User Defined URL")) {
		return 44;
	}

	return 0;
};


char *MatroskaTagInfo::GetTagListStrInfo(uint32 tag_index)
{
	switch (tag_index)
	{
		case 1:
      return "Multi-Comment tag. You can add as many of these as you want";
		case 2:
      return "Multi-Commercial tag. You can add as many of these as you want";
		case 3:
			return "Multi-Date tag. You can add as many of these as you want";
		case 4:
			return "Multi-Entity tag. You can add as many of these as you want";
		case 5:
			return "Multi-Identifier tag. You can add as many of these as you want";
		case 6:
			return "Multi-Legal tag. You can add as many of these as you want";
		case 7:
			return "Multi-Title tag. You can add as many of these as you want";		
		case 8:
			return "Abstract tag";
		case 9:
			return "Archival Location tag";
		case 10:
			return "Audio Gain tag";
		case 11:
			return "Audio Genre tag";
		case 12:
			return "Audio Peak tag";
		case 13:
			return "Audio Secondary Genre tag";
		case 14:
			return "Bibliography tag";
		case 15:
			return "BPM tag";
		case 16:
			return "Capture DPI tag";
		case 17:
			return "Capture Palette Setting tag";
		case 18:
			return "Cropped tag";
		case 19:
			return "Disc Track";
		case 20:
			return "Encoder tag";
		case 21:
			return "Encode Settings tag";
		case 22:
			return "File tag";
		case 23:
			return "Initial Key tag";
		case 24:
			return "Keywords tag";
		case 25:
			return "Language tag";
		case 26:
			return "Length tag";
		case 27:
			return "Official Audio File URL tag";
		case 28:
			return "Official Audio Source URL tag";
		case 29:
			return "Original Dimensions tag";
		case 30:
			return "Original Media Type tag";
		case 31:
			return "Play Counter tag";
		case 32:
			return "Playlist Delay tag";
		case 33:
			return "Popularimeter tag";
		case 34:
			return "Product tag";
		case 35:
			return "Record Location tag";
		case 36:
			return "Set Part tag";
		case 37:
			return "Source tag";
		case 38:
			return "Source Form tag";
		case 39:
			return "Sub Genre tag";
		case 40:
			return "Subject tag";
		case 41:
			return "Synchronised Tempo tag";
		case 42:
			return "Synchronised Text tag";
		case 43:
			return "Unsynchronised Text tag";
		case 44:
			return "User Defined URL tag";	
		/* Binary data tags
		case 44:
			return "Audio Encryption";
		case 45:
			return "Video Genre";
		case 46:
			return "Video Secondary Genre"
		*/
	};
	return "";
};

void MatroskaTagInfo::AddMultiCommentItem(MatroskaTagMultiCommentItem *new_item)
{
	if (this->Tag_MultiComments == NULL)
	{
		//Create a new one
		this->Tag_MultiComments = new MatroskaTagMultiComment();
		this->Tag_MultiComments->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiComments->AddItem(new_item);
	}
}

void MatroskaTagInfo::AddMultiCommercialItem(MatroskaTagMultiCommercialItem *new_item)
{
	if (this->Tag_MultiCommercials == NULL)
	{
		//Create a new one
		this->Tag_MultiCommercials = new MatroskaTagMultiCommercial();
		this->Tag_MultiCommercials->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiCommercials->AddItem(new_item);
	}
}

void MatroskaTagInfo::AddMultiDateItem(MatroskaTagMultiDateItem *new_item)
{
	if (this->Tag_MultiDates == NULL)
	{
		//Create a new one
		this->Tag_MultiDates = new MatroskaTagMultiDate();
		this->Tag_MultiDates->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiDates->AddItem(new_item);
	}
};

void MatroskaTagInfo::AddMultiEntityItem(MatroskaTagMultiEntityItem *new_item)
{
	if (this->Tag_MultiEntities == NULL)
	{
		//Create a new one
		this->Tag_MultiEntities = new MatroskaTagMultiEntity();
		this->Tag_MultiEntities->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiEntities->AddItem(new_item);
	}
};

void MatroskaTagInfo::AddMultiIdentifierItem(MatroskaTagMultiIdentifierItem *new_item)
{
	if (this->Tag_MultiIdentifiers == NULL)
	{
		//Create a new one
		this->Tag_MultiIdentifiers = new MatroskaTagMultiIdentifier();
		this->Tag_MultiIdentifiers->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiIdentifiers->AddItem(new_item);
	}
};

void MatroskaTagInfo::AddMultiLegalItem(MatroskaTagMultiLegalItem *new_item)
{
	if (this->Tag_MultiLegals == NULL)
	{
		//Create a new one
		this->Tag_MultiLegals = new MatroskaTagMultiLegal();
		this->Tag_MultiLegals->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiLegals->AddItem(new_item);
	}
};

void MatroskaTagInfo::AddMultiTitleItem(MatroskaTagMultiTitleItem *new_item)
{
	if (this->Tag_MultiTitles == NULL)
	{
		//Create a new one
		this->Tag_MultiTitles = new MatroskaTagMultiTitle();
		this->Tag_MultiTitles->AddItem(new_item);
	}else {
		//We already have one :)
		this->Tag_MultiTitles->AddItem(new_item);
	}
};

MatroskaTagList::MatroskaTagList()
{

}

MatroskaTagList::~MatroskaTagList()
{
	for (size_t t = 0; t < size(); t++)
		delete at(t);
}

UTFstring &MatroskaTagList::FindSimpleTag(const char *name)
{
	static UTFstring dummy(L"");
	for (size_t t = 0; t < size(); t++)
	{
		MatroskaTagInfo *currentTag = at(t);
		UTFstring &value = currentTag->Tag_Simple_Tags.GetSimpleTag(name);
		if (value.length() > 0)
			return value;
	}
	return dummy;
}

MatroskaTagInfo *MatroskaTagList::FindByTrackUID(uint64 uid, bool bOnly)
{
	for (size_t t = 0; t < size(); t++)
	{
		MatroskaTagInfo *currentTag = at(t);
		
		if (bOnly 			
			&& (currentTag->Tag_TrackUID.Find(uid) != 0)
			&& (currentTag->Tag_TrackUID.size() == 1))
		{
			return currentTag;

		} else if (currentTag->Tag_TrackUID.Find(uid) != 0) {
			return currentTag;
		}

	}

	return NULL;
}

void MatroskaTagList::Remove(MatroskaTagInfo *tag)
{
	MatroskaTagList::iterator itToDelete = begin();
	while (itToDelete != end()){			
		if (tag == *itToDelete) {
			erase(itToDelete);
			return;
		}
		itToDelete++;
	}
}

#ifdef USE_NEW_MKV_TAG_SYSTEM

/*******************************************
 *      Begin MatroskaTagItem class        *
 *******************************************/

MatroskaTagItem::MatroskaTagItem()
{
	tag_type = MTag_None;
	binary_data = NULL;
	binary_data_len = 0;
	float_data = NULL;
	integer_data = NULL;
	string_data = NULL;
	uinteger_data = NULL;
	UTF_data = NULL;
	MultiComment_data = NULL;
	MultiCommercial_data = NULL;
	MultiDate_data = NULL;
	MultiEntity_data = NULL;
	MultiIdentifier_data = NULL;
	MultiLegal_data = NULL;
};

MatroskaTagItem::~MatroskaTagItem()
{
	delete binary_data;
	binary_data = NULL;
	delete float_data;
	float_data = NULL;
	delete integer_data;
	integer_data = NULL;
	delete string_data;
	string_data = NULL;
	delete uinteger_data;
	uinteger_data = NULL;
	delete UTF_data;
	UTF_data = NULL;
	delete MultiComment_data;
	MultiComment_data = NULL;
	delete MultiCommercial_data;
	MultiCommercial_data = NULL;
	delete MultiDate_data;
	MultiDate_data = NULL;
	delete MultiEntity_data;
	MultiEntity_data = NULL;
	delete MultiIdentifier_data;
	MultiIdentifier_data = NULL;
	delete MultiLegal_data;
	MultiLegal_data = NULL;
};

/*******************************************
 *      Begin MatroskaTagList class        *
 *******************************************/

#endif // USE_NEW_MKV_TAG_SYSTEM

}; //End namespace MatroskaUtilsNamespace

