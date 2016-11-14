#include <SoftwareSerial.h>
#include "wfm.h"

#define RETRY_COUNT    1000
#define AP_INFO       "\"DAL\",\"01041560094\""
#define SVR_INFO      "\"TCP\",\"192.168.0.3\",2654"

#define GBUF_SZ        128

SoftwareSerial srl(6,7);
tagWFM  wfm;
char gbuf[GBUF_SZ];

unsigned int intr[2] = {0,0};

char uart_read()
{
  return srl.read();
}

int uart_write(char* p, int sz)
{
  return srl.write(p, sz);
}

int uart_status(char* p, int sz)
{
  Serial.print(p);
  return 0;
}

void xinterrupt()
{
  char c = srl.read();
  Serial.println(c);

  if ( c=='O' || c=='K' || c=='\r' || c=='\n' )
  {
    delay(4000);
  }
}


void set_interrupt(int id, void(*f)(), int mode, int pn)
{
  intr[0] = (0x0001<<pn);
  attachInterrupt(id, f, mode);
}

void setup() {
  Serial.begin(9600);
  srl.begin(BAUD_RATE);
  Serial.print("program start .... \r\n");
  //set_interrupt(0, xinterrupt, CHANGE, 6);
  
  memset(&wfm, 0, sizeof(tagWFM));
  wfm.uwrite = uart_write;
  wfm.uread = uart_read;
  wfm.ustatus = uart_status;

  srl.listen();
}

void loop() {
  int c = 0;
  int e = 0;
  int i = 0;
  int sz = 0;
  int r = 0;
  char buf[UART_BUF_SZ];
  char cbuf[2][32];
  char vbuf[32];
  int ce = 0;

  memset(buf, 0, UART_BUF_SZ);
#if 1
  delay(2000);
  wfm_uwrite(&wfm, "AT+UART_DEF", "9600,8,1,0,0");
  e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
   Serial.print("AT+UART_DEF ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  delay(2000);
  srl.begin(9600);
  delay(2000);
    
  memset(buf, 0, UART_BUF_SZ);
  wfm_uwrite(&wfm, "ATE1", 0);
  e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
  Serial.print("ATE1 ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  memset(buf, 0, UART_BUF_SZ);
  wfm_uwrite(&wfm, "AT+CWAUTOCONN", "0");
  e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
  Serial.print("AT+CWAUTOCONN ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  memset(buf, 0, UART_BUF_SZ);
  wfm_uwrite(&wfm, "AT+CWQAP", 0);
  e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
  Serial.print("AT+CWQAP ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  memset(buf, 0, UART_BUF_SZ);
  wfm_uwrite(&wfm, "AT+CWMODE", "1");
  e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
  Serial.print("AT+CWMODE ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  e = -1;
  while ( 1 )
  {
    if ( e < 0 )
    {
      memset(buf, 0, UART_BUF_SZ);
      wfm_uwrite(&wfm, "AT+CWJAP_CUR", AP_INFO);
    }
    e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
    if ( e > 0 )
    {
      break;
    }
  }
  Serial.print("AT+CWJAP_CUR ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  e = -1;
  while ( 1 )
  {
    if ( e < 0 )
    {
      memset(buf, 0, UART_BUF_SZ);
      wfm_uwrite(&wfm, "AT+CIPMUX", "0");
    }
    e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
    if ( e > 0 )
    {
      break;
    }
  }
  Serial.print("AT+CIPMUX ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  e = -1;
  while ( 1 )
  {
    if ( e < 0 )
    {
      memset(buf, 0, UART_BUF_SZ);
      wfm_uwrite(&wfm, "AT+CIPSTART", SVR_INFO);
    }
    e = wfm_uread_check(&wfm, buf, UART_BUF_SZ, "OK\r\n", 4, 1000);
    if ( e > 0 )
    {
      break;
    }
  }
  Serial.print("AT+CIPSTART ");
  if ( e > 0 ) Serial.print("OK\r\n");
  else Serial.print("FAIL\r\n");

  i = 0;
  while ( 1 )
  {
    e = wfm_uread(&wfm, &buf[i], UART_BUF_SZ);
    if ( e > 0 )
    {
      i+=e;

      Serial.println("..........");
      Serial.println(buf);
      Serial.println("++++++++++");
      sz = wfm_check(&wfm, buf, i, "+IPD,", 5);
      if ( sz > 0 && i>10 )
      {
        //sz = wfm_check(&wfm, buf, i, "\r\n", 2);
        //if ( sz > 0 )
        {
          i = 0;
          ce = wfm_str_dlmtr(&wfm, buf, ":", 0, cbuf[0]);
          cbuf[0][ce] = 0;
          ce = wfm_str_dlmtr(&wfm, buf, ":", 1, cbuf[1]);
          cbuf[1][ce] = 0;
          
          Serial.println("----------");
          ce = wfm_str_dlmtr(&wfm, cbuf[0], ",", 1, vbuf);
          vbuf[ce] = 0;
          {
            char dbg[32];
            sprintf(dbg, "read : %d --> ", atoi(vbuf));
            Serial.print(dbg);
          }
          ce = wfm_str_dlmtr(&wfm, cbuf[1], ",", 0, vbuf);
          vbuf[ce] = 0;
          {
            char dbg[32];
            sprintf(dbg, "a : %d ,  ", atoi(vbuf));
            Serial.print(dbg);
          }
          ce = wfm_str_dlmtr(&wfm, cbuf[1], ",", 1, vbuf);
          vbuf[ce] = 0;
          {
            char dbg[32];
            sprintf(dbg, "b : %d\r\n", atoi(vbuf));
            Serial.print(dbg);
          }
          Serial.println("==========");
        }
      }
    }
  }

#endif

}
