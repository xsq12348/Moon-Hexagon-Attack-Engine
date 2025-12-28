#include"MoonHexagonAttackEngine_function.h"

//创建连接关系
static void HexagonConversion(HEXAGONMAP* hexagonmap, int MAPWIDTH, int MAPHEIGHT)
{
	for (int k = 0; k < MAPHEIGHT * MAPWIDTH; k++)
	{
		int o = k % MAPWIDTH, i = k / MAPWIDTH;
		for (int j = 1; j < 7; j++)hexagonmap[k].hexagon[j] = 0;//首次初始化
		hexagonmap[k].hexagon[0] = &hexagonmap[k];
		hexagonmap[k].hexagon[5] = &hexagonmap[RANGE(o, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i - 1, 0, MAPHEIGHT - 1)];
		hexagonmap[k].hexagon[2] = &hexagonmap[RANGE(o, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i + 1, 0, MAPHEIGHT - 1)];
		switch (o % 2)
		{
		case 0:
			hexagonmap[k].hexagon[1] = &hexagonmap[RANGE(o + 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i, 0, MAPHEIGHT - 1)];
			hexagonmap[k].hexagon[3] = &hexagonmap[RANGE(o - 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i, 0, MAPHEIGHT - 1)];
			hexagonmap[k].hexagon[4] = &hexagonmap[RANGE(o - 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i - 1, 0, MAPHEIGHT - 1)];
			hexagonmap[k].hexagon[6] = &hexagonmap[RANGE(o + 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i - 1, 0, MAPHEIGHT - 1)];
			break;

		case 1:
			hexagonmap[k].hexagon[1] = &hexagonmap[RANGE(o + 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i + 1, 0, MAPHEIGHT - 1)];
			hexagonmap[k].hexagon[3] = &hexagonmap[RANGE(o - 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i + 1, 0, MAPHEIGHT - 1)];
			hexagonmap[k].hexagon[4] = &hexagonmap[RANGE(o - 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i, 0, MAPHEIGHT - 1)];
			hexagonmap[k].hexagon[6] = &hexagonmap[RANGE(o + 1, 0, MAPWIDTH - 1) + MAPWIDTH * RANGE(i, 0, MAPHEIGHT - 1)];
			break;
		}
	}
}

static const char* ScriptConfigCommand[] = 
{
//	"Sid",			//归属阵营			.3
//	"CivF",			//民用工厂等级			.6
//	"MilF",			//军用工厂等级			.7
//	"AirPort",		//机场				.8
//	"ShipPort",		//船港				.9
	"Exit",			//退出				.0
	"Region",		//地区				.1
	"Name",			//名称				.2
	"City_Deg",		//城市规模			.3
	"VP",			//胜利分				.4
	"Traffic",		//交通水平			.5
	"Train",		//铁路				.6
	"Width",		//宽度				.7
	"Fortress",		//堡垒				.8
	"Terrain",		//地形				.9
};

static const char* ScriptConfigCommandOfUnit[] =
{
	"Exit",			//退出				.0
	"Unit",			//单位				.1
	"Name",			//名称				.2
	"Type",			//类型				.3
	"Level",		//等级				.4
	"Armor",		//装甲值				.5
	"Mobility",		//机动能力			.6
	"Supply",		//补给				.7
	"PV",			//总计划				.8
	"Sid",			//阵营				.9
	"People",		//人口				.10
	"Organization",	//组织度				.11
};

static const char* ScriptConfigCommandOfTemplate[] =
{
	"Exit",			//.0
	"Hq",			//.1
	"Inf",			//.2
	"Cav",			//.3
	"Sec",			//.4
	"Mot",			//.5
	"Tank",			//.6
	"Mtnd",			//.7
	"Mari",			//.8
	"MechInf",		//.9
	"Tree",			//.10
	"Plain",		//.11
	"Hills",		//.12
	"Mount",		//.13
	"Ocean",		//.14
	"River",		//.15
	"DenseTree",	//.16
};

static int ScriptConfigGameTemp(FILE* fp)
{
	int number = 0;
	fscanf(fp, "%d", &number);
	return number;
}

typedef struct
{
	int Armor;
	int Mobility;
	int People;
}TemplateTemp;

static TemplateTemp ScriptTemplateTemp(FILE* fp,int mode)
{
	if(mode)
	{
		int alpha = 0, beta = 0, gamma = 0;
		fscanf(fp, "{ Armor : %d , Mobility : %d , People : %d }\n", &alpha, &beta, &gamma);
		return (TemplateTemp) { alpha, beta, gamma };
	}
	else
	{
		int delta = 0;
		fscanf(fp, "{ %d }\n", &delta);
		return (TemplateTemp) { delta, 0, 0 };
	}
}

static void ScriptConfigEngine(PROJECTGOD* project, FILE* fpconfig)
{
	srand(time(0));
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT);
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
	HashFindEntity(project, "ScriptLogicMode", int, scriptlogicmode);
	HashFindEntity(project, "GAMEATTRIBUTE", GAMEATTRIBUTE, gameattribute);
	fscanf(fpconfig, "GAMEMAPWIDTH = %d", GAMEMAPWIDTH);
	fscanf(fpconfig, "\nGAMEMAPHEIGHT = %d", GAMEMAPHEIGHT);
	fscanf(fpconfig, "\nPlayer1Sid = %d", &gameattribute->player_1_sid);
	fscanf(fpconfig, "\nPlayer2Sid = %d", &gameattribute->player_2_sid);
	fscanf(fpconfig, "\nEveryDay = %d", &gameattribute->everyday);
	fscanf(fpconfig, "\nAllRound = %d", &gameattribute->allround);
	fscanf(fpconfig, "\nScriptLogic = %d", scriptlogicmode);
	*GAMEMAPWIDTH = RANGE(*GAMEMAPWIDTH, 52, GAMEMAPWIDTHMAX);
	*GAMEMAPHEIGHT = RANGE(*GAMEMAPHEIGHT, 30, GAMEMAPHEIGHTMAX);
	HexagonConversion(hexagon_map, *GAMEMAPWIDTH, *GAMEMAPHEIGHT);
	printf("GAMEMAPWIDTH =  %d\nGAMEMAPHEIGHT = %d\nPlayer1Sid = %d\nPlayer2Sid = %d\nEveryDay = %d\nAllRound = %d\nScriptLogic = %d\n", *GAMEMAPWIDTH, *GAMEMAPHEIGHT, gameattribute->player_1_sid, gameattribute->player_2_sid, gameattribute->everyday, gameattribute->allround, *scriptlogicmode);
	HexagonConversion(hexagon_map, *GAMEMAPWIDTH, *GAMEMAPHEIGHT);
	WeatherInit(project);
}

extern void ScriptConfigGame(PROJECTGOD* project, int mode)
{
	printf("\n[ScriptConfigGame函数]进入成功!\n");
	FILE* fpscript;
	FILE* fpscriptunit;
	FILE* fpscripttemplate;
	enum
	{
		StartGame,
		MODGame,
		HistoryGame,
		EditorGame,
	};
	fpscript = fopen("Engine_Default_File/Game_Script.txt", "r");
	fpscriptunit = fopen("Engine_Default_File/Game_Unit.txt", "r");
	fpscripttemplate = fopen("Engine_Default_File/Game_Template.txt", "r");
	switch (mode)
	{
	case StartGame:
		fpscript = fopen("Engine_Default_File/Game_Script.txt", "r");
		fpscriptunit = fopen("Engine_Default_File/Game_Unit.txt", "r");
		fpscripttemplate = fopen("Engine_Default_File/Game_Template.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Default_File/Game_Script.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscriptunit == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Default_File/Game_Unit.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscripttemplate == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Default_File/Game_Template.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		{
			FILE* fpconfig = fopen("Engine_Default_File/GAME_Configuration_File.txt", "r");
			if (fpconfig == NULL)
			{
				if (fpconfig == NULL)MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Default_File/GAME_Configuration_File.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return;
			}
			ScriptConfigEngine(project, fpconfig);
			fclose(fpconfig);
		}
		break;
	case MODGame:
		fpscript = fopen("Engine_MOD_File/Game_Script.txt", "r");
		fpscriptunit = fopen("Engine_MOD_File/Game_Unit.txt", "r");
		fpscripttemplate = fopen("Engine_MOD_File/Game_Template.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_MOD_File/Game_Script.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscriptunit == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_MOD_File/Game_Unit.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscripttemplate == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_MOD_File/Game_Template.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		{
			FILE* fpconfig = fopen("Engine_MOD_File/GAME_Configuration_File.txt", "r");
			if (fpconfig == NULL)
			{
				if (fpconfig == NULL)MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_MOD_File/GAME_Configuration_File.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return;
			}
			ScriptConfigEngine(project, fpconfig);
			fclose(fpconfig);
		}
		break;
	case HistoryGame:
		fpscript = fopen("Engine_History_File/Game_Script.txt", "r");
		fpscriptunit = fopen("Engine_History_File/Game_Unit.txt", "r");
		fpscripttemplate = fopen("Engine_History_File/Game_Template.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_History_File/Game_Script.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscriptunit == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_History_File/Game_Unit.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscripttemplate == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_History_File/Game_Template.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		{
			FILE* fpconfig = fopen("Engine_History_File/GAME_Configuration_File.txt", "r");
			if (fpconfig == NULL)
			{
				if (fpconfig == NULL)MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_History_File/GAME_Configuration_File.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return;
			}
			ScriptConfigEngine(project, fpconfig);
			fclose(fpconfig);
		}
		break;
	case EditorGame:
		fpscript = fopen("Engine_Editor_File/Game_Script.txt", "r");
		fpscriptunit = fopen("Engine_Editor_File/Game_Unit.txt", "r");
		fpscripttemplate = fopen("Engine_Editor_File/Game_Template.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_Script.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscriptunit == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_Unit.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		if (fpscripttemplate == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_Template.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return;
		}
		{
			FILE* fpconfig = fopen("Engine_Editor_File/GAME_Configuration_File.txt", "r");
			if (fpconfig == NULL)
			{
				if (fpconfig == NULL)MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/GAME_Configuration_File.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return;
			}
			ScriptConfigEngine(project, fpconfig);
			fclose(fpconfig);
		}
		break;
	}
	//地图脚本解释器
	{
		//初始化剧本头
		HashFindEntity(project, "GAMEATTRIBUTE", GAMEATTRIBUTE, gameattribute);
		HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
		HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH_2);
		HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT_2);
		static char text[1024], buffer[10] = { 0 }, regionname[256] = { 0 };
		fscanf(fpscript, "Script_Title:%s", text);
		fscanf(fpscript, "\nYear:%d", &gameattribute->year);
		fscanf(fpscript, "\nMonth:%d", &gameattribute->month);
		fscanf(fpscript, "\nDay:%d", &gameattribute->day);
		CharToWchar(gameattribute->script_title, text, 1024);
		printf("\n%s\n%d - %d - %d\n", text, gameattribute->year, gameattribute->month, gameattribute->day);
		int commandlen = sizeof(ScriptConfigCommand) / sizeof(ScriptConfigCommand[0]), number = 0, GAMEMAPWIDTH = *GAMEMAPWIDTH_2, GAMEMAPHEIGHT = *GAMEMAPHEIGHT_2;
		//判断地块
		while (1)
		{
			int mapx = 0, mapy = 0, found = NOTFOUND;
			fscanf(fpscript, "\n%s = [ %d , %d ]", buffer, &mapx, &mapy);
			if (!strcmp(buffer, ScriptConfigCommand[0]))
			{
				printf("\n[ScriptConfigGame函数]地形脚本解释器正常退出\n");
				break;
			}
			else if (!strcmp(buffer, ScriptConfigCommand[1]))
			{
				printf("坐标[%-3d][%-3d]开始配置地块[%d]", mapx, mapy, number++);
				mapx = RANGE(mapx, 1, GAMEMAPWIDTH);
				mapy = RANGE(mapy, 1, GAMEMAPHEIGHT);
				CharToWchar(gameattribute->script_title, text, 1024);
				fscanf(fpscript, "\n%s", buffer);
				if (strcmp(buffer, "{")) { printf("[ScriptConfigGame函数错误]脚本中未找到匹配的[ { ]\n地块坐标[%d,%d],模块[%d]\n", mapx, mapy, number); break; }
				else while (1)
				{
					fscanf(fpscript, "\n%s :", buffer);
					if (!strcmp("}", buffer))break;
					for (int i = 2; i < commandlen; i++)
						if (!strcmp(buffer, ScriptConfigCommand[i])) { found = i; break; }
					if (found == NOTFOUND) { printf("\n可能的语法错误[%s],未知的语法命令\n", buffer); continue; }
					switch (found)
					{
						//case 3: hexagon_map[mapx + mapy * GAMEMAPWIDTH].sidcolor = ScriptConfigGameTemp(fpscript); break;
						//case 6: hexagon_map[mapx + mapy * GAMEMAPWIDTH].civilian_factories = ScriptConfigGameTemp(fpscript); break;
						//case 7: hexagon_map[mapx + mapy * GAMEMAPWIDTH].military_factories = ScriptConfigGameTemp(fpscript); break;
						//case 8: hexagon_map[mapx + mapy * GAMEMAPWIDTH].airport = ScriptConfigGameTemp(fpscript); break;
						//case 9: hexagon_map[mapx + mapy * GAMEMAPWIDTH].shipport = ScriptConfigGameTemp(fpscript); break;
					case 2:
						fscanf(fpscript, "%s", regionname);
						strcpy(hexagon_map[mapx + mapy * GAMEMAPWIDTH].name, regionname);
						break;
					case 3: hexagon_map[mapx + mapy * GAMEMAPWIDTH].city_degree = ScriptConfigGameTemp(fpscript); break;
					case 4: hexagon_map[mapx + mapy * GAMEMAPWIDTH].victory_points = ScriptConfigGameTemp(fpscript); break;
					case 5:hexagon_map[mapx + mapy * GAMEMAPWIDTH].traffic = ScriptConfigGameTemp(fpscript); break;
					case 6:hexagon_map[mapx + mapy * GAMEMAPWIDTH].train = ScriptConfigGameTemp(fpscript); break;
					case 7:hexagon_map[mapx + mapy * GAMEMAPWIDTH].battlefield_foundation_width = ScriptConfigGameTemp(fpscript); break;
					case 8:hexagon_map[mapx + mapy * GAMEMAPWIDTH].fortress = ScriptConfigGameTemp(fpscript); break;
					case 9:hexagon_map[mapx + mapy * GAMEMAPWIDTH].terrain = ScriptConfigGameTemp(fpscript) % 8; break;
					}
				}
				printf("[%s]\n", hexagon_map[mapx + mapy * GAMEMAPWIDTH].name);
			}
			else
			{
				MessageBox(project->hwnd, L"[ScriptConfigGame函数]地形脚本解释器不正常退出\n请尝试重新加载游戏或者重新生成脚本", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				return;
			}
		}
	}

	//单位脚本解释器
	{
		HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
		HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH_2);
		HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT_2);
		static char buffer[10] = { 0 }, unitname[256] = { 0 };
		int commandlen = sizeof(ScriptConfigCommandOfUnit) / sizeof(ScriptConfigCommandOfUnit[0]), number = 0, GAMEMAPWIDTH = *GAMEMAPWIDTH_2, GAMEMAPHEIGHT = *GAMEMAPHEIGHT_2;
		//判断地块
		while (1)
		{
			int mapx = 0, mapy = 0, found = NOTFOUND;
			fscanf(fpscriptunit, "\n%s = [ %d , %d ]", buffer, &mapx, &mapy);
			if (!strcmp(buffer, ScriptConfigCommandOfUnit[0]))
			{
				printf("\n[ScriptConfigGame函数]单位脚本解释器部分正常退出");
				break;
			}
			else if (!strcmp(buffer, ScriptConfigCommandOfUnit[1]))
			{
				printf("坐标[%-3d][%-3d]开始配置单位[%d]", mapx, mapy, number++);
				mapx = RANGE(mapx, 1, GAMEMAPWIDTH);
				mapy = RANGE(mapy, 1, GAMEMAPHEIGHT);
				fscanf(fpscriptunit, "\n%s", buffer);
				if (strcmp(buffer, "{")) { printf("[ScriptConfigGame函数错误]脚本中未找到匹配的[ { ]\n单位坐标[%d,%d],模块[%d]\n", mapx, mapy, number); break; }
				else while (1)
				{
					fscanf(fpscriptunit, "\n%s :", buffer);
					if (!strcmp("}", buffer))break;
					for (int i = 2; i < commandlen; i++)
						if (!strcmp(buffer, ScriptConfigCommandOfUnit[i])) { found = i; break; }
					if (found == NOTFOUND) { printf("\n可能的语法错误[%s],未知的语法命令\n", buffer); continue; }
					switch (found)
					{
					case 2:
						fscanf(fpscriptunit, "%s", unitname);
						strcpy(hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.name, unitname);
						break;
					case 3: hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.unit_type = ScriptConfigGameTemp(fpscriptunit); break;
					case 4: hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.attack_level = ScriptConfigGameTemp(fpscriptunit); break;
					case 5:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.armor = ScriptConfigGameTemp(fpscriptunit); break;
					case 6:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.mobility = ScriptConfigGameTemp(fpscriptunit); break;
					case 7:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.supply_level = ScriptConfigGameTemp(fpscriptunit); break;
					case 8:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.planned_value = ScriptConfigGameTemp(fpscriptunit); break;
					case 9:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.sid = ScriptConfigGameTemp(fpscriptunit); break;
					case 10:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.people = ScriptConfigGameTemp(fpscriptunit); break;
					case 11:hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.organization = ScriptConfigGameTemp(fpscriptunit); break;
					}
				}
				printf("[%s]\n", hexagon_map[mapx + mapy * GAMEMAPWIDTH].unit.name);
			}
			else
			{
				MessageBox(project->hwnd, L"[ScriptConfigGame函数]单位脚本解释器不正常退出\n请尝试重新加载游戏或者重新生成脚本", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				return;
			}
		}
	}

	//模板配置脚本
	{
		HashFindEntity(project, "GameTemplate", MODDEFINITION, gametemplate);
		int commandlen = sizeof(ScriptConfigCommandOfTemplate) / sizeof(ScriptConfigCommandOfTemplate[0]),found = NOTFOUND;
		TemplateTemp alpha;
		static char buffer[255] = { 0 };
		while (1)
		{
			fscanf(fpscripttemplate, "%s = ", buffer);
			if (!strcmp(buffer, ScriptConfigCommandOfTemplate[0]))
			{
				printf("\n[ScriptConfigGame函数]配置脚本解释器部分正常退出");
				break;
			}
			for (int i = 1; i < commandlen; i++)
				if (!strcmp(buffer, ScriptConfigCommandOfTemplate[i])) { found = i; break; }
			if (found == NOTFOUND) { printf("\n可能的语法错误[%s],未知的语法命令\n", buffer); continue; }
			switch (found)
			{
			case 1: 
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->hq.armor = alpha.Armor;
				gametemplate->hq.mobility = alpha.Mobility;
				gametemplate->hq.people = alpha.People;
				break;
			case 2:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->inf.armor = alpha.Armor;
				gametemplate->inf.mobility = alpha.Mobility;
				gametemplate->inf.people = alpha.People;				
				break;
			case 3:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->cav.armor = alpha.Armor;
				gametemplate->cav.mobility = alpha.Mobility;
				gametemplate->cav.people = alpha.People;
				break;
			case 4:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->sec.armor = alpha.Armor;
				gametemplate->sec.mobility = alpha.Mobility;
				gametemplate->sec.people = alpha.People;
				break;
			case 5:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->mot.armor = alpha.Armor;
				gametemplate->mot.mobility = alpha.Mobility;
				gametemplate->mot.people = alpha.People;
				break;
			case 6:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->tank.armor = alpha.Armor;
				gametemplate->tank.mobility = alpha.Mobility;
				gametemplate->tank.people = alpha.People;
				break;
			case 7:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->mtnd.armor = alpha.Armor;
				gametemplate->mtnd.mobility = alpha.Mobility;
				gametemplate->mtnd.people = alpha.People;
				break;
			case 8:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->mari.armor = alpha.Armor;
				gametemplate->mari.mobility = alpha.Mobility;
				gametemplate->mari.people = alpha.People;
				break;
			case 9:
				alpha = ScriptTemplateTemp(fpscripttemplate,1);
				gametemplate->mechinf.armor = alpha.Armor;
				gametemplate->mechinf.mobility = alpha.Mobility;
				gametemplate->mechinf.people = alpha.People;
				break;

			case 10: 
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->tree = alpha.Armor;
				break;
			case 11:
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->plain = alpha.Armor;
				break;
			case 12:
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->hills = alpha.Armor;
				break;
			case 13:
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->mount = alpha.Armor;
				break;
			case 14:
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->ocean = alpha.Armor;
				break;
			case 15:
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->river = alpha.Armor;
				break;
			case 16:
				alpha = ScriptTemplateTemp(fpscripttemplate, 0);
				gametemplate->denseTree = alpha.Armor;
				break;
			}
		}
	}

	{
		HashFindEntity(project, "Editor", int, editor);
		if (!*editor)
		{
			HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
			for (int i = 0; i < GAMEMAPWIDTHMAX * GAMEMAPHEIGHTMAX; i++)
			{
				UnitInitTemplate(project, &hexagon_map[i].unit);
				TerrainWidthTemplate(project, &hexagon_map[i]);
			}
		}
	}

	fclose(fpscript);
	fclose(fpscriptunit);
	fclose(fpscripttemplate);
	{
		HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH);
		HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT);
		HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
		for (int o = 0; o < *GAMEMAPWIDTH; o++)
			for (int i = 0; i < *GAMEMAPHEIGHT; i++)
			{
				hexagon_map[o + i * *GAMEMAPWIDTH].coordx = o;
				hexagon_map[o + i * *GAMEMAPWIDTH].coordy = i;
				if (o == 0 || o == *GAMEMAPWIDTH - 1)hexagon_map[o + i * *GAMEMAPWIDTH].terrain = Terrain_No_Entry;
				if (i == 0 || i == *GAMEMAPHEIGHT - 1)hexagon_map[o + i * *GAMEMAPWIDTH].terrain = Terrain_No_Entry;
			}
		printf("\nGAMEMAPWIDTH[%d],GAMEMAPHEIGHT[%d]\n", *GAMEMAPWIDTH, *GAMEMAPHEIGHT);
	}
	printf("\n[ScriptConfigGame函数]已完成!\n");
}

static void CommandBarDrawTemp(IMAGE* image, LPCWSTR text, int x, int y)
{
	TextFont(image, 0, y, text, 0xe16e5b, FALSE, L"SimHei", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET);
	BoxFull(image, x, y, x + 48, y + 23, 0xffffff);
	Box(image, x, y, x + 48, y + 23, 2, 0x3232ac);
}

static PROJECTSETTING(TerrainCommandBarDraw)
{
	HashFindEntity(project, "Image_Terrain_Bar", IMAGE, image_terrain_bar);
	BoxFull(image_terrain_bar, 0, 0, image_terrain_bar->lengths.x, image_terrain_bar->lengths.y, MAPCOLOR);
	Box(image_terrain_bar, 0, 0, image_terrain_bar->lengths.x - 1, image_terrain_bar->lengths.y - 1, 4, 0x3232ac);
	CommandBarDrawTemp(image_terrain_bar, L"地形", 48, 3);
	CommandBarDrawTemp(image_terrain_bar, L"宽度", 48, 26);
	CommandBarDrawTemp(image_terrain_bar, L"交通", 48, 49);
	CommandBarDrawTemp(image_terrain_bar, L"堡垒", 48, 73);
	CommandBarDrawTemp(image_terrain_bar, L"分值", 48, 96);
	CommandBarDrawTemp(image_terrain_bar, L"天气", 48, 119);
	CommandBarDrawTemp(image_terrain_bar, L"城市", 48, 141);
}

static PROJECTSETTING(UnitCommandBarDraw)
{
	HashFindEntity(project, "Image_Unit_Bar", IMAGE, image_unit_bar);
	int left = 10, top = 10 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, right = WINDOWCOMMANDHEIGHT / 2, bottom = WINDOWCOMMANDHEIGHT / 2;
	BoxFull(image_unit_bar, 0, 0 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, WINDOWCOMMANDHEIGHT, 0 + 2 * WINDOWCOMMANDHEIGHT, 0x9dcdff);
	BoxFull(image_unit_bar, 0, 0 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, 65 /* WINDOWCOMMANDHEIGHT * .75 - 10 */, 65 /* 0 + WINDOWCOMMANDHEIGHT * .75 - 10*/, 0xfcdbcb);
	BoxFull(image_unit_bar, left, top, right, bottom, 0xffffff);
	Box(image_unit_bar, left, top, right, bottom, 2, 0);
	TextFont(image_unit_bar, 0, 75 /* WINDOWCOMMANDHEIGHT * .75f */, L"类型", 0x086cff, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	BoxFull(image_unit_bar, 48, 75, 96, 98, 0xffffff);
	Box(image_unit_bar, 48, 75, 96, 98, 2, 0x3232ac);
	{
		//人员
		BoxFull(image_unit_bar, 48, 100, 96, 114, 0xffffff);
		Box(image_unit_bar, 48, 100, 96, 114, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 100, L"人员", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_unit_bar, 50, 100, L"", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//等级
		BoxFull(image_unit_bar, 48, 114, 96, 128, 0xffffff);
		Box(image_unit_bar, 48, 114, 96, 128, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 114, L"等级", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_unit_bar, 50, 114, L"", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//决战计划
		BoxFull(image_unit_bar, 48, 128, 96, 142, 0xffffff);
		BoxFull(image_unit_bar, 48, 128, 48, 142, 0x36f2fb);
		Box(image_unit_bar, 48, 128, 96, 142, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 128, L"准备值", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//总计划值
		BoxFull(image_unit_bar, 48, 142, 96, 156, 0xffffff);
		BoxFull(image_unit_bar, 48, 142, 48, 156, 0x36fb42);
		Box(image_unit_bar, 48, 142, 96, 156, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 142, L"计划值", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//番号
		BoxFull(image_unit_bar, 4, 170, 96, 190, 0xffffff);
		Box(image_unit_bar, 4, 170, 96, 190, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 156, L"番号", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_unit_bar, 6, 172, L"", 0x0080ff, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	TextFont(image_unit_bar, 67, 0, L"移动", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	TextFont(image_unit_bar, 67, 16, L"方向", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	BoxFull(image_unit_bar, 67, 32, 97, 62, 0xffffff);
	Box(image_unit_bar, 67, 32, 97, 62, 2, 0);
}

static PROJECTSETTING( AttributeCommandBarDraw)
{
	HashFindEntity(project, "Image_Attribute_Bar", IMAGE, image_attribute_bar);
	BoxFull(image_attribute_bar, 0, 0, image_attribute_bar->lengths.x, image_attribute_bar->lengths.y, MAPCOLOR);
	Box(image_attribute_bar, 0, 0, image_attribute_bar->lengths.x - 1, image_attribute_bar->lengths.y - 1, 4, 0x3232ac);
	TextFont(image_attribute_bar, 0, 3, L"本回合属性", 0x3b999b, FALSE, L"SimHei", 0, 18, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET);
	CommandBarDrawTemp(image_attribute_bar, L"年", 48, 26);
	CommandBarDrawTemp(image_attribute_bar, L"月", 48, 49);
	CommandBarDrawTemp(image_attribute_bar, L"日", 48, 73);
	CommandBarDrawTemp(image_attribute_bar, L"回合", 48, 96);
	CommandBarDrawTemp(image_attribute_bar, L"玩家", 48, 119);
}

extern PROJECTSETTING(AllSetting)
{
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT);
	HashFindEntity(project, "UnitPalette", IMAGE, unitpalette);
	UnitCommandBarDraw(project);
	TerrainCommandBarDraw(project);
	AttributeCommandBarDraw(project);
	for (int o = 0; o < WINDOWCOMMANDHEIGHT; o++)
		for (int i = 0; i < WINDOWCOMMANDHEIGHT; i++)
			Pix(unitpalette, o, i, RGB(255 - (int)Lerp(0, 255, o * 1.f / WINDOWCOMMANDHEIGHT), (int)Lerp(0, 255, o * 1.f / WINDOWCOMMANDHEIGHT), (int)Lerp(0, 255, i * 1.f / WINDOWCOMMANDHEIGHT)));

}

extern PROJECTSETTING(HexagonMapInit)
{
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT);
	HashFindEntity(project, "Button_Return_Hall", MOONBUTTON, button_return_hall);
	for (int i = 0; i < GAMEMAPWIDTHMAX * GAMEMAPHEIGHTMAX; i++)
	{
		hexagon_map[i].coordx = Error;
		hexagon_map[i].coordy = Error;
		hexagon_map[i].terrain = 0;
		hexagon_map[i].train = 0;
		hexagon_map[i].battlefield_foundation_width = 0;
		hexagon_map[i].city_degree = 0;
		hexagon_map[i].fortress = 0;
		hexagon_map[i].traffic = 0;
		hexagon_map[i].victory_points = 0;
		hexagon_map[i].weather = 0;
		hexagon_map[i].unit.unit_type = 0;
		hexagon_map[i].unit.armor = 0;
		hexagon_map[i].unit.attack_level = 0;
		hexagon_map[i].unit.final_battle_blan = 0;
		hexagon_map[i].unit.mobility = 0;
		hexagon_map[i].unit.move = 0;
		hexagon_map[i].unit.organization = 0;
		hexagon_map[i].unit.people = 0;
		hexagon_map[i].unit.planned_value = 0;
		hexagon_map[i].unit.supply_level = 0;
		hexagon_map[i].unit.sid = 0;
		strcpy(hexagon_map[i].name, "NULL");
		strcpy(hexagon_map[i].unit.name, "NULL");
	}
	for (int o = 0; o < *GAMEMAPWIDTH; o++)
		for (int i = 0; i < *GAMEMAPHEIGHT; i++)
		{
			hexagon_map[o + i * *GAMEMAPWIDTH].coordx = o;
			hexagon_map[o + i * *GAMEMAPWIDTH].coordy = i;
			if (o == 0 || o == *GAMEMAPWIDTH - 1)hexagon_map[o + i * *GAMEMAPWIDTH].terrain = Terrain_No_Entry;
			if (i == 0 || i == *GAMEMAPHEIGHT - 1)hexagon_map[o + i * *GAMEMAPWIDTH].terrain = Terrain_No_Entry;
		}
}

extern PROJECTSETTING(OverSetting)
{
	EditorLogicSetting(project);
	SaveMap(project);
}

static void UnitInitTemplateTemp(UNIT* unit, MODUNITDEFINITION unittemplate)
{
	if (unit->people != 0)return;
	unit->armor = unittemplate.armor;
	unit->attack_level = 0;
	unit->mobility = unittemplate.mobility;
	unit->people = unittemplate.people;
	unit->move = 0;
	unit->planned_value = 255;
	unit->supply_level = 255;
	unit->organization = 255;
	unit->final_battle_blan = 0;
	unit->sid = unit->sid;
	strcpy(unit->name, "NULL");
}

//这些都是基础模板，正式版会给脚本予以自定义

extern void UnitInitTemplate(PROJECTGOD* project, UNIT* unit)
{
	HashFindEntity(project, "GameTemplate", MODDEFINITION, gametemplate);
	switch (unit->unit_type)
	{
	case UNIT_TYPE_HQ:      UnitInitTemplateTemp(unit, gametemplate->hq);      break;
	case UNIT_TYPE_INF:     UnitInitTemplateTemp(unit, gametemplate->inf);     break;
	case UNIT_TYPE_MOT:     UnitInitTemplateTemp(unit, gametemplate->mot);     break;
	case UNIT_TYPE_SEC:     UnitInitTemplateTemp(unit, gametemplate->sec);     break;
	case UNIT_TYPE_CAV:     UnitInitTemplateTemp(unit, gametemplate->cav);     break;
	case UNIT_TYPE_TANK:    UnitInitTemplateTemp(unit, gametemplate->tank);    break;
	case UNIT_TYPE_MTND:    UnitInitTemplateTemp(unit, gametemplate->mtnd);    break;
	case UNIT_TYPE_MARI:    UnitInitTemplateTemp(unit, gametemplate->mari);    break;
	case UNIT_TYPE_MECHINF: UnitInitTemplateTemp(unit, gametemplate->mechinf); break;
	}
}

extern void TerrainWidthTemplate(PROJECTGOD* project, HEXAGONMAP* hexagonmap)
{
	switch (hexagonmap->terrain)
	{
	case Terrain_Tree:      hexagonmap->traffic = 120; break;
	case Terrain_Hills:     hexagonmap->traffic = 140;  break;
	case Terrain_Mount:     hexagonmap->traffic = 180; break;
	case Terrain_Ocean:     hexagonmap->traffic = 225; break;
	case Terrain_Plain:     hexagonmap->traffic = 30;  break;
	case Terrain_River:     hexagonmap->traffic = 130; break;
	case Terrain_DenseTree: hexagonmap->traffic = 150; break;
	}
	if (hexagonmap->battlefield_foundation_width != 0)return;
	HashFindEntity(project, "GameTemplate", MODDEFINITION, gametemplate);
	switch (hexagonmap->terrain)
	{
	case Terrain_Tree:      hexagonmap->battlefield_foundation_width = gametemplate->tree;      break;
	case Terrain_Hills:     hexagonmap->battlefield_foundation_width = gametemplate->hills;     break;
	case Terrain_Mount:     hexagonmap->battlefield_foundation_width = gametemplate->mount;     break;
	case Terrain_Ocean:     hexagonmap->battlefield_foundation_width = gametemplate->ocean;     break;
	case Terrain_Plain:     hexagonmap->battlefield_foundation_width = gametemplate->plain;     break;
	case Terrain_River:     hexagonmap->battlefield_foundation_width = gametemplate->river;     break;
	case Terrain_DenseTree: hexagonmap->battlefield_foundation_width = gametemplate->denseTree; break;
	}
}

extern PROJECTMODULE(WeatherInit)
{
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT);
	for (int o = 0; o < *GAMEMAPWIDTH; o++)
		for (int i = 0; i < *GAMEMAPHEIGHT; i++)
		{
			int weather = Random(0, 100);
			hexagon_map[o + i * *GAMEMAPWIDTH].coordx = o;
			hexagon_map[o + i * *GAMEMAPWIDTH].coordy = i;
			hexagon_map[o + i * *GAMEMAPWIDTH].weather = (weather > 60 && weather <= 90) * 1 + (weather > 90) * 2;
		}
	for (int o = 0; o < *GAMEMAPWIDTH; o++)
		for (int i = 0; i < *GAMEMAPHEIGHT; i++)
		{
			if (hexagon_map[o + i * *GAMEMAPWIDTH].weather == 2)
				for (int k = 1; k < 7; k++)
					hexagon_map[o + i * *GAMEMAPWIDTH].hexagon[k]->weather = (Random(0, 100) > 95 ? 2 : (Random(0, 100) > 50 ? 1 : 0));
		}
}