#include <SoftwareSerial.h>
#include "wfm.h"

#define RETRY_COUNT    1000

SoftwareSerial srl(6,7);
tagWFM  wfm;

int uart_read()
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
  Serial.print("\r\n");
  return 0;
}

void setup() {
  Serial.begin(9600);
  srl.begin(BAUD_RATE);
  
  memset(&wfm, 0, sizeof(tagWFM));
  wfm.uwrite = uart_write;
  wfm.uread = uart_read;
  wfm.ustatus = uart_status;
}


void loop() {
  int c = 0;
  int e = 0;
  int i = 0;
  char buf[UART_BUF_SZ];
  // put your main code here, to run repeatedly:

#if 1
  //wfm_uwrite(&wfm, "AT+UART_DEF", "115200,8,1,0,0");
  //e = wfm_uread(&wfm, 1000, "OK", 2);
  delay(2000);
  //srl.begin(115200);
  delay(2000);


  wfm_uwrite(&wfm, "ATE1", 0);
  e = wfm_uread(&wfm, 1000, "OK", 2);

  wfm_uwrite(&wfm, "AT+CWAUTOCONN", "0");
  e = wfm_uread(&wfm, 1000, "OK", 2);

  /***
  for ( i=0 ; i<RETRY_COUNT ; i++ )
  {
    wfm_uwrite(&wfm, "AT", 0);
    e = wfm_uread(&wfm, 1000, "OK", 2);
    Serial.print("AT -> ");
    if ( e == UART_OK )
    {
      Serial.print("SUCCESS\r\n");
      break;
    }
    else Serial.print("FAIL\r\n");
  }
  ***/
  
  for ( i=0 ; i<RETRY_COUNT ; i++ )
  {
    wfm_uwrite(&wfm, "AT+CWQAP", 0);
    e = wfm_uread(&wfm, 1000, "OK", 2);
    if ( e == UART_OK )
    {
      break;
    }
  }

  for ( i=0 ; i<RETRY_COUNT ; i++ )
  {
    wfm_uwrite(&wfm, "AT+CWMODE", "1");
    e = wfm_uread(&wfm, 1000, "OK", 2);
    if ( e == UART_OK )
    {
      break;
    }
  }


  while (1  )
  {
    if ( e == UART_OK )
    {
      wfm_uwrite(&wfm, "AT+CWJAP_CUR", "\"DALS\",\"DAL@D510\"");
      e = UART_FAIL;
    }
    e = wfm_uread(&wfm, 4000, "OK", 2);
    if ( e == UART_OK )
    {
      break;
    }
  }
  
  for ( i=0 ; i<RETRY_COUNT ; i++ )
  {
    wfm_uwrite(&wfm, "AT+CIPMUX", "0");
    e = wfm_uread(&wfm, 1000, "OK", 2);
    if ( e == UART_OK )
    {
      break;
    }
  }

#if 1
  while ( 1 )
  {
    if ( e == UART_OK )
    {
      wfm_uwrite(&wfm, "AT+CIPSTART", "\"TCP\",\"192.168.0.16\",2654");
      e = UART_FAIL;
    }
    e = wfm_uread(&wfm, 4000, "OK", 2);
    if ( e == UART_OK )
    {
      break;
    }
  }
#endif

Serial.print("Read Start\r\n");

while ( 1 )
{
    e = wfm_read(&wfm, buf, UART_BUF_SZ);
    if ( e > 0 )
    {
      int eol = 0;
      char out[2][16];
      char dbg[64];
      eol = wfm_str_dlmtr(&wfm, buf, ",", 0, out[0]);
      out[0][eol] = 0;
      eol = wfm_str_dlmtr(&wfm, buf, ",", 1, out[1]);
      out[1][eol] = 0;
      sprintf(dbg, "----------\r\n"
                   "|%d:%s -> [%s], [%s]\r\n"
                   "==========\r\n", e, buf, out[0], out[1]);
      
      Serial.print(dbg);

    }
}

while ( 1 );



  Serial.print("Read Start \r\n");
  while ( 1 )
  {
    e = wfm_read(&wfm, buf, UART_BUF_SZ);
    if ( e > 0 )
    {
      int eol = 0;
      char out[2][16];
      char dbg[64];
      eol = wfm_str_dlmtr(&wfm, buf, ",", 0, out[0]);
      out[0][eol] = 0;
      eol = wfm_str_dlmtr(&wfm, buf, ",", 1, out[1]);
      out[1][eol] = 0;
      sprintf(dbg, "----------\r\n"
                   "|%d:%s -> [%s], [%s]\r\n"
                   "==========\r\n", e, buf, out[0], out[1]);
      
      Serial.print(dbg);

      wfm_uwrite(&wfm, "ATE0", 0);
      e = wfm_uread(&wfm, 1000, "OK", 2);
      wfm_write(&wfm, buf, e, 1);
      wfm_uwrite(&wfm, "ATE1", 0);
      e = wfm_uread(&wfm, 1000, "OK", 2);

    }
    else
    {

      wfm_uwrite(&wfm, "ATE0", 0);
      e = wfm_uread(&wfm, 1000, "OK", 2);
      wfm_write(&wfm, "Read Fail", 9, 1);
      wfm_uwrite(&wfm, "ATE1", 0);
      e = wfm_uread(&wfm, 1000, "OK", 2);

    }
  }
#endif

}
