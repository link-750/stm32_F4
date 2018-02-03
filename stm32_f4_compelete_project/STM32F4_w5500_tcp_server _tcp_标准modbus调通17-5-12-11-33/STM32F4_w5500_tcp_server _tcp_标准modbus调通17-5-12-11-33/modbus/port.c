#include "main.h"
void
EnterCriticalSection(  )
{
        __ASM volatile("cpsid i");
}

void
ExitCriticalSection(  )
{
    __ASM volatile("cpsie i");
}

