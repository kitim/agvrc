#include "Arduino.h"
#include "wfm.h"
#include <stdio.h>

void wfm_debug(tagWFM* p, char* buf, int sz, int mode)
{
  int i = 0;
  int ii = 0;
  char dbg[80];

  if ( mode == 0x16 )
  {
    for ( i=0 ; i<sz ; i++ )
    {
      sprintf(&dbg[ii], "%02X:%c ", (unsigned char)*(buf+i), (unsigned char)*(buf+i));
      ii+=5;
    }
    dbg[ii] = 0;
    p->ustatus(dbg, ii);
  }
  else
  {
    p->ustatus(buf, sz);
  }
}

#if 0
int wfm_check_ack(tagWFM* p, char* ack, int sz, char* out, int osz, int m)
{
  int e = UART_PENDDING;
  int idx = 0;
  char c = 0;
  int i = 0;
  int ii=0;
  int _sz = sz-1;

  char str[16];
  
  for ( idx = 0; idx<osz ; idx++ )
  {
    c = p->uread();
    if ( c >= 0 )
    {
      *(out+p->idx) = c;
      if (  (m==0) && (p->idx>0)  ) 
      {
        for ( i=0 ; i<sz ; i++ )
        {
          if (  *(out+p->idx-i) == *(ack+_sz) ) 
          {
            _sz--;
            if ( _sz < 0 )
            {
              e = p->idx;
              p->idx = 0;
              break;
            }
          }
        }
        if ( e > 0 )
        {
          break;
        }
      }
      p->idx++;
      if ( (m>0) && (m==p->idx) )
      {
        e = p->idx;
        break;
      }
    }
  }
  return e;
}
#endif

#if 0
int wfm_uread_check_ack(tagWFM* p, int count, char* ack, int sz, char* out, int osz, int m)
{
  int e = -1;
  int idx = 0;
  for ( idx = 0 ; idx<count ; idx++ )
  {
    e = wfm_check_ack(p, ack, sz, out, osz, m);
    if ( e >= UART_OK  )
    {
      break;
    }
  }
  return e;  
}
#endif

int wfm_check(tagWFM* p, char* buf, int sz, char* ack, int asz)
{
  int e = 0;
  int i = 0;
  int _i=0;
  unsigned int chk = 0x0000;
  for ( i=sz-1, _i=asz-1 ; i>=0 ; i-- )
  {
    if (  *(buf+i) == *(ack+_i)  ) 
    {
      /***************************************
      {
        char dbg[32];
        wfm_debug(p, "\r\n>>>>>>>>>>>>>>>>>>>\r\n", 0, 0);
        sprintf(dbg, "(%d, %d) -> (%02X:%c, %02X:%c)", i, _i, (unsigned char)*(buf+i), *(buf+i), (unsigned char)*(ack+_i), *(ack+_i));
        wfm_debug(p, dbg, 0, 0);
        wfm_debug(p, "\r\n<<<<<<<<<<<<<<<<<<<\r\n", 0, 0);
      }
      ***************************************/
      _i--;
      e++;
      if ( _i < 0 ) break;
    }
    else
    {
      _i = asz-1;
      e = 0;
    }
    
  }

  if ( _i>=0 ) e = 0;
  return e;
}


int wfm_uread(tagWFM* p, char* buf, int sz)
{
  int i = 0;
  char c = 0;
  for ( i=0 ; i<sz ; )
  {
    c=p->uread();
    if ( c >= 0 )
    {
      *(buf+i) = c;
      i++;
      *(buf+i) = 0;
    }
    else
    {
      break;
    }
  }
  return i;
}

int wfm_uread_check(tagWFM* p, char* buf, int sz, char* ack, int asz, int count)
{
  int e = 0;
  int c = 0;
  int i = 0;
  for (c=0 ; c<count ; c++)
  {
    e = wfm_uread(p, (buf+i), sz);
    if ( e > 0 )
    {
      i+=e;
      /***************************************
      wfm_debug(p, "\r\n-------------------\r\n", 0, 0);
      {
        char dbg[80];
        sprintf(dbg, "i : %d\r\n", i);
        wfm_debug(p, dbg, 0, 0);
      }
      wfm_debug(p, buf, i, 0);
      wfm_debug(p, "\r\n===================\r\n", 0, 0);
      ***************************************/

      e = wfm_check(p, buf, i, ack, asz);
      if ( e > 0 )
      {
        break;
      }
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
  //e = wfm_uread(p, 1000, "> ", 2, 0, 0);
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

#if 0
int wfm_read(tagWFM* p, char* buf, int sz)
{
  int e = -1;
  char c = 0;
  int i = 0;
  char ipdbuf[32];

  for ( i=0 ; i<sz ; )
  {
    c = p->uread();
    if ( c>=0 )
    {
      wfm_debug(p, &c, 1, 0x16);    
      switch(c)
      {
        case '+':
        case 'I':
        case 'P':
        case 'D':
          ipdbuf[i] = (char)c;
          break;
        default:
          break;
      }
      i++;
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
  if ( e < 0 ) return -1;

  p->cidx = 0;
  for ( i=0 ; i<sz ;  )
  {
    c = p->uread();
    if ( c>=0 )
    {
      wfm_debug(p, &c, 1, 0x16);    

      p->cbuf[p->cidx][p->idx] = c;
      p->idx++;
      if ( c==':' )
      {
        p->cbuf[p->cidx][p->idx-1] = 0;
        p->cidx++;
        p->idx=0;
        break;
      }
      i++;
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
  int e = -1;
  char c = 0;
  int i = 0;
  char ipdbuf[32];

  for ( i=0 ; i<sz ; )
  {
    c = p->uread();
    if ( c>=0 )
    {
      wfm_debug(p, &c, 1, 0x16);    
      switch(c)
      {
        case '+':
        case 'I':
        case 'P':
        case 'D':
          ipdbuf[i] = (char)c;
          break;
        default:
          break;
      }
      i++;
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
  if ( e < 0 ) return -1;

  p->cidx = 0;
  for ( i=0 ; i<sz ;  )
  {
    c = p->uread();
    if ( c>=0 )
    {
      wfm_debug(p, &c, 1, 0x16);    

      p->cbuf[p->cidx][p->idx] = c;
      p->idx++;
      if ( c==':' )
      {
        p->cbuf[p->cidx][p->idx-1] = 0;
        p->cidx++;
        p->idx=0;
        break;
      }
      i++;
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
