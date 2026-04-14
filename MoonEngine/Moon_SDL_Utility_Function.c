#include"Moon.h"

#if OPEN_SDL

extern void MoonMusic(const wchar_t* File)
{

}

extern int MoonSleep(int timeload)
{
	SDL_Delay(timeload);
	return 0;
}

#endif