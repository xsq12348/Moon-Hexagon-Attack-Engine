#include"MoonHexagonAttackEngine_function.h"

static POINT* mousecoord;
static POINT* hexagon_mouse_coord;
static POINT* hexagon_map_coord;
static int* hexagon_r;
static int* drawingcommand;
static int* unitpalettecolor;
static int* editormode;
static int GAMEMAPWIDTH;
static int GAMEMAPHEIGHT;
static int templatesave;
static HEXAGONMAP* hexagon_map;
GAMEATTRIBUTE* gameattribute;

static int ButtonBarReturnHallStart(PROJECTGOD* project, MOONBUTTON* buton)
{
	SaveMap(project);
	ReturnGameStart(project);
}
static int ButtonTemplateSaveStart(PROJECTGOD* project, MOONBUTTON* buton)
{	
	return templatesave;
}

extern PROJECTMODULE(EditorLogicSetting)
{
	HashFindEntity(project, "Button_Editor_Template", MOONBUTTON, button_editor_template);
	HashFindEntity(project, "Button_Return_Hall", MOONBUTTON, button_return_hall);
	HashFindEntity(project, "HexagonMouseCoord", POINT, hexagon_mouse_coord_2);
	HashFindEntity(project, "Hexagon_Map_Coord", POINT, hexagon_map_coord_2);
	HashFindEntity(project, "ProjectMouseCoord", POINT, mousecoord_2);
	HashFindEntity(project, "GAMEATTRIBUTE", GAMEATTRIBUTE, gameattribute_2);
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map_2);
	HashFindEntity(project, "UnitPaletteColor", int, unitpalettecolor_2);
	HashFindEntity(project, "Drawing_Command", int, drawingcommand_2);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT_2);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH_2);
	HashFindEntity(project, "EditorMode", int, editormode_2);
	HashFindEntity(project, "Hexagon_R", int, hexagon_r_2);
	HashFindEntity(project, "Map_Size", int, map_size);
	hexagon_map = hexagon_map_2;
	mousecoord = mousecoord_2;
	hexagon_r = hexagon_r_2;
	editormode = editormode_2;
	unitpalettecolor = unitpalettecolor_2;
	hexagon_mouse_coord = hexagon_mouse_coord_2;
	hexagon_map_coord = hexagon_map_coord_2;
	drawingcommand = drawingcommand_2;
	GAMEMAPWIDTH = *GAMEMAPWIDTH_2;
	GAMEMAPHEIGHT = *GAMEMAPHEIGHT_2;
	gameattribute = gameattribute_2;
	*map_size = Medium_Map;
	button_return_hall->ButtonModePress = ButtonBarReturnHallStart;
	button_editor_template->ButtonModePress = ButtonTemplateSaveStart;
	templatesave = FALSE;
	ProjectFunctionSwitch(&project->Logic, EditorLogic);
	return 1;
}

//编辑器逻辑
static void ButtonEditorLogic(int mode)
{
	int alpha = 0;
	KeyState(1) && (alpha = 1);
	KeyState(2) && (alpha = -1);
	if (alpha)
	{
		GetHexagonMouseCoord();
		switch (mode)
		{
		case 1: break;
		case 2:
			hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].terrain = abs(hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].terrain + alpha) % 7;
			printf("Terrain[%d]\n", hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].terrain);
			break;
		case 3:hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].train = !hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].train; break;
		case 4:
			hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].unit.unit_type = abs(hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].unit.unit_type + alpha) % 10;
			hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].unit.sid = *unitpalettecolor;
			break;
		case 5:
			hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].city_degree = hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].city_degree + alpha * 10;
			break;
		case 6:
			if (hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].unit.unit_type > 0)*unitpalettecolor = hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH].unit.sid;
			break;
		}
		*drawingcommand = DRAW_Redraw;
		printf("x:%d,y:%d            \n", hexagon_mouse_coord->x, hexagon_mouse_coord->y);
	}
}

extern PROJECTMODULE(EditorLogic)
{
	MoonSleep(1);
	{
		(ButtonDetection(project, "Button_Return_Hall")     == MOON_BUTTONPRESS) && (*editormode = 1);
		(ButtonDetection(project, "Button_Editor_Terrain")  == MOON_BUTTONPRESS) && (*editormode = 2);
		(ButtonDetection(project, "Button_Editor_Train")    == MOON_BUTTONPRESS) && (*editormode = 3);
		(ButtonDetection(project, "Button_Editor_Unit")     == MOON_BUTTONPRESS) && (*editormode = 4);
		(ButtonDetection(project, "Button_Editor_City")     == MOON_BUTTONPRESS) && (*editormode = 5);
		(ButtonDetection(project, "Button_Editor_GetColor") == MOON_BUTTONPRESS) && (*editormode = 6);
		(ButtonDetection(project, "Button_Editor_Template") == MOON_BUTTONPRESS) && (templatesave = !templatesave);
	}
	if (mousecoord->x > 0 && mousecoord->y > 0 && mousecoord->x < WINDOWWIDTH && mousecoord->y < WINDOWHEIGHT)
		ButtonEditorLogic(*editormode);
	else if (KeyState(2) && mousecoord->x > WINDOWWIDTH && mousecoord->y > WINDOWHEIGHT - WINDOWCOMMANDHEIGHT && mousecoord->x < WINDOWWIDTH + WINDOWCOMMANDHEIGHT && mousecoord->y < WINDOWHEIGHT)
	{
		int x = RANGE(mousecoord->x - WINDOWWIDTH, 0, 255), y = RANGE(mousecoord->y - (WINDOWHEIGHT - WINDOWCOMMANDHEIGHT), 0, 255);
		*unitpalettecolor = RGB(
			(255 - (int)Lerp(0, 255, x * 1.f / WINDOWCOMMANDHEIGHT)),
			(int)Lerp(0, 255, x * 1.f / WINDOWCOMMANDHEIGHT), 
			(int)Lerp(0, 255, y * 1.f / WINDOWCOMMANDHEIGHT));
	}
	{
		if (KeyState(VK_UP)) { hexagon_map_coord->y = RANGE(hexagon_map_coord->y - 1, 0, GAMEMAPHEIGHT - (int)(WINDOWHEIGHT / (*hexagon_r * 1.732f))); *drawingcommand = DRAW_Redraw; }
		if (KeyState(VK_DOWN)) { hexagon_map_coord->y = RANGE(hexagon_map_coord->y + 1, 0, GAMEMAPHEIGHT - (int)(WINDOWHEIGHT / (*hexagon_r * 1.732f))); *drawingcommand = DRAW_Redraw; }
		if (KeyState(VK_LEFT)) { hexagon_map_coord->x = RANGE(hexagon_map_coord->x - 2, 0, (GAMEMAPWIDTH - (WINDOWWIDTH / (*hexagon_r * 1.5) - 2)) / 2); *drawingcommand = DRAW_Redraw; }
		if (KeyState(VK_RIGHT)) { hexagon_map_coord->x = RANGE(hexagon_map_coord->x + 2, 0, (GAMEMAPWIDTH - (WINDOWWIDTH / (*hexagon_r * 1.5) - 2)) / 2); *drawingcommand = DRAW_Redraw; }
		{
			wchar_t text[32];
			if (KeyState(VK_1))
			{
				swprintf(text, 32, L"已将玩家一的颜色设置为%d", *unitpalettecolor);
				MessageBox(project->hwnd, text, L"编辑器消息", MB_OK);
				gameattribute->player_1_sid = *unitpalettecolor;
			}
			else if (KeyState(VK_2))
			{
				swprintf(text, 32, L"已将玩家二的颜色设置为%d", *unitpalettecolor);
				MessageBox(project->hwnd, text, L"编辑器消息", MB_OK);
				gameattribute->player_2_sid = *unitpalettecolor;
			}
			if (KeyState(VK_3))
			{
				MessageBox(project->hwnd, L"已将颜色设置为玩家一", L"编辑器消息", MB_OK);
				*unitpalettecolor = gameattribute->player_1_sid;
			}
			else if (KeyState(VK_4))
			{
				MessageBox(project->hwnd, L"已将颜色设置为玩家二", L"编辑器消息", MB_OK);
				*unitpalettecolor = gameattribute->player_2_sid;
			}
		}
	}
	return 1;
}

//保存编辑器地图
extern PROJECTSETTING(SaveMap)
{
	FILE* fpconfig = NULL;
	FILE* fpscript_1 = NULL;
	FILE* fpscriptunit = NULL;
	FILE* fpscript_2 = NULL;
	FILE* fpscripttemplate = NULL;
	HashFindEntity(project, "GameTemplate", MODDEFINITION, gametemplate);
	HashFindEntity(project, "ScriptLogicMode", int, scriptlogicmode);

	if (!project->DEAD)
	{
		fpconfig = fopen("Engine_Editor_File/GAME_Configuration_File.txt", "w");
		fpscript_1 = fopen("Engine_Editor_File/Game_Script.txt", "w");
		fprintf(fpscript_1, "Script_Title: NO_Title\nYear: %d\nMonth: %d\nDay: %d\n", gameattribute->year, gameattribute->month, gameattribute->day);
		fclose(fpscript_1);
		fpscript_2 = fopen("Engine_Editor_File/Game_Script.txt", "a");
		fpscriptunit = fopen("Engine_Editor_File/Game_Unit.txt", "w");
		fpscripttemplate = fopen("Engine_Editor_File/Game_Template.txt", "w");
	}
	else
	{
		fpconfig = fopen("Engine_History_File/GAME_Configuration_File.txt", "w");
		fpscript_1 = fopen("Engine_History_File/Game_Script.txt", "w");
		{
			char title[1024] = { 0 };
			wcstombs(title, gameattribute->script_title, 1024);
			title[1023] = '\0';
			fprintf(fpscript_1, "Script_Title: %s\nYear: %d\nMonth: %d\nDay: %d\n", title, gameattribute->year, gameattribute->month, gameattribute->day);
		}
		fclose(fpscript_1);
		fpscript_2 = fopen("Engine_History_File/Game_Script.txt", "a");
		fpscriptunit = fopen("Engine_History_File/Game_Unit.txt", "w");
		fpscripttemplate = fopen("Engine_History_File/Game_Template.txt", "w");
	}
	{
		printf("GAMEMAPWIDTH=  %d\nGAMEMAPHEIGHT = %d\n", GAMEMAPWIDTH, GAMEMAPHEIGHT);
		fprintf(fpconfig, "GAMEMAPWIDTH =  %d\nGAMEMAPHEIGHT = %d\nPlayer1Sid = %d\nPlayer2Sid = %d\nEveryDay = %d\nAllRound = %d\nScriptLogic = %d\n", GAMEMAPWIDTH, GAMEMAPHEIGHT, gameattribute->player_1_sid, gameattribute->player_2_sid, gameattribute->everyday, gameattribute->allround, *scriptlogicmode);
		fclose(fpconfig);
	}
	for (int o = 0; o < GAMEMAPWIDTH; o++)
		for (int i = 0; i < GAMEMAPHEIGHT; i++)
		{
			TerrainWidthTemplate(project, &hexagon_map[o + i * GAMEMAPWIDTH]);
			fprintf(fpscript_2, "Region = [ %d , %d ]\n{\n", o, i);
			fprintf(fpscript_2, "     Name : %s\n", strlen(hexagon_map[o + i * GAMEMAPWIDTH].name) ? hexagon_map[o + i * GAMEMAPWIDTH].name : "NULL");
			fprintf(fpscript_2, "     City_Deg : %d\n", hexagon_map[o + i * GAMEMAPWIDTH].city_degree);
			fprintf(fpscript_2, "     VP : %d\n", hexagon_map[o + i * GAMEMAPWIDTH].victory_points);
			fprintf(fpscript_2, "     Traffic : %d\n", hexagon_map[o + i * GAMEMAPWIDTH].traffic);
			fprintf(fpscript_2, "     Train : %d\n", hexagon_map[o + i * GAMEMAPWIDTH].train);
			fprintf(fpscript_2, "     Width : %d\n", templatesave ? 0 : hexagon_map[o + i * GAMEMAPWIDTH].battlefield_foundation_width);
			fprintf(fpscript_2, "     Fortress : %d\n", hexagon_map[o + i * GAMEMAPWIDTH].fortress);
			fprintf(fpscript_2, "     Terrain : %d\n", hexagon_map[o + i * GAMEMAPWIDTH].terrain);
			fprintf(fpscript_2, "}\n");

			UnitInitTemplate(project, &hexagon_map[o + i * GAMEMAPWIDTH].unit);
			UNIT unit = hexagon_map[o + i * GAMEMAPWIDTH].unit;
			fprintf(fpscriptunit, "Unit = [ %d , %d ]\n{\n", o, i);
			fprintf(fpscriptunit, "   Name : %s\n", strlen(unit.name) ? hexagon_map[o + i * GAMEMAPWIDTH].name : "NULL");
			fprintf(fpscriptunit, "   Sid : %d\n", unit.sid);
			fprintf(fpscriptunit, "   Type : %d\n", unit.unit_type);
			fprintf(fpscriptunit, "   PV : %d\n", unit.planned_value);
			fprintf(fpscriptunit, "   Level : %d\n", unit.attack_level);
			fprintf(fpscriptunit, "   Armor : %d\n", templatesave ? 0 : unit.armor);
			fprintf(fpscriptunit, "   People : %d\n", templatesave ? 0 : unit.people);
			fprintf(fpscriptunit, "   Supply : %d\n", unit.supply_level);
			fprintf(fpscriptunit, "   Mobility : %d\n", templatesave ? 0 : unit.mobility);
			fprintf(fpscriptunit, "   Organization : %d\n", unit.organization);
			fprintf(fpscriptunit, "}\n");
		}

	fprintf(fpscripttemplate, "Hq        = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->hq.armor, gametemplate->hq.mobility, gametemplate->hq.people);
	fprintf(fpscripttemplate, "Inf       = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->inf.armor, gametemplate->inf.mobility, gametemplate->inf.people);
	fprintf(fpscripttemplate, "Cav       = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->cav.armor, gametemplate->cav.mobility, gametemplate->cav.people);
	fprintf(fpscripttemplate, "Sec       = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->sec.armor, gametemplate->sec.mobility, gametemplate->sec.people);
	fprintf(fpscripttemplate, "Mot       = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->mot.armor, gametemplate->mot.mobility, gametemplate->mot.people);
	fprintf(fpscripttemplate, "Tank      = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->tank.armor, gametemplate->tank.mobility, gametemplate->tank.people);
	fprintf(fpscripttemplate, "Mtnd      = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->mtnd.armor, gametemplate->mtnd.mobility, gametemplate->mtnd.people);
	fprintf(fpscripttemplate, "Mari      = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->mari.armor, gametemplate->mari.mobility, gametemplate->mari.people);
	fprintf(fpscripttemplate, "MechInf   = { Armor : %3d , Mobility : %3d , People : %3d }\n", gametemplate->mechinf.armor, gametemplate->mechinf.mobility, gametemplate->mechinf.people);
	fprintf(fpscripttemplate, "Tree      = { %3d }\n", gametemplate->tree);
	fprintf(fpscripttemplate, "Plain     = { %3d }\n", gametemplate->plain);
	fprintf(fpscripttemplate, "Hills     = { %3d }\n", gametemplate->hills);
	fprintf(fpscripttemplate, "Mount     = { %3d }\n", gametemplate->mount);
	fprintf(fpscripttemplate, "Ocean     = { %3d }\n", gametemplate->ocean);
	fprintf(fpscripttemplate, "River     = { %3d }\n", gametemplate->river);
	fprintf(fpscripttemplate, "DenseTree = { %3d }\n", gametemplate->denseTree);
	fprintf(fpscripttemplate, "Exit      = {   0 }\n");

	fprintf(fpscript_2, "Exit : [ 0 , 0 ]\n");
	fprintf(fpscriptunit, "Exit : [ 0 , 0 ]\n");
	fclose(fpscript_2);
	fclose(fpscriptunit);
	fclose(fpscripttemplate);
}