#include "Win32Stream.h"

#define	CACHESIZE     65536

/* Win32Stream methods */

/* read count bytes into buffer starting at file position pos
* return the number of bytes read, -1 on error or 0 on EOF
*/
static int Win32Read(Win32Stream *st, ulonglong pos, void *buffer, int count) {
  DWORD BytesRead;
  LONG High = (LONG)(pos>>32);
  SetFilePointer(st->fp, (LONG)(pos & 0xffffffff), &High, FILE_BEGIN);
  
  if (!ReadFile(st->fp, buffer, count, &BytesRead, NULL)) {
    return 0;
  }
  return (int)BytesRead;
}

/* scan for a signature sig(big-endian) starting at file position pos
* return position of the first byte of signature or -1 if error/not found
*/
static longlong Win32Scan(Win32Stream *st, ulonglong start, unsigned signature) {
  char c;
  unsigned cmp = 0;
  ulonglong pos = start;
  LONG High = (LONG)(start>>32);

  SetFilePointer(st->fp, (LONG)(start & 0xffffffff), &High, FILE_BEGIN);

  while (Win32Read(st, pos, &c, 1) != 0) {
    cmp = ((cmp << 8) | c) & 0xffffffff;
    if (cmp == signature)
      return pos - 4;
    pos++;
  }

  return -1;
}

/* return cache size, this is used to limit readahead */
static unsigned Win32GetCacheSize(Win32Stream *st) {
  return CACHESIZE;
}

/* return last error message */
static const char *Win32GetLastError(Win32Stream *st) {
  return strerror(st->error);
}

/* memory allocation, this is done via stdlib */
static void  *Win32Malloc(Win32Stream *st, size_t size) {
  return malloc(size);
}

static void  *Win32Realloc(Win32Stream *st, void *mem, size_t size) {
  return realloc(mem,size);
}

static void Win32Free(Win32Stream *st, void *mem) {
  free(mem);
}

/* progress report handler for lengthy operations
* returns 0 to abort operation, nonzero to continue
*/
static int Win32Progress(Win32Stream *st, ulonglong cur, ulonglong max) {
  return 1;
}

void Win32Stream_Init(Win32Stream *st)
{
  /* fill in I/O object */
  memset(st, 0, sizeof(Win32Stream));
  st->base.read = Win32Read;
  st->base.scan = Win32Scan;
  st->base.getsize = Win32GetCacheSize;
  st->base.geterror = Win32GetLastError;
  st->base.memalloc = Win32Malloc;
  st->base.memrealloc = Win32Realloc;
  st->base.memfree = Win32Free;
  st->base.progress = Win32Progress;
}

int Win32Stream_Open(Win32Stream *st, const char *filename)
{
  return Win32Stream_OpenA(st, filename);
}

int Win32Stream_OpenA(Win32Stream *st, const char *filename)
{
  DWORD AccessMode, ShareMode, Disposition;
  DWORD dwFlags = 0;

  AccessMode = GENERIC_READ;
  ShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;
  Disposition = OPEN_EXISTING;

  st->fp = CreateFileA(filename, AccessMode, ShareMode, NULL, Disposition, dwFlags, NULL);
  if ((st->fp == INVALID_HANDLE_VALUE) || ((long)st->fp == 0xffffffff))
  {
    // File was not opened
    return (int)GetLastError();
  }

  return 0;
}

int Win32Stream_OpenW(Win32Stream *st, const wchar_t *filename)
{
  DWORD AccessMode, ShareMode, Disposition;
  DWORD dwFlags = 0;

  AccessMode = GENERIC_READ;
  ShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;
  Disposition = OPEN_EXISTING;

  st->fp = CreateFileW(filename, AccessMode, ShareMode, NULL, Disposition, dwFlags, NULL);
  if ((st->fp == INVALID_HANDLE_VALUE) || ((long)st->fp == 0xffffffff))
  {
    // File was not opened
    return (int)GetLastError();
  }

  return 0;
}

void Win32Stream_Close(Win32Stream *st)
{
  CloseHandle(st->fp);
  st->fp = NULL;
}
