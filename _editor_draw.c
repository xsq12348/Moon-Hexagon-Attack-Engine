#include"MoonHexagonAttackEngine_function.h"

static IMAGE* engineback;
static IMAGE* hexagonmapimage;
static IMAGE* unitpalette;
static POINT* hexagon_map_coord;
static int* hexagon_r;
static int* map_size;
static int* drawingcommand;
static int* editormode;
static int GAMEMAPWIDTH;
static int GAMEMAPHEIGHT;
static int* unitpalettecolor;
static HEXAGONMAP* hexagon_map;
static MOONBUTTON* button_return_hall;
static MOONBUTTON* button_editor_terrain;
static MOONBUTTON* button_editor_train;
static MOONBUTTON* button_editor_unit;
static MOONBUTTON* button_editor_city;
static MOONBUTTON* button_editor_getcolor;
static MOONBUTTON* button_editor_template;

extern PROJECTMODULE(EditorDrawingSetting)
{
	HashFindEntity(project, "Button_Editor_GetColor", MOONBUTTON, button_editor_getcolor_2);
	HashFindEntity(project, "Button_Editor_Template", MOONBUTTON, button_editor_template_2);
	HashFindEntity(project, "Button_Editor_Terrain", MOONBUTTON, button_editor_terrain_2);
	HashFindEntity(project, "Button_Editor_Train", MOONBUTTON, button_editor_train_2);
	HashFindEntity(project, "Button_Editor_Unit", MOONBUTTON, button_editor_unit_2);
	HashFindEntity(project, "Button_Return_Hall", MOONBUTTON, button_return_hall_2);
	HashFindEntity(project, "Button_Editor_City", MOONBUTTON, button_editor_city_2);
	HashFindEntity(project, "Hexagon_Map_Coord", POINT, hexagon_map_coord_2);
	HashFindEntity(project, "GAMEATTRIBUTE", GAMEATTRIBUTE, gameattribute);
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map_2);
	HashFindEntity(project, "hexagonmapimage", IMAGE, hexagonmapimage_2);
	HashFindEntity(project, "ProjectBitmap", IMAGE, engineback_2);
	HashFindEntity(project, "UnitPalette", IMAGE, unitpalette_2);
	HashFindEntity(project, "Drawing_Command", int, drawingcommand_2);
	HashFindEntity(project, "UnitPaletteColor", int, unitpalettecolor_2);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT_2);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH_2);
	HashFindEntity(project, "EditorMode", int, editormode_2);
	HashFindEntity(project, "Hexagon_R", int, hexagon_r_2);
	HashFindEntity(project, "Map_Size", int, map_size_2);
	engineback = engineback_2;
	hexagonmapimage = hexagonmapimage_2;
	unitpalette = unitpalette_2;
	hexagon_map_coord = hexagon_map_coord_2;
	hexagon_r = hexagon_r_2;
	editormode = editormode_2;
	map_size = map_size_2;
	drawingcommand = drawingcommand_2;
	hexagon_map = hexagon_map_2;
	GAMEMAPWIDTH = *GAMEMAPWIDTH_2;
	GAMEMAPHEIGHT = *GAMEMAPHEIGHT_2;
	unitpalettecolor = unitpalettecolor_2;
	button_return_hall = button_return_hall_2;
	button_editor_terrain = button_editor_terrain_2;
	button_editor_train = button_editor_train_2;
	button_editor_unit = button_editor_unit_2;
	button_editor_city = button_editor_city_2;
	button_editor_getcolor = button_editor_getcolor_2;
	button_editor_template = button_editor_template_2;
	*map_size = Medium_Map;
	*drawingcommand = DRAW_Redraw;
	HexagonMapDrawing(project);
	FUNCTIONSWITCH(EditorDrawing);
	return 1;
}

extern PROJECTMODULE(EditorDrawing)
{
	MapCommandDraw(project);
	DrawButton(button_return_hall, L"返回主菜单", *editormode == 1);
	DrawButton(button_editor_terrain, L"设置地形", *editormode == 2);
	DrawButton(button_editor_train, L"设置铁路", *editormode == 3);
	DrawButton(button_editor_unit, L"设置单位", *editormode == 4);
	DrawButton(button_editor_city, L"设置城市", *editormode == 5);
	DrawButton(button_editor_getcolor, L"获取颜色", *editormode == 6);
	{
		RECT rect = { button_editor_template->x,button_editor_template->y,button_editor_template->x + button_editor_template->width,button_editor_template->y + button_editor_template->height };
		switch (button_editor_template->mode)
		{
		case MOON_BUTTONPRESS:
		case MOON_BUTTONRHOVER:
			BoxFull(engineback, rect.left, rect.top, rect.right, rect.bottom, 0xeeeeee);
			Box(engineback, rect.left + 10, rect.top + 10, rect.right - 9, rect.bottom - 9, 2, 0);
			TextFont(engineback, rect.left+12, rect.top+16, L"模板格式", RGB(150, 150, 150), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
			TextFont(engineback, rect.left + 11, rect.top + 15, L"模板格式", RGB(255, 0, 0), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
			break;
		case 0:
			BoxFull(engineback, rect.left, rect.top, rect.right, rect.bottom, 0xffffff);
			if(!button_editor_template->ButtonModePress(project, button_editor_template))
				TextFont(engineback, rect.left + 12, rect.top + 16, L"模板格式", RGB(255, 0, 0), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
			else
			{
				TextFont(engineback, rect.left + 12, rect.top + 16, L"模板格式", RGB(150, 150, 150), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
				TextFont(engineback, rect.left + 11, rect.top + 15, L"模板格式", 0xff9b63, FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
			}
			break;
		}	
		Box(engineback, rect.left + 3, rect.top + 3, rect.right - 3, rect.bottom - 3, 2, 0);
	}
	{
		DrawingArea(engineback, unitpalette, WINDOWWIDTH, WINDOWHEIGHT - WINDOWCOMMANDHEIGHT, unitpalette->lengths.x, unitpalette->lengths.y);
		BoxFull(engineback, WINDOWWIDTH, WINDOWHEIGHT - WINDOWCOMMANDHEIGHT - WINDOWCOMMANDHEIGHT / 2 - 5, WINDOWWIDTH + WINDOWCOMMANDHEIGHT, WINDOWHEIGHT - WINDOWCOMMANDHEIGHT, *unitpalettecolor);
		Box(engineback, WINDOWWIDTH, WINDOWHEIGHT - WINDOWCOMMANDHEIGHT - WINDOWCOMMANDHEIGHT / 2 - 5, WINDOWWIDTH + WINDOWCOMMANDHEIGHT, WINDOWHEIGHT - WINDOWCOMMANDHEIGHT, 3, 0xffffff);
	}
	return 1;
}