#include"Moon.h"

#if OPEN_SDL

#include<SDL3/SDL.h>
#include <SDL3/SDL_render.h>

extern void SDL_Moon_DrawingArea(IMAGE* image_1, IMAGE* image_2, int x, int y, int width, int height)
{

}

extern void SDL_Moon_DrawingAreaAlpha(IMAGE* image_1, IMAGE* image_2, int x, int y, int width, int height, int transparent_color)
{

}

extern void SDL_Moon_DrawingAreaRound(IMAGE* image_1, IMAGE* image_2, int x, int y, int apx, int apy, int width, int height, int deg)
{

}

extern void SDL_Moon_Pix(IMAGE* image, int x, int y, int color)
{

}

extern void SDL_Moon_Line(IMAGE* image, int x1, int y1, int x2, int y2, int width, int color)
{

}

extern void SDL_Moon_Box(IMAGE* image, int x1, int y1, int x2, int y2, int width, int color)
{

}

extern void SDL_Moon_BoxFull(IMAGE* image, int x1, int y1, int x2, int y2, int color)
{

}

extern int  SDL_Moon_AnimeInit(ANIME* anime, LPCSTR name, IMAGE* sequenceframes, int timeload, int totalnumber, int width, int height)
{

}

extern int  SDL_Moon_AnimeRun(IMAGE* image, ANIME* anime, int animeswitch, int x, int y, int widthsize, int heightsize)
{

}

#endif