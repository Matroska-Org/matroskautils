/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  AviTagReader.cpp
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
    \file AviTagReader.cpp
		\version \$Id: AviTagReader.cpp,v 1.3 2004/03/20 05:08:30 jcsston Exp $
    \brief A AVI Tag Importer
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#include "AviTagReader.h"

using namespace MatroskaUtilsNamespace;
using namespace LIBMATROSKA_NAMESPACE;

AviTagReader::AviTagReader(const JString &filename) {	
	ReadFile(filename);
};

void AviTagReader::ReadFile(const JString &filename) {
	m_AviFilename = filename;

	MatroskaUilsFileReader inputFile(m_AviFilename.t_str(), MODE_READ);
	char header[5];
	header[4] = NULL;
	char header2[5];
	header2[4] = NULL;
	DWORD size = 0;
	
	inputFile.setFilePointer(0);
	inputFile.read(header, 4);
	if (header[0] == 'R' 
		&& header[1] == 'I' 
		&& header[2] == 'F'
		&& header[3] == 'F')
	{
		inputFile.read(&size, 4);
		inputFile.read(header2, 4);
		if (header2[0] == 'A' 
			&& header2[1] == 'V' 
			&& header2[2] == 'I'
			&& header2[3] == ' ')
		{
			// Dealing with an AVI file
			bool bEOF = false;
			while (!bEOF) {				
				if (inputFile.read(header, 4) == 0) {
					bEOF = true;
					continue;
				}

				inputFile.read(&size, 4);
				if (header[0] == 'L' 
					&& header[1] == 'I' 
					&& header[2] == 'S'
					&& header[3] == 'T'
					&& size > 4)
				{
					inputFile.read(header2, 4);
					// LIST - INFO chunk?
					if (header2[0] == 'I' 
						&& header2[1] == 'N' 
						&& header2[2] == 'F'
						&& header2[3] == 'O')
					{
						uint64 tagStartPos = inputFile.getFilePointer();
						char tagHeader[5];
						tagHeader[4] = NULL;
						DWORD tagSize = 0;
						AviTag newTag;
						while (inputFile.getFilePointer()+8 < tagStartPos+size) {							
							inputFile.read(tagHeader, 4);
							
							inputFile.read(&tagSize, 4);
							
							newTag.id = tagHeader;
							
							newTag.value.resize(tagSize);
							inputFile.read((void *)newTag.value.c_str(), tagSize);

							// Read until past the NULLs
							char letter = NULL;							
							while (letter == NULL)
								inputFile.read(&letter, 1);
							inputFile.setFilePointer(-1, seek_current);

							m_Tags.push_back(newTag);
						}
					} else if (header2[0] == 'M' 
						&& header2[1] == 'I' 
						&& header2[2] == 'D'
						&& header2[3] == ' ')
					{
						uint64 tagStartPos = inputFile.getFilePointer();
						char tagHeader[5];
						tagHeader[4] = NULL;
						DWORD tagSize = 0;
						AviTag newTag;
						while (inputFile.getFilePointer()+8 < tagStartPos+size) {							
							inputFile.read(tagHeader, 4);
							
							inputFile.read(&tagSize, 4);
							
							newTag.id = tagHeader;
							
							newTag.value.resize(tagSize);
							inputFile.read((void *)newTag.value.c_str(), tagSize);
							
							// Read past any NULLs
							char letter = NULL;							
							while (letter == NULL)
								inputFile.read(&letter, 1);
							inputFile.setFilePointer(-1, seek_current);							
							
							m_Tags.push_back(newTag);
						}
					} else {
						// Skip this chunk
						inputFile.setFilePointer(size-4, seek_current);
					}
				} else {
					inputFile.setFilePointer(size, seek_current);
				}
			} // while (!bEOF) {				
		}
	}
	// Try reading iDivx Tags
	inputFile.setFilePointer(-128, seek_end);
	inputFile.read(&m_DivxTag, sizeof(iDivxTag));	
};

bool AviTagReader::HasTags() {
	if (m_Tags.size() > 0)
		return true;

	return false;
};

void AviTagReader::ImportTags(MatroskaTagInfo *target) {
	for (size_t t = 0; t < m_Tags.size(); t++) {
		AviTag &tag = m_Tags.at(t);
		
		if (!stricmp(tag.id.c_str(), "IARL")) {
			target->Tag_ArchivalLocation = JString(tag.value.c_str()).c_str();
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ICMT") 
						|| !stricmp(tag.id.c_str(), "COMM")) 
		{
			MatroskaTagMultiCommentItem *commentItem = new MatroskaTagMultiCommentItem();
			commentItem->Comments = JString(tag.value.c_str()).c_str();
			target->AddMultiCommentItem(commentItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ICRP")) {
			target->Tag_Cropped = JString(tag.value.c_str()).c_str();
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IDIM")) {	
			target->Tag_OriginalDimensions = tag.value.c_str();
			tag.id = "";		
		} else if (!stricmp(tag.id.c_str(), "IART")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_OriginalArtistPerformer;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "DIRC")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_Director;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IPRO")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_ProducedBy;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ICNM")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_Cinematographer;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IPDS")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_ProductionDesigner;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ICDS")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_CostumeDesigner;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ISTD")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_ProductionStudio;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IDST")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_DistributedBy;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ICMS")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_CommissionedBy;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IENG")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_Engineer;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IEDT")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_EditedBy;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ITCH")
						|| !stricmp(tag.id.c_str(), "CODE")) 
		{			
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_EncodedBy;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IRIP")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_RippedBy;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IWRI")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_LyricistTextWriter;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IMUS")) {
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_Composer;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ISTR")
						|| !stricmp(tag.id.c_str(), "STAR")) 
		{			
			MatroskaTagMultiEntityItem *entityItem = new MatroskaTagMultiEntityItem();
			entityItem->Type = KaxTagMultiEntitiesType_LeadPerformerSoloist;
			entityItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiEntityItem(entityItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IGNR")
						|| !stricmp(tag.id.c_str(), "GENR")) 
		{	
			target->Tag_AudioGenre = tag.value.c_str();
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IKEY")) {	
			target->Tag_Keywords = JString(tag.value.c_str()).c_str();
			tag.id = "";		
		} else if (!stricmp(tag.id.c_str(), "ILNG")
						|| !stricmp(tag.id.c_str(), "LANG")) 
		{	
			target->Tag_Language = tag.value.c_str();
			tag.id = "";		
		} else if (!stricmp(tag.id.c_str(), "IKEY")) {	
			target->Tag_Keywords = JString(tag.value.c_str()).c_str();
			tag.id = "";		
		} else if (!stricmp(tag.id.c_str(), "IMED")) {	
			target->Tag_OriginalMediaType = JString(tag.value.c_str()).c_str();
			tag.id = "";		
		} else if (!stricmp(tag.id.c_str(), "ICRD")
						|| !stricmp(tag.id.c_str(), "YEAR")) 
		{			
			MatroskaTagMultiDateItem *dateItem = new MatroskaTagMultiDateItem();
			dateItem->Type = KaxTagMultiDateType_RecordingDate;
			dateItem->DateBegin = (time_t)GetDate(tag.value);
			target->AddMultiDateItem(dateItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IDIT")) {			
			MatroskaTagMultiDateItem *dateItem = new MatroskaTagMultiDateItem();
			dateItem->Type = KaxTagMultiDateType_DigitizingDate;
			dateItem->DateBegin = (time_t)GetDate(tag.value);
			target->AddMultiDateItem(dateItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "ICOP")) {			
			MatroskaTagMultiLegalItem *legalItem = new MatroskaTagMultiLegalItem();
			legalItem->Type = KaxTagMultiLegalType_Copyright;
			legalItem->Text = JString(tag.value.c_str()).c_str();
			target->AddMultiLegalItem(legalItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "INAM")
						|| !stricmp(tag.id.c_str(), "TITL")) 
		{			
			MatroskaTagMultiTitleItem *titleItem = new MatroskaTagMultiTitleItem();
			titleItem->Type = KaxTagMultiTitleType_AlbumMovieShowTitle;
			titleItem->Name = JString(tag.value.c_str()).c_str();
			target->AddMultiTitleItem(titleItem);
			tag.id = "";
		} else if (!stricmp(tag.id.c_str(), "IDPI")) {	
			target->Tag_CaptureDPI = atol(tag.value.c_str());
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "IPLT")) {	
			target->Tag_CapturePaletteSetting = atol(tag.value.c_str());
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "IPRD")) {	
			target->Tag_Product = JString(tag.value.c_str()).c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "IRTD")
						|| !stricmp(tag.id.c_str(), "AGES")) 
		{	
			//target->Tag_Rating = JString(tag.value.c_str()).c_str();
			//tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "ICNT")) {	
			target->Tag_RecordLocation = tag.value.c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "ISGN")) {	
			target->Tag_SubGenre = tag.value.c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "IFRM")
						|| !stricmp(tag.id.c_str(), "PRT2")) 
		{	

			target->Tag_SetPart = atol(tag.value.c_str());
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "ISFT")) {	
			target->Tag_Encoder = JString(tag.value.c_str()).c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "ISRC")) {	
			target->Tag_Source = JString(tag.value.c_str()).c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "ISRF")) {	
			target->Tag_SourceForm = JString(tag.value.c_str()).c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "ISBJ")) {	
			target->Tag_Subject = JString(tag.value.c_str()).c_str();
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "IPRT")) {	
			target->Tag_DiscTrack = atol(tag.value.c_str());
			tag.id = "";	
		} else if (!stricmp(tag.id.c_str(), "IWEB")) {	
			target->Tag_UserDefinedURL = tag.value.c_str();
			tag.id = "";
		}
	}
};

int64 AviTagReader::GetDate(const std::string &dateStr) {
	int64 dateNumber;		
	tm dateStruct;
	memset(&dateStruct, 0, sizeof(tm));
	
	dateStruct.tm_year = atol(dateStr.c_str());
	
	dateNumber = mktime(&dateStruct);	

	return dateNumber;
};
