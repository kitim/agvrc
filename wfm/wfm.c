#include "Arduino.h"

#include "wfm.h"
#include <stdio.h>


int wfm_check_ack(tagWFM* p, char* ack, int sz)
{
  int e = UART_PENDDING;
  int idx = 0;
  char c = 0;
  int i = 0;
  int _sz = sz-1;
  
  for ( idx = 0; idx<UART_BUF_SZ-1 ; idx++ )
  {
    c = p->uread();
    //c = p->srl.read();
    if ( c >= 0 )
    {
      p->buf[p->idx] = c;
      if (  p->idx > 0  ) 
      {
        for ( i=0 ; i<sz ; i++ )
        {
          if ( (p->buf[p->idx-(i)] == *(ack+_sz) ) )
          {
            _sz--;
            if ( _sz <= 0 )
            {
              memset(p->buf, 0, UART_BUF_SZ);
              p->idx = 0;
              e = UART_OK;
              break;
            }
          }
        }
      }
      p->idx++;
    }
  }
  return e;
}

int wfm_uread(tagWFM* p, int count, char* ack, int sz)
{
  int e = -1;
  int idx = 0;
  for ( idx = 0 ; idx<count ; idx++ )
  {
    e = wfm_check_ack(p, ack, sz);
    if ( e == UART_OK  )
    {
      break;
    }
  }
  return e;  
}

int wfm_uwrite(tagWFM* p, char* cmd, char* arg)
{
  char _cmd[UART_BUF_SZ];
  if ( arg )
  {
    sprintf(_cmd, "%s=%s\r\n", cmd, arg);
  }
  else
  {
    sprintf(_cmd, "%s\r\n", cmd);
  }
  return p->uwrite(_cmd, strlen(_cmd));
  //return p->srl.write(_cmd, strlen(_cmd));
}

int wfm_write(tagWFM* p, char* buf, int sz, int msec)
{
  int e = 0;
  int i = 0;
  char _sb[UART_BUF_SZ];
  sprintf(_sb, "%d", sz);
  
  wfm_uwrite(p, "AT+CIPSENDEX", _sb);
  e = wfm_uread(p, 1000, "> ", 2);
  if ( e == UART_OK )
  {
    sprintf(_sb, "%s", buf);
    p->uwrite(_sb, sz);
    delay(msec);
  }
  return e;
}

int wfm_str_dlmtr(tagWFM* p, char* str, char* dlmtr, int idx, char* out)
{
  int e = 0;
  int i = 0;
  int _i = 0;
  int _idx = 0;
  
  for ( i=0 ; *(str+i) ; i++, _i++)
  {
    *(out+_i) = *(str+i);
    if ( *(str+i) == *(dlmtr+0)  )
    {
      e = _i;
      _i = -1;
      _idx++;
    }
    if ( *(str+i+1) == 0 )
    {
      _i++;
      e = _i;
      _idx++;
    }
    if ( _idx == (idx+1) )
    {
      *(out+_i) = 0;
      break;
    }
  }

  return e;
}


#if 1
int wfm_read(tagWFM* p, char* buf, int sz)
{
  int e = -1;
  char c = 0;
  int i = 0;
  char ipdbuf[32];

  for ( i=0 ; (c = p->uread())>=0 ; i++)
  {
    delay(1);
    if ( c>=0 )
    {
      switch(c)
      {
        case '+':
        case 'I':
        case 'P':
        case 'D':
          ipdbuf[i] = (char)c;
          break;

        default:
          i++;
          break;
      }

      if ( (i>=3) && (ipdbuf[i-3]=='+') && (ipdbuf[i-2]=='I') && (ipdbuf[i-1]=='P') && (ipdbuf[i]=='D') )
      {
        e = i-3;
        memcpy(p->cbuf[0], &ipdbuf[i-3], 4);
        p->idx = 4;
        p->cbuf[0][p->idx] = 0;
        break;
      }
    }
  }

  /****
  {
    char dbg[80];
    int k = 0;
    int j = 0;
    for ( k=0 ; k<i ; k++ )
    {
      sprintf(&dbg[j], "%02X ", (unsigned char)ipdbuf[k]);
      j+=3;
    }
    sprintf(&dbg[j], "\r\n");
    p->ustatus(dbg,0);    
  }
  ****/

  

  if ( e < 0 ) return -1;

  p->cidx = 0;
  for (  ; (c = (char)p->uread())>=0 ;  )
  {
    delay(1);
    
    if ( c>=0 )
    {
      p->cbuf[p->cidx][p->idx] = c;
      p->idx++;

      if ( c==':' )
      {
        p->cbuf[p->cidx][p->idx-1] = 0;
        p->cidx++;
        p->idx=0;
      }
    }
  }
  p->cbuf[p->cidx][p->idx] = 0;

  if ( p->cidx == (CBUF_COUNT-1) )
  {
    e = wfm_str_dlmtr(p, p->cbuf[0], ",", 1, buf);
    if ( e > 0 )
    {
      e = atoi(buf);
      memcpy(buf, p->cbuf[1], e);
      buf[e] = 0;
    }
  }
  /****
  wfm_uwrite(p, "ATE0", 0);
  e = wfm_uread(p, 100, "OK", 2);
  if ( e<UART_OK ) return e;
  ****/
  return e;
}
#endif

#if 0
int wfm_read(tagWFM* p, char* buf, int sz)
{
  int e = 0;
  char c = 0;
  int i = 0;

  for ( ; (c = (char)p->uread())>=0; )
  {
    if ( c=='+' )
    {
      p->cidx = 0;
      p->idx = 0;
    }
    p->cbuf[p->cidx][p->idx] = c;
    p->idx++;

    if ( c==':' )
    {
      p->cbuf[p->cidx][p->idx-1] = 0;
      p->cidx++;
      p->idx=0;
      i = -1;
    }
  }
  p->cbuf[p->cidx][p->idx] = 0;

  p->ustatus(p->cbuf[0], 32);
  p->ustatus(p->cbuf[1], 32);
  
  if ( p->cidx == (CBUF_COUNT-1) )
  {
    e = wfm_str_dlmtr(p, p->cbuf[0], ",", 1, buf);
    if ( e > 0 )
    {
      e = atoi(buf);
      {
        char dbg[32];
        sprintf(dbg, "-> %d -> %s\r\n", e, p->cbuf[1]);
        p->ustatus(dbg, e);
      }
      memcpy(buf, p->cbuf[1], e);
      buf[e] = 0;
    }
  }
  /****
  wfm_uwrite(p, "ATE0", 0);
  e = wfm_uread(p, 100, "OK", 2);
  if ( e<UART_OK ) return e;
  ****/
  return e;
}
#endif
