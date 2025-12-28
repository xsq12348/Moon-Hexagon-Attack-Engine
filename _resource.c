#include"MoonHexagonAttackEngine_function.h"

static int GAMEMAPWIDTH = 50;
static int GAMEMAPHEIGHT = 28;
static int hexagon_r = 15;
static int map_size = Medium_Map;
static int editor = FALSE;
static int playgamemode = NOTFOUND;
static int drawingcommand = DRAW_Redraw;	//绘制命令，防止图像撕裂
static int editormode;						//编辑模式
static int scriptlogicmode;					//运行时脚本
static int unitpalettecolor;				//调色板颜色
static IMAGE hexagonmapimage;
static IMAGE unitpalette;
static IMAGE image_unit_bar;
static IMAGE image_terrain_bar;
static IMAGE image_attribute_bar;
static POINT hexagon_mouse_coord;
static POINT hexagon_map_coord;
static POINT hexagon_select_coord;
static GAMEATTRIBUTE gameattribute;
static HEXAGONMAP hexagon_map[GAMEMAPHEIGHTMAX * GAMEMAPWIDTHMAX];

static MOONBUTTON button_start;
static MOONBUTTON button_history;
static MOONBUTTON button_editor;
static MOONBUTTON button_editor_old;
static MOONBUTTON button_exit;
static MOONBUTTON button_mod;
static MOONBUTTON button_play_editor;
static MOONBUTTON button_play_continue;
static MOONBUTTON button_play_tutorial;
static MOONBUTTON button_CMD;

static MOONBUTTON button_return_hall;
static MOONBUTTON button_editor_terrain;
static MOONBUTTON button_editor_train;
static MOONBUTTON button_editor_unit;
static MOONBUTTON button_editor_city;
static MOONBUTTON button_editor_getcolor;
static MOONBUTTON button_editor_template;

static MOONBUTTON button_game_traffic;
static MOONBUTTON button_game_weather;
static MOONBUTTON button_game_terrain;
static MOONBUTTON button_game_round;
static MOONBUTTON button_game_hall;

MODDEFINITION gametemplate =
{
	.inf =
	{
		.armor = 0,
		.mobility = 64,
		.people = 80
	},

		.cav =
	{
		.armor = 0,
		.mobility = 80,
		.people = 100
	},

		.tank =
	{
		.armor = 128,
		.mobility = 102,
		.people = 160
	},

		.mot =
	{
		.armor = 32,
		.mobility = 128,
		.people = 160
	},

		.mechinf =
	{
		.armor = 80,
		.mobility = 160,
		.people = 160
	},

		.hq =
	{
		.armor = 0,
		.mobility = 128,
		.people = 40
	},

		.sec =
	{
		.armor = 0,
		.mobility = 64,
		.people = 40
	},

		.mtnd =
	{
		.armor = 0,
		.mobility = 64,
		.people = 100
	},

		.mari =
	{
		.armor = 0,
		.mobility = 64,
		.people = 90
	},
	.tree       = 135,
	.hills      = 85,
	.mount      = 75,
	.ocean      = 30,
	.plain      = 225,
	.river      = 105,
	.denseTree  = 95,
};


extern PROJECTSETTING(ResourceLoad)
{
	CreateImage(project, &hexagonmapimage, project->window_width, project->window_height);
	CreateImage(project, &unitpalette, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT);
	CreateImage(project, &image_unit_bar, WINDOWCOMMANDHEIGHT, 2 * WINDOWCOMMANDHEIGHT);
	CreateImage(project, &image_terrain_bar, WINDOWCOMMANDHEIGHT, 2 * WINDOWCOMMANDHEIGHT);
	CreateImage(project, &image_attribute_bar, WINDOWCOMMANDHEIGHT, 1.5f * WINDOWCOMMANDHEIGHT);
	CreateEntityIndex(project, &hexagon_select_coord, "Hexagon_Select_Coord", sizeof(POINT));
	CreateEntityIndex(project, &hexagon_mouse_coord, "HexagonMouseCoord", sizeof(POINT));
	CreateEntityIndex(project, &hexagon_map_coord, "Hexagon_Map_Coord", sizeof(POINT));
	CreateEntityIndex(project, &gameattribute, "GAMEATTRIBUTE", sizeof(GAMEATTRIBUTE));
	CreateEntityIndex(project, &gametemplate, "GameTemplate", sizeof(MODDEFINITION));
	CreateEntityIndex(project, &image_attribute_bar, "Image_Attribute_Bar", sizeof(IMAGE));
	CreateEntityIndex(project, &image_terrain_bar, "Image_Terrain_Bar", sizeof(IMAGE));
	CreateEntityIndex(project, &hexagonmapimage, "hexagonmapimage", sizeof(IMAGE));
	CreateEntityIndex(project, &image_unit_bar, "Image_Unit_Bar", sizeof(IMAGE));
	CreateEntityIndex(project, &unitpalette, "UnitPalette", sizeof(IMAGE));
	CreateEntityIndex(project, hexagon_map, "Hexagon_Map", sizeof(HEXAGONMAP));
	CreateEntityIndex(project, &unitpalettecolor, "UnitPaletteColor", sizeof(int));
	CreateEntityIndex(project, &drawingcommand, "Drawing_Command", sizeof(int));
	CreateEntityIndex(project, &GAMEMAPHEIGHT, "GAMEMAPHEIGHT", sizeof(int));
	CreateEntityIndex(project, &GAMEMAPWIDTH, "GAMEMAPWIDTH", sizeof(int));
	CreateEntityIndex(project, &editormode, "EditorMode", sizeof(int));
	CreateEntityIndex(project, &hexagon_r, "Hexagon_R", sizeof(int));
	CreateEntityIndex(project, &playgamemode, "PlayGameMode", sizeof(int));
	CreateEntityIndex(project, &scriptlogicmode, "ScriptLogicMode", sizeof(int));
	CreateEntityIndex(project, &map_size, "Map_Size", sizeof(int));
	CreateEntityIndex(project, &editor, "Editor", sizeof(int));
	MOONBUTTONCREATE(project, "Button_Start",       button_start,       project->window_width / 20.f,     WINDOWHEIGHT * 1 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_History",     button_history,     project->window_width / 20.f,     WINDOWHEIGHT * 2 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor",      button_editor,      project->window_width / 20.f,     WINDOWHEIGHT * 3 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Exit",        button_exit,        project->window_width / 20.f,     WINDOWHEIGHT * 4 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Mod",         button_mod,         project->window_width * 7 / 20.f, WINDOWHEIGHT * 1 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Play_Continue",button_play_continue,project->window_width * 13 / 20.f, WINDOWHEIGHT * 1 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_Old",  button_editor_old,  project->window_width * 7 / 20.f, WINDOWHEIGHT * 3 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_CMD", button_CMD,  project->window_width * 7 / 20.f, WINDOWHEIGHT * 4 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Play_Editor", button_play_editor, project->window_width * 7 / 20.f, WINDOWHEIGHT * 2 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Play_Tutorial", button_play_tutorial, project->window_width * 13 / 20.f, WINDOWHEIGHT * 2 / 5.f, (project->window_width / 5), (project->window_height / 10), NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Return_Hall",     button_return_hall,     WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 0, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_Terrain",  button_editor_terrain,  WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 1, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_Train",    button_editor_train,    WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 2, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_Unit",     button_editor_unit,     WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 3, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_City",     button_editor_city,     WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 4, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_GetColor", button_editor_getcolor, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 5, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Editor_Template", button_editor_template, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 6, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT / 2, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Game_Round",   button_game_round,   WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 5 + WINDOWCOMMANDHEIGHT / 2, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT / 2, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Game_Traffic", button_game_traffic, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 6,                           WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT / 2, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Game_Weather", button_game_weather, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 6 + WINDOWCOMMANDHEIGHT / 2, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT / 2, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Game_Terrain", button_game_terrain, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 7,                           WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT / 2, NULL, NULL, NULL);
	MOONBUTTONCREATE(project, "Button_Game_Hall",    button_game_hall,    WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 7 + WINDOWCOMMANDHEIGHT / 2, WINDOWCOMMANDHEIGHT, WINDOWCOMMANDHEIGHT / 2 + 5, NULL, NULL, NULL);
	ProjectFindEntityAllNumber(project);	//临时
};