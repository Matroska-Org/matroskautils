#ifndef _WIN32_STREAM_H_
#define _WIN32_STREAM_H_

#include <windows.h>
#include "MatroskaParser.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Win32Stream {
  struct InputStream  base;
  HANDLE fp;
  int error;
};
typedef struct Win32Stream Win32Stream;

// Helper methods
void Win32Stream_Init(Win32Stream *st);
int Win32Stream_Open(Win32Stream *st, const char *filename);
int Win32Stream_OpenA(Win32Stream *st, const char *filename);
int Win32Stream_OpenW(Win32Stream *st, const wchar_t *filename);
void Win32Stream_Close(Win32Stream *st);

#ifdef __cplusplus
}
#endif

#endif // _WIN32_STREAM_H_