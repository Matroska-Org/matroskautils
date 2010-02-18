// ----------------------------------------------------------------------------
// Core Dynamic Library - Plugin framework for The Core Media Player
//
//      http://www.corecoded.com - http://www.corecodec.com
//     Actually used to control the player using the CDL API
//                     CDL API Version 1.01 (101)
//
//                          (C/C++ header)
//
// Ok, here we goes, the SDK for TCMP, just for you, so you can make the player
// even better! Very easy to use, you shouldn't have any problems using it.
// While I'm trying to make this SDK, I may forget to bind some functions that
// you will need while developing your CDL, or if you have some kick-ass ideas
// there is some good chances that I will add them to the CDL SDK. Come on our
// sweet forum and post your wishes!
//
// I hesitated between writing a documentation and commenting the CDL header,
// as you can see I choose the 2nd option for my health. You can find a lot of
// informations in CDL_Howto.txt
// Don't forget: have fun while coding!
//
//  CoreCodec SDK license:
//  Copyright (c) 2001-2002 CoreCodec, Inc.
//
//    This software is provided 'as-is', without any express or implied
//    warranty.  In no event will the authors be held liable for any damages
//    arising from the use of this software.
//
//    Permission is granted to anyone to use this software for any purpose,
//    including commercial applications, and to alter it and redistribute it
//    freely, subject to the following restrictions:
//
//    1. The origin of this software must not be misrepresented; you must not
//       claim that you wrote the original software. If you use this software
//       in a product, an acknowledgment in the product documentation would be
//       appreciated but is not required.
//    2. Altered source versions must be plainly marked as such, and must not be
//       misrepresented as being the original software.
//    3. This notice may not be removed or altered from any source distribution.
//
//
// Ludovic 'BlackSun' Vialle, CoreCodec Lead-developer.
// C/C++ Conversion by Christophe 'Toff' PARIS
// This version was modifed by Jory 'jcsston' Stone to attempt support for v1.01 of the CDL API
// 
// ----------------------------------------------------------------------------
#ifndef _CDLAPI_H_
#define _CDLAPI_H_
// ----------------------------------------------------------------------------

#include <windows.h>

// ----------------------------------------------------------------------------

#define CDL_API_CALL        __stdcall

// ----------------------------------------------------------------------------

#ifdef __cplusplus
#define CDL_API_EXPORT      extern "C" __declspec(dllexport)
#else
#define CDL_API_EXPORT      __declspec(dllexport)
#endif 

// ----------------------------------------------------------------------------

// Error and success codes for the plugin initialization
#define CORE_OK                 1       // Your CDL is loaded
#define CORE_NOT_OK             10      // Report this if you don't want to be loaded

// Events notification codes
#define CORE_FILELOADING        601     // A file is loading (Param1 == filename)
#define CORE_FILELOADED         602     // TCMP is done with loading the file (Param1 == filename)
#define CORE_FILECLOSED         603     // TCMP is closing the file (no params) (also used for state)

#define CORE_FILEPLAY           610     // Playback state (no params)
#define CORE_FILEPAUSE          611     // Playback state (no params)
#define CORE_FILESTOP           612     // Playback state (no params)
#define CORE_FULLSCREEN         615     // Fullscreen mode change (Param2:  0 == false or 1 == true)
#define CORE_FILESEEK           616     // Seeking in the file
#define CORE_VIDEOSIZECHANGE    617     // Video size change

// Plugins flags (what the plugin support)
#define CORE_NOFLAG             0x0
#define CORE_SHOWWINDOW         0x10    // ShowWindow event
#define CORE_SHORTCUTEXEC       0x20    // ShortcutExec event
#define CORE_MEDIAPROPEXEC      0x40    // MediaPropExec event
#define CORE_EVENTHANDLER       0x80    // EventHandler event
#define CORE_READTAGS           0x100   // ReadTags event (API v101)
#define CORE_WRITETAGS          0x200   // WriteTags event (API v101)

// ----------------------------------------------------------------------------

// -- Your exports/functions --
typedef struct
{
    char* MediaType;
    char* TagList;
    char* Title;
    char* Artist;
    char* Album;
    short Track;
    int Year;
    char* Genre;
    int Bitrate; // Mainly used for audio, express it in KB/s
    short Rating; // User rating from 1 to 10
    char* Extension; // File extension in UPPERCASE without the dot (ie: "MP3")
    char* Codec; // ie: "DivX 6.0"
    char* EncodingStyle; // ie: "VBR" or "SV8"
    char* Comment;
    char* Extended; // supplementary metadata, in an XML like presentation <compositor>W.A.Mozart</compositor>
}	CDL_Metadata, *PCDL_Metadata;


typedef struct
{
    // version of your CDL (ie: '1.0 alpha')
    char* version;
    // The name of your CDL
    char* name;
    // Proc called when the player init your plugin
    int (CDL_API_CALL *LoadProc)(int version);
    // Proc called when the user want to configure your CDL
    // You may just show an about box if you hijacked your settings
    // (you can use hwndParent)
    void (CDL_API_CALL *ConfigProc)(void);
    // Called when the player unload your CDL
    void (CDL_API_CALL *UnloadProc)(void);
    // The player give its handle in hwndParent
    HWND hwndParent;
    // Set the plugins caps. ie: CORE_SHOWWINDOW | CORE_EVENTHANDLER
    DWORD dwflags;
    // -- the following functions aren't mandatory --
    // If a registered window is selected, the player ask you if you are
    // the owner, return true if this is the case, otherwise return false.
    BOOL (CDL_API_CALL *ShowWindow)(GUID ID);
    // Same, but with a shortcut
    BOOL (CDL_API_CALL *ShortcutExec)(GUID ID);
    // The player ask you if you want to add your tab to the media properties
    // the filename is sent so you can open it to read the tags for example.
    // Return a string with the title, or an empty string.
    char* (CDL_API_CALL *MediaPropExec)(char* Filename);
    // Event handler. TCMP will notify you of the most important events
    // Depending of the event, the player may send you a parameter, it can be
    // a string (Param1 ('' == null) or an integer (Param2 (-1 == null)
    void (CDL_API_CALL *EventHandler)(int Event, char* Param1, int Param2);
    // This API is used by TCMP to get some standard tags from a file, it will be used
    // for the playlist and the medialib. (API v101)
    void (CDL_API_CALL *ReadTags)(char *Filename, CDL_Metadata& Metadata);
    // Same as the previous one, but TCMP will ask you to write the tags, use the prefered
    // tag format. (API v101)
    void (CDL_API_CALL *WriteTags)(char *Filename, CDL_Metadata Metadata);

} CDL_PluginHeader, *PCDL_PluginHeader;

// ----------------------------------------------------------------------------

// -- The Core Media Player exports/functions at your service --
typedef struct
{
    // Get the CDL version supported by the player
    int (CDL_API_CALL *CDL_GetVersion)(void);
    // Popup a OpenDialog box, with your filter and return the filename
    // return an empty string if the user pressed cancel.
    char* (CDL_API_CALL *CDL_OpenDialog)(char* filter);
    // Same with a SaveDialog box
    char* (CDL_API_CALL *CDL_SaveDialog)(char* filter);
    // Load a file, can be a video, audio or subtitles or a video_ts.ifo
    // if the version of the player allow it.
    void (CDL_API_CALL *CDL_LoadFile)(char* filename);   
    // Get the filename of the current opened file
    char* (CDL_API_CALL *CDL_GetFilename)(void);
    // Previous button
    void (CDL_API_CALL *CDL_PreviousItem)(void);
    // Next button
    void (CDL_API_CALL *CDL_NextItem)(void);
    // Play/Pause toggle button
    void (CDL_API_CALL *CDL_PlayPause)(void);
    // Stop button
    void (CDL_API_CALL *CDL_Stop)(void);
    // Get or set the current position. Don't call it every ms or you'll
    // overload the cpu :) The time is returned in milliseconds.
    int (CDL_API_CALL *CDL_GetPosition)(void);
    void (CDL_API_CALL *CDL_SetPosition)(int position);
    // Get  the duration of the opened file in milliseconds
    int (CDL_API_CALL *CDL_GetDuration)(void);   
    // Get or set the volume value. min: -10000, max: 0 (log)
    int (CDL_API_CALL *CDL_GetVolume)(void);
    void (CDL_API_CALL *CDL_SetVolume)(int position);
    // Move the volume slider of the player up or down
    void (CDL_API_CALL *CDL_VolumeUp)(void);
    void (CDL_API_CALL *CDL_VolumeDown)(void);   
    // Seek to the previous or next bookmark
    void (CDL_API_CALL *CDL_PreviousBookmark)(void);
    void (CDL_API_CALL *CDL_NextBookmark)(void);
    // Jump backward or forward (seeking method)
    void (CDL_API_CALL *CDL_JumpBackward)(void);
    void (CDL_API_CALL *CDL_JumpForward)(void);
    // Retrieve or set the fullscreen mode (boolean)
    BOOL (CDL_API_CALL *CDL_GetFullScreen)(void);
    void (CDL_API_CALL *CDL_SetFullScreen)(void);
    // DVD/PDM and futur Menu navigation procs
    void (CDL_API_CALL *CDL_MenuUp)(void);
    void (CDL_API_CALL *CDL_MenuDown)(void);
    void (CDL_API_CALL *CDL_MenuLeft)(void);
    void (CDL_API_CALL *CDL_MenuRight)(void);
    void (CDL_API_CALL *CDL_MenuSelect)(void);
    // Call the DVD menus
    void (CDL_API_CALL *CDL_DVDRootMenu)(void);
    void (CDL_API_CALL *CDL_DVDAudioMenu)(void);
    void (CDL_API_CALL *CDL_DVDTitleMenu)(void);
    void (CDL_API_CALL *CDL_DVDSubpicMenu)(void);
    void (CDL_API_CALL *CDL_DVDAngleMenu)(void);
    // Cycle display aspect ratio
    void (CDL_API_CALL *CDL_CycleDAR)(void);
    // Cycle the audio streams
    void (CDL_API_CALL *CDL_CycleAudio)(void);
    // Cycle the subtitles streams/files (apply with Nefertiti)
    void (CDL_API_CALL *CDL_CycleSub)(void);
    // Get or set the repeat state
    BOOL (CDL_API_CALL *CDL_GetRepeat)(void);
    void (CDL_API_CALL *CDL_SetRepeat)(void);
    // Get or set the shuffle state
    BOOL (CDL_API_CALL *CDL_GetShuffle)(void);
    void (CDL_API_CALL *CDL_SetShuffle)(void);
    // Get the video height or width. Take in count the modified size
    // (ie: fullscreen or modified AR)
    int (CDL_API_CALL *CDL_GetVideoHeight)(void);
    int (CDL_API_CALL *CDL_GetVideoWidth)(void);
    // Set a new size. The video is automatically centered
    void (CDL_API_CALL *CDL_SetVideoSize)(int height, int width);
    // Set the PanScan value. From 25% to 400%
    int (CDL_API_CALL *CDL_GetPanScan)(void);
    void (CDL_API_CALL *CDL_SetPanScan)(int percent);
    // Set a new Display Aspect Ratio
    void (CDL_API_CALL *CDL_SetDAR)(int RatioC, int RatioD);
    // Set a new Pixels Aspect Ratio
    void (CDL_API_CALL *CDL_SetPAR)(int RatioC, int RatioD);
    // Reset the video to the default aspect ratio/size
    void (CDL_API_CALL *CDL_ResetVideoSize)(void);
    // Write something (maybe funny) on the OSD
    void (CDL_API_CALL *CDL_WriteToOSD)(char* mytext);
    // Retrieve or set the scrolling text of the main player
    char* (CDL_API_CALL *CDL_GetScrollText)(void);
    void (CDL_API_CALL *CDL_SetScrollText)(char* mytext);
    // Load or save a Nefertiti playlist (XML)
    void (CDL_API_CALL *CDL_LoadPlaylist)(char* filename);
    void (CDL_API_CALL *CDL_SavePlaylist)(char* filename);
    // Get the itemindex of the current file playing from the
    // playlist. (base 0)
    int (CDL_API_CALL *CDL_GetPlayingItem)(void);
    // Get the playlist items count
    int (CDL_API_CALL *CDL_GetItemsCount)(void);
    // Play an item from the playlist
    void (CDL_API_CALL *CDL_PlayItem)(int itemindex);
    // Retrieve the title from the playlist item
    char* (CDL_API_CALL *CDL_GetItemTitle)(int itemindex);
    // Retrieve the filename from the playlist item
    char* (CDL_API_CALL *CDL_GetItemFilename)(int itemindex);
    // Select an item in the playlist. This does not unselect the current
    // selected items, so you may need CDL_ItemsClearSel
    void (CDL_API_CALL *CDL_SelectItem)(int itemindex);
    // Clear the playlist selection
    void (CDL_API_CALL *CDL_ItemsClearSel)(void);
    // Add a file to the playlist
    void (CDL_API_CALL *CDL_AddItem)(char* filename);
    // Delete the selected items
    void (CDL_API_CALL *CDL_DeleteItems)(void);
    // Get or set Equalizer state
    BOOL (CDL_API_CALL *CDL_GetEQState)(void);
    void (CDL_API_CALL *CDL_SetEQState)(BOOL enabled);
    // Get or set DRC value: Min: 300 (0 dB), Max: 0 (30 dB)
    int (CDL_API_CALL *CDL_GetEQDRC)(void);
    void (CDL_API_CALL *CDL_SetEQDRC)(int nvalue);
    // Get or set the equalizer bands: Min: 100 (-15 dB), Max: 0 (+15dB)
    // Default is of course 100. 1st param is the band (0 to 8)
    int (CDL_API_CALL *CDL_GetEQBand)(int Band);
    void (CDL_API_CALL *CDL_SetEQBand)(int Band, int nvalue);
    // Ask the player to hijack your config dialog in the options box
    // 1st param is the handle of your window, the 2nd is the caption
    void (CDL_API_CALL *CDL_HijackMe)(HWND myHwnd, char* Name);
    // Same but it will be a child to last non-children hijacked dialog
    void (CDL_API_CALL *CDL_HijackMyChild)(HWND myHwnd, char* ChildName);
    // Register a window in the player windows submenu list. You have to provide
    // a Global Unique ID. Check CDL_Howto.txt for further details.
    void (CDL_API_CALL *CDL_RegisterWindow)(char* Description, GUID WinID);
    // Same, but with a shortcut
    void (CDL_API_CALL *CDL_RegisterShortcut)(char *Description, GUID WinID);
    // Request a tab in the media properties, check CDL_Howto.txt for a complete guide
    void (CDL_API_CALL *CDL_AddMediaTab)(char* Description, HWND myHWND);
    // Write something to the console
    void (CDL_API_CALL *CDL_WriteToConsole)(char* mytext);
    // Get the current playback state
    // (CORE_FILEPLAY, CORE_FILEPAUSE, CORE_FILESTOP and CORE_FILECLOSED)
    int (CDL_API_CALL *CDL_GetPlayState)(void);
    // Get a pointer to the DirectShow IGraphBuilder COM object.
    // Read CDL_Howto.txt for more details (recommended)
    void* (CDL_API_CALL *CDL_GetDSGraph)(void);
    // Add a subtitle extension to the player's opendialog filter
    // use caps, ie: '.SUB'. Read CDL_Howto.txt to know how to add a new subtitle format.
    void (CDL_API_CALL *CDL_AddSubExt)(char* Ext);
    // Draw a subtitle line, duration is in milliseconds
    void (CDL_API_CALL *CDL_DrawXMLSub)(char* Text, int Duration);
    // Clear last XML subtitle
    void (CDL_API_CALL *CDL_ClearXMLSub)(void);
    // Clear the whole screen except OSD
    void (CDL_API_CALL *CDL_ClearAllXMLSub)(void);
    // Get the metadata. They are filled just after the CORE_FILELOADING event by CDL
    // that support MediaPropExec2 (API v101)
		void (CDL_API_CALL *CDL_GetMetadata)(CDL_Metadata *meta);
	  // Notify a CCAS filter that it should update it settings. The Settings pointer is
    // pointing to a structure of your choice or whatever you want
    void (CDL_API_CALL *CDL_NotifyCCASUpdate)(GUID CCASID, void *Settings);
#ifdef _DEBUG
		DWORD fnpBuffer[64];
#endif
} CDL_CoreHeader, *PCDL_CoreHeader;

// ----------------------------------------------------------------------------
#endif
// ----------------------------------------------------------------------------
