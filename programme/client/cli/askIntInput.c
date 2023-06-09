//
// Created by Utilisateur on 30/10/2022.
//

#include "tchar.h"
#include "client/cli/windows_compatibility/winterm.h"

#include "askIntInput.h"

int askIntInput(_TCHAR *prompt, int lowIncluded, int highIncluded)
{
    long val;
    do {
        _tprintf(_T("%"W"s (%d-%d)>"), prompt ? prompt : _T(""), lowIncluded, highIncluded);
        fflush(stdout);
        char buffer[5] = {0};

        int written = fscanf(stdin, "%4s", buffer);

        char * end;
        val = strtol(buffer, &end, 10);

        if(end - buffer == written && val >= lowIncluded && val <= highIncluded) {
            break;
        }
        _putts(_T("Nombre invalide"));
    }while (1);

    return val;
}
