#ifndef _HTTP_H_
#define _HTTP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include <string>
#include "JString.h"

// try to tell linker where WinSock library is
#if defined(_MSC_VER)
#   pragma comment(lib,"wsock32.lib")
#elif defined(__BORLANDC__)
#   pragma comment(lib,"mswsock.lib")
#endif


class CHttp
{
public:
  CHttp();
  ~CHttp();

  std::string ReadPage(const std::string &Host, const std::string &Page);
  
  static JString ConvertToValidURI(JString uri);
private:
  WSADATA m_WsaData;
};

#endif // _HTTP_H_