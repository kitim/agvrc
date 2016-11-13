#ifndef __WFM_H_5002903e_7609_44f2_a211_ee0fa7ef5669__
#define __WFM_H_5002903e_7609_44f2_a211_ee0fa7ef5669__

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BAUD_RATE          115200
#define UART_BUF_SZ        128
#define CBUF_COUNT         2
#define UART_OK            0
#define UART_FAIL          -1
#define UART_PENDDING      -2

typedef struct
{
  ////SoftwareSerial srl(6,7);
  
  char buf[UART_BUF_SZ];
  char cbuf[CBUF_COUNT][UART_BUF_SZ];
  int  idx;
  int  cidx;

  int (*uwrite)(char*, int);
  int (*uread)();
  int (*ustatus)(char*, int);
  
}tagWFM;

int wfm_check_ack(tagWFM* p, char* ack, int sz);
int wfm_uread(tagWFM* p, int count, char* ack, int sz);
int wfm_uwrite(tagWFM* p, char* cmd, char* arg);
int wfm_write(tagWFM* p, char* buf, int sz, int msec);
int wfm_read(tagWFM* p, char* buf, int sz);
int wfm_str_dlmtr(tagWFM* p, char* str, char* dlmtr, int idx, char* out);

#ifdef __cplusplus
}
#endif

#endif
