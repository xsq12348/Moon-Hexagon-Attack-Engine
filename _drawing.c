#include"MoonHexagonAttackEngine_function.h"

static IMAGE* engineback;
static IMAGE* hexagonmapimage;
static IMAGE* image_unit_bar;
static IMAGE* image_terrain_bar;
static IMAGE* image_attribute_bar;
static POINT* hexagon_map_coord;
static POINT* hexagon_select_coord;
static int* hexagon_r;
static int* map_size;
static int* drawingcommand;
static int GAMEMAPWIDTH;
static int GAMEMAPHEIGHT;
static HEXAGONMAP* hexagon_map;
static MOONBUTTON* button_game_hall;
static MOONBUTTON* button_game_terrain;
static MOONBUTTON* button_game_weather;
static MOONBUTTON* button_game_traffic;
static MOONBUTTON* button_game_round;
GAMEATTRIBUTE* gameattribute;

extern PROJECTMODULE(MapCommandDraw);
static UNIT HexagonSelectLight();
static void UnitCommandAttributeBar(UNIT unit);
static void TerrainCommandAttributeBar(HEXAGONMAP hexagon);
static void AttributeCommandBarDraw(int player);
static void HexagonMapDrawingTemp(IMAGE* image,int x, int y);

extern PROJECTMODULE(GameDrawingSetting)
{
	HashFindEntity(project, "Button_Game_Terrain",  MOONBUTTON, button_game_terrain_2);
	HashFindEntity(project, "Button_Game_Weather",  MOONBUTTON, button_game_weather_2);
	HashFindEntity(project, "Button_Game_Traffic",  MOONBUTTON, button_game_traffic_2);
	HashFindEntity(project, "Button_Game_Round",  MOONBUTTON, button_game_round_2);
	HashFindEntity(project, "Button_Game_Hall",     MOONBUTTON, button_game_hall_2);
	HashFindEntity(project, "Hexagon_Select_Coord", POINT, hexagon_select_coord_2);
	HashFindEntity(project, "Hexagon_Map_Coord", POINT, hexagon_map_coord_2);
	HashFindEntity(project, "GAMEATTRIBUTE", GAMEATTRIBUTE, gameattribute_2);
	HashFindEntity(project, "Image_Attribute_Bar", IMAGE, image_attribute_bar_2);
	HashFindEntity(project, "Image_Terrain_Bar", IMAGE, image_terrain_bar_2);
	HashFindEntity(project, "hexagonmapimage", IMAGE, hexagonmapimage_2);
	HashFindEntity(project, "Image_Unit_Bar", IMAGE, image_unit_bar_2);
	HashFindEntity(project, "ProjectBitmap", IMAGE, engineback_2);
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map_2);
	HashFindEntity(project, "Drawing_Command", int, drawingcommand_2);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT_2);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH_2);
	HashFindEntity(project, "Hexagon_R", int, hexagon_r_2);
	HashFindEntity(project, "Map_Size", int, map_size_2);
	HashFindEntity(project, "Editor", int, editor);
	hexagon_select_coord = hexagon_select_coord_2;
	hexagon_map_coord = hexagon_map_coord_2;
	button_game_traffic = button_game_traffic_2;
	image_attribute_bar = image_attribute_bar_2;
	image_terrain_bar = image_terrain_bar_2;
	image_unit_bar = image_unit_bar_2;
	hexagonmapimage = hexagonmapimage_2;
	drawingcommand = drawingcommand_2;
	GAMEMAPHEIGHT = *GAMEMAPHEIGHT_2;
	GAMEMAPWIDTH = *GAMEMAPWIDTH_2;
	hexagon_map = hexagon_map_2;
	engineback = engineback_2;
	hexagon_r = hexagon_r_2;
	map_size = map_size_2;
	button_game_hall = button_game_hall_2;
	button_game_terrain = button_game_terrain_2;
	button_game_weather = button_game_weather_2;
	button_game_round = button_game_round_2;
	gameattribute = gameattribute_2;
	*map_size = Full_Map;
	*drawingcommand = DRAW_Redraw;
	if (!*editor) MessageBox(project->hwnd, gameattribute->script_title, L"当前剧本", MB_OK | MB_ICONINFORMATION);
	HexagonMapDrawing(project);
	if (*editor) FUNCTIONSWITCH(EditorDrawingSetting)
	else FUNCTIONSWITCH(GameDrawing);
	return 1;
}

extern PROJECTMODULE(GameDrawing)
{
	MapCommandDraw(project);
	UNIT unit = HexagonSelectLight();
	UnitCommandAttributeBar(unit);
	TerrainCommandAttributeBar(hexagon_map[hexagon_select_coord->x + hexagon_select_coord->y * GAMEMAPWIDTH]);
	AttributeCommandBarDraw(1);
	DrawButton(button_game_hall, L"返回主菜单", 0);
	DrawButton(button_game_round, L"下一回合", 0);
	DrawButton(button_game_terrain, L"显示地形", 0);
	DrawButton(button_game_weather, L"显示天气", 0);
	DrawButton(button_game_traffic, L"显示交通", 0);
	return 1;
}

extern void HexagonFull(IMAGE* image, int R, int r, int X, int Y, COLORREF color, int linewidth, int linecolor , int width ,int height)
{
	int x, y, x2 = R * 1.732f / 2;
	x = X * 1.5 * R + R;
	y = Y * 1.732f * R + (X % 2 ? x2 : 0) + x2;
	if (x < 0 || y < 0 || x + R > width || y + 1.732f * R > height)return;
	POINT hexagon[6] = { 0 };
	hexagon[0].x = x + r;
	hexagon[0].y = y;
	hexagon[1].x = x + r / 2;
	hexagon[1].y = y + 1.732f * r / 2 + 1;
	hexagon[2].x = x - r / 2;
	hexagon[2].y = y + 1.732f * r / 2 + 1;
	hexagon[3].x = x - r;
	hexagon[3].y = y;
	hexagon[4].x = x - r / 2;
	hexagon[4].y = y - 1.732f * r / 2;
	hexagon[5].x = x + r / 2;
	hexagon[5].y = y - 1.732f * r / 2;
	HBRUSH hbrush = CreateSolidBrush(color);
	HPEN hnPen = CreatePen(PS_SOLID, linewidth, linecolor);
	HPEN hOldPen = (HPEN)SelectObject(image->image.hdc, hnPen);
	SelectObject(image->image.hdc, hbrush);
	Polygon(image->image.hdc, hexagon, 6);
	DeleteObject(hbrush);
	DeleteObject(hnPen);
}

extern void Hexagon(IMAGE* image, int R, int X, int Y, int width, COLORREF color)
{
	int x, y, x2 = R * 1.732f / 2;
	x = X * 1.5 * R + R;
	y = Y * 1.732f * R + (X % 2 ? x2 : 0) + x2;
	Line(image, x + R / 2, y - 1.73 * R / 2, x + R, y, width, color);
	Line(image, x - R / 2, y - 1.73 * R / 2, x - R, y, width, color);
	Line(image, x + R / 2, y + 1.73 * R / 2, x + R, y, width, color);
	Line(image, x - R / 2, y + 1.73 * R / 2, x - R, y, width, color);
	Line(image, x - R / 2, y - 1.73 * R / 2, x + R / 2, y - 1.73 * R / 2, width, color);
	Line(image, x - R / 2, y + 1.73 * R / 2, x + R / 2, y + 1.73 * R / 2, width, color);
}

static void HexagonMapDrawingTemp(IMAGE* image, int x, int y)
{
	HEXAGONMAP hexagonmap = hexagon_map[(x + hexagon_map_coord->x * 2) + (y + hexagon_map_coord->y) * GAMEMAPWIDTH];
	switch (hexagonmap.terrain % (Terrain_No_Entry + 1))
	{
	case Terrain_Plain:			HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);		break;
	case Terrain_Ocean:			HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPOCEANCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);		break;
	case Terrain_DenseTree:
		HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);
		HexagonFull(image, *hexagon_r, (int)(*hexagon_r * 2 / 3), x, y, MAPDENSETREECOLOR, MAPLINEWIDTH, MAPDENSETREECOLOR, image->lengths.x, image->lengths.y);
		break;
	case Terrain_Hills:
		HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);
		HexagonFull(image, *hexagon_r, (int)(*hexagon_r * 2 / 3), x, y, MAPHILLSCOLOR, MAPLINEWIDTH, MAPHILLSCOLOR, image->lengths.x, image->lengths.y);
		break;
	case Terrain_Mount:
		HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);
		HexagonFull(image, *hexagon_r, (int)(*hexagon_r * 2 / 3), x, y, MAPMOUNTCOLOR, MAPLINEWIDTH, MAPMOUNTCOLOR, image->lengths.x, image->lengths.y);
		break;
	case Terrain_River:
		HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);
		{
			POINT rivercoord = HexagonTurningPoint(x, y, *hexagon_r);
			Line(image, rivercoord.x, rivercoord.y, rivercoord.x, rivercoord.y, (int)(*hexagon_r * 2 / 3), MAPRIVERCOLOR);
			for (int k = 1; k < 7; k++)
				if (hexagonmap.hexagon[k]->terrain == hexagonmap.terrain)
				{
					POINT hexagoncoord_1 = HexagonTurningPoint(x, y, *hexagon_r);
					POINT hexagoncoord_2 = HexagonTurningPoint(hexagonmap.hexagon[k]->coordx - hexagon_map_coord->x * 2, hexagonmap.hexagon[k]->coordy - hexagon_map_coord->y, *hexagon_r);
					Line(image, hexagoncoord_1.x, hexagoncoord_1.y, hexagoncoord_2.x, hexagoncoord_2.y, (int)(*hexagon_r * 2 / 3), MAPRIVERCOLOR);
				}
		}
		break;
	case Terrain_Tree:
		HexagonFull(image, *hexagon_r, *hexagon_r, x, y, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);
		HexagonFull(image, *hexagon_r, (int)(*hexagon_r * 2 / 3), x, y, MAPTREECOLOR, MAPLINEWIDTH, MAPTREECOLOR, image->lengths.x, image->lengths.y);
		break;
	case Terrain_No_Entry: HexagonFull(image, *hexagon_r, *hexagon_r, x, y, 0x5e5eff, MAPLINEWIDTH, MAPLINECOLOR, image->lengths.x, image->lengths.y);		break;
	}
	if (hexagonmap.train)
	{
		POINT traincoord = HexagonTurningPoint(x, y, *hexagon_r);
		Line(hexagonmapimage, traincoord.x, traincoord.y, traincoord.x, traincoord.y, (int)(*hexagon_r * 1 / 3), 0);
		for (int k = 1; k < 7; k++)
			if (hexagonmap.train && hexagonmap.hexagon[k]->train == hexagonmap.train)
			{
				POINT hexagoncoord_1 = HexagonTurningPoint(x, y, *hexagon_r);
				POINT hexagoncoord_2 = HexagonTurningPoint(hexagonmap.hexagon[k]->coordx - hexagon_map_coord->x * 2, hexagonmap.hexagon[k]->coordy - hexagon_map_coord->y, *hexagon_r);
				Line(hexagonmapimage, hexagoncoord_1.x, hexagoncoord_1.y, hexagoncoord_2.x, hexagoncoord_2.y, (int)(*hexagon_r * 1 / 3), 0);
			}
	}
	if (hexagonmap.city_degree)
	{
		int R = *hexagon_r;
		wchar_t cityname[256];
		CharToWchar(cityname, hexagonmap.name, 256);
		POINT traincoord = HexagonTurningPoint(x, y, *hexagon_r);
		Line(hexagonmapimage, traincoord.x, traincoord.y, traincoord.x, traincoord.y, (int)Lerp(0, sqrt(R * R - (R / 2) * (R / 2)), hexagonmap.city_degree / 255.f) * 2, 0);
		TextFont(hexagonmapimage, traincoord.x - R * .66f, traincoord.y, cityname, RGB(255, 215, 0), FALSE, L"Arial", 0, 14, 0, 0, FW_NORMAL, FALSE, FALSE, DEFAULT_CHARSET);
	}
}

extern PROJECTMODULE(HexagonMapDrawing)
{
	*map_size == Full_Map && (*hexagon_r = 15);
	*map_size == Medium_Map && (*hexagon_r = 20);
	*map_size == Mini_Map && (*hexagon_r = 30);
	BoxFull(hexagonmapimage, 0, 0, WINDOWWIDTH, WINDOWHEIGHT, RGB(150, 150, 150));
	Line(hexagonmapimage, WINDOWWIDTH, 0, WINDOWWIDTH, WINDOWHEIGHT, 5, 0xffffff);
	unsigned char drawingmax = min(RANGE(WINDOWWIDTH / (*hexagon_r * 1.5), 0, GAMEMAPWIDTH), GAMEMAPWIDTH - hexagon_map_coord->x * 2);
	for (int o = 0; o < drawingmax; o++)
		for (int i = 0; i < GAMEMAPHEIGHT - hexagon_map_coord->y; i++)
			switch (*drawingcommand)
			{
			case DRAW_Redraw: HexagonMapDrawingTemp(hexagonmapimage, o, i); break;
			case Draw_Traffic:
			{
				HEXAGONMAP hexagon = hexagon_map[(o + hexagon_map_coord->x * 2) + (i + hexagon_map_coord->y) * GAMEMAPWIDTH];
				HexagonFull(hexagonmapimage, *hexagon_r, *hexagon_r, o, i,
					RGB(
						RANGE(hexagon.traffic + hexagon.weather * 50, 0, 255),
						RANGE(255 - (hexagon.traffic + hexagon.weather * 50), 0, 255),
						0),
					MAPLINEWIDTH, MAPLINECOLOR, hexagonmapimage->lengths.x, hexagonmapimage->lengths.y);
			}
			break;
			case Draw_Weather:
			{
				int R = *hexagon_r, x, y, x2 = R * 1.732f / 2, weather = hexagon_map[(o + hexagon_map_coord->x * 2) + (i + hexagon_map_coord->y) * GAMEMAPWIDTH].weather;
				x = o * 1.5 * R + R;
				y = i * 1.732f * R + (o % 2 ? x2 : 0) + x2;
				HexagonFull(hexagonmapimage, *hexagon_r, *hexagon_r, o, i, MAPCOLOR, MAPLINEWIDTH, MAPLINECOLOR, hexagonmapimage->lengths.x, hexagonmapimage->lengths.y);
				switch (weather)
				{
				case 0: TextFont(hexagonmapimage, x - 10, y - 10, L"☀", 0x0067ff, FALSE, L"", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET); break;
				case 1: TextFont(hexagonmapimage, x - 10, y - 10, L"☁", 0xff9b63, FALSE, L"", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET); break;
				case 2: TextFont(hexagonmapimage, x - 10, y - 10, L"⛈︎", 0xff2400, FALSE, L"", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET); break;
				}
			}
			break;
			}
	for (int o = 0; o < drawingmax; o++)
		for (int i = 0; i < GAMEMAPHEIGHT - hexagon_map_coord->y; i++)
		{
			HEXAGONMAP hexagonmap = hexagon_map[(o + hexagon_map_coord->x * 2) + (i + hexagon_map_coord->y) * GAMEMAPWIDTH];
			DrawUnit(hexagonmapimage, hexagonmap, hexagonmap.unit.unit_type, o, i, *hexagon_r, hexagonmap.unit.sid, FALSE);
		}
	*drawingcommand = DRAW_Continue_Drawing;
	return 1;
}

static void UnitIconBack(IMAGE* image, int left, int top, int right, int bottom)
{
	BoxFull(image, left, top, right, bottom, 0xffffff);
	Box(image, left, top, right, bottom, 2, 0);
}

static UNITICON(UnitIconINF)
{
	UnitIconBack(image, left, top, right, bottom);
	Line(image, left, top, right, bottom, 2, 0);
	Line(image, left, bottom, right, top, 2, 0);
}

static UNITICON(UnitIconTANK)
{
	UnitIconBack(image, left, top, right, bottom);
	int alpha = (right + left) / 2, beta = (top + bottom) / 2, gamma = (right - left) - 2;
	Line(image, alpha, beta, alpha, beta, gamma - 2, 0);
	Line(image, alpha, beta, alpha, beta, gamma - 4, 0xffffff);
}

static UNITICON(UnitIconMECHINF)
{
	UnitIconBack(image, left, top, right, bottom);
	int alpha = (right + left) / 2, beta = (top + bottom) / 2, gamma = (right - left) - 2;
	Line(image, alpha, beta, alpha, beta, gamma - 2, 0);
	Line(image, alpha, beta, alpha, beta, gamma - 4, 0xffffff);
	Line(image, left, top, right, bottom, 2, 0);
	Line(image, left, bottom, right, top, 2, 0);
}

static UNITICON(UnitIconMOT)
{
	UnitIconBack(image, left, top, right, bottom);
	Line(image, left, top, right, bottom, 2, 0);
	Line(image, left, bottom, right, top, 2, 0);
	Line(image, left, bottom, left, bottom - 1, 5, 0);
	Line(image, right, bottom, right, bottom - 1, 5, 0);
}

static UNITICON(UnitIconHQ)
{
	UnitIconBack(image, left, top, right, bottom);
	int alpha = (right + left) / 2, beta = (top + bottom) / 2;
	BoxFull(image, left, top, alpha, beta, 0);
	BoxFull(image, right, bottom, alpha, beta, 0);
}

static UNITICON(UnitIconSEC)
{
	UnitIconBack(image, left, top, right, bottom);
	int alpha = (right + left) / 2, beta = (top + bottom) / 2;
	Line(image, right, beta, alpha, bottom, 2, 0);
	Line(image, alpha, bottom, left, beta, 2, 0);
	Line(image, alpha, top, right, beta, 2, 0);
	Line(image, left, beta, alpha, top, 2, 0);
}

static UNITICON(UnitIconCAV)
{
	UnitIconBack(image, left, top, right, bottom);
	Line(image, left, top, right, bottom, 2, 0);
}

static UNITICON(UnitIconMTND)
{
	UnitIconBack(image, left, top, right, bottom);
	int alpha = (right + left) / 2;
	Line(image, left, top, right, bottom, 2, 0);
	Line(image, left, bottom, right, top, 2, 0);
	Line(image, alpha, bottom - 1, alpha, bottom - 1, 8, 0);
}

static UNITICON(UnitIconMARI)
{
	UnitIconBack(image, left, top, right, bottom);
	int alpha = (right + left) / 2, beta = (top + bottom) / 2, gamma = (right - left) - 2;
	Line(image, alpha, beta, alpha, beta, gamma - 2, 0);
	Line(image, alpha, beta, alpha, beta, gamma - 4, 0xffffff);
	BoxFull(image, left + 2, top + 1, right - 2, beta, 0xffffff);
	Line(image, alpha, top + 2, alpha, bottom - 2, 2, 0);
	Line(image, left + 5, beta, right - 5, beta, 2, 0);
}

static void DrawUnitTemp(IMAGE* image, int left, int top, int right, int bottom,int unittype)
{
	switch (unittype)
	{
	case UNIT_TYPE_INF:     UnitIconINF(image, left, top, right, bottom);     break;
	case UNIT_TYPE_TANK:    UnitIconTANK(image, left, top, right, bottom);    break;
	case UNIT_TYPE_MECHINF: UnitIconMECHINF(image, left, top, right, bottom); break;
	case UNIT_TYPE_MOT:     UnitIconMOT(image, left, top, right, bottom);     break;
	case UNIT_TYPE_HQ:      UnitIconHQ(image, left, top, right, bottom);      break;
	case UNIT_TYPE_SEC:     UnitIconSEC(image, left, top, right, bottom);     break;
	case UNIT_TYPE_CAV:     UnitIconCAV(image, left, top, right, bottom);     break;
	case UNIT_TYPE_MTND:    UnitIconMTND(image, left, top, right, bottom);    break;
	case UNIT_TYPE_MARI:    UnitIconMARI(image, left, top, right, bottom);    break;
	}
}

extern void DrawUnit(IMAGE* image, HEXAGONMAP hexagonmap, int unittype, int hexX, int hexY, int R, int color,int mode)
{
	if (unittype <= 0)return;
	POINT hexcoord = HexagonTurningPoint(hexX, hexY, R);
	RECT unitcoord = { (int)(hexcoord.x - R / 2.f - 2),(int)(hexcoord.y - 1.732f * R / 4.f - 2), (int)(hexcoord.x - R / 2.f - 2 + R), (int)(hexcoord.y - 1.732f * R / 4.f - 2 + R) };
	RECT iconrect_1 = { unitcoord.left + 2,unitcoord.top + 2,unitcoord.left + R - 2,unitcoord.top + R - 2 }, iconrect_2 = { iconrect_1.left,iconrect_1.top,iconrect_1.left + R * 2 / 3, iconrect_1.top + R * 1 / 2 };
	if(mode == FALSE)
	{
		BoxFull(image, unitcoord.left + 2, unitcoord.top + 2, unitcoord.left + R + 2, unitcoord.top + 2 + R, 0x777777);
		Box(image, unitcoord.left + 2, unitcoord.top + 2, unitcoord.left + R + 2, unitcoord.top + 2 + R, 5, 0x777777);
		BoxFull(image, unitcoord.left, unitcoord.top, unitcoord.left + R, unitcoord.top + R, color);
		Box(image, unitcoord.left, unitcoord.top, unitcoord.left + R, unitcoord.top + R, 5, color);
	}
	else
	{
		BoxFull(image, unitcoord.left + 2, unitcoord.top + 2, unitcoord.left + R + 2, unitcoord.top + 2 + R, 0xaaaaaa);
		Box(image, unitcoord.left + 2, unitcoord.top + 2, unitcoord.left + R + 2, unitcoord.top + 2 + R, 5, 0xaaaaaa);
		unitcoord.left -= 2;
		unitcoord.top -= 2;
		unitcoord.right -= 2;
		unitcoord.bottom -= 2;
		iconrect_2.left -= 2;
		iconrect_2.top -= 2;
		iconrect_2.right -= 2;
		iconrect_2.bottom -= 2;
		iconrect_1.left -= 2;
		iconrect_1.top -= 2;
		iconrect_1.right -= 2;
		iconrect_1.bottom -= 2;
		BoxFull(image, unitcoord.left, unitcoord.top, unitcoord.left + R, unitcoord.top + R, color);
		Box(image, unitcoord.left, unitcoord.top, unitcoord.left + R, unitcoord.top + R, 5, color);
	}
	switch (*map_size)
	{
	case Full_Map:break;
	case Medium_Map: DrawUnitTemp(image, iconrect_1.left, iconrect_1.top, iconrect_1.right, iconrect_1.bottom, unittype); break;
	case Mini_Map:
		DrawUnitTemp(image, iconrect_2.left, iconrect_2.top, iconrect_2.right, iconrect_2.bottom, unittype);
		{
			wchar_t number[5];
			int numbertop = unitcoord.top + R / 2 + 1;
			swprintf(number, 5, L"%d", (int)Lerp(0, 9, hexagonmap.unit.people / 255.f));
			TextFont(image, unitcoord.left/* + (15 + 1) / 2 * 0*/, numbertop, number, 0x36f2fb, FALSE, L"Arial", 0, 15, 0, 0, FW_DONTCARE, FALSE, FALSE, DEFAULT_CHARSET);
			swprintf(number, 5, L"%d", (int)Lerp(0, 9, hexagonmap.unit.organization / 255.f));
			TextFont(image, unitcoord.left + 8/*(15 + 1) / 2 * 1*/, numbertop, number, 0x50e599, FALSE, L"Arial", 0, 15, 0, 0, FW_DONTCARE, FALSE, FALSE, DEFAULT_CHARSET);
			swprintf(number, 5, L"%d", (int)Lerp(0, 9, hexagonmap.unit.mobility / 255.f));
			TextFont(image, unitcoord.left + 16/*(15 + 1) / 2 * 2 */, numbertop, number, 0xe4cd5f, FALSE, L"Arial", 0, 15, 0, 0, FW_DONTCARE, FALSE, FALSE, DEFAULT_CHARSET);
			BoxFull(image, unitcoord.left + R - 1, unitcoord.bottom + 2, unitcoord.left + R * 4 / 5, unitcoord.bottom + 3 - R, 0x68e7f2);
		 	BoxFull(image, unitcoord.left + R - 1, unitcoord.bottom + 2, unitcoord.left + R * 4 / 5, unitcoord.bottom + 3 - (int)Lerp(0, R, hexagonmap.unit.supply_level / 255.f), 0x2671df);				//绘制补给剩余
		}
		break;
	}
}

extern PROJECTMODULE(MapCommandDraw)
{
	switch (*drawingcommand)
	{
	case DRAW_Pause_Draw:		break;
	case DRAW_Continue_Drawing:	DrawingArea(engineback, hexagonmapimage, 0, 0, hexagonmapimage->image.width, hexagonmapimage->image.height);	break;
	case Draw_Weather:
	case Draw_Traffic:
	case DRAW_Redraw:			HexagonMapDrawing(project); break;
	}
	return 1;
}

static UNIT HexagonSelectLight()
{
	HEXAGONMAP hexagon = hexagon_map[hexagon_select_coord->x + hexagon_select_coord->y * GAMEMAPWIDTH];
	int alphax = hexagon_select_coord->x - hexagon_map_coord->x * 2, alphay = hexagon_select_coord->y - hexagon_map_coord->y;
	if (hexagon.unit.unit_type != NULL)
	{
		HexagonMapDrawingTemp(engineback, alphax, alphay);
		Hexagon(engineback, *hexagon_r, alphax, alphay, 5, 0x006400);
		DrawUnit(engineback, hexagon, hexagon.unit.unit_type, alphax, alphay, *hexagon_r, hexagon.unit.sid, TRUE);
	}
	else Hexagon(engineback, *hexagon_r, alphax, alphay, 5, 0x006400);
	return hexagon.unit;
}

static void CommandBarDrawTemp(IMAGE* image, LPCWSTR text, int x, int y, int number)
{
	wchar_t wnumber[255] = { 0 };
	swprintf(wnumber, 255, L"%d", number);
	BoxFull(image, x, y, x + 48, y + 23, 0xffffff);
	Box(image, x, y, x + 48, y + 23, 2, 0x3232ac);
	TextFont(image, 0, y, text, 0xe16e5b, FALSE, L"SimHei", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET);
	TextFont(image, x + 2, y, wnumber, 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
}

static void AttributeCommandBarDraw(int player)
{
	static round;
	if (round == gameattribute->allround)
	{
		DrawingArea(engineback, image_attribute_bar, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 4, image_attribute_bar->lengths.x, image_attribute_bar->lengths.y);
		return;
	}
	round = gameattribute->allround;
	CommandBarDrawTemp(image_attribute_bar, L"年", 48, 26, gameattribute->year);
	CommandBarDrawTemp(image_attribute_bar, L"月", 48, 49, gameattribute->month);
	CommandBarDrawTemp(image_attribute_bar, L"日", 48, 73, gameattribute->day);
	CommandBarDrawTemp(image_attribute_bar, L"回合", 48, 96, gameattribute->allround);
	CommandBarDrawTemp(image_attribute_bar, L"玩家", 48, 119, 0);
	if (player)BoxFull(image_attribute_bar, 48, 119, 96, 141, gameattribute->player_1_sid);
	else BoxFull(image_attribute_bar, 48, 119, 96, 141, gameattribute->player_2_sid);
	Box(image_attribute_bar, 48, 119, 96, 141, 2, 0x3232ac);
	DrawingArea(engineback, image_attribute_bar, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 4, image_attribute_bar->lengths.x, image_attribute_bar->lengths.y);
}

static void TerrainCommandAttributeBar(HEXAGONMAP hexagon)
{
	static int terraintype = 0;
	static POINT coord;
	if (hexagon.terrain == terraintype && coord.x == hexagon_select_coord->x && coord.y == hexagon_select_coord->y)
	{
		DrawingArea(engineback, image_terrain_bar, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 2, image_terrain_bar->lengths.x, image_terrain_bar->lengths.y);
		return;
	}
	coord.x = hexagon_select_coord->x, coord.y = hexagon_select_coord->y;
	terraintype = hexagon.terrain;
	BoxFull(image_terrain_bar, 0, 0, image_terrain_bar->lengths.x, image_terrain_bar->lengths.y, MAPCOLOR);
	Box(image_terrain_bar, 0, 0, image_terrain_bar->lengths.x - 1, image_terrain_bar->lengths.y - 1, 4, 0x3232ac);
	BoxFull(image_terrain_bar, 48, 3, 96, 26, 0xffffff);
	Box(    image_terrain_bar, 48, 3, 96, 26, 2, 0x3232ac);
	TextFont(image_terrain_bar, 0, 3, L"地形", 0xe16e5b, FALSE, L"SimHei", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET);
	switch (hexagon.terrain)
	{
	case Terrain_Plain:TextFont(image_terrain_bar, 50, 3, L"平原", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_Ocean:TextFont(image_terrain_bar, 50, 3, L"海洋", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_DenseTree:TextFont(image_terrain_bar, 50, 3, L"密林", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_Hills:TextFont(image_terrain_bar, 50, 3, L"丘陵", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_Mount:TextFont(image_terrain_bar, 50, 3, L"山地", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_River:TextFont(image_terrain_bar, 50, 3, L"河域", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_Tree:TextFont(image_terrain_bar, 50, 3, L"树林", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case Terrain_No_Entry:TextFont(image_terrain_bar, 50, 3, L"禁区", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	}
	BoxFull(image_terrain_bar, 48, 119, 96, 141, 0xffffff);
	Box(    image_terrain_bar, 48, 119, 96, 141, 2, 0x3232ac);
	TextFont(image_terrain_bar, 0, 119, L"天气", 0xe16e5b, FALSE, L"SimHei", 0, 20, 0, 0, FW_THIN, FALSE, FALSE, DEFAULT_CHARSET);
	switch (hexagon.weather)
	{
	case 0: TextFont(image_terrain_bar, 50, 119, L"晴天", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 1: TextFont(image_terrain_bar, 50, 119, L"阴天", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 2: TextFont(image_terrain_bar, 50, 119, L"雨天", 0x6e9437, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	}
	CommandBarDrawTemp(image_terrain_bar, L"宽度", 48, 26, hexagon.battlefield_foundation_width);
	CommandBarDrawTemp(image_terrain_bar, L"交通", 48, 49, hexagon.traffic);
	CommandBarDrawTemp(image_terrain_bar, L"堡垒", 48, 73, hexagon.fortress);
	CommandBarDrawTemp(image_terrain_bar, L"分值", 48, 96, hexagon.victory_points);
	CommandBarDrawTemp(image_terrain_bar, L"城市", 48, 141, hexagon.city_degree);
	{
		//地名
		wchar_t text[255];
		CharToWchar(text, hexagon.name, 25);
		BoxFull(image_terrain_bar, 4, 175, 96, 195, 0xffffff);
		Box(    image_terrain_bar, 4, 175, 96, 195, 2, 0x3232ac);
		TextFont(image_terrain_bar, 6, 161, L"地名", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_terrain_bar, 6, 177, text, 0x0080ff, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}

	DrawingArea(engineback, image_terrain_bar, WINDOWWIDTH, WINDOWCOMMANDHEIGHT * 2, image_terrain_bar->lengths.x, image_terrain_bar->lengths.y);
}

static void UnitCommandAttributeBar(UNIT unit)
{
	static int left = 10, top = 10 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, right = WINDOWCOMMANDHEIGHT / 2, bottom = WINDOWCOMMANDHEIGHT / 2, unittype = 0, vectory = 0;
	static POINT coord;
	if (unit.move == vectory && unit.unit_type == unittype && coord.x == hexagon_select_coord->x && coord.y == hexagon_select_coord->y)
	{
		DrawingArea(engineback, image_unit_bar, WINDOWWIDTH, 0 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, WINDOWCOMMANDHEIGHT, 0 + 2 * WINDOWCOMMANDHEIGHT);
		return;
	}
	coord.x = hexagon_select_coord->x , coord.y = hexagon_select_coord->y;
	unittype = unit.unit_type;
	vectory = unit.move;
	BoxFull(image_unit_bar, 0, 0 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, WINDOWCOMMANDHEIGHT, 0 + 2 * WINDOWCOMMANDHEIGHT, 0x9dcdff);
	BoxFull(image_unit_bar, 0, 0 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, 65 /* WINDOWCOMMANDHEIGHT * .75 - 10 */, 65 /* 0 + WINDOWCOMMANDHEIGHT * .75 - 10*/, unit.sid ? unit.sid : 0xfcdbcb);
	UnitIconBack(image_unit_bar, left, top, right, bottom);
	TextFont(image_unit_bar, 0, 75 /* WINDOWCOMMANDHEIGHT * .75f */, L"类型", 0x086cff, FALSE, L"Arial", 0, 24, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	BoxFull(image_unit_bar, 48, 75, 96, 98, 0xffffff);
	Box(image_unit_bar, 48, 75, 96, 98, 2, 0x3232ac);
	switch (unit.unit_type)
	{
	case UNIT_TYPE_INF:
		UnitIconINF(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"步兵", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_TANK:
		UnitIconTANK(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"装甲", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_MECHINF:
		UnitIconMECHINF(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"机械化", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_MOT:
		UnitIconMOT(image_unit_bar, left, top, right, bottom);
		Line(image_unit_bar, left, bottom, left, bottom - 1, 15, 0);
		Line(image_unit_bar, right, bottom, right, bottom - 1, 15, 0);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"摩托化", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_HQ:
		UnitIconHQ(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"指挥所", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_SEC:
		UnitIconSEC(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"安保", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_CAV:
		UnitIconCAV(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"骑兵", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_MTND:
		UnitIconMTND(image_unit_bar, left, top, right, bottom);
		Line(image_unit_bar, (right + left) / 2, bottom - 6, (right + left) / 2, bottom - 6, 15, 0);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"山地", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	case UNIT_TYPE_MARI:
		UnitIconMARI(image_unit_bar, left, top, right, bottom);
		TextFont(image_unit_bar, 50, 77 /* WINDOWCOMMANDHEIGHT * .75f */, L"海陆战", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	}
	wchar_t text[255];
	{
		//人员
		BoxFull(image_unit_bar, 48, 100, 96, 114, 0xffffff);
		Box(    image_unit_bar, 48, 100, 96, 114, 2, 0x3232ac);
		swprintf(text, 255, L"%d", (int)Lerp(0, 15000, unit.people / 255.f));
		TextFont(image_unit_bar, 0, 100, L"人员", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_unit_bar, 50, 100, text, 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//等级
		BoxFull(image_unit_bar, 48, 114, 96, 128,    0xffffff);
		Box(    image_unit_bar, 48, 114, 96, 128, 2, 0x3232ac);
		swprintf(text, 255, L"%d", (int)Lerp(0, 100, unit.attack_level / 255.f));
		TextFont(image_unit_bar, 0, 114, L"等级", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_unit_bar, 50, 114, text, 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//决战计划
		BoxFull(image_unit_bar, 48, 128, 96, 142, 0xffffff);
		BoxFull(image_unit_bar, 48, 128, Lerp(48, 96, unit.final_battle_blan / 255.f), 142, 0x36f2fb);
		Box(    image_unit_bar, 48, 128, 96, 142, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 128, L"准备值", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//总计划值
		BoxFull(image_unit_bar, 48, 142, 96, 156, 0xffffff);
		BoxFull(image_unit_bar, 48, 142, Lerp(48, 96, unit.planned_value / 255.f), 156, 0x36fb42);
		Box(	image_unit_bar, 48, 142, 96, 156, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 142, L"计划值", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	{
		//番号
		CharToWchar(text, unit.name, 25);
		BoxFull(image_unit_bar,  4, 170, 96, 190, 0xffffff);
		Box(    image_unit_bar,  4, 170, 96, 190, 2, 0x3232ac);
		TextFont(image_unit_bar, 0, 156,	L"番号", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(image_unit_bar, 6, 172,	text,	0x0080ff, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	}
	TextFont(image_unit_bar, 67, 0, L"移动", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	TextFont(image_unit_bar, 67, 16, L"方向", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
	BoxFull( image_unit_bar, 67, 32, 97, 62, 0xffffff);
	Box(     image_unit_bar, 67, 32, 97, 62, 2, 0);
	switch (unit.move)
	{
	case 0:TextFont(image_unit_bar, 69, 40, L"未定", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 1:TextFont(image_unit_bar, 69, 40, L"东南", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 2:TextFont(image_unit_bar, 69, 40, L"正南", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 3:TextFont(image_unit_bar, 69, 40, L"西南", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 4:TextFont(image_unit_bar, 69, 40, L"西北", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 5:TextFont(image_unit_bar, 69, 40, L"正北", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	case 6:TextFont(image_unit_bar, 69, 40, L"东北", 0x6e9437, FALSE, L"Arial", 0, 14, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET); break;
	}
	DrawingArea(engineback, image_unit_bar, WINDOWWIDTH, 0 /*0 * 1.5 * WINDOWCOMMANDHEIGHT + 10*/, WINDOWCOMMANDHEIGHT, 0 + 2 * WINDOWCOMMANDHEIGHT);
}

extern void DrawButton(MOONBUTTON* button, LPCWSTR text, int mode)
{
	switch (button->mode)
	{
	case MOON_BUTTONRHOVER:
	case MOON_BUTTONPRESS:
		BoxFull(engineback, button->x, button->y, button->x + button->width, button->y + button->height, 0xeeeeee);
		Box(engineback, button->x + 11, button->y + 11, button->x + button->width - 9, button->y + button->height - 9, 2, 0);
		if (mode) BoxFull(engineback, button->x + 10, button->y + 10, button->x + button->width - 10, button->y + button->height - 10, 0xff9b63);
		TextFont(engineback, button->x + 14, button->y + button->height / 2 - 9, text, RGB(150, 150, 150), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		TextFont(engineback, button->x + 13, button->y + button->height / 2 - 10, text, RGB(255, 0, 0), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;

	case 0:
		BoxFull(engineback, button->x, button->y, button->x + button->width, button->y + button->height, 0xffffff);
		if (mode) BoxFull(engineback, button->x + 10, button->y + 10, button->x + button->width - 10, button->y + button->height - 10, 0xff9b63);
		TextFont(engineback, button->x + 14, button->y + button->height / 2 - 10, text, RGB(255, 0, 0), FALSE, L"Microsoft YaHei", 0, 20, 0, 0, FW_BOLD, FALSE, FALSE, DEFAULT_CHARSET);
		break;
	}
	Box(engineback, button->x + 3, button->y + 3, button->x + button->width - 3, button->y + button->height - 3, 2, 0);
}