#include"Moon.h"

extern unsigned int Hash(char* text)
{
	if (text == NULL)return Error;
	unsigned int length = (unsigned)strlen(text), hash = 0;
	if (length <= 0)return Error;
	for (unsigned int i = 0; i < length; i++)hash += text[i] * (i + 1);
	return hash;
}

extern void TimeLoadInit(TIMELOAD* Timeload, int load)
{
	Timeload->time1 = NULL;
	Timeload->time2 = NULL;
	Timeload->timeload = load;
	Timeload->timeswitch = 0;
}

extern int TimeLoad(TIMELOAD* Timeload, int mode)
{
	if (!mode)return FALSE;
	else if (Timeload == NULL)
	{
		printf("Error:[TimeLoad函数错误!]存在空指针");
		return Error;
	}
	if (!Timeload->timeswitch)
	{
		Timeload->time1 = clock();
		if (Timeload->time1 > Timeload->time2 + Timeload->timeload)
		{
			Timeload->time2 = clock();
			Timeload->timeswitch = TRUE;
		}
	}
	else
	{
		Timeload->time2 = clock();
		Timeload->timeswitch = FALSE;
	}
	return Timeload->timeswitch;
}

extern int MoonSleep(int timeload)
{
	Sleep(timeload);
	return 0;
}

extern void* FindEntity(PROJECTGOD* project, char* nameid)
{
	return project->entityindex[(Hash(nameid) % ENTITYNUMBER)].entityindex;
}

extern int CreateEntityIndex(PROJECTGOD* project, void* arrentity, char* nameid, int length)
{
	int index = NOTFOUND;
	int hash = Hash(nameid) % ENTITYNUMBER;
	if (project->entityindex[hash].entityindex == NULL)
	{
		if (hash != Error)
		{
			index = hash;
			project->entityindex[index].entityindex = arrentity;
			project->entityindex[index].nameid = nameid;
			project->entityindex[index].length = length;
		}
		else printf("非法的名称[%s],无法通过这个字符串得到合法的索引", nameid);
	}
	else
	{
		printf("[CreateEntityIndex函数]报错,叫做[%s]的实体,此位置[%d],已有实体存在,名称为[%s],请换一个名字", nameid, hash, project->entityindex[index].nameid);
		strlen(project->entityindex[hash].nameid) <= 0 && ProjectError(&project->entityindex[hash], 2, (char*)"来自[CreateEntityIndex函数]的错误,出现了幽灵实体,没有合法名称");
		index = NOTFOUND;
	}
	return index;
}

extern int KeyState(int Key)
{
	static int KEYSTATEbuffer[255];
	int state = GetAsyncKeyState(Key);
	if (!(state & 0x8000))KEYSTATEbuffer[Key] = 0;
	else if (KEYSTATEbuffer[Key] == 0) { KEYSTATEbuffer[Key] = 1; return 1; }
	return 0;
}

extern void Music(LPCWSTR File)
{
    TCHAR cmd[255];
    wsprintf(cmd, TEXT("open \%s\ alias music"), File);
    mciSendString(TEXT("close music"), 0, 0, 0);
    mciSendString(cmd, NULL, 0, NULL);
    mciSendString(TEXT("play music"), NULL, 0, NULL);
}

extern LPCWSTR CharToLPCWSTR(char* str)
{
	static wchar_t* wideStr;
	wideStr = (wchar_t*)malloc(strlen(str) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wideStr, strlen(str));
	return wideStr;
}

extern void TextFont(IMAGE* image, int x, int y, LPCWSTR text, COLORREF color, BOOL back, LPCWSTR font, int sizewidth, int sizeheight, int texttilt, int fonttilt, int FW_, int underline, int deleteline, int DEFAULT_)
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

extern int GetColor(IMAGE* image, int x, int y)
{
	return GetPixel(image->image.hdc, x, y);
}

extern int TriangleDetection(POINT a, POINT b, POINT c, POINT p)
{
	int d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
	int d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
	int d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);
	return (d1 * d2 > 0) && (d2 * d3 > 0);
}

extern void RunProgram(LPCWSTR name) 
{ 
	ShellExecute(NULL, L"open", name, NULL, NULL, SW_SHOW); 
}

extern int ButtonInit(MOONBUTTON* button, int x, int y, int width, int height)
{
	button->x = x;
	button->y = y;
	button->width = width;
	button->height = height;
	button->mode = FALSE;
	button->triggermode = 1;
	return 1;
}

extern int ButtonDetection(PROJECTGOD* project, char* name)
{
	HashFindEntity(project, name, MOONBUTTON, button);
	if (project->entityindex[(Hash(name) % ENTITYNUMBER)].length != sizeof(MOONBUTTON))
	{
		ProjectError(button, 3, (char*)"[ButtonDetection函数]错误!错误原因:类型导入错误.");
		return 0;
	}
	HashFindEntity(project, "ProjectMouseCoord", POINT, mousecoord);
	if (mousecoord->x > button->x && mousecoord->x < (button->x + button->width) && mousecoord->y > button->y && mousecoord->y < (button->y + button->height))
	{
		if (KeyState(button->triggermode))button->mode = MOON_BUTTONPRESS;
		else button->mode = MOON_BUTTONRHOVER;

	}
	else button->mode = FALSE;
	switch (button->mode)
	{
	case MOON_BUTTONPRESS:	
		button->ButtonModePress && button->ButtonModePress(project, button);
			return MOON_BUTTONPRESS;
		break;
	case MOON_BUTTONRHOVER:
			button->ButtonModeHover && button->ButtonModeHover(project, button);
			return MOON_BUTTONRHOVER;
		break;
	}
	return 0;
}

extern int ButtonSetTriggerMode(PROJECTGOD* project,char* name,unsigned char key)
{
	HashFindEntity(project, name, MOONBUTTON, button);
	if (project->entityindex[Hash(name)].length != sizeof(MOONBUTTON))
	{
		ProjectError(button, 3, (char*)"[ButtonSetTriggerMode函数]错误!错误原因:类型导入错误.");
		return 0;
	}
	button->triggermode = key;
	return key;
}

extern int CharToWchar(wchar_t* text1, char* text2 , int len)
{
	setlocale(LC_ALL, "");
	mbstowcs(text1, text2, len);
	return 1;
}