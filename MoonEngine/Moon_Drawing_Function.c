#include"Moon.h"

#if !OPEN_SDL

extern void MoonDrawingArea(IMAGE* image_1, IMAGE* image_2,int x,int y,int width ,int height)
{
	BitBlt(image_1->image.hdc, x, y, width, height, image_2->image.hdc, 0, 0, SRCCOPY);
}

extern void MoonDrawingAreaAlpha(IMAGE* image_1, IMAGE* image_2, int x, int y, int width, int height,int transparent_color)
{
	TransparentBlt(image_1->image.hdc, x, y, image_2->lengths.x * width, image_2->lengths.y * height, image_2->image.hdc, 0, 0, image_2->lengths.x, image_2->lengths.y, transparent_color);
}

extern void MoonDrawingAreaRound(IMAGE* image_1, IMAGE* image_2, int x, int y, int apx, int apy, int width, int height, int deg)
{
	float cosrad = cos(DegRad(deg)), sinrad = sin(DegRad(deg));
	POINT points[3];
	float matrix2d[4] = { cosrad,-sinrad,sinrad,cosrad };
	points[0].x = x + matrix2d[0] * apx + matrix2d[2] * apy;
	points[0].y = y + matrix2d[1] * apx + matrix2d[3] * apy;
	points[1].x = x + matrix2d[0] * (apx + width) + matrix2d[2] * apy;
	points[1].y = y + matrix2d[1] * (apx + width) + matrix2d[3] * apy;
	points[2].x = x + matrix2d[0] * apx + matrix2d[2] * (apy + height);
	points[2].y = y + matrix2d[1] * apx + matrix2d[3] * (apy + height);
	PlgBlt(image_1->image.hdc, points, image_2->image.hdc, 0, 0, width, height, NULL, 0, 0);
}

extern void MoonCreateImage(PROJECTGOD* project, IMAGE* image, int bmpwidth, int bmpheight)
{
	image->lengths.x = bmpwidth;
	image->lengths.y = bmpheight;
	image->image.width = bmpwidth;
	image->image.height = bmpheight;
	HDC hdcMem = CreateCompatibleDC(GetDC(project->hwnd));
	image->image.hBitmap = CreateCompatibleBitmap(GetDC(project->hwnd), bmpwidth, bmpheight);
	SelectObject(hdcMem, image->image.hBitmap);
	image->image.hdc = hdcMem;
}

extern void MoonDeletImage(IMAGE* image)
{
	DeleteObject(image->image.hBitmap);
	DeleteDC(image->image.hdc);
}

extern void MoonPix(IMAGE* image, int x, int y, int color)
{
	SetPixel(image->image.hdc, x, y, color);
}

extern void MoonLine(IMAGE* image, int x1, int y1, int x2, int y2, int width, int color)
{
	HPEN hpen = CreatePen(PS_SOLID, width, color);
	HPEN holdpen = (HPEN)SelectObject(image->image.hdc, hpen);
	MoveToEx(image->image.hdc, x2, y2, NULL);
	LineTo(image->image.hdc, x1, y1);
	SelectObject(image->image.hdc, holdpen);
	DeleteObject(hpen);
}

extern void MoonBox(IMAGE* image, int x1, int y1, int x2, int y2,int width, int color)
{
	MoonLine(image, x1, y1, x2, y1, width, color);
	MoonLine(image, x1, y1, x1, y2, width, color);
	MoonLine(image, x1, y2, x2, y2, width, color);
	MoonLine(image, x2, y1, x2, y2, width, color);
}

extern void MoonBoxFull(IMAGE* image, int x1, int y1, int x2, int y2, int color)
{
	PAINTSTRUCT ps;
	HBRUSH hbs = CreateSolidBrush(color);
	RECT rect = { x1,y1,x2,y2 };
	FillRect(image->image.hdc, &rect, hbs);
	DeleteObject(hbs);
}

extern void MoonImageLoad(IMAGE* image, const wchar_t** imagefile, int imagenumber)
{
	if (imagenumber == 1)
	{
		HDC hdcmem;
		BITMAP bitmap;
		HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, *imagefile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hdcmem = CreateCompatibleDC(image->image.hdc);
		SelectObject(hdcmem, hBitmap);
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		TransparentBlt(image->image.hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcmem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, 0);
		image->lengths.x = bitmap.bmWidth;
		image->lengths.y = bitmap.bmHeight;
		DeleteDC(hdcmem);
		DeleteObject(hBitmap);
	}
	else if (imagenumber > 0)
		for (int i = 0; i < imagenumber; i++)
		{
			HDC hdcmem;
			BITMAP bitmap;
			HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, imagefile[i], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			hdcmem = CreateCompatibleDC(image[i].image.hdc);
			SelectObject(hdcmem, hBitmap);
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			TransparentBlt(image[i].image.hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcmem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, 0);
			image[i].lengths.x = bitmap.bmWidth;
			image[i].lengths.y = bitmap.bmHeight;
			DeleteDC(hdcmem);
			DeleteObject(hBitmap);
		}
}

extern void MoonImageLoadBatch(PROJECTGOD* project, IMAGE* image, int totalnumber, const wchar_t** name,int width,int height)
{
	for (int i = 0; i < totalnumber; i++)
		MoonCreateImage(project, &image[i], width, height);
	MoonImageLoad(image, name, totalnumber);
}

extern int MoonAnimeInit(ANIME* anime, LPCSTR name, IMAGE* sequenceframes, int timeload, int totalnumber, int width, int height)
{
	anime->Name = name;
	if (totalnumber <= 0) { printf("[InitialisationAnime函数错误]动画序列帧总数有问题,请检查名为[%s]的动画!\n", name); return Error; }
	if (sequenceframes == NULL) { printf("[InitialisationAnime函数错误]动画序列帧有问题,请检查名为[%s]的动画是否存在!\n", name); return Error; }
	anime->animeswitch = 0;
	anime->sequenceframes = sequenceframes;
	anime->totalnumber = totalnumber;
	anime->number = 0;
	for (int i = 0; i < anime->totalnumber; i++)
	{
		anime->sequenceframes[i].lengths.x = width;
		anime->sequenceframes[i].lengths.y = height;
	}
	MoonTimeLoadInit(&(anime->timeload), timeload);		//设置定时器
	return YES;
}

extern int MoonAnimeRun(IMAGE* image, ANIME* anime, int animeswitch, int x, int y, float widthsize, float heightsize)
{
	if (!animeswitch)return 0;
	else
	{
		anime->number %= anime->totalnumber;
		TransparentBlt(image->image.hdc, x, y, anime->sequenceframes[anime->number].lengths.x * widthsize, anime->sequenceframes[anime->number].lengths.y * heightsize, anime->sequenceframes[anime->number].image.hdc, 0, 0, anime->sequenceframes[anime->number].lengths.x, anime->sequenceframes[anime->number].lengths.y, TRANSPARENTCOLOR);
	}
	if (MoonTimeLoad(&(anime->timeload), 1)) ++anime->number;	//添加下一帧	
	return anime->number;
}

extern void MoonAnimeDelete(ANIME* anime)
{
	for (int i = 0; i < anime->totalnumber; i++)MoonDeletImage(&anime->sequenceframes[i]);
}

extern void MoonAnimeCreate(PROJECTGOD* project, IMAGE* image, ANIME* anime, int totalnumber, const wchar_t** animename, char* entityname, int timeload, int width, int height)
{
	MoonImageLoadBatch(project, image, totalnumber, animename, width, height);
	MoonAnimeInit(anime, entityname, image, timeload, totalnumber, width, height);
	MoonCreateEntityIndex(project, image, entityname, sizeof(IMAGE));
}

extern void MoonTextFont(IMAGE* image, int x, int y, const wchar_t* text, COLORREF color, BOOL back, const wchar_t* font, int sizewidth, int sizeheight, int texttilt, int fonttilt, int FW_, int underline, int deleteline, int DEFAULT_)
{
	if (!back)SetBkMode(image->image.hdc, TRANSPARENT);
	HFONT hfont = CreateFont(sizeheight, sizewidth, texttilt, fonttilt, FW_, FALSE, underline, deleteline, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_, font);
	SelectObject(image->image.hdc, hfont);
	SetTextColor(image->image.hdc, color);
	TextOut(image->image.hdc, x, y, text, wcslen(text));
	HFONT hfontold = (HFONT)GetStockObject(SYSTEM_FONT);
	SelectObject(image->image.hdc, hfontold);
	DeleteObject(hfont);
}

extern int MoonGetColor(IMAGE* image, int x, int y)
{
	return GetPixel(image->image.hdc, x, y);
}

#endif