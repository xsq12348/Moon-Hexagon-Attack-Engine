#pragma once
#include"MoonEngine/Moon.h"
#include"_type.h"

/*
作者:xsq1234
邮箱:1993346266@qq.com(常用),c6668883535357a@163.com(不常用)
工程创建日期:2025-12-5
名称:MoonHexagonAttackEngine(MHAE)

如果您是从GitHub得到的,请遵循GitHub项目内的开源协议。

否则协议如下
1. 版权声明
   本软件版权归 xsq1234 所有。

2. 授予的权利
   2.1 您可以：
	   - 使用、复制、修改本软件源代码
	   - 分发修改后的版本
   2.2 您必须：
	   - 保留原作者的版权声明
	   - 在显著位置注明修改内容
   2.3 您不得：
	   - 用于商业目的（需单独授权）
	   - 声称是原始作者

3. 序列号机制（可选）
   建议但不强制要求分发时使用不同序列号
   当前序列号：0000-0000-0000-0001

4. 免责声明
   本软件按"原样"提供,不提供任何担保。
   作者不对使用本软件造成的任何损失负责。

5. 禁止任何形式的未授权商业使用
	包括：
		- 销售软件或包含本软件的产品
		- 作为商业服务的一部分
		- 公司内部生产环境使用
	如需商业授权,请联系作者。

目前来源:GitHub
版本信息:测试版

日志:
	2025.12.6 完成了基础框架的搭建
	2025.12.7 
			  1.优化了坐标检测,现在坐标检测不再有任何问题
			  2.完成了地图结构体
	2025.12.8
			  1.增加完善了地图结构体和单位结构体
			  2.优化了地图绘制逻辑
			  3.使用设置函数解决了多次hash造成的性能损失
			  4.完成了地图绘制的大部分工作
	2025.12.9
			  1.完成了配置文件的读取
			  2.完成了标题界面的美术
	2025.12.10 
			  1.完成了脚本配置地图解释器
			  2.开始了编辑器的编写工作
	2025.12.11
			  1.开始了编辑器的编写工作
			  2.解决了编辑器绘制不了的问题
			  3.完成了编辑器的地形绘制,还差保存到文件
	2025.12.12
			  1.开始了命令栏逻辑的工作
			  2.解决了脚本系统的BUG
			  3.完成了命令栏的部分搭建
	2025.12.13
			  1.开始了命令栏逻辑的工作
			  2.开始处理编辑器的保存工作
	2025.12.14
			   1.完成了编辑器的收尾工作
			   2.更新了读取脚本函数
			   3.添加了初始化地图函数HexagonMapInit
	2025.12.15
			   1.解决了返回按钮无法返回的问题
			   2.实现了编辑器的继续编辑功能
	2025.12.16
			   1.解决了0xC0000005: 写入位置 0x0000000000000118 时发生访问冲突,原因是按钮被注释
			   2.单位图标绘制工作完成
			   3.添加了历史保存存档
			   4.完成了编辑器边框的的按钮设计
			   备忘录:与其写那么多按钮状态,不如多来点回调函数
	2025.12.17 
			   1.完成了编辑器边框的的按钮功能设计
			   2.完善了河流和铁路的绘制
			   备忘录:需要写单位配置脚本解释器
	2025.12.18 
			   1.完成了读取单位脚本解释器的部分设计
			   2.实现了单位配置脚本的保存
			   3.添加了UnitInitTemplate函数
			   备忘录:
			          1.需要接着写单位配置脚本解释器
					  2.还需要写游戏运行时操作脚本解释器
					  3.单位模板应该是可配置的,即MOD需要配置,而游戏本体不需要配置使用默认,编辑器游戏使用默认模板
	2025.12.19 
			   1.完成了读取单位脚本解释器的设计
			   2.继续了编辑器的设计
   			   备忘录:
					  1.需要开始设计战斗系统和移动系统以及回合流程了
					  2.接着昨天的,除了单位模板可配置,地形宽度应该也要是可配置的
	2025.12.20 
			   1.正在解决单位移动问题,大概率是Move出了问题
			   2.将邻居增加到7个,用归一化处理
			   3.解决了单位移动部分问题
			   4.解决了单位移动的问题
							       原因
								        之前手快复制导致的错误
												    原错误
													HEXAGONMAP* hexagon_goto = &hexagon_map[o + i * GAMEMAPWIDTH].hexagon[hexagon_from->unit.move % 7];
													修改
													HEXAGONMAP* hexagon_goto = hexagon_map[o + i * GAMEMAPWIDTH].hexagon[hexagon_from->unit.move % 7];
   			   备忘录:需要用递归来实现UnitMove函数
	2025.12.21 
			   1.正在编写单位移动函数
   			   备忘录:
	2025.12.22 
			   1.基本完成了单位移动函数
			   2.完成了单位模板函数
			   3.完成了地形模板函数
			   备注
					我也不知道为什么移动力是反着来的,到底哪一步错了,总之现在可以用了:在多单位竞争同一格时会导致机动能力更小的单位先进去,这很反直觉
	2025.12.23 
			   1.解决了Medium_Map模式下多绘制两列的问题
			   2.调整了单位模板函数
			   3.调整了地形模板函数
			   4.完成了命令栏中单位属性的绘制
			   5.重构删去了某些冗余代码
			   6.修正了脚本解释器无法读取地形和单位名称的BUG
			   7.开始了单位,地形模板脚本解释器的设计
			   8.将单位模板函数由硬编码改成结构体,适配脚本解释器
			   9. 12.18的备忘录1已完成
			   备忘录: 
					  1.明天需要把地形命令栏完善
					  2.脚本解释器除了重大故障,需要迅速解决
	2025.12.24 
			   1. 12.23的备忘录1的错误定位成功
			   2. 12.23的备忘录1的错误已解决,问题在于忘记初始化
			   3.设计增援时,应该遵循多不补,少则补的思路,防止玩家的非模板自定义单位被模板覆盖
			   4.完成了正常游戏时命令栏的绘制
			   5.天气系统添加了模式绘制和实际阻隔移动的效果
			   6.添加了交通模式的绘制,将宽度影响移动的BUG修复了,改成使用本来就有的交通属性
			   备忘录:
				     1.需要完善城市绘制和游戏流程
					 2.马上就要完工啦,哈哈哈
	2025.12.25 
			   1.完善了命令栏绘制
			   2.正在开始运行时脚本解释器的编写工作
			   3.添加了下一回合按钮
			   3.完善了下一回合按钮的逻辑,写完了运行时脚本的解释器
			   备忘录: 
					  1.需要测试运行时脚本解释器
	2025.12.26 
			   1.正在测试运行时脚本解释器
			   2.运行时脚本解释器的return 3;放在了循环里面,导致每次都只能解析一个命令,已解决这个错误
			   3.运行时脚本解释器运行良好,特别是If
			   4.解决了模板函数无法增援的问题,原因是单位类型写错(单位类型错误编译器怎么不警告？)
			   5.回合流程可以先搁置一下, 现在可以写游戏教程这种善后工作了
			   6.游戏教程正在编写中
			   7.完成了历史游戏中逻辑脚本的读取
			   8.正在逐步实现单位类型中的计划值,准备值,补给的增加减少
			   9.完善了计划值和准备值以及补给的减少,计划值的增加(临时)
			   备忘录:
				     1.需要把移动系统和战斗系统结合起来
					 2.需要将战斗系统彻底完工
					 3.需要补给系统开始编写工作
	2025.12.27 
			   1.测试出了模板无法读取宽度的问题,原因在于TRUE参数忘记改成FALSE
			   2.只差补给系统的测试,游戏说明编写和回合轮次的彻底完工了
			   3.补给系统已完工!
			   4.准备战斗系统的完善工作
			   5.准备回合轮次的编写工作
			   6.解决了日期显示的问题
			   7.战斗系统已完工!
			   8.只差回合轮次系统
			   9.唉,不想写轮次系统了,反正也只是我一个人玩,我宣布完工

	//----------------------------------------------------------------------------------------//
	项目:MHAE Moon Hexagon Attack Engine
	完工于2025-12-27-23:30
	项目已收尾,我封笔了
	整个项目的含AI量小于2%，我使用AI仅用来检查代码是否写快了造成的错误和检查逻辑上设计和内心所想的错误
	比如
	return (gameattribute->player_1_sid == sid && player1_vp >= vp) * 5 + (gameattribute->player_2_sid == sid && player2_vp >= vp) * 6;
	写成了
	return (gameattribute->player_1_sid == sid && player1_vp >= vp) * 5 + (gameattribute->player_2_sid == sid && player1_vp >= vp) * 6;
*/

extern PROJECTMODULE(GameDrawing);				//主绘制模块
extern PROJECTMODULE(GameLogic);				//主逻辑模块
extern PROJECTMODULE(LogicNULL);				//空逻辑模块
extern PROJECTSETTING(ResourceLoad);			//加载资源模块
extern PROJECTSETTING(AllSetting);				//设置模块
extern PROJECTSETTING(GameLogicSetting);		//主逻辑模块开始前设置
extern PROJECTMODULE(GameDrawingSetting);		//主绘制模块开始前设置
extern PROJECTSETTING(OverSetting);				//结束设置
extern PROJECTMODULE(HexagonMapDrawing);		//绘制地图
extern PROJECTMODULE(GameStart);				//开始界面

//----------------------------------------------Editor

extern PROJECTMODULE(EditorLogic);				//编辑器逻辑模块
extern PROJECTMODULE(EditorLogicSetting);		//编辑器绘制模块开始前设置
extern PROJECTMODULE(EditorDrawing);			//绘制编辑器命令栏
extern PROJECTMODULE(EditorDrawingSetting);		//绘制编辑器设置

//---------------------------------------------GameSetting

extern void ScriptConfigGame(PROJECTGOD* project, int mode);	//加载脚本
extern PROJECTSETTING(HexagonMapInit);			//初始化地图
extern PROJECTMODULE(MapCommandDraw);			//根据命令绘制地图
extern PROJECTSETTING(ReturnGameStart);			//返回主页面
extern PROJECTSETTING(SaveMap);					//保存地图脚本

//----------------------------------------------GameUtility

extern PROJECTMODULE(WeatherInit);				//初始化/重设天气
extern void DrawButton(MOONBUTTON* button, LPCWSTR text, int mode);
extern void GetHexagonMouseCoord();				//获取六边形坐标
extern PROJECTMODULE(CommandBarLogic);			//命令栏
extern void UnitInitTemplate(PROJECTGOD* project, UNIT* unit);								//单位初始模板
extern void TerrainWidthTemplate(PROJECTGOD* project, HEXAGONMAP* hexagonmap);				//地形宽度初始模板

//---------------------------------------------Unit

extern int UnitCombat(HEXAGONMAP* hexagon_attack, HEXAGONMAP* hexagon_defender);		//战斗函数
extern int UnitMove(HEXAGONMAP* unit_from);												//移动函数
extern int UnitMoveSelect(HEXAGONMAP* unit_to, HEXAGONMAP* unit_from);					//选择移动方向
extern int UnitMoveAll();																//统一移动
extern void UnitSupplyAll(int x, int y, int sid);										//补给
extern void UnitSupplyHq(int x, int y, int sid, int depth);								//指挥部补给
extern void UnitSupply(int sid);														//单位补给

extern void DrawUnit(IMAGE* image, HEXAGONMAP hexagonmap, int unittype, int hexX, int hexY, int R, int color, int mode);		//绘制单位

extern void HexagonFull(IMAGE* image, int R, int r, int X, int Y, COLORREF color, int linewidth, int linecolor, int width, int height);	//绘制填充六角格
extern void Hexagon(IMAGE* image, int R, int X, int Y, int width, COLORREF color);														//绘制六角格
extern POINT HexagonTurningPoint(int pointx, int pointy, int R);								//六边形坐标转换成实际坐标
extern POINT HexagonTransitionPoint(int transitionpointx, int transitionpointy, int R);			//实际坐标转换成六边形坐标