#include"AI.h"
#include"API.h"

class Point
{
public:
	Point(int x_ = 0, int y_ = 0) :x(x_), y(y_) {}
	Point ToAcc() { return Point(x * 1000 + 500, y * 1000 + 500); }
	Point ToNormal() { return Point(x / 1000, y / 1000); }
	int x;
	int y;
};

template<class IFooAPI>
class Utilities
{
private:
	IFooAPI API;
public:
	Utilities(IFooAPI api) : API(api) {}

	// void Update(std::string info); 更新地图信息，比如门和隐藏校门，需要约定info的格式
	void MoveTo(Point Dest); // 往目的地动一动
	// void MoveToAccurate(Point Dest);
	bool NearPoint(Point P, int level = 1); // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
	void MoveToNearestGate(); // 往最近的校门旁边动一动
	bool NearGate(); // 已经在校门旁边了吗？
	void MoveToNearestHomework(); // 往最近的作业的方向动一动
	bool NearHomework(); // 已经在作业旁边了吗？
	int EstimateTime(Point Dest); // 去目的地的预估时间
};

/* 信息协议
信息头（info[0]）
MapUpdate 地图更新

TrickerInfo 捣蛋鬼信息

NeedHelp 请求支援
	info[1-2]：坐标
WantTool 请求获取道具（可能需要反馈？不然太远了跑去不划算）
*/
#define MapUpdate 0x01
#define TrickerInfo 0x02
#define NeedHelp 0x03
#define WantTool 0x04

/* 人物状态
CantMove 动弹不得（沉迷/前后摇/毕业……）
Default 默认
DoHomework 去写作业/在写作业
OpenGate 去开校门/在开校门
OpenChest 去开箱子/在开箱子
Danger 应对危险
Rousing 唤醒某人
Encouraging 勉励某人
Picking 去捡道具
以上是所有角色普遍拥有的状态，其他状态需要自行定义，建议从0x80开始
比如运动员可以定义一个状态叫正面硬刚，老师可以定义一个状态叫巡逻（试图在视野内跟着捣蛋鬼），等等
基本逻辑就是每次AI::Play都会根据信息来决定维持当前状态还是跳到另一个状态（类似于图灵机模拟思维）

需要一些空间储存必要的信息，可能会用到通信

或许可以给各种状态分个类或者优先级，方便写代码
*/
#define CantMove 0x00
#define Default 0x10
#define DoHomework 0x11
#define OpenGate 0x12
#define OpenChest 0x13
#define Danger 0x14
#define Rousing 0x15
#define Encouraging 0x16
#define Picking 0x17