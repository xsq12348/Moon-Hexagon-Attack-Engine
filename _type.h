#pragma once

/*
* 一些字体参考
* 
	字体名称				中文名			类型				适用场景						备注说明
	SimSun				宋体				衬线				正式文档、公文打印				Windows默认宋体，低分辨率屏幕显示好
	NSimSun				新宋体			衬线				正式文档、长文阅读				宋体改进版，支持更多Unicode字符
	SimHei				黑体				无衬线			标题、UI文字、屏幕显示			经典黑体，屏幕显示清晰
	Microsoft YaHei		微软雅黑			无衬线			高清屏UI、中英文混排			微软专为Vista+开发，高清屏最佳选择
	YouYuan				幼圆				无衬线			轻松活泼的设计					圆润可爱风格，笔画末端呈圆形
	KaiTi				楷体				手写				书法、艺术设计、正式场合			传统手写风格，优雅大方
	LiSu				隶书				书法				传统主题设计					古典书法字体，有毛笔书写特征
	FangSong			仿宋				衬线				公文、正式文件					公文标准字体，笔画纤细清晰
	STXihei				华文细黑			无衬线			现代设计、屏幕显示				从Mac移植的细黑体版本
	STKaiti				华文楷体			手写				艺术设计、标题					从Mac移植的楷体版本
	Arial								无衬线			网页、UI、屏幕显示				Windows系统默认无衬线英文字体
	Times New Roman						衬线				印刷、学术论文					经典衬线字体，正式场合使用
	Courier New							等宽				编程、代码显示					等宽字体，适合终端和代码编辑器
	Calibri								无衬线			Office文档默认				Office 2007+默认字体
	Verdana								无衬线			网页正文、屏幕显示				专为屏幕阅读优化的字体
	Georgia								衬线				长文阅读、标题					适合屏幕显示的衬线字体
	Impact								无衬线			粗体标题、海报					极粗的无衬线字体，用于醒目标题
	Comic Sans MS						手写				非正式场合、儿童内容			手写风格字体（设计界有争议）
	Consolas							等宽				编程开发						微软开发的编程专用等宽字体
	Lucida Console						等宽				终端显示						命令行终端常用等宽字体
	如果需要Unicode字体，字体选择直接为L""即可.
*/


#define VK_0 48
#define VK_1 49
#define VK_2 50
#define VK_3 51
#define VK_4 52
#define VK_5 53
#define VK_6 54
#define VK_7 55
#define VK_8 56
#define VK_9 57

#define VK_A 65
#define VK_B 66
#define VK_C 67
#define VK_D 68
#define VK_E 69
#define VK_F 70
#define VK_G 71
#define VK_H 72
#define VK_I 73
#define VK_J 74
#define VK_K 75
#define VK_L 76
#define VK_M 77
#define VK_N 78
#define VK_O 79
#define VK_P 80
#define VK_Q 81
#define VK_R 82
#define VK_S 83
#define VK_T 84
#define VK_U 85
#define VK_V 86
#define VK_W 87
#define VK_X 88
#define VK_Y 89
#define VK_Z 90

#define WINDOWWIDTH		1200
#define WINDOWHEIGHT	805
#define WINDOWCOMMANDHEIGHT	100

#define VECTORY float

#define GAMEMAPWIDTHMAX  152
#define GAMEMAPHEIGHTMAX 102

//All color values are in BGR format, please do not mix them up!!!
//所有颜色值都是 BGR 格式，请不要混淆!!!

#define MAPLINECOLOR		0x0000aa	//格子边缘线颜色
#define MAPLINEWIDTH		2			//格子边缘线宽
#define MAPOCEANCOLOR		0x8b3f15	//海洋
#define MAPTREECOLOR		0x00ff00	//树林
#define MAPDENSETREECOLOR	0x006400	//密林
#define MAPRIVERCOLOR		0xc83232	//河流
#define MAPHILLSCOLOR		0x3b568f	//丘陵
#define MAPMOUNTCOLOR		0x313966	//山地
#define MAPCOLOR			0xd8f0fa	//平原

#define UNITICON(NAME) void NAME(IMAGE* image, int left, int top, int right, int bottom)

enum
{
	Mini_Map,		//显示小规模地图
	Medium_Map,		//显示中等规模地图
	Full_Map		//显示全地图
};

enum
{
	Terrain_Plain,		//平原
	Terrain_Ocean,		//海洋
	Terrain_Tree,		//树林
	Terrain_DenseTree,	//密林
	Terrain_River,		//河流
	Terrain_Hills,		//丘陵
	Terrain_Mount,		//山地
	Terrain_No_Entry,	//地图边缘特殊属性,不可通行
};

enum
{
	DRAW_Pause_Draw,								//暂停绘制
	DRAW_Redraw,									//重绘
	DRAW_Continue_Drawing,							//继续绘制
	Draw_Weather,									//绘制天气
	Draw_Traffic									//绘制交通状况
};

enum
{
	UNIT_TYPE_INF = 1,								//步兵
	UNIT_TYPE_TANK,									//坦克
	UNIT_TYPE_MECHINF,								//机械化
	UNIT_TYPE_MOT,									//摩托化
	UNIT_TYPE_HQ,									//指挥部
	UNIT_TYPE_SEC,									//安保部队
	UNIT_TYPE_CAV,									//骑兵
	UNIT_TYPE_MTND,									//山地
	UNIT_TYPE_MARI,									//海军陆战队
	UNIT_TYPE_END,
};

typedef struct
{
	char name[25];									//番号名称
	int sid;										//阵营,实际上是颜色值
	unsigned char unit_type;						//单位种类
	unsigned char move;								//移动计划方向 | 范围1~6 0为未移动
	unsigned char people;							//人数		 | 俗称黄条
	unsigned char organization;						//组织度		 | 俗称绿条
	unsigned char mobility;							//机动能力	 | 俗称蓝条
	unsigned char planned_value;					//总计划值,影响移动速度 | 获得补给后缓慢恢复,移动时缓慢减少
	unsigned char final_battle_blan;				//决战计划,影响进攻能力 | 进攻或移动后清零
	unsigned char attack_level;						//军队等级,作战成功次数越多等级越高,没有特殊效果,只是单纯的功勋
	unsigned char supply_level;						//补给等级,检查这支军队还存有多少的补给
	unsigned char armor;							//装甲值
}UNIT;

typedef struct HEXAGONMAP
{
	//int sidcolor;									//所属阵营
	//int afficolor;								//实际所属
	//people;										//居住人口
	//unsigned char civilian_factories;				//民用工厂
	//unsigned char military_factories;				//军用工厂
	//_Bool airport;								//机场
	//_Bool shipport;								//船港
	char name[256];									//地区名称
	short int coordx;								//水平坐标
	short int coordy;								//垂直坐标
	unsigned char traffic;							//交通水平
	unsigned char city_degree;						//城市规模
	unsigned char terrain;							//地形
	unsigned char weather;							//天气  | 分为晴天(0)，阴天(1)，雨雪天(2)，每回合纯重置随机,影响移动和作战
	unsigned char fortress;							//堡垒防御工事
	unsigned char battlefield_foundation_width;		//战场宽度
	unsigned char victory_points;					//该地区胜利分
	_Bool train;									//铁路
	struct HEXAGONMAP* hexagon[7];					//邻居
	UNIT unit;
}HEXAGONMAP;

typedef struct
{
	int player_1_sid;								//玩家1
	int player_2_sid;								//玩家2
	unsigned int everyday;							//每回合天数
	unsigned int allround;							//回合轮数
	unsigned int year;								//起始年
	unsigned int month;								//起始月
	unsigned int day;								//起始日
	wchar_t script_title[1024];						//开局剧本标题简报
}GAMEATTRIBUTE;

typedef struct
{
	unsigned char armor;
	unsigned char mobility;
	unsigned char people;
}MODUNITDEFINITION;

typedef struct
{
	MODUNITDEFINITION inf;
	MODUNITDEFINITION tank;
	MODUNITDEFINITION mechinf;
	MODUNITDEFINITION mot;
	MODUNITDEFINITION hq;
	MODUNITDEFINITION sec;
	MODUNITDEFINITION cav;
	MODUNITDEFINITION mari;
	MODUNITDEFINITION mtnd;
	unsigned char plain;		//平原
	unsigned char ocean;		//海洋
	unsigned char tree;			//树林
	unsigned char denseTree;	//密林
	unsigned char river;		//河流
	unsigned char hills;		//丘陵
	unsigned char mount;		//山地
}MODDEFINITION;