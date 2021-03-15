#include "lib_keylog.h"
#include "colors.h"
#include <stdio.h>
#include <stdlib.h>

/*
*   [Windows Keylogger] 
*
*   This keylogger captures all keystrokes in the following format
*       - [ system up-time in ms ];
*       - [ boolean indicating whether the key was released (1=released, 0=pressed) ];
*       - [ hexadecimal value of the pressed key (windows virtual key code) ]

*   and stores them in a log file defined in: lib_keylog.h.
*
*   This tool does not aim to hide or be stealthy in any way. It was developed for the
*   purpose of automating key strokes with a seperate tool. This requires parsing of 
*   the raw log data and normalizing the timestamps. 
*
*   Note: Held down keys will produce repeated key events until release. Capturing of
*         of special key combinations (shift +, etc.) is not intended and would require
*         tweaking the event logging.
*
*   [Building]
*
*     Compile with: gcc -o keylog.exe -Wall .\keylog.c .\lib_keylog.c
*
*   @crackcat
*/

int main(int argc, char* argv[]) {

    fprintf(stderr, "\n\t%s[Windows Keylogger]%s\n\n", FC_CYAN, FC_END);

    // Make sure an empty logfile exists
    char logPath[MAX_PATH];
    char* logDest = LOG_DESTINATION;
    ExpandEnvironmentStrings(logDest, logPath, MAX_PATH);
    FILE* logFile = fopen(logPath, "w");
    fclose(logFile);

    fprintf(stderr, "%s[>]%s Created log file [%s%s%s]\n", FC_GREEN, FC_END, FC_CYAN, logPath, FC_END);

    fprintf(stderr, "%s[>]%s Starting keylogger\n", FC_GREEN, FC_END);

    /*
    *   See this stackoverflow post for reference: https://codereview.stackexchange.com/questions/46980/windows-keylogger-in-c
    *   Modified and cleaned up in several ways.
    */

    // Cancel thread with "ctrl + alt + 4"
    fprintf(stderr, "\t%s[+]%s Cancel the keylogger with [%sctrl%s+%salt%s+%s4%s]\n", FC_PURPLE, FC_END, FC_CYAN, FC_END, FC_CYAN, FC_END, FC_CYAN, FC_END);
    int ret = StartThread(argv);

    fprintf(stderr, "%s[>]%s Thread cancelled with return code: %d\n", FC_CYAN, FC_END, ret);

    return EXIT_SUCCESS;
}
