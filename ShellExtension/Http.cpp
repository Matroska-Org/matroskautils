#include "Http.h"

#define MAX_PAGE (1024*64)

static char PageBuffer[MAX_PAGE];

static int GetAddr(const char* HostName, int Port, struct sockaddr* Result)
{
  struct hostent* Host;
  SOCKADDR_IN Address;

  memset(Result, 0, sizeof(*Result));
  memset(&Address, 0, sizeof(Address));

  Host                = gethostbyname(HostName);
  if(Host != NULL)
  {
    Address.sin_family  = AF_INET;
    Address.sin_port    = htons((short)Port);
    memcpy(&Address.sin_addr, Host->h_addr_list[0], Host->h_length);
    memcpy(Result, &Address, sizeof(Address));
  }
  return Host != NULL;
}

static void ReadPage(const char* Host, const char* Page)
{
  struct sockaddr SockAddr;
  SOCKET Socket;
  int Port = 80; /* HTTP */

  if(GetAddr(Host, Port, &SockAddr))
  {
    int     Status;

    Socket = socket(AF_INET, SOCK_STREAM, 0);
    Status = connect(Socket, &SockAddr, sizeof(SockAddr));
    if(Status >= 0)
    {
      DWORD   StartTime, EndTime;
      char    Request[512];
      char*   Rover   = PageBuffer;
      int     Read;

      sprintf(Request, "GET %s HTTP/1.0\nHost: %s\n\n", Page, Host);
      send(Socket, Request, strlen(Request), 0);

      StartTime = GetTickCount();
      for(Read=0;Read < MAX_PAGE;)
      {
        int     ThisRead;

        ThisRead = recv(Socket, Rover, MAX_PAGE-Read, 0);
        if(ThisRead == SOCKET_ERROR || ThisRead == 0)
          break;
        else
        {
          Read    += ThisRead;
          Rover   += ThisRead;
        }
      }
      EndTime = GetTickCount();
      printf("%d milliseconds to read %d-byte page\n",
        EndTime-StartTime, Read);
      closesocket(Socket);
    }
    else
      fprintf(stderr, "connect failed (%d)\n",
      WSAGetLastError());
  }
  else
    fprintf(stderr, "Can't map hostname '%s'\n", Host);
}

CHttp::CHttp()
{  
  WSAStartup(0x0001, &m_WsaData);  
}

CHttp::~CHttp()
{
  WSACleanup();
}

std::string CHttp::ReadPage(const std::string &Host, const std::string &Page)
{
  PageBuffer[0] = '\0';
  ::ReadPage(Host.c_str(), Page.c_str());
  return PageBuffer;
}

JString CHttp::ConvertToValidURI(JString uri)
{
  JString out_str;
  JString hexa_code;
  size_t i;

  for (i = 0; i < uri.length(); i++)
  {
    char c = uri[i];

    if (c == ' ')
    {
      out_str += "%20";
    }
    else
    {
      static const char marks[] = "-_!~*'";

      if (!isalnum(c) && !strchr(marks, c))
      {
        hexa_code = JString::Format("%%%02X", c);
        out_str += hexa_code;
      }
      else
      {
        out_str += c;
      }
    }
  }

  return out_str;
}