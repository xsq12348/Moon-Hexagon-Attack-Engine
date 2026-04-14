#include"Moon.h"

static char Moon_Engine_VSn[4] = { 1,2,2,8 };
static TIMELOAD projectfps;
static int fpsmax, fpsmax2;
static IMAGE projectdoublebuffer;
static POINT projectmousecoord;
static ENTITYINDEX entityindex[ENTITYNUMBER];

#if OPEN_SDL

static SDL_Renderer* moon_renderer = NULL;

#endif

static int ProjectConsole(PROJECTGOD* project, int (*developerconsole)(PROJECTGOD*));
static PROJECTMODULE(MoonLogicPause);
static PROJECTMODULE(MoonDrawingPause);
static LRESULT WINAPI WndPorc(HWND, UINT, WPARAM, LPARAM);
static CREATETHREADFUNCTION(ProjectLogicThread);

static LRESULT WINAPI WndPorc(HWND hwnd, UINT msgid, WPARAM wparam, LPARAM lparam)
{
	switch (msgid)
	{
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_SETCURSOR:
		switch (LOWORD(lparam))
		{
		default:SetCursor(LoadCursor(NULL, IDC_CROSS));
			break;
		}
		break;
	}
	return DefWindowProc(hwnd, msgid, wparam, lparam);
}

extern HWND MoonWindow(const wchar_t* name, int window_coord_x, int window_coord_y, int window_width, int window_height)
{
	HINSTANCE hinstance = GetModuleHandle(NULL);
	WNDCLASS wndclass = { 0 };
	wndclass.cbClsExtra = NULL;
	wndclass.cbWndExtra = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor = NULL;
	wndclass.hIcon = NULL;
	wndclass.hInstance = hinstance;
	wndclass.lpfnWndProc = WndPorc;
	wndclass.lpszClassName = name;
	wndclass.lpszMenuName = NULL;
	wndclass.style = NULL;
	RegisterClass(&wndclass);
	HWND hwnd = CreateWindow(name, name, WS_SYSMENU, window_coord_x, window_coord_y, window_width, window_height, NULL, NULL, hinstance, NULL);
	ShowWindow(hwnd, SW_SHOW);
	return hwnd;
}

extern void MoonRunWindow()
{
	//消息循环
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

static CREATETHREADFUNCTION(ProjectLogicThread)
{
	GETTHREADRESOURCE(PROJECTGOD*, project);
	while (!project->DEAD)
		project->Logic(project);
	return 1;
}

extern void MoonProjectInit(PROJECTGOD* project, const wchar_t* project_name, int x, int y, int width, int height, int fps, void (*ProjectSetting_1)(PROJECTGOD*))
{
	printf("\n[ProjectInit]初始化函数进入成功\n");
	printf("                                          ___     \n      ___    ___  ______  ______  ___    /  /         \n     /   \\  /   \\/  __  \\/  __  \\/   \\  /  /     \n    /  /\\ \\/  /\\   /  \\    /  \\    /\\ \\/  /    \n   /  /  \\___/  \\  \\__/    \\__/   /  \\   /       \n  /  /           \\______/\\____   /    \\_/          \n /  /                        /  /                     \n/__/                        /__/                      \n");
	printf("MoonEngine[%d.%d.%d.%d]\n", Moon_Engine_VSn[0], Moon_Engine_VSn[1], Moon_Engine_VSn[2], Moon_Engine_VSn[3]);
	/*
											  ___
		  ___    ___  ______  ______  ___    /  /
		 /   \  /   \/  __  \/  __  \/   \  /  /
		/  /\ \/  /\   /  \    /  \    /\ \/  /
	   /  /  \___/  \  \__/    \__/   /  \   /
	  /  /           \______/\____   /    \_/
	 /  /                        /  /
	/__/                        /__/

	*/
	if (x < 0 || y < 0)
	{
		x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}
#if OPEN_SDL

	//__ SDL实现
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_AUDIO);
	char windowname[1024] = { NULL };
	MoonWcharToChar(windowname, project_name, 1024);
	project->hwnd_sdl = SDL_CreateWindow((const char*)windowname, width, height, NULL);
	moon_renderer = SDL_CreateRenderer(project->hwnd_sdl, NULL);

#else
	project->hwnd = MoonWindow(project_name, x, y, width, height);
#endif
	project->project_name = project_name;
	project->window_coord_x = x;
	project->window_coord_y = y;
	project->window_height = height;
	project->window_width = width;
	project->DEAD = FALSE;
	project->entityindex = entityindex;
	MoonTimeLoadInit(&project->timeload, 1000.f / (fps > 0 ? fps : 60));
	MoonTimeLoadInit(&projectfps, 1000);
	MoonCreateEntityIndex(project, &fpsmax2, (char*)"ProjectFPS", sizeof(int));
	MoonCreateEntityIndex(project, &projectmousecoord, (char*)"ProjectMouseCoord", sizeof(POINT));
	MoonCreateEntityIndex(project, &projectdoublebuffer, (char*)"ProjectBitmap", sizeof(IMAGE));
#if !OPEN_SDL

	MoonCreateImage(project, &projectdoublebuffer, project->window_width, project->window_height);
#else
	MoonCreateEntityIndex(project, moon_renderer, (char*)"ProjectSDLRenderer", sizeof(SDL_Renderer*));
	MoonRendererLoad(project);//加载渲染器
	MoonCreateImage(project, &projectdoublebuffer, project->window_width, project->window_height);

#endif
	if (ProjectSetting_1 != NULL)ProjectSetting_1(project);
	printf("\n[ProjectInit]初始化完成\n");
}

#if OPEN_SDL

static CREATETHREADFUNCTION(ProjectDrawingThread)
{
	//__ SDL实现
	GETTHREADRESOURCE(PROJECTGOD*, project);
	HashFindEntity(project, "ProjectBitmap", IMAGE, projectbitmap);
	HashFindEntity(project, "ProjectSDLRenderer", SDL_Renderer, renderer);
	int runload[3] = { 0 };//帧率计时器
	printf("[ProjectDrawingThread]渲染器: %p, 纹理: %p\n", renderer, projectbitmap->image.bitmapgpu);
	//主循环
	while (!project->DEAD)
	{
		runload[0] = clock();
		{
			if (!MoonTimeLoad(&projectfps, TRUE))fpsmax++;
			else { fpsmax2 = fpsmax; fpsmax = 0; }
		}
		{
			//MoonBoxFull(projectbitmap, 0, 0, project->window_width, project->window_height, 0xff000000);
			project->Drawing(project);
			SDL_SetRenderTarget(renderer, NULL);
			SDL_RenderTexture(renderer, projectbitmap->image.bitmapgpu, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_SetRenderTarget(renderer, projectbitmap->image.bitmapgpu);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
		}
		runload[1] = clock();
		runload[2] = runload[1] - runload[0];
		project->Power <= 0 && runload[2] < project->timeload.timeload && MoonSleep((project->timeload.timeload - runload[2]));
	}
	//SDL_DestroyRenderer(moon_window_renderer);
	return 1;
}

#else
static CREATETHREADFUNCTION(ProjectDrawingThread)
{
	//GDI
	GETTHREADRESOURCE(PROJECTGOD*, project);
	HashFindEntity(project, "ProjectBitmap", IMAGE, projectbitmap);
	int runload[3] = { 0 };//帧率计时器
	HDC hdc = GetDC(project->hwnd);
	//主循环
	while (!project->DEAD)
	{
		runload[0] = clock();
		{
			if (!MoonTimeLoad(&projectfps, TRUE))fpsmax++;
			else { fpsmax2 = fpsmax; fpsmax = 0; }
		}
		{
			MoonBoxFull(projectbitmap, 0, 0, project->window_width, project->window_height, RGB(0, 0, 0));
			project->Drawing(project);
			BitBlt(hdc, 0, 0, project->window_width, project->window_height, projectbitmap->image.hdc, 0, 0, SRCCOPY);
		}
		runload[1] = clock();
		runload[2] = runload[1] - runload[0];
		project->Power <= 0 && runload[2] < project->timeload.timeload && MoonSleep((project->timeload.timeload - runload[2]));
	}
}

#endif


extern void MoonProjectRun(PROJECTGOD* project, int (*ProjectSetting_2)(PROJECTGOD*), int(*ProjectLogic)(PROJECTGOD*), int(*ProjectDrawing)(PROJECTGOD*))
{
	int runload[3] = { 0 };//帧率计时器
	printf("\n[ProjectRun]引擎流程函数进入成功!\n");
	if (ProjectDrawing == NULL)
	{
		MoonProjectError(ProjectDrawing, 1, (char*)"绘图函数传入失败!");
		return;
	}
	project->Drawing = ProjectDrawing;
	if (ProjectSetting_2 != NULL)ProjectSetting_2(project);
	//加载逻辑线程
	if (ProjectLogic != NULL)
	{
		project->Logic = ProjectLogic;
		CREATETHREAD(ProjectLogicThread, project);
	}

	CREATETHREAD(ProjectDrawingThread, project);//加载绘画线程

#if OPEN_SDL

	{
		HashFindEntity(project, (char*)"ProjectMouseCoord", POINT, mousecoord);
		HashFindEntity(project, (char*)"ProjectFPS", int, fpsnumber);
		static int(*drawing)(PROJECTGOD*) = 0, (*logic)(PROJECTGOD*) = 0, developer = FALSE;
		static float mousecoord_2[2];
		drawing = project->Drawing;
		logic = project->Logic;
		int gamepowermode = project->Power, modetemp = FALSE, focus = NULL;
		while (!project->DEAD)
		{
			project->Logic != MoonLogicPause && (logic = project->Logic);
			project->Drawing != MoonDrawingPause && (drawing = project->Drawing);
			SDL_PollEvent(&(project->sdl_event));
			switch (project->sdl_event.type)
			{
			case SDL_EVENT_WINDOW_FOCUS_LOST:
				project->Logic = MoonLogicPause;
				project->Drawing = MoonDrawingPause;
				project->Power = NOTFOUND;
				focus = TRUE;
				break;
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
				project->Logic = logic;
				project->Drawing = drawing;
				project->Power = gamepowermode;
				focus = FALSE;
				break;
			case SDL_EVENT_QUIT:
				project->DEAD = YES;
				break;
			case SDL_EVENT_MOUSE_MOTION:
				SDL_GetMouseState(&mousecoord_2[0], &mousecoord_2[1]);
				mousecoord->x = (int)mousecoord_2[0];
				mousecoord->y = (int)mousecoord_2[1];
				break;
			}
			//printf("%d,%d,%d                              \r", mousecoord->x, mousecoord->y, project->Power);
			(developer || focus) && (project->Power = NOTFOUND) || (project->Power = gamepowermode);
			//当Power改变时
			project->Power != modetemp && MoonProjectPause(project->Power < 0, &project->Logic, MoonLogicPause, logic);
			project->Power != modetemp && MoonProjectPause(project->Power < 0, &project->Drawing, MoonDrawingPause, drawing);
			modetemp = project->Power;
			!developer && MoonKeyState(VK_OEM_3) && (developer = TRUE, CMD(ON));
			developer && (ProjectConsole(project, project->developerconsole), (MoonKeyState(VK_OEM_3) && (developer = FALSE)));
			MoonSleep(1);
		}
	}
#else
	{
		MSG msg = { 0 };
		HDC hdc = GetDC(project->hwnd);
		//这里我想要性能高一点,就不写if了
		HashFindEntity(project, (char*)"ProjectMouseCoord", POINT, mousecoord);
		HashFindEntity(project, (char*)"ProjectFPS", int, fpsnumber);
		static int(*drawing)(PROJECTGOD*) = 0, (*logic)(PROJECTGOD*) = 0, developer = FALSE;
		static POINT mousecoord_2;
		drawing = project->Drawing;
		logic = project->Logic;
		int gamepowermode = project->Power, modetemp = FALSE, focus = NULL;
		while (!project->DEAD)
		{
			project->Logic != MoonLogicPause && (logic = project->Logic);
			project->Drawing != MoonDrawingPause && (drawing = project->Drawing);
			GetCursorPos(&mousecoord_2);
			ScreenToClient(project->hwnd, &mousecoord_2);
			mousecoord->x = mousecoord_2.x;
			mousecoord->y = mousecoord_2.y;
			//!IsWindow(project->hwnd) && (project->DEAD = YES);
			if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
				break;
			case WM_QUIT:
				project->DEAD = YES;
				break;
			}
			focus = GetForegroundWindow() != project->hwnd;
			(developer || focus) && (project->Power = NOTFOUND) || (project->Power = gamepowermode);
			//当Power改变时
			project->Power != modetemp && MoonProjectPause(project->Power < 0, &project->Logic, MoonLogicPause, logic);
			project->Power != modetemp && MoonProjectPause(project->Power < 0, &project->Drawing, MoonDrawingPause, drawing);
			modetemp = project->Power;
			!developer && MoonKeyState(VK_OEM_3) && (developer = TRUE);
			developer && (ProjectConsole(project, project->developerconsole), (MoonKeyState(VK_OEM_3) && (developer = FALSE)));
			MoonSleep(1);
		}
	}
#endif

	printf("\n[ProjectRun]引擎流程结束!\n");
}

extern void MoonProjectOver(PROJECTGOD* project, void (*ProjectOverSetting)(PROJECTGOD*))
{
	printf("\n[ProjectOver]结束函数进入成功\n");
	if (project == NULL)
	{
		MoonProjectError(project, 2, (char*)"核心对象[projectgod]丢失!");
		return;
	}
	if (ProjectOverSetting != 0)ProjectOverSetting(project);
	HashFindEntity(project, "ProjectBitmap", IMAGE, projectbitmap);
	MoonDeletImage(projectbitmap);
#if OPEN_SDL

	//__ SDL实现
	SDL_DestroyWindow(project->hwnd_sdl);
	SDL_DestroyRenderer(moon_renderer);
	SDL_Quit();

#endif
	for (int i = 0; i < ENTITYNUMBER; i++)
	{
		switch (project->entityindex[i].length)
		{
			case sizeof(IMAGE) : MoonDeletImage((IMAGE*)project->entityindex[i].entityindex); break;
				case sizeof(ANIME) : MoonAnimeDelete((ANIME*)project->entityindex[i].entityindex); break;
		}
		project->entityindex[i].length = NULL;
		project->entityindex[i].nameid = NULL;
		project->entityindex[i].entityindex = NULL;
	}
	printf("\n[ProjectOver]资源清理完成\n");
}

extern int MoonProjectError(void* alpha, int degree, char* text)
{
	enum
	{
		Serious = 1,
		General,
		Mild,
	};
	printf("\n来自[%p]的[%s]发生错误!现在转入错误处理函数[ProjectError]", alpha, text);
	switch (degree)
	{
	case Serious:printf("\t等级[Serious/严重]\n"); break;
	case General:printf("\t等级[General/一般]\n"); break;
	case Mild:printf("\t等级[Mild/轻微]\n"); break;
	}
	while (!MoonKeyState(VK_ESCAPE)) MoonSleep(1);
	return degree;
}

extern int MoonProjectPause(int mode, int (**function_1)(PROJECTGOD*), int (*function_2)(PROJECTGOD*), int (*function_3)(PROJECTGOD*))
{
	if (mode) *function_1 = function_2;
	else *function_1 = function_3;
	return 1;
}

extern void MoonProjectFunctionSwitch(int (**function_1)(PROJECTGOD*), int (*function_2)(PROJECTGOD*))
{
	*function_1 = function_2;
}

extern int MoonProjectFindEntityAllNumber(PROJECTGOD* project)
{
	int all_number = 0;
	printf("\n   序号|地址            |索引      |名称                          |类型      |Hash      |\n");
	for (int i = 0; i < ENTITYNUMBER; i++)
		if (project->entityindex[i].length != 0)
		{
			all_number++;
			printf("%-7d|%p|%-10d|%-30s|%-10d|%-10d|\n", all_number, project->entityindex[i].entityindex, i, project->entityindex[i].nameid, project->entityindex[i].length, MoonHash(project->entityindex[i].nameid));
		}
	printf("\n[ProjectFindEntityAllNumber函数]进入成功!\n统计到的实体总数为[%d]\n", all_number);
	return all_number;
}

static int ProjectConsole(PROJECTGOD* project, int (*developerconsole)(PROJECTGOD*))
{
	if (developerconsole == NULL)return FALSE;
	developerconsole(project);
	return 1;
}

static PROJECTMODULE(MoonLogicPause)
{
	//printf("[MoonLogicPause函数]进入成功!\n");
	MoonSleep(1);
	return NOTFOUND;
}

static PROJECTMODULE(MoonDrawingPause)
{
	//printf("[MoonDrawingPause函数]进入成功!\n");
	MoonSleep(1);
	return NOTFOUND;
}