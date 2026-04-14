#include"Moon.h"

#if OPEN_SDL

static SDL_Renderer* moon_renderer = NULL;
static IMAGE* image_old;

extern void MoonRendererLoad(PROJECTGOD* project)
{
	HashFindEntity(project, "ProjectSDLRenderer", SDL_Renderer, renderer);
	HashFindEntity(project, "ProjectBitmap", IMAGE, projectbitmap);
	moon_renderer = renderer;
	printf("[MoonRendererLoad]渲染器: %p, 纹理: %p\n", renderer, projectbitmap->image.bitmapgpu);
	printf("[MoonRendererLoad]纹理加载器工作完成\n");
}

extern void MoonDrawingArea(IMAGE* image_1, IMAGE* image_2, int x, int y, int width, int height)
{
	if (image_old != image_1)
		SDL_SetRenderTarget(moon_renderer, image_1->image.bitmapgpu);
	SDL_SetTextureBlendMode(image_2->image.bitmapgpu, SDL_BLENDMODE_NONE);
	SDL_RenderTexture(moon_renderer, image_2->image.bitmapgpu, NULL, &(SDL_FRect){ x, y, width, height });
	image_old = image_1;
}	


extern void MoonDrawingAreaAlpha(IMAGE* image_1, IMAGE* image_2, int x, int y, int width, int height, int transparent_color)
{
	if (image_old != image_1)
		SDL_SetRenderTarget(moon_renderer, image_1->image.bitmapgpu);
	SDL_SetTextureScaleMode(image_2->image.bitmapgpu, SDL_SCALEMODE_NEAREST);
	SDL_RenderTexture(moon_renderer, image_2->image.bitmapgpu, &(SDL_FRect){ x, y, width, height }, &(SDL_FRect){ x, y, width, height });
	image_old = image_1;
}

extern void MoonDrawingAreaRound(IMAGE* image_1, IMAGE* image_2, int x, int y, int apx, int apy, int width, int height, int deg)
{
	if (image_old != image_1)
		SDL_SetRenderTarget(moon_renderer, image_1->image.bitmapgpu);
	SDL_SetTextureBlendMode(image_2->image.bitmapgpu, SDL_BLENDMODE_NONE);
	SDL_RenderTextureRotated(moon_renderer, image_2->image.bitmapgpu, NULL, &(SDL_FRect){ x - apx, y - apy, width, height }, deg, & (SDL_FPoint){apx, apy}, SDL_FLIP_NONE);
	image_old = image_1;
}

extern void MoonDrawingAreaUV(IMAGE* image_1, IMAGE* image_2, int x, int y, int width, int height, int uv_x1, int uv_y1, int uv_width, int uv_height)
{
	if (image_old != image_1)
		SDL_SetRenderTarget(moon_renderer, image_1->image.bitmapgpu);
	SDL_SetTextureScaleMode(image_2->image.bitmapgpu, SDL_SCALEMODE_NEAREST);
	SDL_RenderTexture(moon_renderer, image_2->image.bitmapgpu, &(SDL_FRect){ uv_x1, uv_y1, uv_width, uv_height }, & (SDL_FRect) { x, y, width, height });
	image_old = image_1;
}

extern void MoonCreateImage(PROJECTGOD* project, IMAGE* image, int bmpwidth, int bmpheight)
{
	image->lengths.x = bmpwidth;
	image->lengths.y = bmpheight;
	image->image.width = bmpwidth;
	image->image.height = bmpheight;
	image->image.bitmapgpu = SDL_CreateTexture(moon_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bmpwidth, bmpheight);
	printf("\n[MoonCreateImage]渲染器: %p, 纹理: %p\n", moon_renderer, image->image.bitmapgpu);
}

extern void MoonDeletImage(IMAGE* image)
{
	SDL_DestroyTexture(image->image.bitmapgpu);
	image->image.bitmapgpu = NULL;
}

extern void MoonPix(IMAGE* image, int x, int y, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderPoint(moon_renderer, (float)x, (float)y);
	image_old = image;
}

extern void MoonLine(IMAGE* image, int x1, int y1, int x2, int y2, int width, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderLine(moon_renderer, x1, y1, x2, y2);
	image_old = image;
}

extern void MoonBox(IMAGE* image, int x1, int y1, int x2, int y2, int width, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderRect(moon_renderer, &(const SDL_FRect){x1, y1, x2 - x1, y2 - y1});
	image_old = image;
}

extern void MoonBoxFull(IMAGE* image, int x1, int y1, int x2, int y2, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderFillRect(moon_renderer, &(const SDL_FRect){x1, y1, x2 - x1, y2 - y1});
	image_old = image;
}

extern void MoonCircle(IMAGE* image, int x, int y, int r, int color)
{
	if (r <= 0)return;
	//需要繪製邊的數量
	int edge = min(360, 6 * r);					// 2 * Pi * r
	float rad = 2 * 3.141f / edge;		//360 / edge
	MOON_SDL_POINT* points = (MOON_SDL_POINT*)malloc(sizeof(MOON_SDL_POINT) * (edge + 1));
	if (points == NULL)return;
	for (int i = 0; i < edge; i++)
	{
		points[i].x = x + r * cos(rad * i);
		points[i].y = y + r * sin(rad * i);
	}
	points[edge] = points[0];
	MoonLineAll(image, points, edge + 1, color);
	free(points);
}


extern void MoonImageLoad(IMAGE* image, const wchar_t** imagefile, int imagenumber)
{
	if (imagenumber == 1)
	{
		char filename[1024];
		MoonWcharToChar(filename, (wchar_t*)*imagefile, 1024);
		SDL_Surface* bmp_file = { NULL };
		bmp_file = SDL_LoadBMP((const char*)filename);
		Uint32 sdl_color = TRANSPARENTCOLOR;
		SDL_SetSurfaceColorKey(bmp_file, TRUE, sdl_color);
		if (image->image.bitmapgpu)SDL_DestroyTexture(image->image.bitmapgpu);
		image->image.bitmapgpu = SDL_CreateTextureFromSurface(moon_renderer, bmp_file);
		SDL_DestroySurface(bmp_file);
	}
	else if (imagenumber > 0)
		for (int i = 0; i < imagenumber; i++)
		{
			char filename[1024];
			MoonWcharToChar(filename, imagefile[i], 1024);
			SDL_Surface* bmp_file = { NULL };
			bmp_file = SDL_LoadBMP((const char*)filename);
			Uint32 sdl_color = TRANSPARENTCOLOR;
			SDL_SetSurfaceColorKey(bmp_file, TRUE, sdl_color);
			if (image[i].image.bitmapgpu)SDL_DestroyTexture(image[i].image.bitmapgpu);
			image[i].image.bitmapgpu = SDL_CreateTextureFromSurface(moon_renderer, bmp_file);
			SDL_DestroySurface(bmp_file);
		}
}

extern void MoonImageLoadBatch(PROJECTGOD* project, IMAGE* image, int totalnumber, const wchar_t** name, int width, int height)
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
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_BLEND);
	if (!animeswitch)return 0;
	else
	{
		anime->number %= anime->totalnumber;
		//TransparentBlt(image->image.hdc, x, y, anime->sequenceframes[anime->number].lengths.x * widthsize, anime->sequenceframes[anime->number].lengths.y * heightsize, anime->sequenceframes[anime->number].image.hdc, 0, 0, anime->sequenceframes[anime->number].lengths.x, anime->sequenceframes[anime->number].lengths.y, TRANSPARENTCOLOR);
		SDL_RenderTexture(moon_renderer, anime->sequenceframes[anime->number].image.bitmapgpu, NULL, &(SDL_FRect){ x, y, anime->sequenceframes[anime->number].lengths.x* widthsize, anime->sequenceframes[anime->number].lengths.y* heightsize });
	}
	if (MoonTimeLoad(&(anime->timeload), 1)) ++anime->number;	//添加下一帧	
	image_old = image;
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

extern void MoonTextFont(IMAGE* image, int x, int y, const wchar_t** text, COLORREF color, BOOL back, const wchar_t** font, int sizewidth, int sizeheight, int texttilt, int fonttilt, int FW_, int underline, int deleteline, int DEFAULT_)
{

}

extern int MoonGetColor(IMAGE* image, int x, int y)
{

}

extern void MoonPixAll(IMAGE* image, MOON_SDL_POINT* points, int allnumber, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderPoints(moon_renderer, points, allnumber);
	image_old = image;
}

extern void MoonLineAll(IMAGE* image, MOON_SDL_POINT* points,int allnumber,int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderLines(moon_renderer, points, allnumber);
	image_old = image;
}

extern void MoonBoxAll(IMAGE* image, MOON_SDL_RECT* rect, int allnumber, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderRects(moon_renderer, rect, allnumber);
	image_old = image;
}

extern void MoonBoxFullAll(IMAGE* image, MOON_SDL_RECT* rect, int allnumber, int color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 r = color & 0xff, g = (color & 0xff00) >> 8, b = (color & 0xff0000) >> 16, alpha = (color & 0xff000000) >> 24;
	if (image_old != image)
		SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
	SDL_SetRenderDrawColor(moon_renderer, r, g, b, alpha);
	SDL_RenderFillRects(moon_renderer, rect, allnumber);
	image_old = image;
}

extern void MoonImageDesignated(IMAGE* image)
{
	SDL_SetRenderTarget(moon_renderer, image->image.bitmapgpu);
}

extern void MoonSetDrawImage(IMAGE* image,int mode)
{
	switch(mode)
	{
	case 1:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_NONE); break;
	case 2:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_BLEND); break;
	case 3:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_BLEND_PREMULTIPLIED); break;
	case 4:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_ADD); break;
	case 5:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_ADD_PREMULTIPLIED); break;
	case 6:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_MOD); break;
	case 7:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_MUL); break;
	case 8:SDL_SetTextureBlendMode(image->image.bitmapgpu, SDL_BLENDMODE_INVALID); break;
	}
}

extern void MoonModifyAlpha(IMAGE* image, int transparent_color)
{
	transparent_color >= 0 && SDL_SetTextureAlphaMod(image->image.bitmapgpu, (Uint8)transparent_color);
}

extern void MoonSDLTextInit(PROJECTGOD* project, IMAGE* textbuffer, const char* entity_name, int back_width, int back_height)
{
	MoonCreateImage(project, textbuffer, back_width, back_height);
	MoonCreateEntityIndex(project, textbuffer, entity_name, sizeof(IMAGE));
}

extern void MoonSDLTextFont(IMAGE* textbuffer, const char* text, int text_transparent_color, int back_transparent_color)
{
	//这里需要兼容GDI的BGR格式
	Uint8 back_r = back_transparent_color & 0xff, back_g = (back_transparent_color & 0xff00) >> 8, back_b = (back_transparent_color & 0xff0000) >> 16, back_alpha = (back_transparent_color & 0xff000000) >> 24;
	Uint8 text_r = text_transparent_color & 0xff, text_g = (text_transparent_color & 0xff00) >> 8, text_b = (text_transparent_color & 0xff0000) >> 16, text_alpha = (text_transparent_color & 0xff000000) >> 24;
	SDL_SetRenderTarget(moon_renderer, textbuffer->image.bitmapgpu);
	SDL_SetTextureBlendMode(textbuffer->image.bitmapgpu, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(moon_renderer, back_r, back_g, back_b, back_alpha);	
	SDL_RenderFillRect(moon_renderer, &(const SDL_FRect){0, 0, textbuffer->image.width, textbuffer->image.height});
	SDL_SetRenderDrawColor(moon_renderer, text_r, text_g, text_b, text_alpha);
	SDL_RenderDebugText(moon_renderer, 0, 0, text);
}


extern void MoonImagePlgBit(IMAGE* image_1, IMAGE* image_2, POINT point[4], int color)
{
	//需要兼容GDI的BGR格式
	float r = Lerp(0.f, 1.f, color & 0xff / 255), g = Lerp(0.f, 1.f, ((color & 0xff00) >> 8) / 255), b = Lerp(0.f, 1.f, ((color & 0xff0000) >> 16) / 255), alpha = Lerp(0.f, 1.f, ((color & 0xff000000) >> 24) / 255);
	SDL_Vertex vertex[4] =
	{
		{.color = {.a = alpha,.b = b,.g = g,.r = (float)r},.position = {.x = (float)point[0].x,.y = (float)point[0].y},.tex_coord = {.x = 0.f,.y = 0.f}},	//0
		{.color = {.a = alpha,.b = b,.g = g,.r = (float)r},.position = {.x = (float)point[1].x,.y = (float)point[1].y},.tex_coord = {.x = 1.f,.y = 0.f}},	//1
		{.color = {.a = alpha,.b = b,.g = g,.r = (float)r},.position = {.x = (float)point[2].x,.y = (float)point[2].y},.tex_coord = {.x = 0.f,.y = 1.f}},	//2
		{.color = {.a = alpha,.b = b,.g = g,.r = (float)r},.position = {.x = (float)point[3].x,.y = (float)point[3].y},.tex_coord = {.x = 1.f,.y = 1.f}},	//3
	};
	int pointindex[6] = { 0,1,2,1,2,3 };
	SDL_SetRenderTarget(moon_renderer, image_1->image.bitmapgpu);
	SDL_SetTextureBlendMode(image_2->image.bitmapgpu, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
	SDL_RenderGeometry(moon_renderer, image_2->image.bitmapgpu, vertex, 4, pointindex, 6);
}

#endif