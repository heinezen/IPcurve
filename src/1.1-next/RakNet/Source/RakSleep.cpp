#if defined(_CONSOLE_1)
#include "Console1Includes.h"
#elif defined(_WIN32)
#include <windows.h> // Sleep
#elif defined(_CONSOLE_2)
#include "Console2Includes.h"
#else
#include <unistd.h> // usleep
#endif

void RakSleep(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#elif defined(_CONSOLE_2)
	// Use the version of usleep on the console here, this is a macro
	_CONSOLE_2_usleep(ms * 1000);
#else
	usleep(ms * 1000);
#endif
}
