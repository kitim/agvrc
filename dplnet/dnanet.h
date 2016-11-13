#ifndef __DNANET_H_F58A630C_11AC_4A56_AF38_99F8BAE0D613__
#define __DNANET_H_F58A630C_11AC_4A56_AF38_99F8BAE0D613__

extern "C" __declspec(dllexport) int dnaOpen(char*, int, int);
extern "C" __declspec(dllexport) int dnaClose(int);
extern "C" __declspec(dllexport) int dnaAccept(int, char*, int);
extern "C" __declspec(dllexport) int dnaWrite(int, char*, int, char*, int);
extern "C" __declspec(dllexport) int dnaRead(int, char*, int, char*, int);

#endif

