#include"Moon.h"

static char Moon_Engine_VSn[4] = { 1,1,9,0 };
static TIMELOAD projectfps;
static int fpsmax, fpsmax2;
static IMAGE projectdoublebuffer;
static POINT projectmousecoord;
static ENTITYINDEX entityindex[ENTITYNUMBER];

static PROJECTMODULE(MoonLogicPause)
{
	//printf("\n[MoonLogicPause函数]进入成功!\n");
	MoonSleep(1);
	return 1;
}

static PROJECTMODULE(MoonDrawingPause)
{
	//printf("\n[MoonDrawingPause函数]进入成功!\n");
	MoonSleep(1);
	return 1;
}

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

extern HWND Window(LPCWSTR name,int window_coord_x, int window_coord_y, int window_width, int window_height)
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

extern void RunWindow()
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

static CREATETHREADFUNCTION(ProjectAttribute)
{
	GETTHREADRESOURCE(PROJECTGOD*, project);
	HashFindEntity(project, (char*)"ProjectMouseCoord", POINT, mousecoord);
	HashFindEntity(project, (char*)"ProjectFPS", int, fpsnumber);
	static int(*drawing)(PROJECTGOD*) = 0, (*logic)(PROJECTGOD*) = 0;
	static POINT mousecoord_2;
	drawing = project->Drawing;
	logic = project->Logic;
	int gamepowermode = project->Power;
	while (!project->DEAD)
	{
#if !MOONCENTRALDISPATCHER
		project->Logic != MoonLogicPause && (logic = project->Logic);
		project->Drawing != MoonDrawingPause && (drawing = project->Drawing);
#endif
		GetCursorPos(&mousecoord_2);
		ScreenToClient(project->hwnd, &mousecoord_2);
		mousecoord->x = mousecoord_2.x;
		mousecoord->y = mousecoord_2.y;
		ProjectPause(project->Power < 0, &project->Logic, MoonLogicPause, logic);
		ProjectPause(project->Power < 0, &project->Drawing, MoonDrawingPause, drawing);
#if MOONCENTRALDISPATCHER
		project->Logic != MoonLogicPause && (logic = project->Logic);
		project->Drawing != MoonDrawingPause && (drawing = project->Drawing);
#endif
		if (GetForegroundWindow() != project->hwnd)project->Power = NOTFOUND;
		else project->Power = gamepowermode;
		if (!IsWindow(project->hwnd))project->DEAD = YES;
		MoonSleep(1);
	}
	return 1;
}

extern void ProjectInit(PROJECTGOD* project, LPCWSTR project_name, int x, int y, int width, int height, int fps, void (*ProjectSetting_1)(PROJECTGOD*))
{
	printf("\n[ProjectInit]初始化函数进入成功\n");
	printf("                                          ___     \n      ___    ___  ______  ______  ___    /  /         \n     /   \\  /   \\/  __  \\/  __  \\/   \\  /  /     \n    /  /\\ \\/  /\\   /  \\    /  \\    /\\ \\/  /    \n   /  /  \\___/  \\  \\__/    \\__/   /  \\   /       \n  /  /           \\______/\\____   /    \\_/          \n /  /                        /  /                     \n/__/                        /__/                      \n");
	printf("MoonEngine[%d.%d.%d.%d]", Moon_Engine_VSn[0], Moon_Engine_VSn[1], Moon_Engine_VSn[2], Moon_Engine_VSn[3]);
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
	project->hwnd = Window(project_name, x, y, width, height);
	project->project_name = project_name;
	project->window_coord_x = x;
	project->window_coord_y = y;
	project->window_height = height;
	project->window_width = width;
	project->DEAD = FALSE;
	project->entityindex = entityindex;
	TimeLoadInit(&project->timeload, 1000.f / (fps > 0 ? fps : 60));
	TimeLoadInit(&projectfps, 1000);
	CreateImage(project, &projectdoublebuffer, project->window_width, project->window_height);
	CreateEntityIndex(project, &fpsmax2, (char*)"ProjectFPS", sizeof(int));
	CreateEntityIndex(project, &projectmousecoord, (char*)"ProjectMouseCoord", sizeof(POINT));
	CreateEntityIndex(project, &projectdoublebuffer, (char*)"ProjectBitmap", sizeof(IMAGE));
	if (ProjectSetting_1 != NULL)ProjectSetting_1(project);
	printf("\n[ProjectInit]初始化完成\n");
}

extern void ProjectRun(PROJECTGOD* project, int (*ProjectSetting_2)(PROJECTGOD*), int(*ProjectLogic)(PROJECTGOD*), int(*ProjectDrawing)(PROJECTGOD*))
{
	printf("\n[ProjectRun]引擎流程函数进入成功!\n");
	MSG msg = { 0 };
	HDC hdc = GetDC(project->hwnd);
	if (ProjectDrawing == NULL)
	{
		ProjectError(ProjectDrawing, 1, (char*)"绘图函数传入失败!");
		return;
	}
	project->Drawing = ProjectDrawing;
	if (ProjectSetting_2 != NULL)ProjectSetting_2(project);
	if (ProjectLogic != NULL)
	{
		project->Logic = ProjectLogic;
		CREATETHREAD(ProjectLogicThread, project);
	}
	CREATETHREAD(ProjectAttribute, project);
	HashFindEntity(project, "ProjectBitmap", IMAGE, projectbitmap);
	int runload[3] = { 0 };
	while (!project->DEAD)
	{
		runload[0] = clock();
		{
			if (!TimeLoad(&projectfps, TRUE))fpsmax++;
			else { fpsmax2 = fpsmax; fpsmax = 0; }
		}
		{
			BoxFull(projectbitmap, 0, 0, project->window_width, project->window_height, RGB(0, 0, 0));
			project->Drawing(project);
			BitBlt(hdc, 0, 0, project->window_width, project->window_height, projectbitmap->image.hdc, 0, 0, SRCCOPY);
		}
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		runload[1] = clock();
		runload[2] = runload[1] - runload[0];
		project->Power <= 0 && runload[2] < project->timeload.timeload && !TimeLoad(&project->timeload, TRUE) && MoonSleep((project->timeload.timeload - runload[2]));
	}
	printf("\n[ProjectRun]引擎流程结束!\n");
}

extern void ProjectOver(PROJECTGOD* project, void (*ProjectOverSetting)(PROJECTGOD*))
{
	printf("\n[ProjectOver]结束函数进入成功\n");
	if (project == NULL)
	{
		ProjectError(project, 2, (char*)"核心对象[projectgod]丢失!");
		return;
	}
	if (ProjectOverSetting != 0)ProjectOverSetting(project);
	HashFindEntity(project, "ProjectBitmap", IMAGE, projectbitmap);
	DeletImage(projectbitmap);
	for (int i = 0; i < ENTITYNUMBER; i++)
	{
		switch (project->entityindex[i].length)
		{
			case sizeof(IMAGE) : DeletImage((IMAGE*)project->entityindex[i].entityindex); break;
				case sizeof(ANIME) : AnimeDelete((ANIME*)project->entityindex[i].entityindex); break;
		}
		project->entityindex[i].length = NULL;
		project->entityindex[i].nameid = NULL;
		project->entityindex[i].entityindex = NULL;
	}
	printf("\n[ProjectOver]资源清理完成\n");
}

extern int ProjectError(void* alpha, int degree, char* text)
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
	while (!KeyState(VK_ESCAPE)) MoonSleep(1);
	return degree;
}

extern void ProjectPause(int mode, int (**function_1)(PROJECTGOD*), int (*function_2)(PROJECTGOD*), int (*function_3)(PROJECTGOD*))
{
	if (mode) *function_1 = function_2;
	else *function_1 = function_3;
}

extern void ProjectFunctionSwitch(int (**function_1)(PROJECTGOD*), int (*function_2)(PROJECTGOD*))
{
	*function_1 = function_2;
}

extern int  ProjectFindEntityAllNumber(PROJECTGOD* project)
{
	int all_number = 0;
	printf("\n   序号|地址            |索引      |名称                          |类型      |Hash      |\n");
	for (int i = 0; i < ENTITYNUMBER; i++)
		if (project->entityindex[i].length != 0)
		{
			all_number++;
			printf("%-7d|%p|%-10d|%-30s|%-10d|%-10d|\n", all_number, project->entityindex[i].entityindex, i, project->entityindex[i].nameid, project->entityindex[i].length, Hash(project->entityindex[i].nameid));
		}
	printf("\n[ProjectFindEntityAllNumber函数]进入成功!\n统计到的实体总数为[%d]\n", all_number);
	return all_number;
}