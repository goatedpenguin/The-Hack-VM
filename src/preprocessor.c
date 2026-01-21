#include <string.h>
#include <ctype.h>
#include "preprocessor.h"

// Remove C++/VM style comments starting with //
void stripComment(char *line)
{
    if (!line)
        return;
    char *c = strstr(line, "//");
    if (c)
        *c = '\0';
}

// Trim leading/trailing whitespace and collapse internal runs to single spaces
void stripWhiteSpace(char *line)
{
    if (!line)
        return;
    char *src = line;
    // trim leading
    while (*src && isspace((unsigned char)*src))
        src++;
    char *dst = line;
    int space_pending = 0;
    while (*src)
    {
        if (isspace((unsigned char)*src))
        {
            space_pending = 1;
        }
        else
        {
            if (space_pending && dst != line)
            {
                *dst++ = ' ';
            }
            *dst++ = *src;
            space_pending = 0;
        }
        src++;
    }
    // remove trailing space if present
    if (dst > line && isspace((unsigned char)dst[-1]))
        dst--;
    *dst = '\0';
}
