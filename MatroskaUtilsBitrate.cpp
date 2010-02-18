/**
	*  Part of The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
	*
	*  MatroskaUtilsBitrate.cpp
	*
	*  Copyright (C) Jory Stone - June 2003
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
    \file MatroskaUtilsBitrate.cpp
		\version \$Id: MatroskaUtilsBitrate.cpp,v 1.7 2004/03/20 05:08:31 jcsston Exp $
    \brief Some code to get bitrate infomation about a Matroska file
		\author Jory Stone     <jcsston @ toughguy.net>		
		
*/

#include "MatroskaUtilsBitrate.h"

namespace MatroskaUtilsNamespace {

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

MatroskaBitrateInfoTrack::MatroskaBitrateInfoTrack()
{
	avgBitrate = 0;
	bitrateLength = 0;
	duration = 0;
	maxBitrate = 0;
	minBitrate = 0;
}

void MatroskaBitrateInfoTrack::ProcessBitrateVector()
{
	avgBitrate = 0;
	maxBitrate = 0;
	minBitrate = 0;	
	for (size_t b = 0; b < bitrates.size(); b++) {
		float currentBitrate = bitrates.at(b);

		if (currentBitrate > maxBitrate)
			maxBitrate = currentBitrate;

		if (minBitrate == 0)
			minBitrate = currentBitrate;

		if (currentBitrate < minBitrate)
			minBitrate = currentBitrate;

		avgBitrate += currentBitrate;
	}
	avgBitrate = avgBitrate / bitrates.size();
}

MatroskaBitrateInfoReader::MatroskaBitrateInfoReader(MatroskaBitrateInfo *info) 
	: m_TextOutput(info->m_TextOutput)
{
	m_Info = info;	
	m_TimecodeScale = TIMECODE_SCALE;
};

MatroskaBitrateInfoReader::~MatroskaBitrateInfoReader() {

};

void *MatroskaBitrateInfoReader::Entry() {
	try {
		MatroskaUilsFileReader reader(m_Info->m_SourceFilename.c_str(), MODE_READ);

		int UpperElementLevel = 0;
		bool bAllowDummy = false;
		// Elements for different levels
		EbmlElement *ElementLevel0 = NULL;
		EbmlElement *ElementLevel1 = NULL;
		EbmlElement *ElementLevel2 = NULL;
		EbmlElement *ElementLevel3 = NULL;
		EbmlElement *ElementLevel4 = NULL;
		//EbmlElement *ElementLevel5 = NULL;

		EbmlStream inputStream(reader);

		// Find the EbmlHead element. Must be the first one.
		ElementLevel0 = inputStream.FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFFFFFFFFFL);
		if (ElementLevel0 == NULL) {
			m_Info->hr = 2;
			return 0;
		}
		//We must have found the EBML head :)
		ElementLevel0->SkipData(inputStream, ElementLevel0->Generic().Context);
		delete ElementLevel0;

		// Next element must be a segment
		ElementLevel0 = inputStream.FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFFFFFFFFFL);
		if (ElementLevel0 == NULL) {
			//No segment/level 0 element found.
			m_Info->hr = 2;
			return 0;
		}
		if (!(EbmlId(*ElementLevel0) == KaxSegment::ClassInfos.GlobalId)) {
			delete ElementLevel0;
			m_Info->hr = 2;
			return 0;
		}

		float timecode = 0;
		std::vector<float> prevTimecodes;
		std::vector<float> currentSize;
		currentSize.resize(m_Info->m_TrackList.size());
		prevTimecodes.resize(m_Info->m_TrackList.size());
		UpperElementLevel = 0;
		// We've got our segment, so let's find the tracks
		ElementLevel1 = inputStream.FindNextElement(ElementLevel0->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
		while (ElementLevel1 != NULL) {
			TRACE1("Parsing Level 1 element %s", typeid(*ElementLevel1).name());
			if (UpperElementLevel > 0) {
				break;
			}
			if (UpperElementLevel < 0) {
				UpperElementLevel = 0;
			}

			if (EbmlId(*ElementLevel1) == KaxInfo::ClassInfos.GlobalId) {
				ElementLevel2 = inputStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
				while (ElementLevel2 != NULL) {
					if (UpperElementLevel > 0) {
						break;
					}
					if (UpperElementLevel < 0) {
						UpperElementLevel = 0;
					}

					if (EbmlId(*ElementLevel2) == KaxTimecodeScale::ClassInfos.GlobalId) {
						KaxTimecodeScale &TimeScale = *static_cast<KaxTimecodeScale *>(ElementLevel2);
						TimeScale.ReadData(inputStream.I_O());

						m_TimecodeScale = uint64(TimeScale);
					}

					if (UpperElementLevel > 0) {	// we're coming from l3
						UpperElementLevel--;
						delete ElementLevel2;
						ElementLevel2 = ElementLevel3;
						if (UpperElementLevel > 0)
							break;
					} else {
						ElementLevel2->SkipData(inputStream, ElementLevel2->Generic().Context);
						delete ElementLevel2;
						ElementLevel2 = inputStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
					}
				}
				
			} else if (EbmlId(*ElementLevel1) == KaxCluster::ClassInfos.GlobalId) {
				KaxCluster *SegmentCluster = static_cast<KaxCluster *>(ElementLevel1);
				uint32 ClusterTimecode = 0;

				// read blocks and discard the ones we don't care about
				ElementLevel2 = inputStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
				while (ElementLevel2 != NULL) {
					if (UpperElementLevel > 0) {
						break;
					}
					if (UpperElementLevel < 0) {
						UpperElementLevel = 0;
					}
					if (EbmlId(*ElementLevel2) == KaxClusterTimecode::ClassInfos.GlobalId) {						
						KaxClusterTimecode & ClusterTime = *static_cast<KaxClusterTimecode*>(ElementLevel2);
						ClusterTime.ReadData(inputStream.I_O());
						ClusterTimecode = uint32(ClusterTime);
						SegmentCluster->InitTimecode(ClusterTimecode, m_TimecodeScale);
					} else  if (EbmlId(*ElementLevel2) == KaxBlockGroup::ClassInfos.GlobalId) {
						KaxBlockGroup & aBlockGroup = *static_cast<KaxBlockGroup*>(ElementLevel2);

						ElementLevel3 = inputStream.FindNextElement(ElementLevel2->Generic().Context, UpperElementLevel, ElementLevel2->ElementSize(), bAllowDummy);
						while (ElementLevel3 != NULL) {
							if (UpperElementLevel > 0) {
								break;
							}
							if (UpperElementLevel < 0) {
								UpperElementLevel = 0;
							}
							if (EbmlId(*ElementLevel3) == KaxBlock::ClassInfos.GlobalId) {								
								KaxBlock & DataBlock = *static_cast<KaxBlock*>(ElementLevel3);								

								DataBlock.SetParent(*SegmentCluster);
								//DataBlock.ReadData(inputStream.I_O(), SCOPE_PARTIAL_DATA);
								DataBlock.ReadInternalHead(inputStream.I_O());
								
								uint16 trackNum = DataBlock.TrackNum() - 1;
								
								MatroskaBitrateInfoTrack &track = m_Info->m_TrackList.at(trackNum);
								//wxLogDebug("Track # %u / %u frame%s / m_TimecodeScale %I64d\n",DataBlock.TrackNum(), DataBlock.NumberFrames(), (DataBlock.NumberFrames() > 1)?"s":"", DataBlock.GlobalTimecode());
								timecode = (float)DataBlock.GlobalTimecode() / (float)m_TimecodeScale / (float)1000.0;
								
								if (timecode - prevTimecodes.at(trackNum) > track.bitrateLength) {
									track.bitrates.push_back(currentSize.at(trackNum));
									if (m_Info->m_TargetTextCtrl != NULL) {										
										TCHAR buffer[256];
										_sntprintf(buffer, 255, _T("Track: %i, %i - %i s: %.2f kb \r\n"), trackNum+1, track.bitrates.size()-1, track.bitrates.size(), (float)currentSize.at(trackNum) / 1024);
										m_TextOutput += buffer;									

										SendMessage(m_Info->m_TargetTextCtrl, WM_SETTEXT, 0, (LPARAM)m_TextOutput.c_str());
										SendMessage(m_Info->m_TargetTextCtrl, EM_SETSEL, m_TextOutput.length()-1, m_TextOutput.length());
										SendMessage(m_Info->m_TargetTextCtrl, EM_SCROLLCARET, 0, 0);
									}
									currentSize.at(trackNum) = 0;
									prevTimecodes.at(trackNum) = timecode;
								} else {
									currentSize.at(trackNum) += DataBlock.GetSize();
								}								

							}
							if (UpperElementLevel > 0) {
								UpperElementLevel--;
								delete ElementLevel3;
								ElementLevel3 = ElementLevel4;
								if (UpperElementLevel > 0)
									break;
							} else {
								ElementLevel3->SkipData(inputStream, ElementLevel3->Generic().Context);
								delete ElementLevel3;
								ElementLevel3 = NULL;

								if (!TestDestroy())
									ElementLevel3 = inputStream.FindNextElement(ElementLevel2->Generic().Context, UpperElementLevel, ElementLevel2->ElementSize(), bAllowDummy);
							}							
						}
					}

					if (UpperElementLevel > 0) {
						UpperElementLevel--;
						delete ElementLevel2;
						ElementLevel2 = ElementLevel3;
						if (UpperElementLevel > 0)
							break;
					} else {
						ElementLevel2->SkipData(inputStream, ElementLevel2->Generic().Context);
						delete ElementLevel2;								
						ElementLevel2 = NULL;

						if (!TestDestroy())
							ElementLevel2 = inputStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
					}
				}
			}

			// Update current pos
			m_Info->m_FilePosCritSec.Lock();
			m_Info->m_FilePos = reader.getFilePointer();
			m_Info->m_FilePosCritSec.Unlock();

			if (UpperElementLevel > 0) {		// we're coming from ElementLevel2
				UpperElementLevel--;
				delete ElementLevel1;
				ElementLevel1 = ElementLevel2;
				if (UpperElementLevel > 0)
					break;
			} else {
				ElementLevel1->SkipData(inputStream, ElementLevel1->Generic().Context);
				delete ElementLevel1;
				ElementLevel1 = NULL;

				if (!TestDestroy())
					ElementLevel1 = inputStream.FindNextElement(ElementLevel0->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFL, true, 1);
			}
		} // while (ElementLevel1 != NULL		
		delete ElementLevel0;

		// We are done parsing
		m_Info->m_FilePosCritSec.Lock();
		m_Info->m_FilePos = m_Info->m_FileSize;
		m_Info->m_FilePosCritSec.Unlock();
				
		if (m_Info->m_TargetTextCtrl != NULL) {										
			TRACE("Sending bitrate report to TextCtrl");
			TCHAR buffer[256];
			for (size_t t = 0; t < m_Info->m_TrackList.size(); t++) {
				MatroskaBitrateInfoTrack &localTrack = m_Info->m_TrackList.at(t);			
				localTrack.ProcessBitrateVector();
				_sntprintf(buffer, 255, _T("Track: %i \r\n\tMin: %.2f kbit/s \r\n\tMax: %.2f kbit/s \r\n\tAvg Bitrate: %.2f kbit/s \r\n"), t+1, localTrack.minBitrate / 1024 * 8, localTrack.maxBitrate / 1024 * 8, localTrack.avgBitrate / 1024 * 8);
				m_TextOutput += buffer;									
			}
			SendMessage(m_Info->m_TargetTextCtrl, WM_SETTEXT, 0, (LPARAM)m_TextOutput.c_str());
			SendMessage(m_Info->m_TargetTextCtrl, EM_SETSEL, m_TextOutput.length()-1, m_TextOutput.length());
			SendMessage(m_Info->m_TargetTextCtrl, EM_SCROLLCARET, 0, 0);
		}

		/*TRACE("Bitrate Report");
		for (size_t t = 0; t < m_Info->m_TrackList.size(); t++) {
			MatroskaBitrateInfoTrack &localTrack = m_Info->m_TrackList.at(t);
			TRACE2(" Track: %i Duration: %i", t, (long)localTrack.duration);
			
			localTrack.ProcessBitrateVector();

			for (size_t b = 0; b < localTrack.bitrates.size(); b++) {
				TRACE2(" Bitrate: %i Size: %i", b, (long)localTrack.bitrates.at(b));
			}						
		}*/

	} catch (std::exception &ex) {
		NOTE1("std::exception in MatroskaBitrateInfoReader::Entry() '%s'", ex.what());
		m_Info->hr = -1;
		return 0;

	} catch (...) {
		m_Info->hr = -2;
		return 0;
	}

	m_Info->hr = 0;

	return 0;
};

MatroskaBitrateInfo::MatroskaBitrateInfo() {
	m_FileSize = 0;
	m_FilePos = 0;
	hr = -255;
	m_Reader = NULL;
	m_TargetTextCtrl = NULL;
}

MatroskaBitrateInfo::~MatroskaBitrateInfo() {
	if ((hr == -255) && (m_Reader != NULL))
		m_Reader->Stop();	
}

int MatroskaBitrateInfo::Open(MatroskaInfoParser *parser) {
	assert(parser != NULL);
	m_Parser = parser;
	if (parser->trackInfos.size() == 0)
		// No Tracks
		return -1;

	m_SourceFilename = parser->GetFilename();
	m_FileSize = parser->file_size;
	
	float duration = parser->m_segmentInfo.GetDuration();
	m_TrackList.resize(parser->trackInfos.size());
	for (size_t t = 0; t < parser->trackInfos.size(); t++) {
		MatroskaTrackInfo *track = parser->trackInfos.at(t);
		MatroskaBitrateInfoTrack &localTrack = m_TrackList.at(t);
		localTrack.duration = duration;
		localTrack.bitrateLength = 1;
		localTrack.uid = track->m_trackUID;
		localTrack.bitrates.reserve((size_t)(localTrack.duration / localTrack.bitrateLength));
	}

	m_Reader = new MatroskaBitrateInfoReader(this);
	m_Reader->Run();

	return 0;
};

int MatroskaBitrateInfo::Save()
{
	for (size_t t = 0; t < m_TrackList.size(); t++) {		
		MatroskaBitrateInfoTrack &localTrack = m_TrackList.at(t);
		MatroskaTagInfo *tag = m_Parser->tags.FindByTrackUID(localTrack.uid);
		
		if (tag == NULL) {
			tag = new MatroskaTagInfo;
			tag->Tag_TrackUID = localTrack.uid;
			m_Parser->tags.push_back(tag);
		}
		char bitrateBuffer[256];
		snprintf(bitrateBuffer, 255, "%i", (int)(localTrack.avgBitrate / 8));
		tag->Tag_Simple_Tags.SetSimpleTagValue("BITSPS", bitrateBuffer);
	}

	return m_Parser->WriteTags();
}

int MatroskaBitrateInfo::Draw(HWND hWnd, DWORD x1, DWORD y1, DWORD x2, DWORD y2) {

	HDC hdc = GetDC(hWnd);
	// Generate the graph

	
	// Get the longest track length and min/max bitrates
	double totalDuration = 0;
	float minTotalBitrate = 0;
	float maxTotalBitrate = 0;

	for (size_t t = 0; t < m_TrackList.size(); t++) {
		MatroskaBitrateInfoTrack &track = m_TrackList.at(t);
		if (track.duration > totalDuration)
			totalDuration = track.duration;
		if (track.minBitrate > minTotalBitrate)
			minTotalBitrate = track.minBitrate;
		if (track.maxBitrate > maxTotalBitrate)
			maxTotalBitrate = track.maxBitrate;
	}

	DWORD topY = 0;
	// Draw each graph
	for (size_t t = 0; t < m_TrackList.size(); t++) {
		MatroskaBitrateInfoTrack &track = m_TrackList.at(t);	
		int p = 0;
		POINT *points = new POINT[track.bitrates.size()+1];
		points[p].x = x1;
		points[p].y = topY;
		
		Polyline(hdc, points, p);

		delete points;
	}
	/*BOOL Polyline(
  HDC hdc,            // handle to device context
  CONST POINT *lppt,  // array of endpoints
  int cPoints         // number of points in array
);*/

	return 0;
};

void MatroskaBitrateInfo::SetReportTextCtrl(HWND txtCtrl) {
	m_TargetTextCtrl = txtCtrl;
}

uint64 MatroskaBitrateInfo::GetFilePos() {
	JCAutoLockCritSec lockFilePos(&m_FilePosCritSec);
	return m_FilePos;
};

uint8 MatroskaBitrateInfo::GetCurrentPercent() {
	if (m_FileSize == 0)
		return 100;

	return (uint8)((100.0 / (float)m_FileSize) * (float)GetFilePos() + 0.5);
}

void MatroskaBitrateInfo::Stop()
{
	if (hr == -255) {
		m_Reader->Stop();
		m_Reader = NULL;
	}
	//while (hr == -255) Sleep(25);
}

};// End namespace MatroskaUtilsNamespace
