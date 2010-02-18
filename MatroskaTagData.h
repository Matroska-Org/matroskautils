/*
 *  Part of The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
 *
 *  MatroskaTagData.h
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
    \file MatroskaTagData.h
		\version \$Id: MatroskaTagData.h,v 1.18 2004/03/08 07:56:51 jcsston Exp $
    \brief This file deals mostly with the tag stuctures ;)
		\author Jory Stone     <jcsston @ toughguy.net>
		
		Inspired by ;) John Cannon (spyder482@yahoo.com) (c) 2003
*/

#ifndef _MATROSKA_TAG_DATA_H_
#define _MATROSKA_TAG_DATA_H_

#ifdef WIN32
#include <windows.h>
#endif //WIN32
#include <time.h>

#include "MatroskaUtilsDebug.h"

// libebml includes
#include "ebml/StdIOCallback.h"
#include "ebml/EbmlTypes.h"
#include "ebml/EbmlHead.h"
#include "ebml/EbmlVoid.h"
#include "ebml/EbmlCrc32.h"
#include "ebml/EbmlSubHead.h"
#include "ebml/EbmlStream.h"
#include "ebml/EbmlBinary.h"
#include "ebml/EbmlString.h"
#include "ebml/EbmlUnicodeString.h"
#include "ebml/EbmlContexts.h"
#include "ebml/EbmlVersion.h"

// libmatroska includes
#include "matroska/KaxConfig.h"
#include "matroska/KaxBlock.h"
#include "matroska/KaxSegment.h"
#include "matroska/KaxContexts.h"
#include "matroska/KaxSeekHead.h"
#include "matroska/KaxTracks.h"
#include "matroska/KaxInfo.h"
#include "matroska/KaxInfoData.h"
#include "matroska/KaxTags.h"
#include "matroska/KaxTag.h"
#include "matroska/KaxTagMulti.h"
#include "matroska/KaxCluster.h"
#include "matroska/KaxClusterData.h"
#include "matroska/KaxTrackAudio.h"
#include "matroska/KaxTrackVideo.h"
#include "matroska/KaxAttachments.h"
#include "matroska/KaxAttached.h"
#include "matroska/KaxVersion.h"

namespace MatroskaUtilsNamespace {

struct MatroskaSimpleTagOfficalListEntry {
	TCHAR *name;
	TCHAR *desc;
};

class MatroskaTagUID : public std::vector<uint64> {
public:
	MatroskaTagUID();

	void Add(uint64 newUID);
	void Remove(uint64 deleteUID);	
	uint64 &Find(uint64 uid);
	uint64 &operator = (uint64 newUID);
	operator uint64();
	
};

class MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiItemBase();
	/// Checks if the multi-tag is valid
	virtual bool IsValid();	
	/// Invalidates the tag and marks for deletion
	virtual void Invalidate();	
	uint16 Type;
};

class MatroskaTagMultiCommentItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiCommentItem();
	std::string Name;
	UTFstring Comments;
	std::string Language;
	///Pointer to next item
	MatroskaTagMultiCommentItem *next_item;
};

class MatroskaTagMultiCommercialItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiCommercialItem();	
	UTFstring Address;
	std::string URL;
	std::string Email;
	std::string PriceCurrency;
	double PriceAmount;
	int64 PriceDate;
	///Pointer to next item
	MatroskaTagMultiCommercialItem *next_item;
};

class MatroskaTagMultiDateItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiDateItem();

	time_t DateBegin;
	time_t DateEnd;
	///Pointer to next item
	MatroskaTagMultiDateItem *next_item;
};

class MatroskaTagMultiEntityItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiEntityItem();

	UTFstring Name;
	std::string URL;
	std::string Email;
	UTFstring Address;
	///Pointer to next item
	MatroskaTagMultiEntityItem *next_item;
};

class MatroskaTagMultiIdentifierItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiIdentifierItem();
	
	binary *BinaryData;
	uint32 BinaryData_Length;
	UTFstring StringData;
	///Pointer to next item
	MatroskaTagMultiIdentifierItem *next_item;
};

class MatroskaTagMultiLegalItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiLegalItem();
	
	std::string URL;
	UTFstring Address;
	UTFstring Text;
	///Pointer to next item
	MatroskaTagMultiLegalItem *next_item;
};

class MatroskaTagMultiTitleItem : public MatroskaTagMultiItemBase {
public:
	MatroskaTagMultiTitleItem();
	
	UTFstring Name;
	UTFstring SubTitle;
	UTFstring Edition;
	UTFstring Address;
	std::string URL;
	std::string Email;
	std::string Language;
	///Pointer to next item
	MatroskaTagMultiTitleItem *next_item;
};

class MatroskaTagMultiComment
{
public:
	MatroskaTagMultiComment();
	~MatroskaTagMultiComment();
	void AddItem(MatroskaTagMultiCommentItem *new_item);
	MatroskaTagMultiCommentItem *FindTrackItemWithLang(char *language_id);
	int GetMultiCommentCount();
	MatroskaTagMultiCommentItem *GetItem(int requested_index);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiCommentItem *first_item;
};

class MatroskaTagMultiCommercial
{
public:
	MatroskaTagMultiCommercial();
	~MatroskaTagMultiCommercial();
	void AddItem(MatroskaTagMultiCommercialItem *new_item);
	MatroskaTagMultiCommercialItem *FindTrackItemWithType(int type_id);
	int GetMultiCommercialCount();
	MatroskaTagMultiCommercialItem *GetItem(int requested_index);
	static char *ConvertTypeUIntToStr(uint16 type_id);
	static uint16 ConvertTypeStrToUInt(char *type_str);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiCommercialItem *first_item;
};

class MatroskaTagMultiDate
{
public:
	MatroskaTagMultiDate();
	~MatroskaTagMultiDate();
	void AddItem(MatroskaTagMultiDateItem *new_item);
	MatroskaTagMultiDateItem *FindTrackItemWithType(int type_id);
	int GetMultiDateCount();
	MatroskaTagMultiDateItem *GetItem(int requested_index);
	static char *ConvertTypeUIntToStr(uint16 type_id);
	static uint16 ConvertTypeStrToUInt(char *type_str);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiDateItem *first_item;
};

class MatroskaTagMultiEntity
{
public:
	MatroskaTagMultiEntity();
	~MatroskaTagMultiEntity();
	void AddItem(MatroskaTagMultiEntityItem *new_item);
	MatroskaTagMultiEntityItem *FindTrackItemWithType(int type_id);
	int GetMultiEntityCount();
	MatroskaTagMultiEntityItem *GetItem(int requested_index);
	static char *ConvertTypeUIntToStr(uint16 type_id);
	static uint16 ConvertTypeStrToUInt(char *type_str);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiEntityItem *first_item;
};

class MatroskaTagMultiIdentifier
{
public:
	MatroskaTagMultiIdentifier();
	~MatroskaTagMultiIdentifier();
	void AddItem(MatroskaTagMultiIdentifierItem *new_item);
	MatroskaTagMultiIdentifierItem *FindTrackItemWithType(int type_id);
	int GetMultiIdentifierCount();
	MatroskaTagMultiIdentifierItem *GetItem(int requested_index);
	static char *ConvertTypeUIntToStr(uint16 type_id);
	static uint16 ConvertTypeStrToUInt(char *type_str);

protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiIdentifierItem *first_item;
};

class MatroskaTagMultiLegal
{
public:
	MatroskaTagMultiLegal();
	~MatroskaTagMultiLegal();
	void AddItem(MatroskaTagMultiLegalItem *new_item);
	MatroskaTagMultiLegalItem *FindTrackItemWithType(int type_id);
	int GetMultiLegalCount();
	MatroskaTagMultiLegalItem *GetItem(int requested_index);
	static char *ConvertTypeUIntToStr(uint16 type_id);
	static uint16 ConvertTypeStrToUInt(char *type_str);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiLegalItem *first_item;
};

class MatroskaTagMultiTitle
{
public:
	MatroskaTagMultiTitle();
	~MatroskaTagMultiTitle();
	void AddItem(MatroskaTagMultiTitleItem *new_item);
	MatroskaTagMultiTitleItem *FindTrackItemWithType(int type_id);
	int GetMultiTitleCount();
	MatroskaTagMultiTitleItem *GetItem(int requested_index);
	static char *ConvertTypeUIntToStr(uint16 type_id);
	static uint16 ConvertTypeStrToUInt(char *type_str);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagMultiTitleItem *first_item;
};

class MatroskaSimpleTag {
public:
	MatroskaSimpleTag();		

	static MatroskaSimpleTagOfficalListEntry &GetOfficalList(size_t i);
	static TCHAR *GetOfficalList_Name(size_t i);
	static TCHAR *GetOfficalList_Desc(size_t i);
	UTFstring name;
	UTFstring value;
	bool bDelete;
};

class MatroskaSimpleTagList : public std::vector<MatroskaSimpleTag *> {
public:
	MatroskaSimpleTagList();
	~MatroskaSimpleTagList();

	UTFstring &GetSimpleTag(const char *name);
	void SetSimpleTagValue(const char *name, const char *value);
	void SetSimpleTagValue(const wchar_t *name, const wchar_t *value);
	void Remove(size_t index);
	void Remove(MatroskaSimpleTag *deleteTag);
	size_t Lookup(MatroskaSimpleTag *tagLookup);
	void Cleanup();
};

/*********************** Main Tag Class *************************/
class MatroskaTagInfo {
	public:
		MatroskaTagInfo();
		~MatroskaTagInfo();
		//Merge another tag info data, leaving this tag left
		void MergeWithThisTagInfo(MatroskaTagInfo *source_tags);
				
		//Functions to easily get all of the tags
		//Returns the tag name of the tag index passed
		static char *GetTagListStr(uint32 tag_index);
		//Converts the tag name back to the tag index number
		static uint32 GetTagListUInt(char* tag_name);
		//Get the tag name and more infomation about a tag index
		static char *MatroskaTagInfo::GetTagListStrInfo(uint32 tag_index);


		//Base Tags
		MatroskaTagUID Tag_TrackUID;
		/// The UID of the chapter this Tag applies to
		MatroskaTagUID Tag_ChapterUID;
		MatroskaTagUID Tag_AttachmentUID;

		UTFstring Tag_ArchivalLocation;
		std::vector<binary> Tag_AudioEncryption;
		double Tag_AudioGain;
		std::string Tag_AudioGenre;
		double Tag_AudioPeak;
		std::string Tag_AudioSecondaryGenre;
		UTFstring Tag_Bibliography;
		double Tag_BPM;
		uint32 Tag_CaptureDPI;
		uint32 Tag_CapturePaletteSetting;
		UTFstring Tag_Cropped;
		uint32 Tag_DiscTrack;
		UTFstring Tag_Encoder;
		UTFstring Tag_EncodeSettings;
		UTFstring Tag_File;
		std::string Tag_InitialKey;
		UTFstring Tag_Keywords;
		std::string Tag_Language;
		uint32 Tag_Length;
		UTFstring Tag_Mood;
		std::string Tag_OfficialAudioFileURL;
		std::string Tag_OfficialAudioSourceURL;
		std::string Tag_OriginalDimensions;
		UTFstring Tag_OriginalMediaType;
		uint32 Tag_PlayCounter;
		uint32 Tag_PlaylistDelay;
		int32 Tag_Popularimeter;
		UTFstring Tag_Product;
		std::vector<binary> Tag_Rating;
		std::string Tag_RecordLocation;
		uint32 Tag_SetPart;
		UTFstring Tag_Source;
		UTFstring Tag_SourceForm;
		std::string Tag_SubGenre;
		UTFstring Tag_Subject;
		std::string Tag_SynchronisedTempo;
		UTFstring Tag_SynchronisedText;
		UTFstring Tag_UnsynchronisedText;
		std::string Tag_UserDefinedURL;
		std::vector<binary> Tag_VideoGenre;
		std::vector<binary> Tag_VideoSecondaryGenre;
		
		//The amazing Multi-* tags
		//Smart Multi-Comment Item Add
		void AddMultiCommentItem(MatroskaTagMultiCommentItem *new_item);
		MatroskaTagMultiComment *Tag_MultiComments;
		//Smart Multi-Commercia Item Add
		void AddMultiCommercialItem(MatroskaTagMultiCommercialItem *new_item);
		MatroskaTagMultiCommercial *Tag_MultiCommercials;
		//Smart Multi-Date Item Add
		void AddMultiDateItem(MatroskaTagMultiDateItem *new_item);
		MatroskaTagMultiDate *Tag_MultiDates;
		//Smart Multi-Entity Item Add
		void AddMultiEntityItem(MatroskaTagMultiEntityItem *new_item);
		MatroskaTagMultiEntity *Tag_MultiEntities;
		//Smart Multi-Identifier Item Add
		void AddMultiIdentifierItem(MatroskaTagMultiIdentifierItem *new_item);
		MatroskaTagMultiIdentifier *Tag_MultiIdentifiers;
		//Smart Multi-Legal Item Add
		void AddMultiLegalItem(MatroskaTagMultiLegalItem *new_item);
		MatroskaTagMultiLegal	*Tag_MultiLegals;
		//Smart Multi-Title Item Add
		void AddMultiTitleItem(MatroskaTagMultiTitleItem *new_item);
		MatroskaTagMultiTitle *Tag_MultiTitles;
		
		//This really shouldn't be here
		//UTFstring Tag_MuxingApp;  //Now it isn't :P
		//This really shoudn't be here
		//UTFstring Tag_WritingApp; //Now it isn't :P

		//Pointers to the correct Multi-* Tag element text
		//Pointer to the Multi-Title field for the ID3 Title Field
		UTFstring *Tag_Simple_Title;
		//Pointer to the Multi-Entity field for ID3 Artist Field
		UTFstring *Tag_Simple_Artist;
		//Pointer to the Multi-Title field for ID3 Album Field
		UTFstring *Tag_Simple_Album;

		MatroskaSimpleTagList Tag_Simple_Tags;
};

class MatroskaTagList : public std::vector<MatroskaTagInfo *> {
public:
	MatroskaTagList();
	~MatroskaTagList();

	UTFstring &FindSimpleTag(const char *name);
	/// Find a tag entry with the assigned track uid
	/// \param bOnly If true the returned tag will only be assigned to the track uid, no other uids
	/// \return NULL If not found
	MatroskaTagInfo *FindByTrackUID(uint64 uid, bool bOnly = false);

	void Remove(MatroskaTagInfo *tag);
};

#ifdef USE_NEW_MKV_TAG_SYSTEM

enum MTag_TagTypes
{
	MTag_None = 0,
	MTag_General = 100,
	MTag_General_MultiComment,
	MTag_General_MultiCommercial,
	MTag_General_MultiDate,
	MTag_General_MultiEntity,
	MTag_General_MultiIdentifier,
	MTag_General_MultiLegal,
	MTag_General_MultiTitle,
	MTag_General_Subject,//UTF-8
	MTag_General_Bibliography,//UTF-8
	MTag_General_Language,//string
	MTag_General_Rating,//binary
	MTag_General_Encoder,//UTF-8
	MTag_General_EncodeSettings,//UTF-8
	MTag_General_File,//UTF-8
	MTag_General_ArchivalLocation,//UTF-8
	MTag_General_RecordLocation,//string
	MTag_General_Keywords,//UTF-8
	MTag_General_Mood,//UTF-8
	MTag_General_Source, //UTF-8
	MTag_General_SourceForm, //UTF-8
	MTag_General_Product,  	//UTF-8
	MTag_General_OriginalMediaType, //UTF-8
	MTag_General_PlayCounter, //u-integer
	MTag_General_Popularimeter, //integer
	MTag_Genres = 2000,
	MTag_Genres_AudioGenre, //string 	The genre of the audio. (same as the TCON tag in ID3).
	MTag_Genres_VideoGenre, //binary 	Genre of the video using a 4 byte ID. More info about the content on the older page.
	MTag_Genres_SubGenre, //string
	MTag_AudioSpecific = 3000,
	MTag_AudioSpecific_AudioEncryption,  	//binary 	This is an exact dump of the AENC tag in ID3v2.4. It should never be used otherwise.
	MTag_AudioSpecific_AudioGain, 	//float 	The gain to apply to reach 89db SPL on playback.
	MTag_AudioSpecific_AudioPeak,	//float 	The maximum absolute peak value of the item.
	MTag_AudioSpecific_BPM, 	//float 	Contains the number of beats per minute in the main part of the audio. This is general. (same as the TBPM tag in ID3).
	MTag_AudioSpecific_Equalisation, 	//binary 	This is an exact dump of the EQU2 tag in ID3v2.4. It should never be used otherwise.
	MTag_AudioSpecific_DiscTrack, 	//u-integer 	Track number in the original CD. (same as the TRCK tag in ID3).
	MTag_AudioSpecific_SetPart, 	//u-integer 	The total number of tracks on a disc. (same as the TPOS tag in ID3).
	MTag_AudioSpecific_InitialKey,//string 	The initial key that a musical track starts in. (same as the TKEY tag in ID3).
	MTag_AudioSpecific_OfficialFileURL, 	//string 	A URL pointing to the official webpage for this item. (same as the WOAF tag in ID3).
	MTag_AudioSpecific_OfficialSourceURL, //string
	MTag_ImageSpecific = 4000,
	MTag_ImageSpecific_CaptureDPI, //u-integer  	Stores dots per inch setting of the digitizer used to produce the file, such as "300." (same as the IDPI tag in RIFF).
	MTag_ImageSpecific_CaptureLightness, //Binary 	Describes the changes in lightness settings on the digitizer required to produce the file. Note that the format of this information depends on hardware used. (same as the ILGT tag in RIFF).
	MTag_ImageSpecific_CapturePaletteSetting, //u-integer 	Specifies the number of colors requested when digitizing an image, such as "256." (same as the IPLT tag in RIFF).
	MTag_ImageSpecific_CaptureSharpness, //Binary 	Identifies the changes in sharpness for the digitizer required to produce the file (the format depends on the hardware used). (same as the ISHP tag in RIFF).
	MTag_ImageSpecific_Cropped, //string 	Describes whether an image has been cropped and, if so, how it was cropped. For example, "lower right corner." (same as the ICRP tag in RIFF).
	MTag_ImageSpecific_OriginalDimensions, //string
};

/// This is the base class for tags
class MTagBase
{
public:
	enum MTag_TagTypes type;
};

class MatroskaTagItem
{
public:
	MatroskaTagItem();
	~MatroskaTagItem();

	/// Used Internally
	MTag_TagTypes tag_type;

	UTFstring *UTF_data;
	std::string *string_data;
	uint64 *uinteger_data;
	int64 *integer_data;
	double *float_data;
	binary *binary_data;
	uint64 binary_data_len;

	//The amazing Multi-* tags
	MatroskaTagMultiComment *MultiComment_data;
	MatroskaTagMultiCommercial *MultiCommercial_data;
	MatroskaTagMultiDate *MultiDate_data;
	MatroskaTagMultiEntity *MultiEntity_data;
	MatroskaTagMultiIdentifier *MultiIdentifier_data;
	MatroskaTagMultiLegal	*MultiLegal_data;
	MatroskaTagMultiTitle *MultiTitle_data;
};

class MatroskaTagList
{
public:
	MatroskaTagList();
	~MatroskaTagList();
	void AddItem(MatroskaTagItem *new_item);
	int DeleteItem(MatroskaTagItem *new_item);
	int DeleteItem(int requested_index);
	MatroskaTagItem *GetItem(int requested_index);
	int GetCount();
	MatroskaTagItem *FindTrackItemWithType(int type_id);
protected:
  ///Pointer to the first item in the linked list
	MatroskaTagItem *first_item;
	MatroskaTagItem *last_item;
};

#endif // USE_NEW_MKV_TAG_SYSTEM

}; //End namespace MatroskaUtilsNamespace

#endif // _MATROSKA_TAG_DATA_H_
