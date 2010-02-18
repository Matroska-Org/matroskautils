/**
	*  Part of The Matroska Shell Extension and The TCMP Matroska CDL
	*
	*  MatroskaUtilsTag.cpp
	*
	*  Copyright (C) Jory Stone - June 2003
	*  Copyright (C) John Cannon - 2003
	*
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
	\file MatroskaTag.cpp
	\version \$Id: MatroskaUtilsTag.cpp,v 1.14 2004/03/08 07:56:51 jcsston Exp $
	\brief Some general code to make working with Matroska easier, this file has the bulk of the tag code
	\author Jory Stone     <jcsston @ toughguy.net>
	\author John Cannon    <spyder482 @ yahoo.com>
*/

#include "MatroskaUtils.h"

using namespace MatroskaUtilsNamespace;
namespace MatroskaUtilsNamespace {

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

int MatroskaInfoParser::Parse_Tags(EbmlStream *inputfile_stream, EbmlElement *tags_element) {
	ODS("MatroskaInfoParser::Parse_Tags()");
	try {
		if ((inputfile_stream == NULL) || (tags_element == NULL))
			return -1;

		EbmlElement *l2 = NULL;
		EbmlElement *l3 = NULL;
		EbmlElement *l4 = NULL;
		EbmlElement *l5 = NULL;

		// Patch from Zen : for Borland C++ Builder, "Goto bypasses initialization of a local variable"
		// That error message is strange, I never use goto in any of my code
		uint64 tag_type4 = 0;
		uint64 tag_type5 = 0;
		MatroskaTagInfo *currentTags = NULL;
#ifndef MATROSKA_INFO
		MatroskaTagMultiCommercialItem *new_MultiCommercialItem = NULL;
		MatroskaTagMultiCommentItem *new_MultiCommentItem = NULL;
		MatroskaTagMultiTitleItem *new_MultiTitleItem = NULL;
		MatroskaTagMultiDateItem *new_MultiDateItem = NULL;
		MatroskaTagMultiIdentifierItem *new_MultiIdentifierItem = NULL;
		MatroskaTagMultiEntityItem *new_MultiEntityItem = NULL;
#endif
		MatroskaSimpleTag *newSimpleTag = NULL;
		//End of Patch, but modifications was made in the rests of code to handle modifications

		int UpperElementLevel = 0;

		/// Has the number of tag elements found
		int number_of_tags = 0;

		//Let's store the tags element info real quick
		m_tagElementPos = tags_element->GetElementPosition();
		m_tagElementSize = tags_element->GetSize() + tags_element->HeadSize();

		l2 = inputfile_stream->FindNextElement(tags_element->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
		while (l2 != NULL) {
			if (UpperElementLevel > 0) {
				break;
			} else if (UpperElementLevel < 0) {
				UpperElementLevel = 0;
			}
			if (EbmlId(*l2) == KaxTag::ClassInfos.GlobalId) {
				// We actually found a tag :) We're happy now.
				really_found_tags = true;
				//We found another :)
				number_of_tags++;
				//matroskaTagTrack = this->GetGlobalTrackInfo();
				currentTags = new MatroskaTagInfo();

				l3 = inputfile_stream->FindNextElement(l2->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
				while (l3 != NULL) {
					if (UpperElementLevel > 0) {
						break;
					}
					if (UpperElementLevel < 0) {
						UpperElementLevel = 0;
					}

					if (EbmlId(*l3) == KaxTagTargets::ClassInfos.GlobalId) {

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							// Now evaluate the data belonging to this track
							if (EbmlId(*l4) == KaxTagTrackUID::ClassInfos.GlobalId) {
								KaxTagTrackUID &tuid = *static_cast<KaxTagTrackUID *>(l4);
								tuid.ReadData(inputfile_stream->I_O());
								currentTags->Tag_TrackUID.Add(uint64(tuid));

							} else if (EbmlId(*l4) == KaxTagChapterUID::ClassInfos.GlobalId) {
								KaxTagChapterUID &tChapterUID = *static_cast<KaxTagChapterUID *>(l4);
								tChapterUID.ReadData(inputfile_stream->I_O());
								currentTags->Tag_ChapterUID.Add(uint64(tChapterUID));

							} else if (EbmlId(*l4) == KaxTagAttachmentUID::ClassInfos.GlobalId) {
								KaxTagAttachmentUID &tuid = *static_cast<KaxTagAttachmentUID *>(l4);
								tuid.ReadData(inputfile_stream->I_O());
								currentTags->Tag_AttachmentUID.Add((uint64)tuid);
							}
							if (UpperElementLevel > 0) {
								assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						// Read the new simple tags
					} else if (EbmlId(*l3) == KaxTagSimple::ClassInfos.GlobalId) {
						KaxTagSimple *simpleTag = static_cast<KaxTagSimple *>(l3);
						simpleTag->Read(static_cast<EbmlStream &>(*inputfile_stream), KaxTagSimple::ClassInfos.Context, UpperElementLevel, l4, false);

						newSimpleTag = new MatroskaSimpleTag();

						unsigned int Index0;
						for (Index0 = 0; Index0 < simpleTag->ListSize(); Index0++) {
							if ((*simpleTag)[Index0]->Generic().GlobalId == KaxTagName::ClassInfos.GlobalId) {
								KaxTagName &simpleTagName = *static_cast<KaxTagName *>((*simpleTag)[Index0]);

								newSimpleTag->name = wcsupr((wchar_t *)UTFstring(simpleTagName).c_str());

							} else if ((*simpleTag)[Index0]->Generic().GlobalId == KaxTagString::ClassInfos.GlobalId) {
								KaxTagString &simpleTagString = *static_cast<KaxTagString *>((*simpleTag)[Index0]);

								newSimpleTag->value = simpleTagString;
							}
						}
								
						currentTags->Tag_Simple_Tags.push_back(newSimpleTag);
#ifndef MATROSKA_INFO						
						//Read the General section
					} else if (EbmlId(*l3) == KaxTagGeneral::ClassInfos.GlobalId) {
						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							//Group
							if (EbmlId(*l4) == KaxTagSubject::ClassInfos.GlobalId) {
								KaxTagSubject &tSubject = *static_cast<KaxTagSubject *>(l4);
								tSubject.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Subject = *static_cast<EbmlUnicodeString *>(&tSubject);

							} else if (EbmlId(*l4) == KaxTagBibliography::ClassInfos.GlobalId) {
								KaxTagBibliography &tBibliography = *static_cast<KaxTagBibliography *>(l4);
								tBibliography.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Bibliography = *static_cast<EbmlUnicodeString *>(&tBibliography);

							} else if (EbmlId(*l4) == KaxTagLanguage::ClassInfos.GlobalId) {
								KaxTagLanguage &tLanguage = *static_cast<KaxTagLanguage *>(l4);
								tLanguage.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Language = *static_cast<EbmlString *>(&tLanguage);

							} else if (EbmlId(*l4) == KaxTagEncoder::ClassInfos.GlobalId) {
								KaxTagEncoder &tEncoder = *static_cast<KaxTagEncoder *>(l4);
								tEncoder.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Encoder = *static_cast<EbmlUnicodeString *>(&tEncoder);

							} else if (EbmlId(*l4) == KaxTagEncodeSettings::ClassInfos.GlobalId) {
								KaxTagEncodeSettings &tEncodeSettings = *static_cast<KaxTagEncodeSettings *>(l4);
								tEncodeSettings.ReadData(inputfile_stream->I_O());
								currentTags->Tag_EncodeSettings = *static_cast<EbmlUnicodeString *>(&tEncodeSettings);

							} else if (EbmlId(*l4) == KaxTagFile::ClassInfos.GlobalId) {
								KaxTagFile &tFile = *static_cast<KaxTagFile *>(l4);
								tFile.ReadData(inputfile_stream->I_O());
								currentTags->Tag_File = *static_cast<EbmlUnicodeString *>(&tFile);

							} else if (EbmlId(*l4) == KaxTagKeywords::ClassInfos.GlobalId) {
								KaxTagKeywords &tKeywords = *static_cast<KaxTagKeywords *>(l4);
								tKeywords.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Keywords = *static_cast<EbmlUnicodeString *>(&tKeywords);

								// Locations
							} else if (EbmlId(*l4) == KaxTagRecordLocation::ClassInfos.GlobalId) {
								KaxTagRecordLocation &tRecordLocation = *static_cast<KaxTagRecordLocation *>(l4);
								tRecordLocation.ReadData(inputfile_stream->I_O());
								currentTags->Tag_RecordLocation = *static_cast<EbmlString *>(&tRecordLocation);

							} else if (EbmlId(*l4) == KaxTagArchivalLocation::ClassInfos.GlobalId) {
								KaxTagArchivalLocation &tArchivalLocation = *static_cast<KaxTagArchivalLocation *>(l4);
								tArchivalLocation.ReadData(inputfile_stream->I_O());
								currentTags->Tag_ArchivalLocation = *static_cast<EbmlUnicodeString *>(&tArchivalLocation);

							} else if (EbmlId(*l4) == KaxTagSource::ClassInfos.GlobalId) {
								KaxTagSource &tSource = *static_cast<KaxTagSource *>(l4);
								tSource.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Source = *static_cast<EbmlUnicodeString *>(&tSource);

							} else if (EbmlId(*l4) == KaxTagSourceForm::ClassInfos.GlobalId) {
								KaxTagSourceForm &tSourceForm = *static_cast<KaxTagSourceForm *>(l4);
								tSourceForm.ReadData(inputfile_stream->I_O());
								currentTags->Tag_SourceForm = *static_cast<EbmlUnicodeString *>(&tSourceForm);

							} else if (EbmlId(*l4) == KaxTagOriginalMediaType::ClassInfos.GlobalId) {
								KaxTagOriginalMediaType &tOriginalMediaType = *static_cast<KaxTagOriginalMediaType *>(l4);
								tOriginalMediaType.ReadData(inputfile_stream->I_O());
								currentTags->Tag_OriginalMediaType = *static_cast<EbmlUnicodeString *>(&tOriginalMediaType);

							} else if (EbmlId(*l4) == KaxTagProduct::ClassInfos.GlobalId) {
								KaxTagProduct &tProduct = *static_cast<KaxTagProduct *>(l4);
								tProduct.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Product = *static_cast<EbmlUnicodeString *>(&tProduct);

								// Ratings and popularimeter type tags
							} else if (EbmlId(*l4) == KaxTagMood::ClassInfos.GlobalId) {
								KaxTagMood &tMood = *static_cast<KaxTagMood *>(l4);
								tMood.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Mood = *static_cast<EbmlUnicodeString *>(&tMood);

							} else if (EbmlId(*l4) == KaxTagPlayCounter::ClassInfos.GlobalId) {
								KaxTagPlayCounter &tPlayCounter = *static_cast<KaxTagPlayCounter *>(l4);
								tPlayCounter.ReadData(inputfile_stream->I_O());
								currentTags->Tag_PlayCounter = uint32(tPlayCounter);

							} else if (EbmlId(*l4) == KaxTagPopularimeter::ClassInfos.GlobalId) {
								KaxTagPopularimeter &tPopularimeter = *static_cast<KaxTagPopularimeter *>(l4);
								tPopularimeter.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Popularimeter = int32(tPopularimeter);

							} else if (EbmlId(*l4) == KaxTagRating::ClassInfos.GlobalId) {
								KaxTagRating &tRating = *static_cast<KaxTagRating *>(l4);
								tRating.ReadData(inputfile_stream->I_O(), SCOPE_ALL_DATA);
								//This is binary data
								//currentTags->Tag_Rating = *static_cast<EbmlBinary *>(&tRating);
							}

							if (UpperElementLevel > 0) {
								assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						//Read the Genres section
					} else if (EbmlId(*l3) == KaxTagGenres::ClassInfos.GlobalId) {
						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							//Group
							//Genre and catgory related tags
							if (EbmlId(*l4) == KaxTagAudioGenre::ClassInfos.GlobalId) {
								KaxTagAudioGenre &tAudioGenre = *static_cast<KaxTagAudioGenre *>(l4);
								tAudioGenre.ReadData(inputfile_stream->I_O());
								currentTags->Tag_AudioGenre = *static_cast<EbmlString *>(&tAudioGenre);

							} else if (EbmlId(*l4) == KaxTagSubGenre::ClassInfos.GlobalId) {
								KaxTagSubGenre &tSubGenre = *static_cast<KaxTagSubGenre *>(l4);
								tSubGenre.ReadData(inputfile_stream->I_O());
								currentTags->Tag_SubGenre = *static_cast<EbmlString *>(&tSubGenre);

							} else if (EbmlId(*l4) == KaxTagVideoGenre::ClassInfos.GlobalId) {
								KaxTagVideoGenre &tVideoGenre = *static_cast<KaxTagVideoGenre *>(l4);
								tVideoGenre.ReadData(inputfile_stream->I_O(), SCOPE_ALL_DATA);
								//More binary data
								//currentTags->Tag_SubGenre = *static_cast<EbmlString *>(&tVideoGenre);

							}

							if (UpperElementLevel > 0) {
								assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						//Read the AudioSpecific section
					} else if (EbmlId(*l3) == KaxTagAudioSpecific::ClassInfos.GlobalId) {
						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							//Group
							//Audio volume related tags
							if (EbmlId(*l4) == KaxTagAudioGain::ClassInfos.GlobalId) {
								KaxTagAudioGain &tAudioGain = *static_cast<KaxTagAudioGain *>(l4);
								tAudioGain.ReadData(inputfile_stream->I_O());
								currentTags->Tag_AudioGain = float(tAudioGain);

							} else if (EbmlId(*l4) == KaxTagAudioPeak::ClassInfos.GlobalId) {
								KaxTagAudioPeak &tAudioPeak = *static_cast<KaxTagAudioPeak *>(l4);
								tAudioPeak.ReadData(inputfile_stream->I_O());
								currentTags->Tag_AudioPeak = float(tAudioPeak);

							} else if (EbmlId(*l4) == KaxTagBPM::ClassInfos.GlobalId) {
								KaxTagBPM &tBPM = *static_cast<KaxTagBPM *>(l4);
								tBPM.ReadData(inputfile_stream->I_O());
								currentTags->Tag_BPM = float(tBPM);

							} else if (EbmlId(*l4) == KaxTagDiscTrack::ClassInfos.GlobalId) {
								KaxTagDiscTrack &tDiscTrack = *static_cast<KaxTagDiscTrack *>(l4);
								tDiscTrack.ReadData(inputfile_stream->I_O());
								currentTags->Tag_DiscTrack = uint32(tDiscTrack);

							} else if (EbmlId(*l4) == KaxTagSetPart::ClassInfos.GlobalId) {
								KaxTagSetPart &tSetPart = *static_cast<KaxTagSetPart *>(l4);
								tSetPart.ReadData(inputfile_stream->I_O());
								currentTags->Tag_SetPart = uint32(tSetPart);

							} else if (EbmlId(*l4) == KaxTagInitialKey::ClassInfos.GlobalId) {
								KaxTagInitialKey &tInitialKey = *static_cast<KaxTagInitialKey *>(l4);
								tInitialKey.ReadData(inputfile_stream->I_O());
								currentTags->Tag_InitialKey = *static_cast<EbmlString *>(&tInitialKey);

							} else if (EbmlId(*l4) == KaxTagOfficialAudioFileURL::ClassInfos.GlobalId) {
								KaxTagOfficialAudioFileURL &tOfficialAudioFileURL = *static_cast<KaxTagOfficialAudioFileURL *>(l4);
								tOfficialAudioFileURL.ReadData(inputfile_stream->I_O());
								currentTags->Tag_OfficialAudioFileURL = *static_cast<EbmlString *>(&tOfficialAudioFileURL);

							} else if (EbmlId(*l4) == KaxTagOfficialAudioSourceURL::ClassInfos.GlobalId) {
								KaxTagOfficialAudioSourceURL &tOfficialAudioSourceURL = *static_cast<KaxTagOfficialAudioSourceURL *>(l4);
								tOfficialAudioSourceURL.ReadData(inputfile_stream->I_O());
								currentTags->Tag_OfficialAudioSourceURL = *static_cast<EbmlString *>(&tOfficialAudioSourceURL);
							}

							if (UpperElementLevel > 0) {
								assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						//Read the ImageSpecific section
					} else if (EbmlId(*l3) == KaxTagImageSpecific::ClassInfos.GlobalId) {
						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							//Group
							if (EbmlId(*l4) == KaxTagCaptureDPI::ClassInfos.GlobalId) {
								KaxTagCaptureDPI &tCaptureDPI = *static_cast<KaxTagCaptureDPI *>(l4);
								tCaptureDPI.ReadData(inputfile_stream->I_O());
								currentTags->Tag_CaptureDPI = uint32(tCaptureDPI);

							} else if (EbmlId(*l4) == KaxTagCapturePaletteSetting::ClassInfos.GlobalId) {
								KaxTagCapturePaletteSetting &tCapturePaletteSetting = *static_cast<KaxTagCapturePaletteSetting *>(l4);
								tCapturePaletteSetting.ReadData(inputfile_stream->I_O());
								currentTags->Tag_CapturePaletteSetting = uint32(tCapturePaletteSetting);

							} else if (EbmlId(*l4) == KaxTagCropped::ClassInfos.GlobalId) {
								KaxTagCropped &tCropped = *static_cast<KaxTagCropped *>(l4);
								tCropped.ReadData(inputfile_stream->I_O());
								currentTags->Tag_Cropped = *static_cast<EbmlUnicodeString *>(&tCropped);

							} else if (EbmlId(*l4) == KaxTagOriginalDimensions::ClassInfos.GlobalId) {
								KaxTagOriginalDimensions &tOriginalDimensions = *static_cast<KaxTagOriginalDimensions *>(l4);
								tOriginalDimensions.ReadData(inputfile_stream->I_O());
								currentTags->Tag_OriginalDimensions = *static_cast<EbmlString *>(&tOriginalDimensions);
							}

							if (UpperElementLevel > 0) {
								assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

					} else if (EbmlId(*l3) == KaxTagPlaylistDelay::ClassInfos.GlobalId) {
						KaxTagPlaylistDelay &tPlaylistDelay = *static_cast<KaxTagPlaylistDelay *>(l3);
						tPlaylistDelay.ReadData(inputfile_stream->I_O());
						currentTags->Tag_PlaylistDelay = uint32(tPlaylistDelay);

					} else if (EbmlId(*l3) == KaxTagLength::ClassInfos.GlobalId) {
						KaxTagLength &tLength = *static_cast<KaxTagLength *>(l3);
						tLength.ReadData(inputfile_stream->I_O());
						currentTags->Tag_Length = uint32(tLength);

						// Lyric related tags
					} else if (EbmlId(*l3) == KaxTagUnsynchronisedText::ClassInfos.GlobalId) {
						KaxTagUnsynchronisedText &tUnsynchronisedText = *static_cast<KaxTagUnsynchronisedText *>(l3);
						tUnsynchronisedText.ReadData(inputfile_stream->I_O());
						currentTags->Tag_UnsynchronisedText = *static_cast<EbmlUnicodeString *>(&tUnsynchronisedText);

						// Millions of URLs ;)
					} else if (EbmlId(*l3) == KaxTagUserDefinedURL::ClassInfos.GlobalId) {
						KaxTagUserDefinedURL &tUserDefinedURL = *static_cast<KaxTagUserDefinedURL *>(l3);
						tUserDefinedURL.ReadData(inputfile_stream->I_O());
						currentTags->Tag_UserDefinedURL = *static_cast<EbmlString *>(&tUserDefinedURL);

						//The Multi-* tags :D
					} else if (EbmlId(*l3) == KaxTagMultiComment::ClassInfos.GlobalId) {
						//Setup Multi-Comment Tag class
						new_MultiCommentItem = new MatroskaTagMultiCommentItem();

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel,	0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}
							if (EbmlId(*l4) == KaxTagMultiCommentLanguage::ClassInfos.GlobalId) {
								KaxTagMultiCommentLanguage &tag_CommentsLanuage = *static_cast<KaxTagMultiCommentLanguage *>(l4);
								tag_CommentsLanuage.ReadData(inputfile_stream->I_O());

								new_MultiCommentItem->Language = *static_cast<EbmlString *>(&tag_CommentsLanuage);

							} else if (EbmlId(*l4) == KaxTagMultiCommentComments::ClassInfos.GlobalId) {
								KaxTagMultiCommentComments &tag_CommentComments = *static_cast<KaxTagMultiCommentComments *>(l4);
								tag_CommentComments.ReadData(inputfile_stream->I_O());

								new_MultiCommentItem->Comments = *static_cast<EbmlUnicodeString *>(&tag_CommentComments);

							} else {
								//unknown element, level 4
							}

							if (UpperElementLevel > 0) {
								assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)
						//Gathered all this element's sub-elments
						currentTags->AddMultiCommentItem(new_MultiCommentItem);
						new_MultiCommentItem = NULL;

					} else if (EbmlId(*l3) == KaxTagMultiCommercial::ClassInfos.GlobalId) {
						//Create a new item to fill out
						new_MultiCommercialItem = new MatroskaTagMultiCommercialItem();

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}
							if (EbmlId(*l4) == KaxTagCommercial::ClassInfos.GlobalId) {
								//Create a new item to fill out
								if (new_MultiCommercialItem == NULL)
									new_MultiCommercialItem = new MatroskaTagMultiCommercialItem();

								l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
								while (l5 != NULL) {
									if (UpperElementLevel > 0) {
										break;
									}
									if (UpperElementLevel < 0) {
										UpperElementLevel = 0;
									}

									if (EbmlId(*l5) == KaxTagMultiCommercialType::ClassInfos.GlobalId) {
										KaxTagMultiCommercialType &tag_MultiCommercialType = *static_cast<KaxTagMultiCommercialType *>(l5);
										tag_MultiCommercialType.ReadData(inputfile_stream->I_O());
										//Store the Type
										new_MultiCommercialItem->Type = uint16(tag_MultiCommercialType);

									} else if (EbmlId(*l5) == KaxTagMultiCommercialAddress::ClassInfos.GlobalId) {
										KaxTagMultiCommercialAddress &tag_CommercialAddress = *static_cast<KaxTagMultiCommercialAddress *>(l5);
										tag_CommercialAddress.ReadData(inputfile_stream->I_O());
										//Store the Address
										new_MultiCommercialItem->Address  = *static_cast<EbmlUnicodeString *>(&tag_CommercialAddress);

									} else if (EbmlId(*l5) == KaxTagMultiCommercialEmail::ClassInfos.GlobalId) {
										KaxTagMultiCommercialEmail &tag_CommercialEmail = *static_cast<KaxTagMultiCommercialEmail *>(l5);
										tag_CommercialEmail.ReadData(inputfile_stream->I_O());
										//Store the Email
										new_MultiCommercialItem->Email = *static_cast<EbmlString *>(&tag_CommercialEmail);

									} else if (EbmlId(*l5) == KaxTagMultiCommercialURL::ClassInfos.GlobalId) {
										KaxTagMultiCommercialURL &tag_CommercialURL = *static_cast<KaxTagMultiCommercialURL *>(l5);
										tag_CommercialURL.ReadData(inputfile_stream->I_O());
										//Store a/the web URL
										new_MultiCommercialItem->URL = *static_cast<EbmlString *>(&tag_CommercialURL);

									} else {
										//unknown element, level 5
									}

									if (UpperElementLevel > 0) {
										assert(1 == 0);	// this should never happen
									} else {
										l5->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l5->Generic().Context);
										delete l5;
										l5 = inputfile_stream->FindNextElement(l4->Generic().Context,	UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
									}
								} // while (l5 != NULL)
								//Gathered all this element's sub-elments
								currentTags->AddMultiCommercialItem(new_MultiCommercialItem);
								new_MultiCommercialItem = NULL;
							}else if (EbmlId(*l4) == KaxTagMultiCommercialType::ClassInfos.GlobalId) {
								KaxTagMultiCommercialType &tag_MultiCommercialType = *static_cast<KaxTagMultiCommercialType *>(l4);
								tag_MultiCommercialType.ReadData(inputfile_stream->I_O());
								//Store the Type
								new_MultiCommercialItem->Type = uint16(tag_MultiCommercialType);

							} else if (EbmlId(*l4) == KaxTagMultiCommercialAddress::ClassInfos.GlobalId) {
								KaxTagMultiCommercialAddress &tag_CommercialAddress = *static_cast<KaxTagMultiCommercialAddress *>(l4);
								tag_CommercialAddress.ReadData(inputfile_stream->I_O());
								//Store the Address
								new_MultiCommercialItem->Address  = *static_cast<EbmlUnicodeString *>(&tag_CommercialAddress);

							} else if (EbmlId(*l4) == KaxTagMultiCommercialEmail::ClassInfos.GlobalId) {
								KaxTagMultiCommercialEmail &tag_CommercialEmail = *static_cast<KaxTagMultiCommercialEmail *>(l4);
								tag_CommercialEmail.ReadData(inputfile_stream->I_O());
								//Store the Email
								new_MultiCommercialItem->Email = *static_cast<EbmlString *>(&tag_CommercialEmail);

							} else if (EbmlId(*l4) == KaxTagMultiCommercialURL::ClassInfos.GlobalId) {
								KaxTagMultiCommercialURL &tag_CommercialURL = *static_cast<KaxTagMultiCommercialURL *>(l4);
								tag_CommercialURL.ReadData(inputfile_stream->I_O());
								//Store a/the web URL
								new_MultiCommercialItem->URL = *static_cast<EbmlString *>(&tag_CommercialURL);

							} else {
								//unknown element, level 4
							}

							if (UpperElementLevel > 0) {
								UpperElementLevel--;
								delete l4;
								l4 = l5;
								if (UpperElementLevel > 0)
									break;
								//assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context,	UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						//Gathered all this element's sub-elments
						currentTags->AddMultiCommercialItem(new_MultiCommercialItem);
						new_MultiCommercialItem = NULL;

					} else if (EbmlId(*l3) == KaxTagMultiDate::ClassInfos.GlobalId) {
						//Create a new item to fill out
						new_MultiDateItem = new MatroskaTagMultiDateItem();

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}
							if (EbmlId(*l4) == KaxTagDate::ClassInfos.GlobalId) {
								//Create a new item to fill out
								if (new_MultiDateItem == NULL)
									new_MultiDateItem = new MatroskaTagMultiDateItem();

								l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
								while (l5 != NULL) {
									if (UpperElementLevel > 0) {
										break;
									}
									if (UpperElementLevel < 0) {
										UpperElementLevel = 0;
									}

									if (EbmlId(*l5) == KaxTagMultiDateType::ClassInfos.GlobalId) {
										KaxTagMultiDateType &tag_MultiDateType = *static_cast<KaxTagMultiDateType *>(l5);
										tag_MultiDateType.ReadData(inputfile_stream->I_O());
										//Store the Type
										new_MultiDateItem->Type = uint16(tag_MultiDateType);

									} else if (EbmlId(*l5) == KaxTagMultiDateDateBegin::ClassInfos.GlobalId) {
										KaxTagMultiDateDateBegin &tag_DateDateBegin = *static_cast<KaxTagMultiDateDateBegin *>(l5);
										tag_DateDateBegin.ReadData(inputfile_stream->I_O(), SCOPE_ALL_DATA);
										//Store the Beginning Date
										new_MultiDateItem->DateBegin = tag_DateDateBegin.GetEpochDate();

									} else if (EbmlId(*l5) == KaxTagMultiDateDateEnd::ClassInfos.GlobalId) {
										KaxTagMultiDateDateEnd &tag_DateDateEnd = *static_cast<KaxTagMultiDateDateEnd *>(l5);
										tag_DateDateEnd.ReadData(inputfile_stream->I_O(), SCOPE_ALL_DATA);
										//Store the Ending Date
										new_MultiDateItem->DateEnd = tag_DateDateEnd.GetEpochDate();

									} else {
										//unknown element, level 5
									}

									if (UpperElementLevel > 0) {
										assert(1 == 0);	// this should never happen
									} else {
										l5->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l5->Generic().Context);
										delete l5;
										l5 = inputfile_stream->FindNextElement(l4->Generic().Context,	UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
									}
								} // while (l5 != NULL)
								//Gathered all this element's sub-elments
								currentTags->AddMultiDateItem(new_MultiDateItem);
								new_MultiDateItem = NULL;
							} else {
								// I don't support the old tags, because I never wrote them before :P
								// (not to mention the id's changed)
								//unknown element, level 4
							}

							if (UpperElementLevel > 0) {
								UpperElementLevel--;
								delete l4;
								l4 = l5;
								if (UpperElementLevel > 0)
									break;
								//assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context,	UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						//Gathered all this element's sub-elments
						currentTags->AddMultiDateItem(new_MultiDateItem);
						new_MultiDateItem = NULL;

					} else if (EbmlId(*l3) == KaxTagMultiIdentifier::ClassInfos.GlobalId) {
						//Setup Multi-Identifier Tag Item class
						new_MultiIdentifierItem = new MatroskaTagMultiIdentifierItem();

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							if (EbmlId(*l4) == KaxTagIdentifier::ClassInfos.GlobalId) {
								//Setup Multi-Identifier Tag Item class
								if (new_MultiIdentifierItem == NULL) {
									new_MultiIdentifierItem = new MatroskaTagMultiIdentifierItem();
								}

								l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
								while (l5 != NULL) {
									if (UpperElementLevel > 0) {
										break;
									}
									if (UpperElementLevel < 0) {
										UpperElementLevel = 0;
									}

									if (EbmlId(*l5) == KaxTagMultiIdentifierType::ClassInfos.GlobalId) {
										KaxTagMultiIdentifierType &tag_MultiIdentifierType = *static_cast<KaxTagMultiIdentifierType *>(l5);
										tag_MultiIdentifierType.ReadData(inputfile_stream->I_O());

										new_MultiIdentifierItem->Type = uint16(tag_MultiIdentifierType);

									} else if (EbmlId(*l5) == KaxTagMultiIdentifierString::ClassInfos.GlobalId) {
										KaxTagMultiIdentifierString &tag_IdentifierString = *static_cast<KaxTagMultiIdentifierString *>(l5);
										tag_IdentifierString.ReadData(inputfile_stream->I_O());

										new_MultiIdentifierItem->StringData  = *static_cast<EbmlUnicodeString *>(&tag_IdentifierString);
									} else if (EbmlId(*l5) == KaxTagMultiIdentifierBinary::ClassInfos.GlobalId) {
										KaxTagMultiIdentifierBinary &tag_IdentifierBinary = *static_cast<KaxTagMultiIdentifierBinary *>(l5);
										tag_IdentifierBinary.ReadData(inputfile_stream->I_O(), SCOPE_ALL_DATA);
										/// \todo How to correctly copy the binary data?
									} else {
										//unknown element, level 5
									}

									if (UpperElementLevel > 0) {
										assert(1 == 0);	// this should never happen
									} else {
										l5->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l5->Generic().Context);
										delete l5;
										l5 = inputfile_stream->FindNextElement(l4->Generic().Context,	UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
									}
								} // while (l5 != NULL)
								//Gathered all this element's sub-elments
								currentTags->AddMultiIdentifierItem(new_MultiIdentifierItem);
								new_MultiIdentifierItem = NULL;

							} else if (EbmlId(*l4) == KaxTagMultiIdentifierType::ClassInfos.GlobalId) {
								KaxTagMultiIdentifierType &tag_MultiIdentifierType = *static_cast<KaxTagMultiIdentifierType *>(l4);
								tag_MultiIdentifierType.ReadData(inputfile_stream->I_O());

								new_MultiIdentifierItem->Type = uint16(tag_MultiIdentifierType);

							} else if (EbmlId(*l4) == KaxTagMultiIdentifierString::ClassInfos.GlobalId) {
								KaxTagMultiIdentifierString &tag_IdentifierString = *static_cast<KaxTagMultiIdentifierString *>(l4);
								tag_IdentifierString.ReadData(inputfile_stream->I_O());

								new_MultiIdentifierItem->StringData  = *static_cast<EbmlUnicodeString *>(&tag_IdentifierString);
							} else if (EbmlId(*l4) == KaxTagMultiIdentifierBinary::ClassInfos.GlobalId) {
								KaxTagMultiIdentifierBinary &tag_IdentifierBinary = *static_cast<KaxTagMultiIdentifierBinary *>(l4);
								tag_IdentifierBinary.ReadData(inputfile_stream->I_O(), SCOPE_ALL_DATA);
								/// \todo How to correctly copy the binary data?
							} else {
								//unknown element, level 4
							}

							if (UpperElementLevel > 0) {
								UpperElementLevel--;
								delete l4;
								l4 = l5;
								if (UpperElementLevel > 0)
									break;

								//assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context,	UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)

						//Gathered all this element's sub-elments
						currentTags->AddMultiIdentifierItem(new_MultiIdentifierItem);
						new_MultiIdentifierItem = NULL;

					} else if (EbmlId(*l3) == KaxTagMultiEntity::ClassInfos.GlobalId) {
						//Setup Multi-Entity Tag class
						new_MultiEntityItem = new MatroskaTagMultiEntityItem();

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							if (EbmlId(*l4) == KaxTagEntity::ClassInfos.GlobalId) {
								//Setup Multi-Entity Tag class
								if (new_MultiEntityItem == NULL) {
									new_MultiEntityItem = new MatroskaTagMultiEntityItem();
								}

								l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
								while (l5 != NULL) {
									if (UpperElementLevel > 0) {
										break;
									}
									if (UpperElementLevel < 0) {
										UpperElementLevel = 0;
									}

									if (EbmlId(*l5) == KaxTagMultiEntityType::ClassInfos.GlobalId) {
										KaxTagMultiEntityType &entity_type = *static_cast<KaxTagMultiEntityType *>(l5);
										entity_type.ReadData(inputfile_stream->I_O());

										tag_type5 = uint64(entity_type);
										new_MultiEntityItem->Type = uint16(entity_type);

									} else if (EbmlId(*l5) == KaxTagMultiEntityAddress::ClassInfos.GlobalId) {
										KaxTagMultiEntityAddress &entity_addrinputfile_streams = *static_cast<KaxTagMultiEntityAddress *>(l5);
										entity_addrinputfile_streams.ReadData(inputfile_stream->I_O());

										new_MultiEntityItem->Address = *static_cast<EbmlUnicodeString *>(&entity_addrinputfile_streams);

									} else if (EbmlId(*l5) == KaxTagMultiEntityEmail::ClassInfos.GlobalId) {
										KaxTagMultiEntityEmail &entity_email = *static_cast<KaxTagMultiEntityEmail *>(l5);
										entity_email.ReadData(inputfile_stream->I_O());

										new_MultiEntityItem->Email = *static_cast<EbmlString *>(&entity_email);

									} else if (EbmlId(*l5) == KaxTagMultiEntityURL::ClassInfos.GlobalId) {
										KaxTagMultiEntityURL &entity_URL = *static_cast<KaxTagMultiEntityURL *>(l5);
										entity_URL.ReadData(inputfile_stream->I_O());

										new_MultiEntityItem->URL = *static_cast<EbmlString *>(&entity_URL);

									} else if (EbmlId(*l5) == KaxTagMultiEntityName::ClassInfos.GlobalId) {
										KaxTagMultiEntityName &entity_name = *static_cast<KaxTagMultiEntityName *>(l5);
										entity_name.ReadData(inputfile_stream->I_O());

										new_MultiEntityItem->Name = *static_cast<EbmlUnicodeString *>(&entity_name);
										if (tag_type5 ==	KaxTagMultiEntitiesType_LeadPerformerSoloist) {
											currentTags->Tag_Simple_Artist = &new_MultiEntityItem->Name;
										}

									} else {
										//unknown element, level 5
									}

									if (UpperElementLevel > 0) {
										assert(1 == 0);	// this should never happen
									} else {
										l5->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l5->Generic().Context);
										delete l5;
										l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
									}
								} // while (l5 != NULL)
								//Gathered all this element's sub-elments
								currentTags->AddMultiEntityItem(new_MultiEntityItem);
								new_MultiEntityItem = NULL;

							} else if (EbmlId(*l4) == KaxTagMultiEntityType::ClassInfos.GlobalId) {
								KaxTagMultiEntityType &entity_type = *static_cast<KaxTagMultiEntityType *>(l4);
								entity_type.ReadData(inputfile_stream->I_O());

								tag_type5 = uint64(entity_type);
								new_MultiEntityItem->Type = uint16(entity_type);

							} else if (EbmlId(*l4) == KaxTagMultiEntityAddress::ClassInfos.GlobalId) {
								KaxTagMultiEntityAddress &entity_addrinputfile_streams = *static_cast<KaxTagMultiEntityAddress *>(l4);
								entity_addrinputfile_streams.ReadData(inputfile_stream->I_O());

								new_MultiEntityItem->Address = *static_cast<EbmlUnicodeString *>(&entity_addrinputfile_streams);

							} else if (EbmlId(*l4) == KaxTagMultiEntityEmail::ClassInfos.GlobalId) {
								KaxTagMultiEntityEmail &entity_email = *static_cast<KaxTagMultiEntityEmail *>(l4);
								entity_email.ReadData(inputfile_stream->I_O());

								new_MultiEntityItem->Email = *static_cast<EbmlString *>(&entity_email);

							} else if (EbmlId(*l4) == KaxTagMultiEntityURL::ClassInfos.GlobalId) {
								KaxTagMultiEntityURL &entity_URL = *static_cast<KaxTagMultiEntityURL *>(l4);
								entity_URL.ReadData(inputfile_stream->I_O());

								new_MultiEntityItem->URL = *static_cast<EbmlString *>(&entity_URL);

							} else if (EbmlId(*l4) == KaxTagMultiEntityName::ClassInfos.GlobalId) {
								KaxTagMultiEntityName &entity_name = *static_cast<KaxTagMultiEntityName *>(l4);
								entity_name.ReadData(inputfile_stream->I_O());

								new_MultiEntityItem->Name = *static_cast<EbmlUnicodeString *>(&entity_name);
								/* OLD STYLE
								if (tag_type5 ==	KaxTagMultiEntitiesType_LeadPerformerSoloist) {
									currentTags->Tag_Simple_Artist = &new_MultiEntityItem->Name;
								}
								*/

							} else {
								//unknown element, level 4
							}

							if (UpperElementLevel > 0) {
								UpperElementLevel--;
								delete l4;
								l4 = l5;
								if (UpperElementLevel > 0)
									break;

								//assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)
						//Gathered all this element's sub-elments
						currentTags->AddMultiEntityItem(new_MultiEntityItem);
						new_MultiEntityItem = NULL;

					} else if (EbmlId(*l3) == KaxTagMultiTitle::ClassInfos.GlobalId) {
						//Setup Multi-Title Tag class
						new_MultiTitleItem = new MatroskaTagMultiTitleItem();

						l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel,	0xFFFFFFFFFFFFFFFFL, true, 1);
						while (l4 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}

							if (EbmlId(*l4) == KaxTagTitle::ClassInfos.GlobalId) {
								//Setup Multi-Title Tag class
								if (new_MultiTitleItem == NULL) {
									new_MultiTitleItem = new MatroskaTagMultiTitleItem();
								}

								l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel,	0xFFFFFFFFFFFFFFFFL, true, 1);
								while (l5 != NULL) {
									if (UpperElementLevel > 0) {
										break;
									}
									if (UpperElementLevel < 0) {
										UpperElementLevel = 0;
									}

									if (EbmlId(*l5) == KaxTagMultiTitleType::ClassInfos.GlobalId) {
										KaxTagMultiTitleType &Title_type = *static_cast<KaxTagMultiTitleType *>(l5);
										Title_type.ReadData(inputfile_stream->I_O());

										tag_type5 = uint64(Title_type);
										new_MultiTitleItem->Type = uint16(Title_type);

									} else if (EbmlId(*l5) == KaxTagMultiTitleAddress::ClassInfos.GlobalId) {
										KaxTagMultiTitleAddress &Title_addrinputfile_streams = *static_cast<KaxTagMultiTitleAddress *>(l5);
										Title_addrinputfile_streams.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->Address = *static_cast<EbmlUnicodeString *>(&Title_addrinputfile_streams);

									} else if (EbmlId(*l5) == KaxTagMultiTitleEmail::ClassInfos.GlobalId) {
										KaxTagMultiTitleEmail &Title_email = *static_cast<KaxTagMultiTitleEmail *>(l5);
										Title_email.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->Email = *static_cast<EbmlString *>(&Title_email);

									} else if (EbmlId(*l5) == KaxTagMultiTitleURL::ClassInfos.GlobalId) {
										KaxTagMultiTitleURL &Title_URL = *static_cast<KaxTagMultiTitleURL *>(l5);
										Title_URL.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->URL = *static_cast<EbmlString *>(&Title_URL);

									} else if (EbmlId(*l5) == KaxTagMultiTitleName::ClassInfos.GlobalId) {
										KaxTagMultiTitleName &Title_name = *static_cast<KaxTagMultiTitleName *>(l5);
										Title_name.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->Name = *static_cast<EbmlUnicodeString *>(&Title_name);
										/* OLD STYLE
										if (tag_type5 == KaxTagMultiTitleType_TrackTitle) {
											currentTags->Tag_Simple_Title = &new_MultiTitleItem->Name;
										} else if (tag_type5 == KaxTagMultiTitleType_AlbumMovieShowTitle) {
											currentTags->Tag_Simple_Album = &new_MultiTitleItem->Name;
										}
										*/

									} else if (EbmlId(*l5) == KaxTagMultiTitleSubTitle::ClassInfos.GlobalId) {
										KaxTagMultiTitleSubTitle &Title_SubTitle = *static_cast<KaxTagMultiTitleSubTitle *>(l5);
										Title_SubTitle.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->SubTitle = *static_cast<EbmlUnicodeString *>(&Title_SubTitle);

									} else if (EbmlId(*l5) == KaxTagMultiTitleEdition::ClassInfos.GlobalId) {
										KaxTagMultiTitleEdition &Title_Edition = *static_cast<KaxTagMultiTitleEdition *>(l5);
										Title_Edition.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->Edition = *static_cast<EbmlUnicodeString *>(&Title_Edition);

									} else if (EbmlId(*l5) == KaxTagMultiTitleLanguage::ClassInfos.GlobalId) {
										KaxTagMultiTitleLanguage &Title_Language = *static_cast<KaxTagMultiTitleLanguage *>(l5);
										Title_Language.ReadData(inputfile_stream->I_O());

										new_MultiTitleItem->Language = *static_cast<EbmlString *>(&Title_Language);

									} else {
										//unknown element, level 5
									}

									if (UpperElementLevel > 0) {
										assert(1 == 0);	// this should never happen
									} else {
										l5->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l5->Generic().Context);
										delete l5;
										l5 = inputfile_stream->FindNextElement(l4->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
									}
								} // while (l4 != NULL)
								//Gathered all this element's sub-elments
								currentTags->AddMultiTitleItem(new_MultiTitleItem);
								new_MultiTitleItem = NULL;
							} else if (EbmlId(*l4) == KaxTagMultiTitleType::ClassInfos.GlobalId) {
								KaxTagMultiTitleType &Title_type = *static_cast<KaxTagMultiTitleType *>(l4);
								Title_type.ReadData(inputfile_stream->I_O());

								tag_type4 = uint64(Title_type);
								new_MultiTitleItem->Type = uint16(Title_type);

							} else if (EbmlId(*l4) == KaxTagMultiTitleAddress::ClassInfos.GlobalId) {
								KaxTagMultiTitleAddress &Title_addrinputfile_streams = *static_cast<KaxTagMultiTitleAddress *>(l4);
								Title_addrinputfile_streams.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->Address = *static_cast<EbmlUnicodeString *>(&Title_addrinputfile_streams);

							} else if (EbmlId(*l4) == KaxTagMultiTitleEmail::ClassInfos.GlobalId) {
								KaxTagMultiTitleEmail &Title_email = *static_cast<KaxTagMultiTitleEmail *>(l4);
								Title_email.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->Email = *static_cast<EbmlString *>(&Title_email);

							} else if (EbmlId(*l4) == KaxTagMultiTitleURL::ClassInfos.GlobalId) {
								KaxTagMultiTitleURL &Title_URL = *static_cast<KaxTagMultiTitleURL *>(l4);
								Title_URL.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->URL = *static_cast<EbmlString *>(&Title_URL);

							} else if (EbmlId(*l4) == KaxTagMultiTitleName::ClassInfos.GlobalId) {
								KaxTagMultiTitleName &Title_name = *static_cast<KaxTagMultiTitleName *>(l4);
								Title_name.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->Name = *static_cast<EbmlUnicodeString *>(&Title_name);

								/* OLD STYLE
								if (tag_type4 == KaxTagMultiTitleType_TrackTitle) {
									currentTags->Tag_Simple_Title = &new_MultiTitleItem->Name;
								}
								*/

							} else if (EbmlId(*l4) == KaxTagMultiTitleSubTitle::ClassInfos.GlobalId) {
								KaxTagMultiTitleSubTitle &Title_SubTitle = *static_cast<KaxTagMultiTitleSubTitle *>(l4);
								Title_SubTitle.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->SubTitle = *static_cast<EbmlUnicodeString *>(&Title_SubTitle);

							} else if (EbmlId(*l4) == KaxTagMultiTitleEdition::ClassInfos.GlobalId) {
								KaxTagMultiTitleEdition &Title_Edition = *static_cast<KaxTagMultiTitleEdition *>(l4);
								Title_Edition.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->Edition = *static_cast<EbmlUnicodeString *>(&Title_Edition);

							} else if (EbmlId(*l4) == KaxTagMultiTitleLanguage::ClassInfos.GlobalId) {
								KaxTagMultiTitleLanguage &Title_Language = *static_cast<KaxTagMultiTitleLanguage *>(l4);
								Title_Language.ReadData(inputfile_stream->I_O());

								new_MultiTitleItem->Language = *static_cast<EbmlString *>(&Title_Language);

							} else {
								//unknown element, level 4
							}

							if (UpperElementLevel > 0) {
								UpperElementLevel--;
								delete l4;
								l4 = l5;
								if (UpperElementLevel > 0)
									break;

								//assert(1 == 0);	// this should never happen
							} else {
								l4->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l4->Generic().Context);
								delete l4;
								l4 = inputfile_stream->FindNextElement(l3->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
							}
						} // while (l4 != NULL)
						//Gathered all this element's sub-elments
						currentTags->AddMultiTitleItem(new_MultiTitleItem);

					} else {
						//unknown element level 3
#endif
					}

					if (UpperElementLevel > 0) {	// we're coming from l4
						UpperElementLevel--;
						delete l3;
						l3 = l4;
						if (UpperElementLevel > 0)
							break;
					} else {
						l3->SkipData(static_cast<EbmlStream &>(*inputfile_stream),l3->Generic().Context);
						delete l3;
						l3 = inputfile_stream->FindNextElement(l2->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
					}
				} // while (l3 != NULL)
				
				// Add this tag to the master tag list
				tags.push_back(currentTags);

				//Thinputfile_streame should only be found in old filinputfile_stream
			} else if (EbmlId(*l2) == KaxMuxingApp::ClassInfos.GlobalId)	{
				KaxMuxingApp &tag_MuxingApp = *static_cast<KaxMuxingApp *>(l2);
				tag_MuxingApp.ReadData(inputfile_stream->I_O());

				ODS("Found Old MuxingApp Tag");
				m_segmentInfo.m_infoMuxingApp = *static_cast<EbmlUnicodeString *>(&tag_MuxingApp);

			} else if (EbmlId(*l2) == KaxWritingApp::ClassInfos.GlobalId) {
				KaxWritingApp &tag_WritingApp = *static_cast<KaxWritingApp *>(l2);
				tag_WritingApp.ReadData(inputfile_stream->I_O());

				ODS("Found Old WritingApp Tag");
				m_segmentInfo.m_infoWritingApp  = *static_cast<EbmlUnicodeString *>(&tag_WritingApp);

			} else {
				//unknown element level	2
			}

			if (UpperElementLevel > 0) {	// we're coming from l3
				UpperElementLevel--;
				delete l2;
				l2 = l3;
				if (UpperElementLevel > 0)
					break;
			} else {
				l2->SkipData(static_cast<EbmlStream &>(*inputfile_stream), l2->Generic().Context);
				delete l2;
				l2 = inputfile_stream->FindNextElement(tags_element->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
			}
		} // while (l2 != NULL)
		
		return number_of_tags;
	}catch (...) {
		return -1;
	};
}

//Returns the title for the file found in the Infos element
UTFstring &MatroskaInfoParser::GetTitle()
{
	return m_segmentInfo.m_infoTitle;
}

UTFstring MatroskaInfoParser::GetTagTitle(MatroskaTagInfo *tagTrack) {
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_Simple_Title != NULL)
				{
					return *current_tags->Tag_Simple_Title;

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("TITLE").length() > 0) {
					return current_tags->Tag_Simple_Tags.GetSimpleTag("TITLE");
				}
			}
		}
	}else {
		// We have a tag track
		if (tagTrack->Tag_Simple_Title != NULL) {
			return *tagTrack->Tag_Simple_Title;

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("TITLE").length() > 0) {
			return tagTrack->Tag_Simple_Tags.GetSimpleTag("TITLE");
		}
	}
	return UTFstring(L"");
}

UTFstring MatroskaInfoParser::GetTagArtist(MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_Simple_Artist != NULL) {
					return *current_tags->Tag_Simple_Artist;
				
				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("ARTIST").length() > 0) {
					return current_tags->Tag_Simple_Tags.GetSimpleTag("ARTIST");
				}
			}
		}
	}else {
		// We have a tag track
		if (tagTrack->Tag_Simple_Artist != NULL) {
			return *tagTrack->Tag_Simple_Artist;
		
		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("ARTIST").length() > 0) {
			return tagTrack->Tag_Simple_Tags.GetSimpleTag("ARTIST");
		}
	}
	return UTFstring(L"");
};

UTFstring MatroskaInfoParser::GetTagAlbum(MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_Simple_Album != NULL) {
					return *current_tags->Tag_Simple_Album;
				
				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("ALBUM").length() > 0) {
					return current_tags->Tag_Simple_Tags.GetSimpleTag("ALBUM");
				}
			}
		}
	}else {
		// We have a tag track
		if (tagTrack->Tag_Simple_Album != NULL) {
			return *tagTrack->Tag_Simple_Album;
		
		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("ALBUM").length() > 0) {
			return tagTrack->Tag_Simple_Tags.GetSimpleTag("ALBUM");
		}
	}
	return UTFstring(L"");
};

UTFstring MatroskaInfoParser::GetTagComment(MatroskaTagInfo *tagTrack)
{
	for (size_t t = 0; t < tags.size(); t++)
	{
		MatroskaTagInfo *current_tags = tags.at(t);
		if (current_tags != NULL)
		{
			if (current_tags->Tag_MultiComments != NULL)
			{
				for (int c = 0; c < current_tags->Tag_MultiComments->GetMultiCommentCount(); c++)
				{
					MatroskaTagMultiCommentItem *comment_field = current_tags->Tag_MultiComments->GetItem(c);
					if ((comment_field != NULL) && (comment_field->Comments.length() > 1))
					{
						return comment_field->Comments;
					}
				}
			}
		}
	}
	return UTFstring(L"");
};

uint32 MatroskaInfoParser::GetTagDiscTrack(MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_DiscTrack != 0) {
					return current_tags->Tag_DiscTrack;

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("TRACK").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("TRACK").GetUTF8().c_str());

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("TRACKNO").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("TRACKNO").GetUTF8().c_str());

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("TRACKNUM").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("TRACKNUM").GetUTF8().c_str());

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("TRACK_NO").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("TRACK_NO").GetUTF8().c_str());

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("TRACK_NUM").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("TRACK_NUM").GetUTF8().c_str());
				}
			}
		}
	} else {
		// We have a tag track
		if (tagTrack->Tag_DiscTrack != 0) {
			return tagTrack->Tag_DiscTrack;

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACK").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACK").GetUTF8().c_str());

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACKNO").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACKNO").GetUTF8().c_str());

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACKNUM").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACKNUM").GetUTF8().c_str());

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACK_NO").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACK_NO").GetUTF8().c_str());

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACK_NUM").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("TRACK_NUM").GetUTF8().c_str());
		}
	}
	return 0;
};

std::string MatroskaInfoParser::GetTagGenre(MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_AudioGenre.length() > 1)
				{
					return current_tags->Tag_AudioGenre;
				}
				else if (current_tags->Tag_SubGenre.length() > 1)
				{
					return current_tags->Tag_SubGenre;

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("GENERE").length() > 0) {
					return static_cast<std::string>(current_tags->Tag_Simple_Tags.GetSimpleTag("GENERE").GetUTF8());
				}
			}
		}
	} else {
		//we have tag track
		if (tagTrack->Tag_AudioGenre.length() > 1) {
			return tagTrack->Tag_AudioGenre;

		} else if (tagTrack->Tag_SubGenre.length() > 1)	{
			return tagTrack->Tag_SubGenre;

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("GENERE").length() > 0) {
			return tagTrack->Tag_Simple_Tags.GetSimpleTag("GENERE").GetUTF8();
		}
	}
	return std::string("");
};

uint32 MatroskaInfoParser::GetTagYear(MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_MultiDates != NULL)
				{
					for (int c = 0; c < current_tags->Tag_MultiDates->GetMultiDateCount(); c++)
					{
						MatroskaTagMultiDateItem *date_field = current_tags->Tag_MultiDates->GetItem(c);
						if ((date_field != NULL) && (date_field->DateBegin > 1) && (date_field->Type == KaxTagMultiDateType_ReleaseDate))
						{
							tm *theTimeStruct = localtime(&date_field->DateBegin);
							if (theTimeStruct != NULL)
								return theTimeStruct->tm_year+1900;;
						}
					}
				}
				if (current_tags->Tag_Simple_Tags.GetSimpleTag("DATE").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("DATE").GetUTF8().c_str());
				
				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("YEAR").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("YEAR").GetUTF8().c_str());
				}
			}
		}
	} else {
		//We have a tag track
		if (tagTrack->Tag_MultiDates != NULL)
		{
			for (int c = 0; c < tagTrack->Tag_MultiDates->GetMultiDateCount(); c++)
			{
				MatroskaTagMultiDateItem *date_field = tagTrack->Tag_MultiDates->GetItem(c);
				if ((date_field != NULL) && (date_field->DateBegin > 1) && (date_field->Type == KaxTagMultiDateType_ReleaseDate))
				{
					tm *theTimeStruct = localtime(&date_field->DateBegin);
					if (theTimeStruct != NULL)
						return theTimeStruct->tm_year+1900;;
				}
			}
		}
		if (tagTrack->Tag_Simple_Tags.GetSimpleTag("DATE").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("DATE").GetUTF8().c_str());
		
		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("YEAR").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("YEAR").GetUTF8().c_str());
		}
	}
	return 0;
};

int32 MatroskaInfoParser::GetTagRating(MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_Popularimeter != 0)
				{
					return current_tags->Tag_Popularimeter;

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("Popularimeter").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("Popularimeter").GetUTF8().c_str());

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("RATING").length() > 0) {
					return atol(current_tags->Tag_Simple_Tags.GetSimpleTag("RATING").GetUTF8().c_str());

				}
			}
		}
	} else {
		//we have tag track
		if (tagTrack->Tag_Popularimeter != 0)	{
			return tagTrack->Tag_Popularimeter;

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("Popularimeter").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("Popularimeter").GetUTF8().c_str());

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("RATING").length() > 0) {
			return atol(tagTrack->Tag_Simple_Tags.GetSimpleTag("RATING").GetUTF8().c_str());
		}
	}

	return 0;
};

double MatroskaInfoParser::GetTagBPM(MatroskaTagInfo *tagTrack) {
	if (tagTrack == NULL) {
		for (size_t t = 0; t < tags.size(); t++)
		{
			MatroskaTagInfo *current_tags = tags.at(t);
			if (current_tags != NULL)
			{
				if (current_tags->Tag_BPM != 0)
				{
					return current_tags->Tag_BPM;

				} else if (current_tags->Tag_Simple_Tags.GetSimpleTag("BPM").length() > 0) {
					return atof(current_tags->Tag_Simple_Tags.GetSimpleTag("BPM").GetUTF8().c_str());
				}
			}
		}
	} else {
		//we have tag track
		if (tagTrack->Tag_BPM != 0)	{
			return tagTrack->Tag_BPM;

		} else if (tagTrack->Tag_Simple_Tags.GetSimpleTag("BPM").length() > 0) {
			return atof(tagTrack->Tag_Simple_Tags.GetSimpleTag("BPM").GetUTF8().c_str());
		}
	}

	return 0;
};

int MatroskaInfoParser::SetTagArtist(UTFstring artist, MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("ARTIST", artist.GetUTF8().c_str());
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("ARTIST", artist.GetUTF8().c_str());
	}
	
	return 0;
};

int MatroskaInfoParser::SetTagTitle(UTFstring title, MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("TITLE", title.GetUTF8().c_str());
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("TITLE", title.GetUTF8().c_str());
	}

	return 0;
};

int MatroskaInfoParser::SetTagAlbum(UTFstring album, MatroskaTagInfo *tagTrack)
{
	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("ALBUM", album.GetUTF8().c_str());
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("ALBUM", album.GetUTF8().c_str());
	}

	return 0;
};

int MatroskaInfoParser::SetTagDiscTrack(uint32 discTrack, MatroskaTagInfo *tagTrack)
{
	char buffer[256];
	snprintf(buffer, 255, "%i", discTrack);

	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("TRACK", buffer);
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("TRACK", buffer);
	}

	return 0;
};

int MatroskaInfoParser::SetTagYear(uint32 year, MatroskaTagInfo *tagTrack)
{
	char buffer[256];
	snprintf(buffer, 255, "%i", year);

	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("DATE", buffer);
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("DATE", buffer);
	}

	return 0;
};

int MatroskaInfoParser::SetTagGenre(std::string genre, MatroskaTagInfo *tagTrack) {
	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("GENRE", genre.c_str());
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("GENRE", genre.c_str());
	}

	return 0;
};

int MatroskaInfoParser::SetTagComment(UTFstring comment, MatroskaTagInfo *tagTrack) {
	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("COMMENT", comment.GetUTF8().c_str());
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("COMMENT", comment.GetUTF8().c_str());
	}
	return 0;
};

int MatroskaInfoParser::SetTagRating(int32 rating, MatroskaTagInfo *tagTrack) {
	char buffer[256];
	snprintf(buffer, 255, "%i", rating);

	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("RATING", buffer);
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("RATING", buffer);
	}

	return 0;
};
int MatroskaInfoParser::SetTagBPM(double bpm, MatroskaTagInfo *tagTrack) {
	char buffer[256];
	snprintf(buffer, 255, "%f", bpm);

	if (tagTrack == NULL) {
		if (tags.size() == 0)
			tags.push_back(new MatroskaTagInfo());

		tags.at(0)->Tag_Simple_Tags.SetSimpleTagValue("BPM", buffer);
	} else {
		//We have a tag track
		tagTrack->Tag_Simple_Tags.SetSimpleTagValue("BPM", buffer);
	}

	return 0;
};

#ifdef USING_TINYXML
int MatroskaInfoParser::SaveTagsToXMLFile(JString &xmlFilename) {
	TiXmlDocument tagXml;

	TiXmlDeclaration baseLevel("1.0", "UTF-8", "yes");
	tagXml.InsertEndChild(baseLevel);

	TiXmlElement rootLevel("Tags");

	MatroskaTagInfo *currentTags = NULL;
	for (size_t t = 0; t < tags.size(); t++)
	{
		currentTags = tags.at(t);

		if (currentTags != NULL) {
			//Lets add this tag to the xml file
			TiXmlElement tagElement("Tag");
			// Add the Targets
			TiXmlElement tagTargetsElement("Targets");

			if (currentTags->Tag_TrackUID != 0)
			{
				for (size_t u = 0; u < currentTags->Tag_TrackUID.size(); u++) {
					TiXmlElement tagTrackUIDElement("TrackUID");
					TiXmlText tagTrackUIDText(JString::Format("%u", (uint64)currentTags->Tag_TrackUID.at(u)).mb_str());

					tagTrackUIDElement.InsertEndChild(tagTrackUIDText);
					tagTargetsElement.InsertEndChild(tagTrackUIDElement);
				}
			}

			if (currentTags->Tag_ChapterUID != 0)
			{
				for (size_t u = 0; u < currentTags->Tag_ChapterUID.size(); u++) {
					TiXmlElement tagChapterUIDElement("ChapterUID");
					TiXmlText tagChapterUIDText(JString::Format("%u", (uint64)currentTags->Tag_ChapterUID.at(u)).mb_str());

					tagChapterUIDElement.InsertEndChild(tagChapterUIDText);
					tagTargetsElement.InsertEndChild(tagChapterUIDElement);
				}
			}

			if (currentTags->Tag_AttachmentUID != 0)
			{
				for (size_t u = 0; u < currentTags->Tag_AttachmentUID.size(); u++) {
					TiXmlElement tagAttachmentUIDElement("AttachmentUID");
					TiXmlText tagAttachmentUIDText(JString::Format("%u", (uint64)currentTags->Tag_AttachmentUID.at(u)).mb_str());

					tagAttachmentUIDElement.InsertEndChild(tagAttachmentUIDText);
					tagTargetsElement.InsertEndChild(tagAttachmentUIDElement);
				}
			}
			tagElement.InsertEndChild(tagTargetsElement);
			// Add the general

			TiXmlElement tagGeneral("General");
			TiXmlElement tagGenres("Genres");
			TiXmlElement tagAudioSpecific("AudioSpecific");
			TiXmlElement tagImageSpecific("ImageSpecific");

			if (currentTags->Tag_ArchivalLocation.length() > 0)
			{
				TiXmlElement tagNewElement("ArchivalLocation");
				TiXmlText tagNewText(currentTags->Tag_ArchivalLocation.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Bibliography.length() > 0) {
				TiXmlElement tagNewElement("Bibliography");
				TiXmlText tagNewText(currentTags->Tag_Bibliography.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Encoder.length() > 0) {
				TiXmlElement tagNewElement("Encoder");
				TiXmlText tagNewText(currentTags->Tag_Encoder.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_EncodeSettings.length() > 0) {
				TiXmlElement tagNewElement("EncodeSettings");
				TiXmlText tagNewText(currentTags->Tag_EncodeSettings.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_File.length() > 0) {
				TiXmlElement tagNewElement("File");
				TiXmlText tagNewText(currentTags->Tag_File.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Keywords.length() > 0) {
				TiXmlElement tagNewElement("Keywords");
				TiXmlText tagNewText(currentTags->Tag_Keywords.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Language.length() > 0)
			{
				TiXmlElement tagNewElement("Language");
				TiXmlText tagNewText(currentTags->Tag_Language.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			/*if (currentTags->Tag_Length != 0)
			{
				KaxTagLength & MyKaxTagLength = GetChild<KaxTagLength>(*MyKaxTag);
				*static_cast<EbmlUInteger *>(&MyKaxTagLength) = currentTags->Tag_Length;
			}*/
			if (currentTags->Tag_Mood.length() > 0) {
				TiXmlElement tagNewElement("Mood");
				TiXmlText tagNewText(currentTags->Tag_Mood.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Source.length() > 0) {
				TiXmlElement tagNewElement("Source");
				TiXmlText tagNewText(currentTags->Tag_Source.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_SourceForm.length() > 0) {
				TiXmlElement tagNewElement("SourceForm");
				TiXmlText tagNewText(currentTags->Tag_SourceForm.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_PlayCounter != 0) {
				TiXmlElement tagNewElement("PlayCounter");
				TiXmlText tagNewText(JString::Format("%u", currentTags->Tag_PlayCounter).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_PlaylistDelay != 0) {
				TiXmlElement tagNewElement("PlaylistDelay");
				TiXmlText tagNewText(JString::Format("%u", currentTags->Tag_PlaylistDelay).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Popularimeter != 0) {
				TiXmlElement tagNewElement("Popularimeter");
				TiXmlText tagNewText(JString::Format("%i", currentTags->Tag_Popularimeter).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Product.length() > 0) {
				TiXmlElement tagNewElement("Product");
				TiXmlText tagNewText(currentTags->Tag_Product.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_RecordLocation.length() > 0) {
				TiXmlElement tagNewElement("RecordLocation");
				TiXmlText tagNewText(currentTags->Tag_RecordLocation.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Subject.length() > 0)
			{
				TiXmlElement tagNewElement("Subject");
				TiXmlText tagNewText(currentTags->Tag_Subject.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGeneral.InsertEndChild(tagNewElement);
			}

			// Audio stuff
			if (currentTags->Tag_AudioGain != 0) {
				TiXmlElement tagNewElement("AudioGain");
				TiXmlText tagNewText(JString::Format("%f", currentTags->Tag_AudioGain).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_AudioPeak != 0) {
				TiXmlElement tagNewElement("AudioPeak");
				TiXmlText tagNewText(JString::Format("%f", currentTags->Tag_AudioPeak).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_BPM != 0) {
				TiXmlElement tagNewElement("BPM");
				TiXmlText tagNewText(JString::Format("%f", currentTags->Tag_BPM).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_DiscTrack != 0) {
				TiXmlElement tagNewElement("DiscTrack");
				TiXmlText tagNewText(JString::Format("%u", currentTags->Tag_DiscTrack).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_InitialKey.length() > 1) {
				TiXmlElement tagNewElement("InitialKey");
				TiXmlText tagNewText(currentTags->Tag_InitialKey.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_OfficialAudioFileURL.length() > 0) {
				TiXmlElement tagNewElement("OfficialAudioFileURL");
				TiXmlText tagNewText(currentTags->Tag_OfficialAudioFileURL.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_OfficialAudioSourceURL.length() > 0) {
				TiXmlElement tagNewElement("OfficialAudioSourceURL");
				TiXmlText tagNewText(currentTags->Tag_OfficialAudioSourceURL.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_SetPart != 0) {
				TiXmlElement tagNewElement("SetPart");
				TiXmlText tagNewText(JString::Format("%u", currentTags->Tag_SetPart).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagAudioSpecific.InsertEndChild(tagNewElement);
			}

			// Image stuff
			if (currentTags->Tag_CaptureDPI != 0) {
				TiXmlElement tagNewElement("CaptureDPI");
				TiXmlText tagNewText(JString::Format("%u", currentTags->Tag_CaptureDPI).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagImageSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_CapturePaletteSetting != 0) {
				TiXmlElement tagNewElement("CapturePaletteSetting");
				TiXmlText tagNewText(JString::Format("%u", currentTags->Tag_CapturePaletteSetting).mb_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagImageSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_Cropped.length() > 0) {
				TiXmlElement tagNewElement("Cropped");
				TiXmlText tagNewText(currentTags->Tag_Cropped.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagImageSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_OriginalDimensions.length() > 0) {
				TiXmlElement tagNewElement("OriginalDimensions");
				TiXmlText tagNewText(currentTags->Tag_OriginalDimensions.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagImageSpecific.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_OriginalMediaType.length() > 0) {
				TiXmlElement tagNewElement("OriginalMediaType");
				TiXmlText tagNewText(currentTags->Tag_OriginalMediaType.GetUTF8().c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagImageSpecific.InsertEndChild(tagNewElement);
			}

			// Genre stuff
			if (currentTags->Tag_AudioGenre.length() > 0) {
				TiXmlElement tagNewElement("AudioGenre");
				TiXmlText tagNewText(currentTags->Tag_AudioGenre.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGenres.InsertEndChild(tagNewElement);
			}
			if (currentTags->Tag_SubGenre.length() > 0) {
				TiXmlElement tagNewElement("SubGenre");
				TiXmlText tagNewText(currentTags->Tag_SubGenre.c_str());

				tagNewElement.InsertEndChild(tagNewText);
				tagGenres.InsertEndChild(tagNewElement);
			}

			// Multi stuff
			if (currentTags->Tag_MultiComments != NULL) {
				MatroskaTagMultiCommentItem *comment_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiComments->GetMultiCommentCount(); c++)
				{
					comment_item = currentTags->Tag_MultiComments->GetItem(c);
					if (comment_item != NULL) {
						TiXmlElement tagMultiCommentElement("MultiComment");

						if (comment_item->Language.length() > 0) {
							TiXmlElement tagNewElement("Language");
							TiXmlText tagNewText(comment_item->Language.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagMultiCommentElement.InsertEndChild(tagNewElement);
						}
						if (comment_item->Comments.length() > 0) {
							TiXmlElement tagNewElement("Comment");
							TiXmlText tagNewText(comment_item->Comments.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagMultiCommentElement.InsertEndChild(tagNewElement);
						}
						if (!tagMultiCommentElement.NoChildren())
							tagElement.InsertEndChild(tagMultiCommentElement);
					}
				}
			}
			if (!tagGeneral.NoChildren())
				tagElement.InsertEndChild(tagGeneral);
			if (!tagAudioSpecific.NoChildren())
				tagElement.InsertEndChild(tagAudioSpecific);
			if (!tagImageSpecific.NoChildren())
				tagElement.InsertEndChild(tagImageSpecific);
			if (!tagGenres.NoChildren())
				tagElement.InsertEndChild(tagGenres);

			if (currentTags->Tag_MultiCommercials != NULL) {
				//Create the base element
				TiXmlElement tagMultiCommercialElement("MultiCommercial");

				MatroskaTagMultiCommercialItem *commercial_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiCommercials->GetMultiCommercialCount(); c++)
				{
					commercial_item = currentTags->Tag_MultiCommercials->GetItem(c);;
					if (commercial_item != NULL)
					{
						TiXmlElement tagCommercialElement("Commercial");

						TiXmlElement tagCommercialTypeElement("CommercialType");
						TiXmlText tagCommercialTypeText(JString::Format("%u", commercial_item->Type).mb_str());

						tagCommercialTypeElement.InsertEndChild(tagCommercialTypeText);
						tagCommercialElement.InsertEndChild(tagCommercialTypeElement);

						if (commercial_item->Address.length() > 0) {
							TiXmlElement tagNewElement("Address");
							TiXmlText tagNewText(commercial_item->Address.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagCommercialElement.InsertEndChild(tagNewElement);
						}
						if (commercial_item->Email.length() > 0) {
							TiXmlElement tagNewElement("Email");
							TiXmlText tagNewText(commercial_item->Email.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagCommercialElement.InsertEndChild(tagNewElement);
						}

						TiXmlElement tagMultiPriceElement("MultiPrice");
						if (commercial_item->PriceAmount != 0) {
							TiXmlElement tagNewElement("Amount");
							TiXmlText tagNewText(JString::Format("%f", commercial_item->PriceAmount).mb_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagMultiPriceElement.InsertEndChild(tagNewElement);
						}
						if (commercial_item->PriceCurrency.length() > 0) {
							TiXmlElement tagNewElement("Currency");
							TiXmlText tagNewText(commercial_item->PriceCurrency.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagMultiPriceElement.InsertEndChild(tagNewElement);
						}
						/* TODO What is extended ISO 8601 format?
						if (commercial_item->PriceDate != 0) {
							TiXmlElement tagNewElement("PriceDate");
							TiXmlText tagNewText(commercial_item->PriceDate);

							tagNewElement.InsertEndChild(tagNewText);
							tagMultiPriceElement.InsertEndChild(tagNewElement);
						}*/
						if (!tagMultiPriceElement.NoChildren())
							tagCommercialElement.InsertEndChild(tagMultiPriceElement);

						if (commercial_item->URL.length() > 0) {
							TiXmlElement tagNewElement("URL");
							TiXmlText tagNewText(commercial_item->URL.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagCommercialElement.InsertEndChild(tagNewElement);
						}
						tagMultiCommercialElement.InsertEndChild(tagCommercialElement);
					}
				}
				if (!tagMultiCommercialElement.NoChildren())
					tagElement.InsertEndChild(tagMultiCommercialElement);
			}
			/* What is extended ISO 8601 format?
			if (currentTags->Tag_MultiDates != NULL) {
				//Get the base element
				KaxTagMultiDate &MyKaxTagMultiDate_Base = GetChild<KaxTagMultiDate>(*MyKaxTag);

				MatroskaTagMultiDateItem *date_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiDates->GetMultiDateCount(); c++)
				{
					date_item = currentTags->Tag_MultiDates->GetItem(c);;
					if (date_item != NULL)
					{
						KaxTagDate *MyKaxTagMultiDate = NULL;
						if (MyKaxTagMultiDate_last != NULL) {
							MyKaxTagMultiDate = &GetNextChild<KaxTagDate>(MyKaxTagMultiDate_Base, *MyKaxTagMultiDate_last);
							MyKaxTagMultiDate_last = MyKaxTagMultiDate;
						}else {
							MyKaxTagMultiDate = &GetChild<KaxTagDate>(MyKaxTagMultiDate_Base);
							MyKaxTagMultiDate_last = MyKaxTagMultiDate;
						}

						KaxTagMultiDateType & MyKaxTagMultiDateType = GetChild<KaxTagMultiDateType>(*MyKaxTagMultiDate);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiDateType) = date_item->Type;

						if (date_item->DateBegin != 0) {
							KaxTagMultiDateDateBegin & MyKaxTagMultiDateDateBegin = GetChild<KaxTagMultiDateDateBegin>(*MyKaxTagMultiDate);
							MyKaxTagMultiDateDateBegin.SetEpochDate(date_item->DateBegin);
						}
						if (date_item->DateEnd != 0) {
							KaxTagMultiDateDateEnd & MyKaxTagMultiDateDateEnd = GetChild<KaxTagMultiDateDateEnd>(*MyKaxTagMultiDate);
							MyKaxTagMultiDateDateEnd.SetEpochDate(date_item->DateEnd);
						}
					}
				}
			}*/
			if (currentTags->Tag_MultiEntities != NULL) {
				//Create the base element
				TiXmlElement tagMultiEntityElement("MultiEntity");

				MatroskaTagMultiEntityItem  *entity_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiEntities->GetMultiEntityCount(); c++)
				{
					entity_item = currentTags->Tag_MultiEntities->GetItem(c);;
					if (entity_item != NULL)
					{
						TiXmlElement tagEntityElement("Entity");

						TiXmlElement tagEntityTypeElement("EntityType");
						TiXmlText tagEntityTypeText(JString::Format("%u", entity_item->Type).mb_str());

						tagEntityTypeElement.InsertEndChild(tagEntityTypeText);
						tagEntityElement.InsertEndChild(tagEntityTypeElement);

						if (entity_item->Address.length() > 0) {
							TiXmlElement tagNewElement("Address");
							TiXmlText tagNewText(entity_item->Address.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagEntityElement.InsertEndChild(tagNewElement);
						}
						if (entity_item->Email.length() > 0) {
							TiXmlElement tagNewElement("Email");
							TiXmlText tagNewText(entity_item->Email.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagEntityElement.InsertEndChild(tagNewElement);
						}
						if (entity_item->Name.length() > 0) {
							TiXmlElement tagNewElement("Name");
							TiXmlText tagNewText(entity_item->Name.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagEntityElement.InsertEndChild(tagNewElement);
						}
						if (entity_item->URL.length() > 0) {
							TiXmlElement tagNewElement("URL");
							TiXmlText tagNewText(entity_item->URL.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagEntityElement.InsertEndChild(tagNewElement);
						}
						tagMultiEntityElement.InsertEndChild(tagEntityElement);
					}
				}
				if (!tagMultiEntityElement.NoChildren())
					tagElement.InsertEndChild(tagMultiEntityElement);
			}
			if (currentTags->Tag_MultiIdentifiers != NULL)
			{
				//Create the base element
				TiXmlElement tagMultiIdentifierElement("MultiIdentifier");

				MatroskaTagMultiIdentifierItem *identifier_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiIdentifiers->GetMultiIdentifierCount(); c++)
				{
					identifier_item = currentTags->Tag_MultiIdentifiers->GetItem(c);;
					if (identifier_item != NULL)
					{
						TiXmlElement tagIdentifierElement("Identifier");

						TiXmlElement tagIdentifierTypeElement("IdentifierType");
						TiXmlText tagIdentifierTypeText(JString::Format("%u", identifier_item->Type).mb_str());

						tagIdentifierTypeElement.InsertEndChild(tagIdentifierTypeText);
						tagIdentifierElement.InsertEndChild(tagIdentifierTypeElement);

						if (identifier_item->StringData.length() > 0) {
							TiXmlElement tagIdentifierNewElement("String");
							TiXmlText tagIdentifierNewText(identifier_item->StringData.GetUTF8().c_str());

							tagIdentifierNewElement.InsertEndChild(tagIdentifierNewText);
							tagIdentifierElement.InsertEndChild(tagIdentifierNewElement);
						}
						if (identifier_item->BinaryData_Length > 0) {
							/*TiXmlElement tagIdentifierNewElement("Binary");

							TiXmlText tagIdentifierNewText(identifier_item->BinaryData);

							tagIdentifierNewElement.InsertEndChild(tagIdentifierNewText);
							tagIdentifierElement.InsertEndChild(tagIdentifierNewElement);*/
						}
						tagMultiIdentifierElement.InsertEndChild(tagIdentifierElement);
					}
				}
				if (!tagMultiIdentifierElement.NoChildren())
					tagElement.InsertEndChild(tagMultiIdentifierElement);
			}
			if (currentTags->Tag_MultiLegals != NULL)
			{
				//Create the base element
				TiXmlElement tagMultiLegalElement("MultiLegal");

				MatroskaTagMultiLegalItem *legal_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiLegals->GetMultiLegalCount(); c++)
				{
					legal_item = currentTags->Tag_MultiLegals->GetItem(c);;
					if (legal_item != NULL)
					{
						TiXmlElement tagLegalElement("Legal");

						TiXmlElement tagLegalTypeElement("LegalType");
						TiXmlText tagLegalTypeText(JString::Format("%u", legal_item->Type).mb_str());

						tagLegalTypeElement.InsertEndChild(tagLegalTypeText);
						tagLegalElement.InsertEndChild(tagLegalTypeElement);

						if (legal_item->Address.length() > 0) {
							TiXmlElement tagNewElement("Address");
							TiXmlText tagNewText(legal_item->Address.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagLegalElement.InsertEndChild(tagNewElement);
						}
						if (legal_item->URL.length() > 0) {
							TiXmlElement tagNewElement("URL");
							TiXmlText tagNewText(legal_item->URL.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagLegalElement.InsertEndChild(tagNewElement);
						}
						tagMultiLegalElement.InsertEndChild(tagLegalElement);
					}
				}
				if (!tagMultiLegalElement.NoChildren())
					tagElement.InsertEndChild(tagMultiLegalElement);
			}
			if (currentTags->Tag_MultiTitles != NULL)
			{
				//Create the base element
				TiXmlElement tagMultiTitleElement("MultiTitle");

				MatroskaTagMultiTitleItem *title_item = NULL;
				for (int c = 0; c < currentTags->Tag_MultiTitles->GetMultiTitleCount(); c++)
				{
					title_item = currentTags->Tag_MultiTitles->GetItem(c);;
					if (title_item != NULL)
					{
						TiXmlElement tagTitleElement("Title");

						TiXmlElement tagTitleTypeElement("TitleType");
						TiXmlText tagTitleTypeText(JString::Format("%u", title_item->Type).mb_str());

						tagTitleTypeElement.InsertEndChild(tagTitleTypeText);
						tagTitleElement.InsertEndChild(tagTitleTypeElement);

						if (title_item->Address.length() > 0) {
							TiXmlElement tagNewElement("Address");
							TiXmlText tagNewText(title_item->Address.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						if (title_item->Edition.length() > 0) {
							TiXmlElement tagNewElement("Edition");
							TiXmlText tagNewText(title_item->Edition.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						if (title_item->Email.length() > 0) {
							TiXmlElement tagNewElement("Email");
							TiXmlText tagNewText(title_item->Email.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						if (title_item->Language.length() > 0) {
							TiXmlElement tagNewElement("Language");
							TiXmlText tagNewText(title_item->Language.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						if (title_item->Name.length() > 0) {
							TiXmlElement tagNewElement("Name");
							TiXmlText tagNewText(title_item->Name.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						if (title_item->SubTitle.length() > 0) {
							TiXmlElement tagNewElement("SubTitle");
							TiXmlText tagNewText(title_item->SubTitle.GetUTF8().c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						if (title_item->URL.length() > 0) {
							TiXmlElement tagNewElement("URL");
							TiXmlText tagNewText(title_item->URL.c_str());

							tagNewElement.InsertEndChild(tagNewText);
							tagTitleElement.InsertEndChild(tagNewElement);
						}
						tagMultiTitleElement.InsertEndChild(tagTitleElement);
					}
				}
				if (!tagMultiTitleElement.NoChildren())
					tagElement.InsertEndChild(tagMultiTitleElement);
			}
			if (currentTags->Tag_Simple_Tags.size() > 0)
			{
				for (size_t c = 0; c < currentTags->Tag_Simple_Tags.size(); c++)
				{
					MatroskaSimpleTag *simpleTag = currentTags->Tag_Simple_Tags.at(c);
					if (simpleTag != NULL)
					{
						TiXmlElement tagSimpleTagElement("Simple");

						TiXmlElement tagSimpleTagNameElement("Name");
						TiXmlText tagSimpleTagNameText(simpleTag->name.GetUTF8().c_str());

						tagSimpleTagNameElement.InsertEndChild(tagSimpleTagNameText);
						tagSimpleTagElement.InsertEndChild(tagSimpleTagNameElement);

						TiXmlElement tagSimpleTagValueElement("Value");
						TiXmlText tagSimpleTagValueText(simpleTag->value.GetUTF8().c_str());

						tagSimpleTagValueElement.InsertEndChild(tagSimpleTagValueText);
						tagSimpleTagElement.InsertEndChild(tagSimpleTagValueElement);

						tagElement.InsertEndChild(tagSimpleTagElement);
					}
				}						
			}

			rootLevel.InsertEndChild(tagElement);
		}
	}

	tagXml.InsertEndChild(rootLevel);
	tagXml.SaveFile(xmlFilename.mb_str());
	return 0;
};
#endif //USING_TINYXML

int MatroskaInfoParser::WriteTags()
{
	TRACE("MatroskaInfoParser::WriteTags()");
	try {
		int UpperElementLevel = 0;
		// Elements for different levels
		EbmlElement *l0 = NULL;
		EbmlElement *l1 = NULL;
		//EbmlElement *l2 = NULL;
		//EbmlElement *l3 = NULL;
		//EbmlElement *l4 = NULL;
		//EbmlElement *l5 = NULL;
		

		MatroskaUilsFileReader inout_file(m_filename, MODE_SAFE);
		if (!inout_file.IsOk())
			throw std::runtime_error(_("Unable open file. Please check that the file isn't open in another program."));

		EbmlStream es(inout_file);

		// Find the EbmlHead element. Must be the first one.
		l0 = es.FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFFFFFFFFFL);
		if (l0 == NULL) {
			throw std::runtime_error(_("No EBML header was found."));
		}
		
		if (!(EbmlId(*l0) == EbmlHead::ClassInfos.GlobalId)) {
			throw std::runtime_error(_("No EBML header was found."));
		}
		l0->SkipData(es, l0->Generic().Context);
		delete l0;
		//We must have found the EBML head :)

		// Next element must be a segment
		l0 = es.FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFFFFFFFFFL);
		if (l0 == NULL) {
			//No segment/level 0 element found.
			throw std::runtime_error(_("No Segment/Level 0 Element was found."));
		}
		if (!(EbmlId(*l0) == KaxSegment::ClassInfos.GlobalId)) {
			throw std::runtime_error(_("First Level 0 Element was not a Segment."));
		}

		std::auto_ptr<KaxSegment> segment((KaxSegment *)l0);

		UpperElementLevel = 0;

		uint64 orig_segment_size = l0->GetSize();
		//Seek to the tag start pos
		inout_file.setFilePointer(m_tagElementPos);

		uint64 orig_tag_size = m_tagElementSize;

		KaxTags & MyKaxTags = GetChild<KaxTags>(*static_cast<EbmlMaster *>(l0));

		std::vector<MatroskaTagInfo *> &tag_list = tags;


		//Start going through the list and adding tags
		KaxTag *MyKaxTag_last = NULL;
		for(size_t current_tag_track = 0; current_tag_track < tag_list.size(); current_tag_track++) {
			KaxTag *MyKaxTag = NULL;
			if (MyKaxTag_last != NULL)
			{
				MyKaxTag = &GetNextChild<KaxTag>(MyKaxTags, *MyKaxTag_last);
				MyKaxTag_last = MyKaxTag;
			}else {
				MyKaxTag = &GetChild<KaxTag>(MyKaxTags);
				MyKaxTag_last = MyKaxTag;
			}
			//The Targets group
			KaxTagTargets & MyKaxTagTargets = GetChild<KaxTagTargets>(*MyKaxTag);
			if (tag_list[current_tag_track]->Tag_TrackUID != 0) {
				KaxTagTrackUID *MyKaxTagTrackUID_last = NULL;
				KaxTagTrackUID *MyKaxTagTrackUID = NULL;
				for (size_t u = 0; u < tag_list[current_tag_track]->Tag_TrackUID.size(); u++) {										
					if (MyKaxTagTrackUID_last != NULL)
					{
						MyKaxTagTrackUID = &GetNextChild<KaxTagTrackUID>(MyKaxTagTargets, *MyKaxTagTrackUID_last);
						MyKaxTagTrackUID_last = MyKaxTagTrackUID;
					}else {
						MyKaxTagTrackUID = &GetChild<KaxTagTrackUID>(MyKaxTagTargets);
						MyKaxTagTrackUID_last = MyKaxTagTrackUID;
					}
					*static_cast<EbmlUInteger *>(MyKaxTagTrackUID) = tag_list[current_tag_track]->Tag_TrackUID.at(u);
				}
			}

			if (tag_list[current_tag_track]->Tag_ChapterUID != 0) {
				KaxTagChapterUID *MyKaxTagChapterUID_last = NULL;
				KaxTagChapterUID *MyKaxTagChapterUID = NULL;
				for (size_t u = 0; u < tag_list[current_tag_track]->Tag_ChapterUID.size(); u++) {										
					if (MyKaxTagChapterUID_last != NULL)
					{
						MyKaxTagChapterUID = &GetNextChild<KaxTagChapterUID>(MyKaxTagTargets, *MyKaxTagChapterUID_last);
						MyKaxTagChapterUID_last = MyKaxTagChapterUID;
					}else {
						MyKaxTagChapterUID = &GetChild<KaxTagChapterUID>(MyKaxTagTargets);
						MyKaxTagChapterUID_last = MyKaxTagChapterUID;
					}
					*static_cast<EbmlUInteger *>(MyKaxTagChapterUID) = tag_list[current_tag_track]->Tag_ChapterUID.at(u);
				}
			}

			if (tag_list[current_tag_track]->Tag_AttachmentUID != 0) {
				KaxTagAttachmentUID *MyKaxTagAttachmentUID_last = NULL;
				KaxTagAttachmentUID *MyKaxTagAttachmentUID = NULL;
				for (size_t u = 0; u < tag_list[current_tag_track]->Tag_AttachmentUID.size(); u++) {										
					if (MyKaxTagAttachmentUID_last != NULL)
					{
						MyKaxTagAttachmentUID = &GetNextChild<KaxTagAttachmentUID>(MyKaxTagTargets, *MyKaxTagAttachmentUID_last);
						MyKaxTagAttachmentUID_last = MyKaxTagAttachmentUID;
					}else {
						MyKaxTagAttachmentUID = &GetChild<KaxTagAttachmentUID>(MyKaxTagTargets);
						MyKaxTagAttachmentUID_last = MyKaxTagAttachmentUID;
					}
					*static_cast<EbmlUInteger *>(MyKaxTagAttachmentUID) = tag_list[current_tag_track]->Tag_AttachmentUID.at(u);
				}
			}
#ifndef MATROSKA_INFO
			KaxTagGeneral *MyKaxTagGeneral = &GetChild<KaxTagGeneral>(*MyKaxTag);
			KaxTagGenres *MyKaxTagGenres = &GetChild<KaxTagGenres>(*MyKaxTag);
			KaxTagAudioSpecific *MyKaxTagAudioSpecific = &GetChild<KaxTagAudioSpecific>(*MyKaxTag);
			KaxTagImageSpecific *MyKaxTagImageSpecific = &GetChild<KaxTagImageSpecific>(*MyKaxTag);

			if (tag_list[current_tag_track]->Tag_ArchivalLocation.length() > 0)
			{
				KaxTagArchivalLocation & MyKaxTagArchivalLocation = GetChild<KaxTagArchivalLocation>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagArchivalLocation) = tag_list[current_tag_track]->Tag_ArchivalLocation;
			}
			if (tag_list[current_tag_track]->Tag_AudioGain != 0)
			{
				KaxTagAudioGain & MyKaxTagAudioGain = GetChild<KaxTagAudioGain>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlFloat *>(&MyKaxTagAudioGain) = tag_list[current_tag_track]->Tag_AudioGain;
			}
			if (tag_list[current_tag_track]->Tag_AudioGenre.length() > 0)
			{
				KaxTagAudioGenre & MyKaxTagAudioGenre = GetChild<KaxTagAudioGenre>(*MyKaxTagGenres);
				*static_cast<EbmlString *>(&MyKaxTagAudioGenre) = tag_list[current_tag_track]->Tag_AudioGenre;
			}
			if (tag_list[current_tag_track]->Tag_AudioPeak != 0)
			{
				KaxTagAudioPeak & MyKaxTagAudioPeak = GetChild<KaxTagAudioPeak>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlFloat *>(&MyKaxTagAudioPeak) = tag_list[current_tag_track]->Tag_AudioPeak;
			}
			if (tag_list[current_tag_track]->Tag_Bibliography.length() > 0)
			{
				KaxTagBibliography & MyKaxTagBibliography = GetChild<KaxTagBibliography>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagBibliography) = tag_list[current_tag_track]->Tag_Bibliography;
			}
			if (tag_list[current_tag_track]->Tag_BPM != 0)
			{
				KaxTagBPM & MyKaxTagBPM = GetChild<KaxTagBPM>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlFloat *>(&MyKaxTagBPM) = tag_list[current_tag_track]->Tag_BPM;
			}
			if (tag_list[current_tag_track]->Tag_CaptureDPI != 0)
			{
				KaxTagCaptureDPI & MyKaxTagCaptureDPI= GetChild<KaxTagCaptureDPI>(*MyKaxTagImageSpecific);
				*static_cast<EbmlUInteger *>(&MyKaxTagCaptureDPI) = tag_list[current_tag_track]->Tag_CaptureDPI;
			}
			if (tag_list[current_tag_track]->Tag_CapturePaletteSetting != 0)
			{
				KaxTagCapturePaletteSetting & MyKaxTagCapturePaletteSetting = GetChild<KaxTagCapturePaletteSetting>(*MyKaxTagImageSpecific);
				*static_cast<EbmlUInteger *>(&MyKaxTagCapturePaletteSetting) = tag_list[current_tag_track]->Tag_CapturePaletteSetting;
			}
			if (tag_list[current_tag_track]->Tag_Cropped.length() > 0)
			{
				KaxTagCropped & MyKaxTagCropped = GetChild<KaxTagCropped>(*MyKaxTagImageSpecific);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagCropped) = tag_list[current_tag_track]->Tag_Cropped;
			}
			if (tag_list[current_tag_track]->Tag_DiscTrack != 0)
			{
				KaxTagDiscTrack & MyKaxTagDiscTrack = GetChild<KaxTagDiscTrack>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlUInteger *>(&MyKaxTagDiscTrack) = tag_list[current_tag_track]->Tag_DiscTrack;
			}
			if (tag_list[current_tag_track]->Tag_Encoder.length() > 0)
			{
				KaxTagEncoder & MyKaxTagEncoder = GetChild<KaxTagEncoder>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagEncoder) = tag_list[current_tag_track]->Tag_Encoder;
			}

			if (tag_list[current_tag_track]->Tag_EncodeSettings.length() > 0)
			{
				KaxTagEncodeSettings & MyKaxTagEncodeSettings = GetChild<KaxTagEncodeSettings>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagEncodeSettings) = tag_list[current_tag_track]->Tag_EncodeSettings;
			}
			if (tag_list[current_tag_track]->Tag_File.length() > 0)
			{
				KaxTagFile & MyKaxTagFile = GetChild<KaxTagFile>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagFile) = tag_list[current_tag_track]->Tag_File;
			}
			if (tag_list[current_tag_track]->Tag_InitialKey.length() > 0)
			{
				KaxTagInitialKey & MyKaxTagInitialKey = GetChild<KaxTagInitialKey>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlString *>(&MyKaxTagInitialKey) = tag_list[current_tag_track]->Tag_InitialKey;
			}
			if (tag_list[current_tag_track]->Tag_Keywords.length() > 0)
			{
				KaxTagKeywords & MyKaxTagKeywords= GetChild<KaxTagKeywords>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagKeywords) = tag_list[current_tag_track]->Tag_Keywords;
			}
			if (tag_list[current_tag_track]->Tag_Language.length() > 0)
			{
				KaxTagLanguage & MyKaxTagLanguage = GetChild<KaxTagLanguage>(*MyKaxTagGeneral);
				*static_cast<EbmlString *>(&MyKaxTagLanguage) = tag_list[current_tag_track]->Tag_Language;
			}
			if (tag_list[current_tag_track]->Tag_Length != 0)
			{
				KaxTagLength & MyKaxTagLength = GetChild<KaxTagLength>(*MyKaxTag);
				*static_cast<EbmlUInteger *>(&MyKaxTagLength) = tag_list[current_tag_track]->Tag_Length;
			}
			if (tag_list[current_tag_track]->Tag_Mood.length() > 1)
			{
				KaxTagMood & MyKaxTagMood = GetChild<KaxTagMood>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagMood) = tag_list[current_tag_track]->Tag_Mood;
			}
			if (tag_list[current_tag_track]->Tag_OfficialAudioFileURL.length() > 0)
			{
				KaxTagOfficialAudioFileURL & MyKaxTagOfficialAudioFileURL = GetChild<KaxTagOfficialAudioFileURL>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlString *>(&MyKaxTagOfficialAudioFileURL) = tag_list[current_tag_track]->Tag_OfficialAudioFileURL;
			}
			if (tag_list[current_tag_track]->Tag_OfficialAudioSourceURL.length() > 0)
			{
				KaxTagOfficialAudioSourceURL & MyKaxTagOfficialAudioSourceURL = GetChild<KaxTagOfficialAudioSourceURL>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlString *>(&MyKaxTagOfficialAudioSourceURL) = tag_list[current_tag_track]->Tag_OfficialAudioSourceURL;
			}
			if (tag_list[current_tag_track]->Tag_OriginalDimensions.length() > 0)
			{
				KaxTagOriginalDimensions & MyKaxTagOriginalDimensions = GetChild<KaxTagOriginalDimensions>(*MyKaxTagImageSpecific);
				*static_cast<EbmlString *>(&MyKaxTagOriginalDimensions) = tag_list[current_tag_track]->Tag_OriginalDimensions;
			}
			if (tag_list[current_tag_track]->Tag_OriginalMediaType.length() > 0)
			{
				KaxTagOriginalMediaType & MyKaxTagOriginalMediaType = GetChild<KaxTagOriginalMediaType>(*MyKaxTagImageSpecific);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagOriginalMediaType) = tag_list[current_tag_track]->Tag_OriginalMediaType;
			}
			if (tag_list[current_tag_track]->Tag_PlayCounter != 0)
			{
				KaxTagPlayCounter & MyKaxTagPlayCounter = GetChild<KaxTagPlayCounter>(*MyKaxTagGeneral);
				*static_cast<EbmlUInteger *>(&MyKaxTagPlayCounter) = tag_list[current_tag_track]->Tag_PlayCounter;
			}
			if (tag_list[current_tag_track]->Tag_PlaylistDelay != 0)
			{
				KaxTagPlaylistDelay & MyKaxTagPlaylistDelay = GetChild<KaxTagPlaylistDelay>(*MyKaxTag);
				*static_cast<EbmlUInteger *>(&MyKaxTagPlaylistDelay) = tag_list[current_tag_track]->Tag_PlaylistDelay;
			}
			if (tag_list[current_tag_track]->Tag_Popularimeter != 0)
			{
				KaxTagPopularimeter & MyKaxTagPopularimeter = GetChild<KaxTagPopularimeter>(*MyKaxTagGeneral);
				*static_cast<EbmlSInteger *>(&MyKaxTagPopularimeter) = tag_list[current_tag_track]->Tag_Popularimeter;
			}
			if (tag_list[current_tag_track]->Tag_Product.length() > 0)
			{
				KaxTagProduct & MyKaxTagProduct = GetChild<KaxTagProduct>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagProduct) = tag_list[current_tag_track]->Tag_Product;
			}
			if (tag_list[current_tag_track]->Tag_RecordLocation.length() > 0)
			{
				KaxTagRecordLocation & MyKaxTagRecordLocation = GetChild<KaxTagRecordLocation>(*MyKaxTagGeneral);
				*static_cast<EbmlString *>(&MyKaxTagRecordLocation) = tag_list[current_tag_track]->Tag_RecordLocation;
			}
			if (tag_list[current_tag_track]->Tag_SetPart != 0)
			{
				KaxTagSetPart & MyKaxTagSetPart = GetChild<KaxTagSetPart>(*MyKaxTagAudioSpecific);
				*static_cast<EbmlUInteger *>(&MyKaxTagSetPart) = tag_list[current_tag_track]->Tag_SetPart;
			}
			if (tag_list[current_tag_track]->Tag_Source.length() > 0)
			{
				KaxTagSource & MyKaxTagSource = GetChild<KaxTagSource>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagSource) = tag_list[current_tag_track]->Tag_Source;
			}
			if (tag_list[current_tag_track]->Tag_SourceForm.length() > 0)
			{
				KaxTagSourceForm & MyKaxTagSourceForm = GetChild<KaxTagSourceForm>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagSourceForm) = tag_list[current_tag_track]->Tag_SourceForm;
			}
			if (tag_list[current_tag_track]->Tag_SubGenre.length() > 0)
			{
				KaxTagSubGenre & MyKaxTagSubGenre = GetChild<KaxTagSubGenre>(*MyKaxTagGenres);
				*static_cast<EbmlString *>(&MyKaxTagSubGenre) = tag_list[current_tag_track]->Tag_SubGenre;
			}
			if (tag_list[current_tag_track]->Tag_Subject.length() > 0)
			{
				KaxTagSubject & MyKaxTagSubject = GetChild<KaxTagSubject>(*MyKaxTagGeneral);
				*static_cast<EbmlUnicodeString *>(&MyKaxTagSubject) = tag_list[current_tag_track]->Tag_Subject;
			}

			KaxTagMultiComment *MyKaxTagMultiComment_last = NULL;
			KaxTagCommercial *MyKaxTagMultiCommercial_last = NULL;
			KaxTagDate *MyKaxTagMultiDate_last = NULL;
			KaxTagIdentifier *MyKaxTagMultiIdentifier_last = NULL;
			KaxTagLegal *MyKaxTagMultiLegal_last = NULL;
			KaxTagTitle *MyKaxTagMultiTitle_last = NULL;
			KaxTagEntity *MyKaxTagMultiEntity_last = NULL;

			if (tag_list[current_tag_track]->Tag_MultiComments != NULL && tag_list[current_tag_track]->Tag_MultiComments->GetMultiCommentCount() > 0)
			{
				MatroskaTagMultiCommentItem *comment_item = NULL;

				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiComments->GetMultiCommentCount(); c++)
				{

					comment_item = tag_list[current_tag_track]->Tag_MultiComments->GetItem(c);
					if (comment_item != NULL && comment_item->IsValid())
					{
						KaxTagMultiComment *MyKaxTagMultiComment = NULL;
						if (MyKaxTagMultiComment_last != NULL) {
							MyKaxTagMultiComment = &GetNextChild<KaxTagMultiComment>(*MyKaxTag, *MyKaxTagMultiComment_last);
							MyKaxTagMultiComment_last = MyKaxTagMultiComment;
						}else {
							MyKaxTagMultiComment = &GetChild<KaxTagMultiComment>(*MyKaxTag);
							MyKaxTagMultiComment_last = MyKaxTagMultiComment;
						}
						if (comment_item->Name.length() > 0) {
							KaxTagMultiCommentName & MyKaxTagMultiCommentName = GetChild<KaxTagMultiCommentName>(*MyKaxTagMultiComment);
							*static_cast<EbmlString *>(&MyKaxTagMultiCommentName) = comment_item->Name;
						}

						if (comment_item->Language.length() > 0) {
							KaxTagMultiCommentLanguage & MyKaxTagMultiCommentLanguage = GetChild<KaxTagMultiCommentLanguage>(*MyKaxTagMultiComment);
							*static_cast<EbmlString *>(&MyKaxTagMultiCommentLanguage) = comment_item->Language;
						}
						if (comment_item->Comments.length() > 0) {
							KaxTagMultiCommentComments & MyKaxTagMultiCommentComments = GetChild<KaxTagMultiCommentComments>(*MyKaxTagMultiComment);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiCommentComments) = comment_item->Comments;
						}
					}
				}
			}
			if (tag_list[current_tag_track]->Tag_MultiCommercials != NULL && tag_list[current_tag_track]->Tag_MultiCommercials->GetMultiCommercialCount() > 0)
			{
				//Get the base element
				KaxTagMultiCommercial &MyKaxTagMultiCommercial_Base = GetChild<KaxTagMultiCommercial>(*MyKaxTag);

				MatroskaTagMultiCommercialItem *commercial_item = NULL;
				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiCommercials->GetMultiCommercialCount(); c++)
				{
					commercial_item = tag_list[current_tag_track]->Tag_MultiCommercials->GetItem(c);;
					if (commercial_item != NULL && commercial_item->IsValid())
					{
						KaxTagCommercial *MyKaxTagMultiCommercial = NULL;
						if (MyKaxTagMultiCommercial_last != NULL) {
							MyKaxTagMultiCommercial = &GetNextChild<KaxTagCommercial>(MyKaxTagMultiCommercial_Base, *MyKaxTagMultiCommercial_last);
							MyKaxTagMultiCommercial_last = MyKaxTagMultiCommercial;
						}else {
							MyKaxTagMultiCommercial = &GetChild<KaxTagCommercial>(MyKaxTagMultiCommercial_Base);
							MyKaxTagMultiCommercial_last = MyKaxTagMultiCommercial;
						}

						KaxTagMultiCommercialType & MyKaxTagMultiCommercialType = GetChild<KaxTagMultiCommercialType>(*MyKaxTagMultiCommercial);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiCommercialType) = commercial_item->Type;

						if (commercial_item->Address.length() > 0) {
							KaxTagMultiCommercialAddress & MyKaxTagMultiCommercialAddress = GetChild<KaxTagMultiCommercialAddress>(*MyKaxTagMultiCommercial);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiCommercialAddress) = commercial_item->Address;
						}
						if (commercial_item->Email.length() > 0) {
							KaxTagMultiCommercialEmail & MyKaxTagMultiCommercialEmail = GetChild<KaxTagMultiCommercialEmail>(*MyKaxTagMultiCommercial);
							*static_cast<EbmlString *>(&MyKaxTagMultiCommercialEmail) = commercial_item->Email;
						}
						if (commercial_item->PriceAmount != 0) {
							KaxTagMultiPriceAmount & MyKaxTagMultiPriceAmount = GetChild<KaxTagMultiPriceAmount>(*MyKaxTagMultiCommercial);
							*static_cast<EbmlFloat *>(&MyKaxTagMultiPriceAmount) = commercial_item->PriceAmount;
						}
						if (commercial_item->PriceCurrency.length() > 0) {
							KaxTagMultiPriceCurrency & MyKaxTagMultiPriceCurrency = GetChild<KaxTagMultiPriceCurrency>(*MyKaxTagMultiCommercial);
							*static_cast<EbmlString *>(&MyKaxTagMultiPriceCurrency) = commercial_item->PriceCurrency;
						}

						if (commercial_item->PriceDate != 0) {
							KaxTagMultiPricePriceDate & MyKaxTagMultiPricePriceDate = GetChild<KaxTagMultiPricePriceDate>(*MyKaxTagMultiCommercial);
							MyKaxTagMultiPricePriceDate.SetEpochDate((int32)commercial_item->PriceDate);
						}
						if (commercial_item->URL.length() > 0) {
							KaxTagMultiCommercialURL & MyKaxTagMultiCommercialURL = GetChild<KaxTagMultiCommercialURL>(*MyKaxTagMultiCommercial);
							*static_cast<EbmlString *>(&MyKaxTagMultiCommercialURL) = commercial_item->URL;
						}
					}
				}
			}
			if (tag_list[current_tag_track]->Tag_MultiDates != NULL && tag_list[current_tag_track]->Tag_MultiDates->GetMultiDateCount() > 0)
			{
				//Get the base element
				KaxTagMultiDate &MyKaxTagMultiDate_Base = GetChild<KaxTagMultiDate>(*MyKaxTag);

				MatroskaTagMultiDateItem *date_item = NULL;
				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiDates->GetMultiDateCount(); c++)
				{
					date_item = tag_list[current_tag_track]->Tag_MultiDates->GetItem(c);;
					if (date_item != NULL && date_item->IsValid())
					{
						KaxTagDate *MyKaxTagMultiDate = NULL;
						if (MyKaxTagMultiDate_last != NULL) {
							MyKaxTagMultiDate = &GetNextChild<KaxTagDate>(MyKaxTagMultiDate_Base, *MyKaxTagMultiDate_last);
							MyKaxTagMultiDate_last = MyKaxTagMultiDate;
						}else {
							MyKaxTagMultiDate = &GetChild<KaxTagDate>(MyKaxTagMultiDate_Base);
							MyKaxTagMultiDate_last = MyKaxTagMultiDate;
						}

						KaxTagMultiDateType & MyKaxTagMultiDateType = GetChild<KaxTagMultiDateType>(*MyKaxTagMultiDate);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiDateType) = date_item->Type;

						if (date_item->DateBegin != 0) {
							KaxTagMultiDateDateBegin & MyKaxTagMultiDateDateBegin = GetChild<KaxTagMultiDateDateBegin>(*MyKaxTagMultiDate);
							MyKaxTagMultiDateDateBegin.SetEpochDate(date_item->DateBegin);
						}
						if (date_item->DateEnd != 0) {
							KaxTagMultiDateDateEnd & MyKaxTagMultiDateDateEnd = GetChild<KaxTagMultiDateDateEnd>(*MyKaxTagMultiDate);
							MyKaxTagMultiDateDateEnd.SetEpochDate(date_item->DateEnd);
						}
					}
				}
			}
			if (tag_list[current_tag_track]->Tag_MultiEntities != NULL && tag_list[current_tag_track]->Tag_MultiEntities->GetMultiEntityCount() > 0)
			{
				//Get the base element
				KaxTagMultiEntity &MyKaxTagMultiEntity_Base = GetChild<KaxTagMultiEntity>(*MyKaxTag);

				MatroskaTagMultiEntityItem  *entity_item = NULL;
				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiEntities->GetMultiEntityCount(); c++)
				{
					entity_item = tag_list[current_tag_track]->Tag_MultiEntities->GetItem(c);;
					if (entity_item != NULL && entity_item->IsValid())
					{
						KaxTagEntity *MyKaxTagMultiEntity = NULL;
						if (MyKaxTagMultiEntity_last != NULL) {
							MyKaxTagMultiEntity = &GetNextChild<KaxTagEntity>(MyKaxTagMultiEntity_Base, *MyKaxTagMultiEntity_last);
							MyKaxTagMultiEntity_last = MyKaxTagMultiEntity;
						}else {
							MyKaxTagMultiEntity = &GetChild<KaxTagEntity>(MyKaxTagMultiEntity_Base);
							MyKaxTagMultiEntity_last = MyKaxTagMultiEntity;
						}

						KaxTagMultiEntityType & MyKaxTagMultiEntityType = GetChild<KaxTagMultiEntityType>(*MyKaxTagMultiEntity);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiEntityType) = entity_item->Type;

						if (entity_item->Address.length() > 0) {
							KaxTagMultiEntityAddress & MyKaxTagMultiEntityAddress = GetChild<KaxTagMultiEntityAddress>(*MyKaxTagMultiEntity);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiEntityAddress) = entity_item->Address;
						}
						if (entity_item->Email.length() > 0) {
							KaxTagMultiEntityEmail & MyKaxTagMultiEntityEmail = GetChild<KaxTagMultiEntityEmail>(*MyKaxTagMultiEntity);
							*static_cast<EbmlString *>(&MyKaxTagMultiEntityEmail) = entity_item->Email;
						}
						if (entity_item->Name.length() > 0) {
							KaxTagMultiEntityName & MyKaxTagMultiEntityName = GetChild<KaxTagMultiEntityName>(*MyKaxTagMultiEntity);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiEntityName) = entity_item->Name;
						}
						if (entity_item->URL.length() > 0) {
							KaxTagMultiEntityURL & MyKaxTagMultiEntityURL = GetChild<KaxTagMultiEntityURL>(*MyKaxTagMultiEntity);
							*static_cast<EbmlString *>(&MyKaxTagMultiEntityURL) = entity_item->URL;
						}
					}
				}
			}
			if (tag_list[current_tag_track]->Tag_MultiIdentifiers != NULL && tag_list[current_tag_track]->Tag_MultiIdentifiers->GetMultiIdentifierCount() > 0)
			{
				//Get the base element
				KaxTagMultiIdentifier &MyKaxTagMultiIdentifier_Base = GetChild<KaxTagMultiIdentifier>(*MyKaxTag);

				MatroskaTagMultiIdentifierItem *identifier_item = NULL;
				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiIdentifiers->GetMultiIdentifierCount(); c++)
				{
					identifier_item = tag_list[current_tag_track]->Tag_MultiIdentifiers->GetItem(c);;
					if (identifier_item != NULL && identifier_item->IsValid())
					{
						KaxTagIdentifier *MyKaxTagMultiIdentifier = NULL;
						if (MyKaxTagMultiIdentifier_last != NULL) {
							MyKaxTagMultiIdentifier = &GetNextChild<KaxTagIdentifier>(MyKaxTagMultiIdentifier_Base, *MyKaxTagMultiIdentifier_last);
							MyKaxTagMultiIdentifier_last = MyKaxTagMultiIdentifier;
						}else {
							MyKaxTagMultiIdentifier = &GetChild<KaxTagIdentifier>(MyKaxTagMultiIdentifier_Base);
							MyKaxTagMultiIdentifier_last = MyKaxTagMultiIdentifier;
						}

						KaxTagMultiIdentifierType & MyKaxTagMultiIdentifierType = GetChild<KaxTagMultiIdentifierType>(*MyKaxTagMultiIdentifier);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiIdentifierType) = identifier_item->Type;

						if (identifier_item->StringData.length() > 0) {
							KaxTagMultiIdentifierString & MyKaxTagMultiIdentifierString = GetChild<KaxTagMultiIdentifierString>(*MyKaxTagMultiIdentifier);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiIdentifierString) = identifier_item->StringData;
						}
					}
				}
			}
			if (tag_list[current_tag_track]->Tag_MultiLegals != NULL && tag_list[current_tag_track]->Tag_MultiLegals->GetMultiLegalCount() > 0)
			{
				//Get the base element
				KaxTagMultiLegal &MyKaxTagMultiLegal_Base = GetChild<KaxTagMultiLegal>(*MyKaxTag);

				MatroskaTagMultiLegalItem *legal_item = NULL;
				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiLegals->GetMultiLegalCount(); c++)
				{
					legal_item = tag_list[current_tag_track]->Tag_MultiLegals->GetItem(c);;
					if (legal_item != NULL && legal_item->IsValid())
					{
						KaxTagLegal *MyKaxTagMultiLegal = NULL;
						if (MyKaxTagMultiLegal_last != NULL) {
							MyKaxTagMultiLegal = &GetNextChild<KaxTagLegal>(MyKaxTagMultiLegal_Base, *MyKaxTagMultiLegal_last);
							MyKaxTagMultiLegal_last = MyKaxTagMultiLegal;
						}else {
							MyKaxTagMultiLegal = &GetChild<KaxTagLegal>(MyKaxTagMultiLegal_Base);
							MyKaxTagMultiLegal_last = MyKaxTagMultiLegal;
						}

						KaxTagMultiLegalType & MyKaxTagMultiLegalType = GetChild<KaxTagMultiLegalType>(*MyKaxTagMultiLegal);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiLegalType) = legal_item->Type;

						if (legal_item->Text.length() > 0) {
							//KaxTagMultiLegalText & MyKaxTagMultiLegalText = GetChild<KaxTagMultiLegalText>(*MyKaxTagMultiLegal);
							//*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiLegalText) = legal_item->Text;
						}

						if (legal_item->Address.length() > 0) {
							KaxTagMultiLegalAddress & MyKaxTagMultiLegalAddress = GetChild<KaxTagMultiLegalAddress>(*MyKaxTagMultiLegal);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiLegalAddress) = legal_item->Address;
						}
						if (legal_item->URL.length() > 0) {
							KaxTagMultiLegalURL & MyKaxTagMultiLegalURL = GetChild<KaxTagMultiLegalURL>(*MyKaxTagMultiLegal);
							*static_cast<EbmlString *>(&MyKaxTagMultiLegalURL) = legal_item->URL;
						}
					}
				}
			}
			if (tag_list[current_tag_track]->Tag_MultiTitles != NULL && tag_list[current_tag_track]->Tag_MultiTitles->GetMultiTitleCount() > 0)
			{
				//Get the base element
				KaxTagMultiTitle &MyKaxTagMultiTitle_Base = GetChild<KaxTagMultiTitle>(*MyKaxTag);

				MatroskaTagMultiTitleItem *title_item = NULL;
				for (int c = 0; c < tag_list[current_tag_track]->Tag_MultiTitles->GetMultiTitleCount(); c++)
				{
					title_item = tag_list[current_tag_track]->Tag_MultiTitles->GetItem(c);;
					if (title_item != NULL && title_item->IsValid())
					{
						KaxTagTitle *MyKaxTagMultiTitle = NULL;
						if (MyKaxTagMultiTitle_last != NULL) {
							MyKaxTagMultiTitle = &GetNextChild<KaxTagTitle>(MyKaxTagMultiTitle_Base, *MyKaxTagMultiTitle_last);
							MyKaxTagMultiTitle_last = MyKaxTagMultiTitle;
						}else {
							MyKaxTagMultiTitle = &GetChild<KaxTagTitle>(MyKaxTagMultiTitle_Base);
							MyKaxTagMultiTitle_last = MyKaxTagMultiTitle;
						}

						KaxTagMultiTitleType & MyKaxTagMultiTitleType = GetChild<KaxTagMultiTitleType>(*MyKaxTagMultiTitle);
						*static_cast<EbmlUInteger *>(&MyKaxTagMultiTitleType) = title_item->Type;

						if (title_item->Address.length() > 0) {
							KaxTagMultiTitleAddress & MyKaxTagMultiTitleAddress = GetChild<KaxTagMultiTitleAddress>(*MyKaxTagMultiTitle);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiTitleAddress) = title_item->Address;
						}
						if (title_item->Edition.length() > 0) {
							KaxTagMultiTitleEdition & MyKaxTagMultiTitleEdition = GetChild<KaxTagMultiTitleEdition>(*MyKaxTagMultiTitle);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiTitleEdition) = title_item->Edition;
						}
						if (title_item->Email.length() > 0) {
							KaxTagMultiTitleEmail & MyKaxTagMultiTitleEmail = GetChild<KaxTagMultiTitleEmail>(*MyKaxTagMultiTitle);
							*static_cast<EbmlString *>(&MyKaxTagMultiTitleEmail) = title_item->Email;
						}
						if (title_item->Language.length() > 0) {
							KaxTagMultiTitleLanguage & MyKaxTagMultiTitleLanguage = GetChild<KaxTagMultiTitleLanguage>(*MyKaxTagMultiTitle);
							*static_cast<EbmlString *>(&MyKaxTagMultiTitleLanguage) = title_item->Language;
						}
						if (title_item->Name.length() > 0) {
							KaxTagMultiTitleName & MyKaxTagMultiTitleName = GetChild<KaxTagMultiTitleName>(*MyKaxTagMultiTitle);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiTitleName) = title_item->Name;
						}
						if (title_item->SubTitle.length() > 0) {
							KaxTagMultiTitleSubTitle & MyKaxTagMultiTitleSubTitle = GetChild<KaxTagMultiTitleSubTitle>(*MyKaxTagMultiTitle);
							*static_cast<EbmlUnicodeString *>(&MyKaxTagMultiTitleSubTitle) = title_item->SubTitle;
						}
						if (title_item->URL.length() > 0) {
							KaxTagMultiTitleURL & MyKaxTagMultiTitleURL = GetChild<KaxTagMultiTitleURL>(*MyKaxTagMultiTitle);
							*static_cast<EbmlString *>(&MyKaxTagMultiTitleURL) = title_item->URL;
						}
					}
				}
			}
#endif
			if (tag_list[current_tag_track]->Tag_Simple_Tags.size() > 0) {
				// Add the millions of simple tags we can have ;)
				KaxTagSimple *MySimpleTag_last = NULL;
				for(size_t st = 0; st < tag_list[current_tag_track]->Tag_Simple_Tags.size(); st++) {
					KaxTagSimple *MySimpleTag = NULL;
					if (MySimpleTag_last != NULL)
					{
						MySimpleTag = &GetNextChild<KaxTagSimple>(*MyKaxTag, *MySimpleTag_last);
						MySimpleTag_last = MySimpleTag;
					}else {
						MySimpleTag = &GetChild<KaxTagSimple>(*MyKaxTag);
						MySimpleTag_last = MySimpleTag;
					}
					MatroskaSimpleTag *currentSimpleTag = tag_list[current_tag_track]->Tag_Simple_Tags.at(st);

					KaxTagName & MyKaxTagName = GetChild<KaxTagName>(*MySimpleTag);
					*static_cast<EbmlUnicodeString *>(&MyKaxTagName) = currentSimpleTag->name;

					KaxTagString & MyKaxTagString = GetChild<KaxTagString>(*MySimpleTag);
					*static_cast<EbmlUnicodeString *>(&MyKaxTagString) = currentSimpleTag->value;
				}
			}

			// Clean out empty tag groups
#if 0
			if (MyKaxTagTargets.ListSize() == 0) {
				for (uint32 i = 0; i < MyKaxTag->ListSize(); i++) {
					if ((*MyKaxTag)[i] == &MyKaxTagTargets) {
						MyKaxTag->Remove(i);				
						// Restart the search
						i = 0;
					}
				}
			}
#endif
#ifndef MATROSKA_INFO
			if (MyKaxTagGeneral->ListSize() == 0) {
				for (uint32 i = 0; i < MyKaxTag->ListSize(); i++) {
					if ((*MyKaxTag)[i] == MyKaxTagGeneral) {
						MyKaxTag->Remove(i);				
						// Restart the search
						i = 0;
					}
				}
			}

			if (MyKaxTagGenres->ListSize() == 0) {
				for (uint32 i = 0; i < MyKaxTag->ListSize(); i++) {
					if ((*MyKaxTag)[i] == MyKaxTagGenres) {
						MyKaxTag->Remove(i);				
						// Restart the search
						i = 0;
					}
				}
			}

			if (MyKaxTagAudioSpecific->ListSize() == 0) {
				for (uint32 i = 0; i < MyKaxTag->ListSize(); i++) {
					if ((*MyKaxTag)[i] == MyKaxTagAudioSpecific) {
						MyKaxTag->Remove(i);				
						// Restart the search
						i = 0;
					}
				}
			}

			if (MyKaxTagImageSpecific->ListSize() == 0) {
				for (uint32 i = 0; i < MyKaxTag->ListSize(); i++) {
					if ((*MyKaxTag)[i] == MyKaxTagImageSpecific) {
						MyKaxTag->Remove(i);				
						// Restart the search
						i = 0;
					}
				}
			}
#endif
		}

		EbmlVoid & Dummy = GetChild<EbmlVoid>(*static_cast<EbmlMaster *>(l0));
		uint64 size_of_tags = orig_tag_size;
		MyKaxTags.UpdateSize(true);
		if (size_of_tags < MyKaxTags.GetSize())
			size_of_tags = MyKaxTags.GetSize();

		Dummy.SetSize(size_of_tags+8); // Size of the previous tag element
		uint64 pos = inout_file.getFilePointer();		
		if ((pos == 0) && (m_tagElementPos == 0)) {
			inout_file.setFilePointer(file_size);
			pos = inout_file.getFilePointer();		
		}
		if (pos == 0)			
			//We don't want to overwrite the EBML header :P
			throw std::runtime_error(_("Failed to seek in file."));

		// Check that there are no elements following the Tag
		
		l1 = es.FindNextElement(l0->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, false, 1);
		std::auto_ptr<EbmlElement> unknown_element(l1);
		if (unknown_element.get() != NULL) {	
			throw std::runtime_error(_("Tags are not at the end of the file!"));
		}

		Dummy.Render(inout_file);
		inout_file.setFilePointer(Dummy.GetElementPosition());
		pos = inout_file.getFilePointer();
		if (pos == 0)
			//We don't want to overwrite the EBML header :P
			throw std::runtime_error(_("Failed to seek in file."));

		MyKaxTags.Render(inout_file);
		
		// Truncate the file
		inout_file.GetWinIOCallback()->SetEOF();

		//l0->UpdateSize(false, true);
		std::auto_ptr<KaxSegment> new_segment(new KaxSegment);		

    inout_file.setFilePointer(segment->GetElementPosition());
    new_segment->WriteHead(inout_file, segment->HeadSize() - 4);
    inout_file.setFilePointer(0, seek_end);
		int ret = new_segment->ForceSize(inout_file.getFilePointer() -
                                segment->HeadSize() -
                                segment->GetElementPosition());
    if (!ret) { 
      segment->OverwriteHead(inout_file);

			throw std::runtime_error(_("Tags were written. However the "
                      "segment size couldn't be updated. Therefore the tags may not be "
                      "visible to some programs, as this is a bending of the specs."));
		} else {
      new_segment->OverwriteHead(inout_file);      
		}

		return 0;
	} catch (std::exception &ex) {
		std::string errorMsg;
		errorMsg = _("Failed to write tags.");
		errorMsg += "\n";
		errorMsg += _("Exception: ");
		errorMsg += ex.what();	
		
		throw std::runtime_error(errorMsg);		
	}
};

};// End namespace MatroskaUtilsNamespace

