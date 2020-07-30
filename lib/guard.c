#include "tool.h"

void guard(bool condition, char msg[])
{
    if (!condition)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}