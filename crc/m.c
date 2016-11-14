#include <windows.h>
#include <stdio.h>

typedef struct
{
	HANDLE h;
  
	int (*dnaOpen)(char*, int, int);
	int (*dnaClose)(int);
	int (*dnaAccept)(int, char*, int);
	int (*dnaWrite)(int, char*, int, char*, int);
	int (*dnaRead)(int, char*, int, char*, int);

	int csd[1024];
} tagDNANET;


void main()
{
  int e = 0;
  int csd = 0;
  char buf[1024];
  int sz = 0;
  int idx = 0;
  unsigned char sent = 0x00;
  int es=0, er = 0;

  tagDNANET dn;

  char dbg[32];
  int i,j;

  memset(&dn, 0, sizeof(tagDNANET));

  dn.h = LoadLibrary("dplnet.dll");

 *(FARPROC*)&dn.dnaOpen = GetProcAddress(dn.h,"dnaOpen");
 *(FARPROC*)&dn.dnaClose = GetProcAddress(dn.h, "dnaClose");
 *(FARPROC*)&dn.dnaAccept = GetProcAddress(dn.h, "dnaAccept");
 *(FARPROC*)&dn.dnaWrite = GetProcAddress(dn.h, "dnaWrite");
 *(FARPROC*)&dn.dnaRead = GetProcAddress(dn.h, "dnaRead");

  e = dn.dnaOpen("127.0.0.1", 2654, 0x00);
  if (e > 0)
  {
    printf("Open SUCCESS\r\n");
  }
  while (1)
  {
    csd = dn.dnaAccept(e, buf, 0);
    if (csd > 0 ) break;
  }

  printf("Client Connected \r\n");


  Sleep(4000);

  sent = 0x01;

  i = j = 0;
  while (1)
  {
    sprintf(dbg, "%d,%d", i++, j++);
	  if (sent & 0x01)
	  {
    	es = dn.dnaWrite(csd, dbg, strlen(dbg), 0, 0);
	    printf("%s -> write : %d \r\n", dbg, es);
      sent = 0x01;
    }
    //if (es > 0)
    //{
    //  er = dn.dnaRead(csd, buf, 1024, 0, 0);
    //  if (er > 0)
    //  {
    //    buf[er] = 0;
    //    printf("%08d : %s\r\n", idx, buf);
    //    idx++;
    //    sent = 0x01;
    //  }
    //  else
    //  {
    //    sent = 0x00;
    //  }
    //}
	  Sleep(1000);
  }
}