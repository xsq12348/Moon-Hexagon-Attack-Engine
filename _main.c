#include"MoonHexagonAttackEngine_function.h"
void main()
{
	CMD(OFF);
	PROJECTGOD project;
	ProjectInit(&project, L"MoonHexagonAttackEngine", -1, -1, WINDOWWIDTH + 15 + WINDOWCOMMANDHEIGHT, WINDOWHEIGHT + 39, 60, ResourceLoad);
	ProjectRun(&project, AllSetting, LogicNULL, GameStart);
	ProjectOver(&project, OverSetting);
}

extern PROJECTMODULE(LogicNULL)
{
	MoonSleep(1);
}

static PROJECTSETTING(ButtonTemp)
{
	HashFindEntity(project, "PlayGameMode", int, playgamemode);
	ScriptConfigGame(project, *playgamemode);
	ProjectFunctionSwitch(&project->Logic, GameLogicSetting);
	FUNCTIONSWITCH(GameDrawingSetting);
}

static int ButtonTemporaryFunction(PROJECTGOD* project, char* text, IMAGE* image, MOONBUTTON* button ,LPCWSTR lptext)
{
	switch (ButtonDetection(project, text))
	{
	case MOON_BUTTONPRESS:
		Box(image, button->x + 5, button->y + 5, button->x + button->width + 5, button->y + button->height + 5, 20, RGB(84, 101, 88));
		Box(image, button->x, button->y - 15, button->x + button->width, button->y + button->height - 15, 20, RGB(40, 57, 88));
		BoxFull(image, button->x, button->y - 15, button->x + button->width, button->y + button->height - 15, RGB(40, 57, 88));
		TextFont(image, button->x, button->y - 15, lptext, RGB(255, 215, 0), FALSE, L"Impact", 0, 48, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		return MOON_BUTTONPRESS;
		break;

	case MOON_BUTTONRHOVER:
		Box(    image, button->x + 5, button->y + 5, button->x + button->width + 5, button->y + button->height + 5,  20, RGB(84, 101, 88));
		BoxFull(image, button->x + 5, button->y + 5, button->x + button->width + 5, button->y + button->height + 5,      RGB(84, 101, 88));
		Box(    image, button->x,     button->y - 15, button->x + button->width,     button->y + button->height - 15,  20, RGB(40, 57, 88));
		BoxFull(image, button->x,     button->y - 15, button->x + button->width,     button->y + button->height - 15,      RGB(40, 57, 88));
		TextFont(image, button->x, button->y - 15, lptext, RGB(255, 215, 0), FALSE, L"Impact", 0, 48, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;

	case 0:
		Box(    image, button->x + 5, button->y + 5, button->x + button->width + 5, button->y + button->height + 5,  20, RGB(40, 57, 44));
		Box(    image, button->x,     button->y,     button->x + button->width,     button->y + button->height,      20, RGB(40, 57, 88));
		BoxFull(image, button->x,     button->y,     button->x + button->width,     button->y + button->height,          RGB(40, 57, 88));
		TextFont(image, button->x, button->y, lptext, RGB(255, 215, 0), FALSE, L"Impact", 0, 48, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	}
	return 0;
}

extern PROJECTMODULE(GameStart)
{
	HashFindEntity(project, "ProjectBitmap", IMAGE, engineback);
	HashFindEntity(project, "Button_Start",   MOONBUTTON, button_start);
	HashFindEntity(project, "Button_History", MOONBUTTON, button_history);
	HashFindEntity(project, "Button_Editor",  MOONBUTTON, button_editor);
	HashFindEntity(project, "Button_Editor_Old",  MOONBUTTON, button_editor_old);
	HashFindEntity(project, "Button_CMD",  MOONBUTTON, button_CMD);
	HashFindEntity(project, "Button_Exit",    MOONBUTTON, button_exit);
	HashFindEntity(project, "Button_Mod", MOONBUTTON, button_mod);
	HashFindEntity(project, "Button_Play_Editor", MOONBUTTON, button_play_editor);
	HashFindEntity(project, "Button_Play_Continue", MOONBUTTON, button_play_continue);
	HashFindEntity(project, "Button_Play_Tutorial", MOONBUTTON, button_play_tutorial);
	HashFindEntity(project, "ScriptLogicMode", int, scriptlogicmode);
	HashFindEntity(project, "PlayGameMode", int, playgamemode);
	BoxFull(engineback, 0, 0, WINDOWWIDTH + WINDOWCOMMANDHEIGHT, WINDOWHEIGHT, RGB(15, 25, 45));
	if (ButtonTemporaryFunction(project, "Button_Start", engineback, button_start, L"开始游戏")) 
	{
		*scriptlogicmode = 0;
		*playgamemode = 0;
		ButtonTemp(project);
	}
	if (ButtonTemporaryFunction(project, "Button_Mod", engineback, button_mod, L"模组MOD"))
	{
		*scriptlogicmode = 1;
		*playgamemode = 1;
		ButtonTemp(project);
	}
	if (ButtonTemporaryFunction(project, "Button_Play_Editor", engineback, button_play_editor, L"玩编辑器"))
	{
		*scriptlogicmode = 3;
		*playgamemode = 3;
		ButtonTemp(project);
	}
	if (ButtonTemporaryFunction(project, "Button_History", engineback, button_history, L"历史游戏"))
	{
		*playgamemode = 0;
		ButtonTemp(project);
	}
	if (ButtonTemporaryFunction(project, "Button_Editor", engineback, button_editor, L" 新编辑"))
	{
		HashFindEntity(project, "Editor", int, editor);
		HashFindEntity(project, "Map_Size", int, map_size);
		*editor = TRUE;
		*map_size = Medium_Map;
		ScriptConfigGame(project, 3);
		HexagonMapInit(project);
		ProjectFunctionSwitch(&project->Logic, GameLogicSetting);
		FUNCTIONSWITCH(GameDrawingSetting);
	}
	if (ButtonTemporaryFunction(project, "Button_CMD", engineback, button_CMD, L"开发者"))
	{
		static alpha;
		alpha = !alpha;
		CMD(alpha);
	}
	if (ButtonTemporaryFunction(project, "Button_Editor_Old", engineback, button_editor_old, L"继续编辑"))
	{
		HashFindEntity(project, "Editor", int, editor);
		HashFindEntity(project, "Map_Size", int, map_size);
		*editor = TRUE;
		*map_size = Medium_Map;
		*playgamemode = 3;
		ButtonTemp(project);
	}
	if (ButtonTemporaryFunction(project, "Button_Play_Continue", engineback, button_play_continue, L"继续游戏") && *playgamemode != 3 && *playgamemode != NOTFOUND)
	{
		ProjectFunctionSwitch(&project->Logic, GameLogicSetting);
		FUNCTIONSWITCH(GameDrawingSetting);
	}
	if (ButtonTemporaryFunction(project, "Button_Play_Tutorial", engineback, button_play_tutorial, L"游戏说明"))
	{
		MessageBox(project->hwnd, 
			L"Moon Hexagon Attack Engine是我[xsq1234]设计的一款带编辑器的游戏\n"
			 "为什么加了Engine这个后缀呢, 因为这最初只是我在MoonEngine上拿来练手的作品\n"
			 "主要是为了拍视频做战报和当成大学里的毕业作品而不是模拟游玩\n"
			 "即在MoonEngine这个引擎的通用框架上再写一款特定方向的游戏\n所以在其中您可以看见编辑器和自定义MOD\n"
			 "之后呢我觉得还不够所以添加了战斗和移动系统以及回合流程\n就变成了如今可以自定义也可以玩的游戏\n"
			 "                                                                                                             祝您玩得愉快\n"
			 "                                                                                                             2025-12-26\n"
			 "                                                                                                           [作者xsq1234]\n"
			 "                                                                                  Email[1993346266@qq.com]\n"
			, L"Moon Hexagon Attack 游戏说明			                                         ", MB_OK);
		MessageBox(project->hwnd, 
			L"方向键[⬆⬇⬅⮕]可以移动地图\n"
			 "上排数字键[1][2]可以调整地图缩放,总计三个规模,当然编辑器是不能缩放的\n"
			 "鼠标左键可以选择单位和地块,右侧命令栏会显示它们的状态\n"
			 "如果您选中了单位,它会呈现抬升的美术效果,右键它周围的地块可以指定移动方向\n"
			, L"游戏内按键说明", MB_OK);
		MessageBox(project->hwnd, 
			 L"上排数字键[1][2]可以设置玩家一和玩家二的颜色值\n"
			 "上排数字键[3][4]可以设置玩家一和玩家二的到当前单位颜色\n"
			 "鼠标左键和右键调整单位和地形类型\n"
			, L"编辑器按键说明", MB_OK);

	}
	if (ButtonTemporaryFunction(project, "Button_Exit", engineback, button_exit, L"退出游戏")) project->DEAD = TRUE;
	{
		int width = project->window_width / 4 - 250, height = project->window_height, titleY = height / 20, buttonreturn = 0;
		TextFont(engineback, width + 3, titleY + 3, L"Moon Hexagon Attack Engine", RGB(100, 100, 100), FALSE, L"Comic Sans MS", 0, 48, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(engineback, width, titleY, L"Moon Hexagon Attack Engine", RGB(255, 215, 0), FALSE, L"Comic Sans MS", 0, 48, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(engineback, width, titleY + 70, L"MHAE引擎", RGB(200, 200, 255), FALSE, L"SimHei", 0, 24, 0, 0, FW_NORMAL, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(engineback, 0, WINDOWHEIGHT - 15, L"序列号:0000-0000-0000-0001", RGB(150, 150, 150), FALSE, L"Arial", 0, 14, 0, 0, FW_NORMAL, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(engineback, 0, WINDOWHEIGHT - 30, L"作者: xsq1234", RGB(150, 150, 150), FALSE, L"Arial", 0, 14, 0, 0, FW_NORMAL, FALSE, FALSE, DEFAULT_CHARSET);
	}
	return 1;
}

extern PROJECTSETTING(ReturnGameStart)
{
	HashFindEntity(project, "Editor", int, editor);
	*editor = FALSE;
	FUNCTIONSWITCH(GameStart);
	ProjectFunctionSwitch(&project->Logic, LogicNULL);
}
//Easter egg
// 12.12 I broke up with the girlfriend in my heart.
// Chinese version
//或许，我曾与你相遇
//但此刻
//我的心静如止水
//原谅我的飘忽不定
//只因
//你我之间本无可能
// English version
//Perhaps we have met before,
//But now
//My heart is calm as still water.
//Forgive my wavering ways,
//For
//There was never a possibility between us.