#include"MoonHexagonAttackEngine_function.h"

static POINT* mousecoord;
static POINT* hexagon_mouse_coord;
static POINT* hexagon_map_coord;
static POINT* hexagon_select_coord;
static int* map_size;
static int* hexagon_r;
static int* drawingcommand;
static int* playgamemode;
static int GAMEMAPWIDTH;
static int GAMEMAPHEIGHT;
static int year;
static int month;
static int day;
static HEXAGONMAP* hexagon_map;
GAMEATTRIBUTE* gameattribute;
static _Bool trainsupply[GAMEMAPWIDTHMAX * GAMEMAPHEIGHTMAX] = { 0 };

//运行时脚本
static const char* ScriptLogic[] =
{
	"Exit",				//退出回合																	|	语法		Exit																				.0		已测试解决
	"Round",			//回合设置，必须用Exit结尾,如果参数为-1则视为一般回合								|	语法		Round [ Round ]																		.1		已测试解决
	"Supply",			//设置补给基地,需要有指挥部在该格上方方可沿周围的铁路格发放补给						|	语法		Supply [ coordx , coordy , 阵营 ]													.2		已测试解决
	"Unit",				//设置单位,如果人数是零则用模板匹配												|	语法		Unit [ Name ] [ coordx , coordy , 阵营 ]	[ Type , Armor , Mobility , People ]		.3		已测试解决
	"Title",			//输出消息																	|	语法		Title [ 消息 ]																		.4		已测试解决
	"If",				//检查某一格的阵营,执行操作														|	语法		If [ coordx , coordy , 阵营 ]														.5		已测试解决
	"Endif",			//结束If																		|	语法		Endif																				.6		已测试解决
	"Vp",				//检查VP大于等于直接获胜结束游戏													|	语法		Vp [ 阵营 ] [应该的得到的分值]															.7		已测试解决
	"Victory",			//直接胜利																	|	语法		Victory [ 阵营 ] 																	.8		已测试解决
	"#",				//注释																		|	语法		# 注释																				.9		已测试解决
	"Weather",			//设置某一格天气																|	语法		Weather [ x , y , weather ]															.10		已测试解决
	"SetUnit"			//调整某一格单位状态															|	语法		SetUnit  [ coordx , coordy ] [ org : org , fbb : fbb , pv : pv , supply : supply , people : people ]			.11		已测试解决
};

static void MapControl();//控制地图移动
static void HexagonSelect();//选择格子
static void UnitMoveSetting();
static void UnitClear(UNIT* unit);	//清除单位
static PROJECTMODULE(ScriptRunLogic);		//运行时脚本解释器

static int ButtonBarReturnHallStart(PROJECTGOD* project, MOONBUTTON* buton)
{
	ReturnGameStart(project);
}

static int ButtonNextRound(PROJECTGOD* project, MOONBUTTON* buton)
{
	UnitMoveAll();
	WeatherInit(project);
	gameattribute->allround++;
	int allday = year * 360 + month * 30 + day + gameattribute->everyday * gameattribute->allround;
	gameattribute->year = allday / 360;
	gameattribute->month = (allday  % 360) / 30 + 1;
	gameattribute->day = (allday % 360) % 30 + 1;
	for (int o = 0; o < GAMEMAPWIDTH; o++)
		for (int i = 0; i < GAMEMAPHEIGHT; i++)
			if (hexagon_map[o + i * GAMEMAPWIDTH].unit.unit_type < UNIT_TYPE_END && hexagon_map[o + i * GAMEMAPWIDTH].unit.unit_type > 0)
			{
				int supply = hexagon_map[o + i * GAMEMAPWIDTH].unit.supply_level,
					//pv = hexagon_map[o + i * GAMEMAPWIDTH].unit.planned_value,//临时
					fbb = hexagon_map[o + i * GAMEMAPWIDTH].unit.final_battle_blan;
				supply = RANGE(supply - 10, 0, 255);
				fbb = RANGE(fbb + 16, 0, 255);
				//pv = RANGE(pv + 10, 0, 255);//临时
				hexagon_map[o + i * GAMEMAPWIDTH].unit.supply_level = supply;
				hexagon_map[o + i * GAMEMAPWIDTH].unit.final_battle_blan = fbb;
				//hexagon_map[o + i * GAMEMAPWIDTH].unit.planned_value = pv;//临时
			}
	int alpha = ScriptRunLogic(project);
	printf("脚本返回值[%d]\n", alpha);
	if (alpha == 5)
	{
		MessageBox(project->hwnd, L"玩家[1]达到了分数\n恭喜玩家[1]取得了胜利", L" 玩家[1]胜利", MB_OK | MB_ICONINFORMATION);
		ReturnGameStart(project);
	}
	else if (alpha == 6)
	{
		MessageBox(project->hwnd, L"玩家[2]达到了分数,赢得了胜利\n恭喜玩家[2]取得了胜利", L"玩家[2]胜利", MB_OK | MB_ICONINFORMATION);
		ReturnGameStart(project);
	}
	*drawingcommand = DRAW_Redraw;
}

extern PROJECTSETTING(GameLogicSetting)
{
	HashFindEntity(project, "Button_Game_Round", MOONBUTTON, Button_Game_Round);
	HashFindEntity(project, "Button_Game_Hall", MOONBUTTON, Button_Game_Hall);
	HashFindEntity(project, "GAMEATTRIBUTE", GAMEATTRIBUTE, gameattribute_2);
	HashFindEntity(project, "Hexagon_Select_Coord", POINT, hexagon_select_coord_2);
	HashFindEntity(project, "HexagonMouseCoord", POINT, hexagon_mouse_coord_2);
	HashFindEntity(project, "Hexagon_Map_Coord", POINT, hexagon_map_coord_2);
	HashFindEntity(project, "ProjectMouseCoord", POINT, mousecoord_2);
	HashFindEntity(project, "Hexagon_Map", HEXAGONMAP, hexagon_map_2);
	HashFindEntity(project, "Drawing_Command", int, drawingcommand_2);
	HashFindEntity(project, "GAMEMAPHEIGHT", int, GAMEMAPHEIGHT_2);
	HashFindEntity(project, "GAMEMAPWIDTH", int, GAMEMAPWIDTH_2);
	HashFindEntity(project, "PlayGameMode", int, playgamemode_2);
	HashFindEntity(project, "Hexagon_R", int, hexagon_r_2);
	HashFindEntity(project, "Map_Size", int, map_size_2);
	HashFindEntity(project, "Editor", int, editor);
	hexagon_select_coord = hexagon_select_coord_2;
	hexagon_mouse_coord = hexagon_mouse_coord_2;
	hexagon_map_coord = hexagon_map_coord_2;
	drawingcommand = drawingcommand_2;
	GAMEMAPHEIGHT = *GAMEMAPHEIGHT_2;
	GAMEMAPWIDTH = *GAMEMAPWIDTH_2;
	playgamemode = playgamemode_2;
	hexagon_map = hexagon_map_2;
	mousecoord = mousecoord_2;
	hexagon_r = hexagon_r_2;
	map_size = map_size_2;
	gameattribute = gameattribute_2;
	Button_Game_Hall->ButtonModePress = ButtonBarReturnHallStart;
	Button_Game_Round->ButtonModePress = ButtonNextRound;
	year = gameattribute->year;
	month = gameattribute->month;
	day = gameattribute->day;
	gameattribute->allround = 0;
	if (*editor) ProjectFunctionSwitch(&project->Logic, EditorLogicSetting);
	else ProjectFunctionSwitch(&project->Logic, GameLogic);
	return 1;
}

extern PROJECTMODULE(GameLogic)
{
	MoonSleep(1);
	GetHexagonMouseCoord();
	printf("x:%d,y:%d            \r", hexagon_mouse_coord->x, hexagon_mouse_coord->y);
	if (*drawingcommand == DRAW_Continue_Drawing) MapControl();
	CommandBarLogic(project);
	if (mousecoord->x > 0 && mousecoord->y > 0 && mousecoord->x < WINDOWWIDTH && mousecoord->y < WINDOWHEIGHT)
	{
		HexagonSelect();
		UnitMoveSetting();
	}
	return 1;
}

extern POINT HexagonTransitionPoint(int transitionpointx, int transitionpointy, int R)
{
	int xi = transitionpointy / (1.73 * R), xo = transitionpointx / (1.5f * R);
	POINT alpha = { Error,Error };
	for (int i = xi - 1; i <= xi + 1; i++)
		for (int o = xo - 1; o <= xo + 1; o++)
		{
			int x2 = R * 1.73 / 2, x = 1.5 * R * o + R, y = 1.73 * R * i + (o % 2 ? x2 : 0) + x2, beta[6] = { 0 },
				hexagonmapx = o * 1.5 * R + R, hexagonmapy = i * 1.73 * R + (o % 2 ? x2 : 0) + x2,
				pointx[6] = { hexagonmapx + R, hexagonmapx + R / 2, hexagonmapx - R / 2, hexagonmapx - R, hexagonmapx - R / 2, hexagonmapx + R / 2 },
				pointy[6] = { hexagonmapy, hexagonmapy + 1.73 * R / 2, hexagonmapy + 1.73 / 2 * R, hexagonmapy, hexagonmapy - 1.73 * R / 2, hexagonmapy - 1.73 * R / 2 };
			for (int k = 0; k < 6; k++) beta[k] = (transitionpointx - pointx[k]) * (pointy[k] - pointy[(k + 1) % 6]) - (transitionpointy - pointy[k]) * (pointx[k] - pointx[(k + 1) % 6]);
			if (beta[0] > 0 && beta[1] > 0 && beta[2] > 0 && beta[3] > 0 && beta[4] > 0 && beta[5] > 0) { alpha.x = o; alpha.y = i; }
		}
	//(alpha.x < 0 || alpha.y < 0) && (alpha.x = Error, alpha.y = Error);
	return alpha;
}

extern POINT HexagonTurningPoint(int pointx, int pointy, int R)
{
	POINT returnpoint = { 0 };
	returnpoint.x = 1.5 * R * pointx + R;
	returnpoint.y = 1.73 * R * pointy + (pointx % 2 ? 1.73 / 2 * R : 0) + 1.73 / 2 * R;
	return returnpoint;
}

extern void GetHexagonMouseCoord()
{
	POINT hexagon_mouse_coordbuffer = HexagonTransitionPoint(mousecoord->x, mousecoord->y, *hexagon_r);
	static POINT hexagon_mouse_coordbuffer_2;
	hexagon_mouse_coordbuffer.x != Error && (hexagon_mouse_coordbuffer_2.x = hexagon_mouse_coordbuffer.x);	//记录非法坐标之前的坐标
	hexagon_mouse_coordbuffer.y != Error && (hexagon_mouse_coordbuffer_2.y = hexagon_mouse_coordbuffer.y);
	hexagon_mouse_coordbuffer.x == Error && (hexagon_mouse_coordbuffer.x = hexagon_mouse_coordbuffer_2.x);	//还原非法坐标之前的坐标
	hexagon_mouse_coordbuffer.y == Error && (hexagon_mouse_coordbuffer.y = hexagon_mouse_coordbuffer_2.y);
	hexagon_mouse_coordbuffer.x = RANGE(hexagon_mouse_coordbuffer.x + hexagon_map_coord->x * 2, 1, GAMEMAPWIDTH - 2);	//限制范围
	hexagon_mouse_coordbuffer.y = RANGE(hexagon_mouse_coordbuffer.y + hexagon_map_coord->y, 1, GAMEMAPHEIGHT - 2);
	hexagon_mouse_coord->x = hexagon_mouse_coordbuffer.x;
	hexagon_mouse_coord->y = hexagon_mouse_coordbuffer.y;
	return;
}

extern PROJECTMODULE(CommandBarLogic)
{
	ButtonDetection(project, "Button_Game_Hall");
	ButtonDetection(project, "Button_Game_Round");
	ButtonDetection(project, "Button_Game_Terrain") == MOON_BUTTONPRESS && (*drawingcommand = DRAW_Redraw);
	ButtonDetection(project, "Button_Game_Weather") == MOON_BUTTONPRESS && (*drawingcommand = Draw_Weather);
	ButtonDetection(project, "Button_Game_Traffic") == MOON_BUTTONPRESS && (*drawingcommand = Draw_Traffic);
	return 1;
}

extern int UnitCombat(HEXAGONMAP* hexagon_attack, HEXAGONMAP* hexagon_defender)
{
	//需要加上地形影响
	hexagon_attack->unit.final_battle_blan = NULL;
	//绝对非法
	if (
		hexagon_defender->unit.sid == 0 || hexagon_attack->unit.sid == 0 ||
		hexagon_defender->terrain == Terrain_No_Entry || hexagon_attack->terrain == Terrain_No_Entry ||
		hexagon_defender->unit.unit_type > 9 || hexagon_defender->unit.unit_type == 0 ||
		hexagon_attack->unit.unit_type > 9 || hexagon_attack->unit.unit_type == 0 ||
		hexagon_attack->unit.organization < 64/*25%*/ ||
		hexagon_defender->unit.sid == hexagon_attack->unit.sid
		)return Error;

	unsigned int Breakthrough_Efficiency = 0,
		Terrain_Bonus = 0,
		Attack_battlefield_foundation_width = 0,
		Defender_battlefield_foundation_width = 0,
		Attack_Frontline_Personnel = 0,
		Defender_Frontline_Personnel = 0;
	for (int i = 1; i < 7; i++)
		if (hexagon_defender->hexagon[i]->unit.sid == hexagon_attack->unit.sid)Attack_battlefield_foundation_width += hexagon_defender->hexagon[i]->battlefield_foundation_width;
	Defender_battlefield_foundation_width = hexagon_defender->battlefield_foundation_width;
	Attack_battlefield_foundation_width = RANGE(Attack_battlefield_foundation_width - hexagon_defender->fortress, 0, Attack_battlefield_foundation_width);
	Attack_battlefield_foundation_width = RANGE(Attack_battlefield_foundation_width - (int)Lerp(0, Attack_battlefield_foundation_width, hexagon_defender->city_degree / 255.f), 0, Attack_battlefield_foundation_width);
	Attack_Frontline_Personnel = RANGE(hexagon_attack->unit.people / 6, 1, Attack_battlefield_foundation_width / 10);
	Defender_Frontline_Personnel = RANGE(hexagon_defender->unit.people / 3, 1, Defender_battlefield_foundation_width / 10);
	Breakthrough_Efficiency = (int)Lerp(0, 400, RANGE(hexagon_attack->unit.armor * 1.f / hexagon_attack->unit.people, 0, 1.f));
	Attack_Frontline_Personnel = Attack_Frontline_Personnel + Attack_Frontline_Personnel * Breakthrough_Efficiency / 100.f;
	Attack_Frontline_Personnel = Attack_Frontline_Personnel + Attack_Frontline_Personnel * 1.f / ((hexagon_attack->unit.final_battle_blan / 16.f - 8) * (hexagon_attack->unit.final_battle_blan / 16.f - 8) + 1.f);
	static const int arrcombatjudgmenttable[9][9] =
	{
		{0x2a0, 0x2a0, 0x1a0, 0x1e1, 0x1e1, 0x1e1, 0x1e2, 0x1e2, 0x1e2},
		{0x2a0, 0x1a0, 0x1e1, 0x1e1, 0x1e1, 0x1e2, 0x1e2, 0x1e2, 0x1d0},
		{0x1a0, 0x1e1, 0x1e1, 0x1e1, 0x1e2, 0x1e2, 0x1e2, 0x1d0, 0x1d0},
		{0x1e1, 0x1e1, 0x1e1, 0x1e2, 0x1e2, 0x1e2, 0x1d0, 0x1d0, 0x1d1},
		{0x1e1, 0x1e1, 0x1e2, 0x1e2, 0x1e2, 0x1d0, 0x1d0, 0x1d1, 0x2d1},
		{0x1e1, 0x1e2, 0x1e2, 0x1e2, 0x1d0, 0x1d0, 0x1d1, 0x2d1, 0x3d1},
		{0x1e2, 0x1e2, 0x1e2, 0x1d0, 0x1d0, 0x1d1, 0x2d1, 0x3d1, 0x4d1},
		{0x1e2, 0x1e2, 0x1d0, 0x1d0, 0x1d1, 0x2d1, 0x3d1, 0x4d1, 0xad0},
		{0x1e2, 0x1d0, 0x1d0, 0x1d1, 0x2d1, 0x3d1, 0x4d1, 0xad0, 0xad0},
	};
	int battlesrand = 0, outcome = 0;
	if (Defender_Frontline_Personnel == 0)
	{
		UnitClear(&hexagon_defender->unit);
		hexagon_defender->unit = hexagon_attack->unit;
		UnitClear(&hexagon_attack->unit);
		return 1;
	}
	else
	{
		battlesrand = Attack_Frontline_Personnel / Defender_Frontline_Personnel;
		if (battlesrand <= 0) outcome = arrcombatjudgmenttable[0][Random(0, 4)];
		else outcome = arrcombatjudgmenttable[Random(0, 9)][battlesrand];
	}
	int attck_harm = 0, defend_harm = 0;
	printf("比值[%d]战斗结果为[%x]\n", battlesrand, outcome);
	int alpha = (outcome & 0x0f0), beta = (outcome & 0xf00) >> 8, gamma = (outcome & 0x00f);
	switch (alpha)
	{
	case 0x0a0:
		hexagon_attack->unit.people = max(hexagon_attack->unit.people - hexagon_attack->unit.people * beta / 10.f, 0);
		hexagon_attack->unit.organization = max(hexagon_attack->unit.organization - hexagon_attack->unit.organization * beta / 10.f, 0);
		break;
	case 0x0e0:
		hexagon_attack->unit.attack_level = RANGE(hexagon_attack->unit.attack_level++, 0, 255);
		hexagon_attack->unit.people = max(hexagon_attack->unit.people - hexagon_attack->unit.people * beta / 10.f, 0);
		hexagon_attack->unit.organization = max(hexagon_attack->unit.organization - hexagon_attack->unit.organization * beta / 10.f, 0);
		hexagon_defender->unit.people = max(hexagon_defender->unit.people - hexagon_defender->unit.people * gamma / 10.f, 0);
		hexagon_defender->unit.organization = max(hexagon_defender->unit.organization - hexagon_defender->unit.organization * beta / 10.f, 0);
		break;
	case 0x0d0:
		hexagon_attack->unit.attack_level = RANGE(hexagon_attack->unit.attack_level++, 0, 255);
		hexagon_defender->unit.people = max(hexagon_defender->unit.people - hexagon_defender->unit.people * beta / 10.f, 0);
		hexagon_defender->unit.organization = max(hexagon_defender->unit.organization - hexagon_defender->unit.organization * beta / 10.f, 0);
		break;
	}
	int score[7] = { 0 }, retreatmove = 0;
	if (hexagon_defender->unit.people && gamma)
	{
		for (int i = 1; i < 7; i++)
		{
			if (hexagon_defender->hexagon[i]->unit.sid == hexagon_attack->unit.sid)
				score[i] -= 999;
			if (hexagon_defender->hexagon[i]->unit.unit_type < UNIT_TYPE_END && hexagon_defender->hexagon[i]->unit.unit_type > 0)
				score[i] -= 999;
			else score[i] += 999;
			score[i] += hexagon_defender->hexagon[i]->battlefield_foundation_width;
		}
		int motemp = score[0];
		for (int i = 1; i < 7; i++)
			if (motemp < score[i])
			{
				retreatmove = i;
				motemp = score[i];
			}
	}
	int pp = 0;
	if (!hexagon_attack->unit.people)
	{
		UnitClear(&hexagon_attack->unit);
		pp = 5;
	}
	if (!hexagon_defender->unit.people)
	{
		UnitClear(&hexagon_defender->unit);
		hexagon_defender->unit = hexagon_attack->unit;
		UnitClear(&hexagon_attack->unit);
		pp = 4;
	}
	if (pp)return pp;
	if (alpha == 0x0d0)
		if (!(hexagon_defender->hexagon[retreatmove]->unit.unit_type < UNIT_TYPE_END && hexagon_defender->hexagon[retreatmove]->unit.unit_type > 0))
		{
			hexagon_defender->hexagon[retreatmove]->unit = hexagon_defender->unit;
			UnitClear(&hexagon_defender->unit);
		}
		else
		{
			hexagon_defender->unit = hexagon_attack->unit;
			UnitClear(&hexagon_attack->unit);
		}
	return 3;
}

extern int UnitMoveSelect(HEXAGONMAP* unit_to, HEXAGONMAP* unit_from)
{
	for (int i = 1; i <= 6; i++)
		if (unit_from->hexagon[i] == unit_to) { unit_from->unit.move = i; return i; }
	unit_from->unit.move = 0;
	return NOTFOUND;
}

extern int UnitMove(HEXAGONMAP* unit_from)
{
	if (unit_from->unit.unit_type >= UNIT_TYPE_END || unit_from->unit.unit_type == 0 || unit_from->unit.move == 0) { unit_from->unit.move = 0; return 0; }
	HEXAGONMAP* unit_to = unit_from->hexagon[unit_from->unit.move];
	if (unit_to->terrain == Terrain_No_Entry || unit_to->terrain == Terrain_Ocean) { unit_from->unit.move = 0; return 1; }
	if (unit_to->unit.unit_type < UNIT_TYPE_END && unit_to->unit.unit_type > 0)
	{
		unit_from->unit.move = 0;
		unit_to->unit.move = 0;
		int alpha = UnitCombat(unit_from, unit_to);
		printf("番号为[%s]的部队和番号为[%s]发生了战斗!!!战斗函数返回码为[%d]\n", unit_from->unit.name, unit_to->unit.name, alpha);
		return 2;
	}
	unsigned char unobstructed = 0;
	HEXAGONMAP* hexagontemp[2] = { 0 };
	hexagontemp[0] = unit_to->hexagon[0];
	for (int i = 0; i < 7; i++)
	{
		hexagontemp[1] = unit_to->hexagon[i];
		//if (hexagontemp[1]->hexagon[hexagontemp[1]->unit.move] == hexagontemp[0])continue;
		if ((255 - hexagontemp[0]->unit.mobility) >= hexagontemp[1]->unit.mobility)//>=寻找空格,<=寻找单位格
			unobstructed = i;
	}
	if (unobstructed)
	{
		//竞争过了
		for (int i = 1; i < 7; i++)
		{
			if (i == unobstructed)continue;
			hexagontemp[1] = unit_to->hexagon[i];
			if (hexagontemp[1]->hexagon[hexagontemp[1]->unit.move] == hexagontemp[0])hexagontemp[1]->unit.move = 0;
		}
		unit_from->unit.move = 0;
	}
	else { unit_from->unit.move = 0; return 3; }//竞争力不够
	if (unit_from->unit.organization < 64 //25%
		|| unit_from->unit.supply_level < 64//25%
		) return 4;//状态不好
	unit_from->unit.final_battle_blan = 0;
	unit_from->unit.planned_value = RANGE(unit_from->unit.planned_value - 30, 0, 255);
	short int alpha = (Random(128, 255) + (unit_to->traffic)), beta = unit_from->unit.mobility + Lerp(0, 383/*255*1.5*/, unit_from->unit.planned_value / 255.f);
	beta = beta + 150 * ((unit_to->terrain == Terrain_Mount || unit_to->terrain == Terrain_Hills) && unit_from->unit.unit_type == UNIT_TYPE_MTND);//山地师加成
	beta = beta - 75 * (
		(unit_to->terrain == Terrain_Mount || unit_to->terrain == Terrain_Hills || unit_to->terrain == Terrain_DenseTree) &&
		(unit_from->unit.unit_type == UNIT_TYPE_MOT || unit_from->unit.unit_type == UNIT_TYPE_TANK || unit_from->unit.unit_type == UNIT_TYPE_MECHINF))
		- 30 * (unit_to->terrain == Terrain_Tree &&
			(unit_from->unit.unit_type == UNIT_TYPE_MOT || unit_from->unit.unit_type == UNIT_TYPE_TANK || unit_from->unit.unit_type == UNIT_TYPE_MECHINF));//坦克削弱
	beta = beta + 100 * ((unit_from->terrain == Terrain_Ocean || unit_to->terrain == Terrain_River) && unit_from->unit.unit_type == UNIT_TYPE_MARI);//海军陆战队加成
	beta = beta + 75 * ((unit_to->terrain == Terrain_Plain || unit_to->terrain == Terrain_River) && unit_from->unit.unit_type == UNIT_TYPE_CAV);//骑兵师/侦察加成
	beta = beta - unit_to->weather * 50;
	printf("alpha[%3d],beta[%3d],pv[%d]", alpha, beta, unit_from->unit.planned_value);
	if (alpha > beta)return 5;
	unit_to->unit = unit_from->unit;
	UnitClear(&unit_from->unit);
	unit_to->unit.move = 0;
	return 6;
}

extern int UnitMoveAll()
{
	for (int o = 0; o < GAMEMAPWIDTH; o++)
		for (int i = 0; i < GAMEMAPHEIGHT; i++)
		{
			int alpha = UnitMove(&hexagon_map[o + i * GAMEMAPWIDTH]);
			if (alpha)printf("番号为[%s]的部队移动返回码[%d]\n", hexagon_map[o + i * GAMEMAPWIDTH].unit.name, alpha);//临时
		}
	*drawingcommand = DRAW_Redraw;
	return 1;
}

static void MapControl()
{
	int alpha = (int)((GAMEMAPWIDTH - (WINDOWWIDTH / (*hexagon_r * 1.5) - 2)) / 2);
	if (KeyState(VK_UP)) { hexagon_map_coord->y = RANGE(hexagon_map_coord->y - 1, 0, GAMEMAPHEIGHT - (int)(WINDOWHEIGHT / (*hexagon_r * 1.732f))); *drawingcommand = DRAW_Redraw; }
	if (KeyState(VK_DOWN)) { hexagon_map_coord->y = RANGE(hexagon_map_coord->y + 1, 0, GAMEMAPHEIGHT - (int)(WINDOWHEIGHT / (*hexagon_r * 1.732f))); *drawingcommand = DRAW_Redraw; }
	if (KeyState(VK_LEFT)) { hexagon_map_coord->x = RANGE(hexagon_map_coord->x - 1, 0, alpha); *drawingcommand = DRAW_Redraw; }
	if (KeyState(VK_RIGHT)) { hexagon_map_coord->x = RANGE(hexagon_map_coord->x + 1, 0, alpha); *drawingcommand = DRAW_Redraw; }
	if (mousecoord->x > 0 && mousecoord->y > 0 && mousecoord->x < WINDOWWIDTH && mousecoord->y < WINDOWHEIGHT)
	{
		if (KeyState(VK_1))
		{
			*map_size = RANGE(*map_size - 1, Mini_Map, Full_Map);
			*map_size == Full_Map && ((hexagon_map_coord->x = 0), (hexagon_map_coord->y = 0));
			*drawingcommand = DRAW_Redraw;
		}
		if (KeyState(VK_2))
		{
			*map_size = RANGE(*map_size + 1, Mini_Map, Full_Map);
			*map_size == Full_Map && ((hexagon_map_coord->x = 0), (hexagon_map_coord->y = 0));
			*drawingcommand = DRAW_Redraw;
		}
	}
}

static void HexagonSelect()
{
	if (KeyState(1))
	{
		hexagon_select_coord->x = hexagon_mouse_coord->x;
		hexagon_select_coord->y = hexagon_mouse_coord->y;
	}
}
static void UnitMoveSetting()
{
	if (mousecoord->x > 0 && mousecoord->y > 0 && mousecoord->x < WINDOWWIDTH && mousecoord->y < WINDOWHEIGHT)
		if (KeyState(2))
			if (hexagon_map[hexagon_select_coord->x + hexagon_select_coord->y * GAMEMAPWIDTH].unit.unit_type)
			{
				HEXAGONMAP* hexagon_from = &hexagon_map[hexagon_select_coord->x + hexagon_select_coord->y * GAMEMAPWIDTH];
				HEXAGONMAP* hexagon_goto = &hexagon_map[hexagon_mouse_coord->x + hexagon_mouse_coord->y * GAMEMAPWIDTH];
				UnitMoveSelect(hexagon_goto, hexagon_from);
			}
}

static void UnitClear(UNIT* unit)
{
	strcpy(unit->name, "NULL");
	unit->final_battle_blan = 0;
	unit->planned_value = 0;
	unit->attack_level = 0;
	unit->organization = 0;
	unit->supply_level = 0;
	unit->unit_type = 0;
	unit->mobility = 0;
	unit->people = 0;
	unit->armor = 0;
	unit->move = 0;
	unit->sid = 0;
}

static int ScriptRunLogicClearTemp(FILE* fp)
{
	while (1)
	{
		char ch = getc(fp);
		if (ch == '\n')return 1;
		else if (ch == EOF)return 0;
	}
}

static int ScriptRunLogicTemp(PROJECTGOD* project, FILE* fp, int len, int* cycle)
{
	if (*cycle)return 4;
	while (1)
	{
		char command[25] = { 0 };
		int found = NOTFOUND, coordx, coordy, sid, type, armor, mobility, people, vp, weather;
		fscanf(fp, "%s", command);
		for (int i = 0; i < len; i++)
			if (!strcmp(command, ScriptLogic[i]))
			{
				found = i;
				break;
			}
		if (found == NOTFOUND)
		{
			wchar_t w_text[255];
			wchar_t w_command[25];
			CharToWchar(w_command, command, 25);
			swprintf(w_text, 255, L"未知的语法[ %s ]", w_command);
			MessageBox(project->hwnd, w_text, L"运行时脚本错误", MB_ICONERROR | MB_OK);
			*cycle = TRUE;
			return Error;
		}
		printf("已匹配到命令[ %s ]\n", ScriptLogic[found]);
		switch (found)
		{
		case 0://Exit
			*cycle = TRUE;
			return 1;
			break;
		case 6://Endif
			printf("if结束!\n");
			return 2;
			break;
		case 2://Supply
		{
			fscanf(fp, " [ %d , %d , %d ] \n", &coordx, &coordy, &sid);
			coordx = RANGE(coordx, 1, GAMEMAPWIDTH - 2);
			coordy = RANGE(coordy, 1, GAMEMAPHEIGHT - 2);
			if (sid != 0 && hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.sid == sid && hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.unit_type == UNIT_TYPE_HQ)
				UnitSupplyAll(coordx, coordy, sid);
		}
		break;
		case 3://Unit
		{
			char name[25] = { 0 };
			fscanf(fp, " [ %s ] \n", name);
			fscanf(fp, " [ %d , %d , %d ] \n [ %d , %d , %d , %d ]\n", &coordx, &coordy, &sid, &type, &armor, &mobility, &people);
			coordx = RANGE(coordx, 1, GAMEMAPWIDTH - 1);
			coordy = RANGE(coordy, 1, GAMEMAPHEIGHT - 1);
			if (hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.unit_type >= UNIT_TYPE_END || hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.unit_type == 0)
			{
				printf("已设置番号为[%s]的单位在[%d][%d],类型[%d],阵营[%d]\n", name, coordx, coordy, type, sid);
				hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.unit_type = type;
				hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.sid = sid;
				UnitInitTemplate(project, &hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit);
				strcpy(hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.name, name);
			}
		}
		break;
		case 4://Title
		{
			wchar_t text[255] = { 0 };
			char text_2[255] = { 0 };
			fscanf(fp, " [ %s ] \n", text_2);
			CharToWchar(text, text_2, 255);
			MessageBox(project->hwnd, text, L"当前消息", MB_OK | MB_ICONINFORMATION);
		}
		break;
		case 5://If
		{
			fscanf(fp, " [ %d , %d , %d ] \n", &coordx, &coordy, &sid);
			coordx = RANGE(coordx, 1, GAMEMAPWIDTH - 2);
			coordy = RANGE(coordy, 1, GAMEMAPHEIGHT - 2);
			if (sid != 0 && hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.sid == sid)
				ScriptRunLogicTemp(project, fp, len, cycle);
			else while (1)
			{
				fscanf(fp, "%s", command);
				if (!strcmp(command, ScriptLogic[6]))break;
				else ScriptRunLogicClearTemp;
			}
		}
		break;
		case 7://Vp
		{
			int player1_vp = 0, player2_vp = 0;
			fscanf(fp, " [ %d ] [ %d ] \n", &sid, &vp);
			for (int o = 0; o < GAMEMAPWIDTH; o++)
				for (int i = 0; i < GAMEMAPHEIGHT; i++)
				{
					if (hexagon_map[o + i * GAMEMAPWIDTH].unit.sid == sid)
					{
						if (sid == gameattribute->player_1_sid)
							player1_vp += hexagon_map[o + i * GAMEMAPWIDTH].victory_points;
						else if (sid == gameattribute->player_2_sid)
							player2_vp += hexagon_map[o + i * GAMEMAPWIDTH].victory_points;
					}
				}
			printf("玩家[%d]的分值为[%d],玩家[%d]的分值为[%d]", gameattribute->player_1_sid, player1_vp, gameattribute->player_2_sid, player2_vp);
			*cycle = TRUE;
			return (gameattribute->player_1_sid == sid && player1_vp >= vp) * 5 + (gameattribute->player_2_sid == sid && player2_vp >= vp) * 6;
		}
		break;
		case 8://Victory
		{
			fscanf(fp, " [ %d ] \n", &sid);
			*cycle = TRUE;
			if (sid == gameattribute->player_1_sid)
				return 5;
			else if (sid == gameattribute->player_2_sid)
				return 6;
		}
		break;
		case 9:// # 
			ScriptRunLogicClearTemp(fp);
			break;
		case 10://Weather
		{
			fscanf(fp, " [ %d , %d , %d ] \n", &coordx, &coordy, &weather);
			coordx = RANGE(coordx, 1, GAMEMAPWIDTH - 2);
			coordy = RANGE(coordy, 1, GAMEMAPHEIGHT - 2);
			printf("已将[%d][%d]设置为天气[%d]\n", coordx, coordy, weather);
			hexagon_map[coordx + coordy * GAMEMAPWIDTH].weather = RANGE(weather, 0, 2);
		}
		break;
		case 11:
		{
			char name[25] = { 0 };
			int org = 0, fbb = 0, pv = 0, supply = 0;
			fscanf(fp, " [ %d , %d ] \n [ org: %d , fbb: %d , pv: %d , supply: %d , people: %d ] \n", &coordx, &coordy, &org, &fbb, &pv, &supply, &people);
			coordx = RANGE(coordx, 1, GAMEMAPWIDTH - 1);
			coordy = RANGE(coordy, 1, GAMEMAPHEIGHT - 1);
			hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.people = (people == -1 ? hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.people : people);
			hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.organization = (org == -1 ? hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.organization : org);
			hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.final_battle_blan = (fbb == -1 ? hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.final_battle_blan : fbb);
			hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.planned_value = (pv == -1 ? hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.planned_value : pv);
			hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.supply_level = (supply == -1 ? hexagon_map[coordx + coordy * GAMEMAPWIDTH].unit.supply_level : supply);
		}
		break;
		}
	}
	return 3;
}

static PROJECTMODULE(ScriptRunLogic)
{
	HashFindEntity(project, "ScriptLogicMode", int, scriptlogicmode);
	FILE* fpscript = NULL;
	switch (*playgamemode)
	{
	case 0://startgame
		fpscript = fopen("Engine_Default_File/Game_ScriptLogic.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_ScriptLogic.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return 0;
		}
		break;
	case 1://modgame
		fpscript = fopen("Engine_MOD_File/Game_ScriptLogic.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_ScriptLogic.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return 0;
		}
		break;
	case 2://historygame
		switch (*scriptlogicmode)
		{
		case 0://startgame
			fpscript = fopen("Engine_Default_File/Game_ScriptLogic.txt", "r");
			if (fpscript == NULL)
			{
				MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_ScriptLogic.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return 0;
			}
			break;
		case 1://modgame
			fpscript = fopen("Engine_MOD_File/Game_ScriptLogic.txt", "r");
			if (fpscript == NULL)
			{
				MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_ScriptLogic.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return 0;
			}
			break;
		case 2://historygame

			break;
		case 3://editorgame
			fpscript = fopen("Engine_Editor_File/Game_ScriptLogic.txt", "r");
			if (fpscript == NULL)
			{
				MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_ScriptLogic.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
				project->DEAD = TRUE;
				return 0;
			}
			break;
		}
		break;
	case 3://editorgame
		fpscript = fopen("Engine_Editor_File/Game_ScriptLogic.txt", "r");
		if (fpscript == NULL)
		{
			MessageBox(project->hwnd, L"未找到可供游戏配置的脚本文件\n请确认\n[Engine_Editor_File/Game_ScriptLogic.txt]\n的存在!!!", L"MoonHexagonAttackEngine  Waring", MB_ICONWARNING | MB_OK);
			project->DEAD = TRUE;
			return 0;
		}
		break;
	}

	char command[25] = { 0 };
	int number = 0, len = sizeof(ScriptLogic) / sizeof(ScriptLogic[0]), cycle = FALSE;
	int endgame = 0;
	while (!cycle)
	{
		fscanf(fpscript, "%s", command);
		if (!strcmp(command, ScriptLogic[0]))return 2;
		if (!strcmp(command, ScriptLogic[9]))	//忽略注释
			ScriptRunLogicClearTemp(fpscript);
		else if (!strcmp(command, ScriptLogic[1]))
		{
			fscanf(fpscript, " [ %d ]\n", &number);
			if (number == gameattribute->allround || number == -1)
			{
				printf("内层进入成功\n");
				endgame = ScriptRunLogicTemp(project, fpscript, len, &cycle);
			}
			printf("内层已退出,解释器返回[%d]\n", endgame);
		}
		else ScriptRunLogicClearTemp(fpscript);
	}
	fclose(fpscript);
	return endgame;
}

extern void UnitSupplyAll(int x, int y, int sid)
{
	UnitSupplyHq(x, y, sid, 20);
	UnitSupply(sid);
	for (int i = 0; i < GAMEMAPWIDTH * GAMEMAPHEIGHT; i++)
		trainsupply[i] = FALSE;
	//for (int i = 0; i < GAMEMAPWIDTH * GAMEMAPHEIGHT; i++)
		//trainsupply[i] = FALSE;
}

//这个函数只供给指挥部补给值
extern void UnitSupplyHq(int x, int y, int sid, int depth)
{
	if (depth-- <= 0)return;
	if (hexagon_map[x + y * GAMEMAPWIDTH].unit.unit_type == UNIT_TYPE_HQ && hexagon_map[x + y * GAMEMAPWIDTH].unit.sid == sid)
		hexagon_map[x + y * GAMEMAPWIDTH].unit.supply_level = 255;
	trainsupply[x + y * GAMEMAPWIDTH] = TRUE;
	for (int i = 1; i < 7; i++)
		if (
			(hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->unit.sid == 0 || hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->unit.sid == sid) &&
			hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->train == TRUE &&
			!trainsupply[hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->coordx + hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->coordy * GAMEMAPWIDTH])
			UnitSupplyHq(hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->coordx, hexagon_map[x + y * GAMEMAPWIDTH].hexagon[i]->coordy, sid, depth);
}

//这个函数将用来判断指挥部补给是否为满,然后再为自己添加补给
extern void UnitSupply(int sid)
{
	for (int i = 0; i < GAMEMAPWIDTH * GAMEMAPHEIGHT; i++)
		for (int k = 0; k < 7; k++)
			for (int j = 0; j < 7; j++)
				if (hexagon_map[i].unit.sid == sid&& hexagon_map[i].hexagon[k]->hexagon[j]->unit.unit_type == UNIT_TYPE_HQ)
				{
					short int supply = hexagon_map[i].unit.supply_level, pv = hexagon_map[i].unit.planned_value;
					supply = RANGE(supply + 30, 0, 255);
					pv = RANGE(pv + 10, 0, 255);
					hexagon_map[i].unit.supply_level = supply;
					hexagon_map[i].unit.planned_value = pv;
				}
	/*
	for (int o = 0; o < GAMEMAPWIDTH; o++)
		for (int i = 0; i < GAMEMAPHEIGHT; i++)
			//if (hexagon_map[o + i * GAMEMAPWIDTH].unit.unit_type == UNIT_TYPE_HQ &&
				//hexagon_map[o + i * GAMEMAPWIDTH].unit.sid == sid)
				for (int k = 0; k < 7; k++)
					for (int j = 0; j < 7; j++)
					{
						trainsupply[
							hexagon_map[o + i * GAMEMAPWIDTH].hexagon[k]->hexagon[j]->coordx +
								hexagon_map[o + i * GAMEMAPWIDTH].hexagon[k]->hexagon[j]->coordy * GAMEMAPWIDTH
						] = TRUE;
					}
	for (int i = 0; i < GAMEMAPWIDTH * GAMEMAPHEIGHT; i++)
		if (hexagon_map[i].unit.unit_type < UNIT_TYPE_END && hexagon_map[i].unit.unit_type>0 && hexagon_map[i].unit.sid == sid)
		{
			short int supply = hexagon_map[i].unit.supply_level, pv = hexagon_map[i].unit.planned_value;
			supply = RANGE(supply + 10, 0, 255);
			pv = RANGE(pv + 10, 0, 255);
			hexagon_map[i].unit.supply_level = supply;
			hexagon_map[i].unit.planned_value = pv;
		}
	*/
}

/*
extern int UnitMove(HEXAGONMAP* unit_to, HEXAGONMAP* unit_from)
{
	//绝对非法
	//unit_from->unit.organization < 64/*25% || unit_from->unit.supply_level < 64/*25%
	if (unit_to->terrain == Terrain_No_Entry || unit_to->terrain == Terrain_Ocean)return 0;
	if (unit_from->unit.unit_type >= UNIT_TYPE_END || unit_from->unit.unit_type == 0)
	{
		if (unit_to->unit.move == 0) return 0;
	}
	//临时
	{
		unit_from->unit.people = 255;
		unit_from->unit.organization = 255;
		unit_from->unit.mobility = Random(0, 255);
		unit_from->unit.attack_level = 255;
		unit_from->unit.supply_level = 255;
	}
	unsigned char alpha = Random(0, 255), beta = (unsigned char)RANGE((int)(unit_from->unit.mobility + Lerp(0, 128, unit_from->unit.planned_value / 255)), 0, 255);
	printf("alpha[%3d],beta[%3d],Mo[%3d]|", alpha, beta, unit_from->unit.mobility);
	if (alpha > beta)return 0;
	{
		printf("番号[%s]移动方向[%d]", unit_from->unit.name, unit_from->unit.move);
		unit_to->unit = unit_from->unit;
		if (unit_to != unit_from)UnitClear(&unit_from->unit);
		unit_to->unit.move = 0;
	}
	return 1;
}

extern int UnitMoveAll()
{
	//临时
	{
		for (int o = 0; o < GAMEMAPWIDTH; o++)
			for (int i = 0; i < GAMEMAPHEIGHT; i++)
			{
				HEXAGONMAP* hexagon_from = &hexagon_map[o + i * GAMEMAPWIDTH];
				HEXAGONMAP* hexagon_goto = hexagon_map[o + i * GAMEMAPWIDTH].hexagon[hexagon_from->unit.move % 7];
				if (UnitMove(hexagon_goto, hexagon_from) == TRUE)printf("当前坐标[%-3d|%-3d]\n", o, i);
			}
		printf("开始移动\n");
		*drawingcommand = DRAW_Redraw;
	}
	return 1;
}
*/