#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "API.h"
#include "constants.h"
#include <stack>
#include <queue>
#include <algorithm>

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数

class Point
{
public:
	Point(int x_ = 0, int y_ = 0) :x(x_), y(y_) {}
	Point ToAcc() { return Point(x * 1000 + 500, y * 1000 + 500); }
	Point ToNormal() { return Point(x / 1000, y / 1000); }
	int x;
	int y;
};

class Node :public Point
{
public:
	Node(int x_ = 0, int y_ = 0, int px_ = 0, int py_ = 0,
		float fc_ = FLT_MAX, float gc_ = FLT_MAX, float hc_ = FLT_MAX)
		:Point(x_, y_), parentX(px_), parentY(py_), fCost(fc_), gCost(gc_), hCost(hc_) {};
	Node(Point p, int px_ = 0, int py_ = 0,
		float fc_ = FLT_MAX, float gc_ = FLT_MAX, float hc_ = FLT_MAX)
		:Point(p), parentX(px_), parentY(py_), fCost(fc_), gCost(gc_), hCost(hc_) {};
	int parentX;
	int parentY;
	float fCost;
	float gCost;
	float hCost;
};

static bool HasInitMap;
static unsigned char Map[50][50];
static unsigned char Access[50][50];
static std::vector<Point> Classroom;
static std::vector<Point> Gate;
static std::vector<Point> OpenGate;
static std::vector<Point> Chest;

void InitMap(IStudentAPI& api)
{
	int i, j;
	for (i = 0; i < 50; i++)
	{
		for (j = 0; j < 50; j++)
		{
			Map[i][j] = (unsigned char)api.GetPlaceType(i, j);
			switch (Map[i][j])
			{
			case 2U:	// Wall
			case 6U:	// HiddenGate

				Access[i][j] = 0U;
				break;
			case 3U:	// Grass
				Access[i][j] = 3U;
				break;
			case 7U:	// Window
				Access[i][j] = 1U;
				break;
			case 8U:	// Door3
			case 9U:	// Door5
			case 10U:	// Door6
				Access[i][j] = 2U;
				break;
			case 4U:	// ClassRoom
				Access[i][j] = 0U;
				Classroom.emplace_back(Point(i, j));
				break;
			case 5U:	// Gate
				Access[i][j] = 0U;
				Gate.emplace_back(Point(i, j));
				break;
			case 11U:	// Chest
				Access[i][j] = 0U;
				Chest.emplace_back(Point(i, j));
				break;
			default:
				Access[i][j] = 2U;
				break;
			}
		}
	}
}

class AStar
{
public:
	bool IsValidWithoutWindows(int x, int y) { return (bool)(Access[x][y] / 2); }
	bool IsValidWithWindows(int x, int y) { return (bool)Access[x][y]; }
	static bool IsDestination(int x, int y, Node dest)
	{
		if (x == dest.x && y == dest.y) return true; else return false;
	}
	double CalculateH(int x, int y, Node dest)
	{
		double H = (sqrt((x - dest.x) * (x - dest.x) + (y - dest.y) * (y - dest.y)));
		return H;
	}
	std::vector<Node> MakePath(std::array<std::array<Node, 50>, 50> map, Node dest)
	{
		try
		{
			int x = dest.x;
			int y = dest.y;
			std::stack<Node> Path;
			std::vector<Node> UsablePath;
			while (!(map[x][y].parentX == x && map[x][y].parentY == y)
				&& map[x][y].x != -1 && map[x][y].y != -1)
			{
				Path.push(map[x][y]);
				int tempX = map[x][y].parentX;
				int tempY = map[x][y].parentY;
				x = tempX;
				y = tempY;

			}
			Path.push(map[x][y]);
			while (!Path.empty()) {
				Node top = Path.top();
				Path.pop();
				UsablePath.emplace_back(top);
			}
			return UsablePath;
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
	std::vector<Node> AStarWithoutWindows(Node src, Node dest)
	{
		std::vector<Node> empty;
		if (IsValidWithoutWindows(dest.x, dest.y) == false)
		{
			return empty;
		}
		if (IsDestination(src.x, src.y, dest))
		{
			return empty;
		}
		bool ClosedList[50][50];
		std::array<std::array < Node, 50>, 50> AStarMap;
		for (int x = 0; x < 50; x++) {
			for (int y = 0; y < 50; y++) {
				AStarMap[x][y].fCost = FLT_MAX;
				AStarMap[x][y].gCost = FLT_MAX;
				AStarMap[x][y].hCost = FLT_MAX;
				AStarMap[x][y].parentX = -1;
				AStarMap[x][y].parentY = -1;
				AStarMap[x][y].x = x;
				AStarMap[x][y].y = y;
				ClosedList[x][y] = false;
			}
		}
		int x = src.x;
		int y = src.y;
		AStarMap[x][y].fCost = 0.0;
		AStarMap[x][y].gCost = 0.0;
		AStarMap[x][y].hCost = 0.0;
		AStarMap[x][y].parentX = x;
		AStarMap[x][y].parentY = y;
		std::vector<Node> OpenList;
		OpenList.emplace_back(AStarMap[x][y]);
		bool FoundDest = false;
		while (!OpenList.empty() && OpenList.size() < 50 * 50) {
			Node node;
			do {
				float temp = FLT_MAX;
				std::vector<Node>::iterator itNode;
				for (std::vector<Node>::iterator it = OpenList.begin();
					it != OpenList.end(); it = next(it)) {
					Node n = *it;
					if (n.fCost < temp) {
						temp = n.fCost;
						itNode = it;
					}
				}
				node = *itNode;
				OpenList.erase(itNode);
			} while (IsValidWithoutWindows(node.x, node.y) == false);
			x = node.x;
			y = node.y;
			ClosedList[x][y] = true;
			for (int newX = -1; newX <= 1; newX++) {
				for (int newY = -1; newY <= 1; newY++) {
					if (newX != 0 && newY != 0) continue;
					double gNew, hNew, fNew;
					if (IsValidWithoutWindows(x + newX, y + newY)) {
						if (IsDestination(x + newX, y + newY, dest))
						{
							AStarMap[x + newX][y + newY].parentX = x;
							AStarMap[x + newX][y + newY].parentY = y;
							FoundDest = true;
							return MakePath(AStarMap, dest);
						}
						else if (ClosedList[x + newX][y + newY] == false)
						{
							gNew = node.gCost + 1.0;
							hNew = CalculateH(x + newX, y + newY, dest);
							fNew = gNew + hNew;
							if (AStarMap[x + newX][y + newY].fCost == FLT_MAX ||
								AStarMap[x + newX][y + newY].fCost > fNew)
							{
								AStarMap[x + newX][y + newY].fCost = fNew;
								AStarMap[x + newX][y + newY].gCost = gNew;
								AStarMap[x + newX][y + newY].hCost = hNew;
								AStarMap[x + newX][y + newY].parentX = x;
								AStarMap[x + newX][y + newY].parentY = y;
								OpenList.emplace_back(AStarMap[x + newX][y + newY]);
							}
						}
					}
				}
			}
		}
		if (FoundDest == false) {
			return empty;
		}
	}
	std::vector<Node> AStarWithWindows(Node src, Node dest)
	{
		std::cout << src.x << src.y;
		std::vector<Node> empty;
		if (IsValidWithWindows(dest.x, dest.y) == false)
		{
			return empty;
		}
		if (IsDestination(src.x, src.y, dest))
		{
			return empty;
		}
		bool ClosedList[50][50];
		std::array<std::array < Node, 50>, 50> AStarMap;
		for (int x = 0; x < 50; x++) {
			for (int y = 0; y < 50; y++) {
				AStarMap[x][y].fCost = FLT_MAX;
				AStarMap[x][y].gCost = FLT_MAX;
				AStarMap[x][y].hCost = FLT_MAX;
				AStarMap[x][y].parentX = -1;
				AStarMap[x][y].parentY = -1;
				AStarMap[x][y].x = x;
				AStarMap[x][y].y = y;
				ClosedList[x][y] = false;
			}
		}
		int x = src.x;
		int y = src.y;
		AStarMap[x][y].fCost = 0.0;
		AStarMap[x][y].gCost = 0.0;
		AStarMap[x][y].hCost = 0.0;
		AStarMap[x][y].parentX = x;
		AStarMap[x][y].parentY = y;
		std::vector<Node> OpenList;
		OpenList.emplace_back(AStarMap[x][y]);
		bool FoundDest = false;
		while (!OpenList.empty() && OpenList.size() < 50 * 50) {
			Node node;
			do {
				float temp = FLT_MAX;
				std::vector<Node>::iterator itNode;
				for (std::vector<Node>::iterator it = OpenList.begin();
					it != OpenList.end(); it = next(it)) {
					Node n = *it;
					if (n.fCost < temp) {
						temp = n.fCost;
						itNode = it;
					}
				}
				node = *itNode;
				OpenList.erase(itNode);
			} while (IsValidWithWindows(node.x, node.y) == false);
			x = node.x;
			y = node.y;
			ClosedList[x][y] = true;
			for (int newX = -1; newX <= 1; newX++) {
				for (int newY = -1; newY <= 1; newY++) {
					if (newX != 0 && newY != 0) continue;
					double gNew, hNew, fNew;
					if (IsValidWithWindows(x + newX, y + newY)) {
						if (IsDestination(x + newX, y + newY, dest))
						{
							AStarMap[x + newX][y + newY].parentX = x;
							AStarMap[x + newX][y + newY].parentY = y;
							FoundDest = true;
							return MakePath(AStarMap, dest);
						}
						else if (ClosedList[x + newX][y + newY] == false)
						{
							gNew = node.gCost + 1.0;
							hNew = CalculateH(x + newX, y + newY, dest);
							fNew = gNew + hNew;
							if (AStarMap[x + newX][y + newY].fCost == FLT_MAX ||
								AStarMap[x + newX][y + newY].fCost > fNew)
							{
								AStarMap[x + newX][y + newY].fCost = fNew;
								AStarMap[x + newX][y + newY].gCost = gNew;
								AStarMap[x + newX][y + newY].hCost = hNew;
								AStarMap[x + newX][y + newY].parentX = x;
								AStarMap[x + newX][y + newY].parentY = y;
								OpenList.emplace_back(AStarMap[x + newX][y + newY]);
							}
						}
					}
				}
			}
		}
		if (FoundDest == false) {
			return empty;
		}
	}
}astar;

template<typename IFooAPI>
class Utilities
{
private:
	const IFooAPI& API;
	Point TEMP;
public:
	Utilities(IFooAPI api) : API(api) {}

	// void Update(Point Door, bool State);			//更新地图信息，比如门和隐藏校门，需要约定info的格式
	void UpdateClassroom();
	void UpdateGate();
	void UpdateChest();
	void UpdateDoor();
	void MoveTo(Point Dest, bool WithWindows);		// 往目的地动一动
	bool NearPoint(Point P, int level = 1);
	// level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
	void MoveToNearestClassroom(bool WithWindows);	// 往最近的作业的方向动一动
	bool NearClassroom();							// 已经在作业旁边了吗？
	void MoveToNearestGate(bool WithWindows);		// 往最近的校门旁边动一动
	bool NearGate();								// 已经在校门旁边了吗？
	void MoveToNearestChest(bool WithWindows);		// 往最近的箱子的方向动一动
	bool NearChest();								// 已经在箱子旁边了吗？
	int EstimateTime(Point Dest);					// 去目的地的预估时间
	void DirectLearning(bool WithWindows);			// 前往最近的作业并学习
	void DirectOpeningChest(bool WithWindows);		// 前往最近的箱子并开箱
};

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateClassroom()
{
	int Size = Classroom.size();
	for (int i = 0; i < Size; i++)
	{
		if (API.GetClassroomProgress(Classroom[i].x, Classroom[i].y) >= 10000000)
		{
			Map[Classroom[i].x][Classroom[i].y] = 2U;
			Classroom.erase(Classroom.begin() + i);
			i--; Size--;
		}
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateGate()
{
	int Size = Gate.size();
	for (int i = 0; i < Size; i++)
	{
		if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 10000000)
		{
			Map[Gate[i].x][Gate[i].y] = 12U;
			OpenGate.emplace_back(Point(Gate[i].x, Gate[i].y));
			Gate.erase(Gate.begin() + i);
			i--; Size--;
		}
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateChest()
{
	int Size = Chest.size();
	for (int i = 0; i < Size; i++)
	{
		if (API.GetChestProgress(Chest[i].x, Chest[i].y) >= 10000000)
		{
			Map[Chest[i].x][Chest[i].y] = 2U;
			Chest.erase(Chest.begin() + i);
			i--; Size--;
		}
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::MoveTo(Point Dest, bool WithWindows)
{
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	bool IsStuck = (sx == TEMP.x && sy == TEMP.y);
	std::vector<Node> UsablePath;
	if (WithWindows) UsablePath = astar.AStarWithWindows(Node(sx / 1000, sy / 1000), Dest);
	else UsablePath = astar.AStarWithoutWindows(Node(sx / 1000, sy / 1000), Dest);
	if (UsablePath.size() < 2) return;
	int tx, ty;
	if (UsablePath.size() >= 3
		&& astar.IsValidWithoutWindows(sx / 1000, sy / 1000)
		&& astar.IsValidWithoutWindows(UsablePath[1].x, UsablePath[1].y)
		&& astar.IsValidWithoutWindows(UsablePath[2].x, UsablePath[2].y)
		&& astar.IsValidWithoutWindows(sx / 1000, UsablePath[2].y)
		&& astar.IsValidWithoutWindows(UsablePath[2].x, sy / 1000))
	{
		tx = UsablePath[2].x * 1000 + 500;
		ty = UsablePath[2].y * 1000 + 500;
	}
	else
	{
		tx = UsablePath[1].x * 1000 + 500;
		ty = UsablePath[1].y * 1000 + 500;
	}
	int dx = tx - sx;
	int dy = ty - sy;
	if (Map[tx / 1000][ty / 1000] != 7U)
	{
		if (!IsStuck)
		{
			API.Move(1000 * sqrt(dx * dx + dy * dy) / API.GetSelfInfo()->speed, atan2(dy, dx));
		}
		else
		{
			time_t t;
			srand((unsigned)time(&t));
			API.Move(100 * sqrt(dx * dx + dy * dy) / API.GetSelfInfo()->speed,
				atan2(dy, dx) + rand());
		}
	}
	else
	{
		API.SkipWindow();
	}
	TEMP.x = sx; TEMP.y = sy;
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearPoint(Point P, int level)
{
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	switch (level)
	{
	case 0:
		return (P.x == Self.x && P.y == Self.y) ? true : false;
		break;
	case 1:
		return (abs(P.x - Self.x) + abs(P.y - Self.y) <= 1) ? true : false;
		break;
	case 2:
		return (abs(P.x - Self.x) <= 1 && abs(P.y - Self.y) <= 1) ? true : false;
		break;
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::MoveToNearestClassroom(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = 0;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	for (int i = 0; i < Classroom.size(); i++)
	{
		if (!NearClassroom())
		{
			Access[Classroom[i].x][Classroom[i].y] = 2U;
		}
		else
		{
			Access[Classroom[i].x][Classroom[i].y] = 0U;
		}
	}
	for (int i = 0; i < Classroom.size(); i++)
	{
		Distance = astar.AStarWithWindows(Self, Classroom[i]).size();
		if (Distance < minDistance)
		{
			minDistance = Distance;
			minNum = i;
		}
	}
	MoveTo(Classroom[minNum], WithWindows);
	for (int i = 0; i < Classroom.size(); i++)
	{
		Access[Classroom[i].x][Classroom[i].y] = 0U;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearClassroom()
{
	for (int i = 0; i < Classroom.size(); i++)
	{
		if (NearPoint(Classroom[i], 2)) return true;
	}
	return false;
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::MoveToNearestGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = 0;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	for (int i = 0; i < Gate.size(); i++)
	{
		if (!NearGate())
		{
			Access[Gate[i].x][Gate[i].y] = 2U;
		}
		else
		{
			Access[Gate[i].x][Gate[i].y] = 0U;
		}
	}
	for (int i = 0; i < Gate.size(); i++)
	{
		Distance = astar.AStarWithWindows(Self, Gate[i]).size();
		if (Distance < minDistance)
		{
			minDistance = Distance;
			minNum = i;
		}
	}
	MoveTo(Gate[minNum], WithWindows);
	for (int i = 0; i < Gate.size(); i++)
	{
		Access[Gate[i].x][Gate[i].y] = 0U;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearPoint(Gate[i], 2)) return true;
	}
	return false;
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::MoveToNearestChest(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = 0;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	for (int i = 0; i < Chest.size(); i++)
	{
		if (!NearChest())
		{
			Access[Chest[i].x][Chest[i].y] = 2U;
		}
		else
		{
			Access[Chest[i].x][Chest[i].y] = 0U;
		}
	}
	for (int i = 0; i < Chest.size(); i++)
	{
		Distance = astar.AStarWithWindows(Self, Chest[i]).size();
		if (Distance < minDistance)
		{
			minDistance = Distance;
			minNum = i;
		}
	}
	MoveTo(Chest[minNum], WithWindows);
	for (int i = 0; i < Chest.size(); i++)
	{
		Access[Chest[i].x][Chest[i].y] = 0U;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearChest()
{
	for (int i = 0; i < Chest.size(); i++)
	{
		if (NearPoint(Chest[i], 2)) return true;
	}
	return false;
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::EstimateTime(Point Dest)
{
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	int Distance = astar.AStarWithWindows(Self, Dest).size();
	int Speed = API.GetSelfInfo()->speed;
	int Time = Distance * 1000 / Speed;
	return Time;
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectLearning(bool WithWindows)
{
	UpdateClassroom();
	if (!NearClassroom())
	{
		MoveToNearestClassroom(true);
	}
	else
	{
		API.StartLearning();
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectOpeningChest(bool WithWindows)
{
	UpdateChest();
	if (!NearChest())
	{
		MoveToNearestChest(true);
	}
	else
	{
		API.StartOpenChest();
	}
}

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

class Encoder
{
private:
	static const int MaxLength = 255;
	char msg[MaxLength];
	int Pointer;
public:
	Encoder();
	void SetHeader(char header);
	template<typename T>
	void PushInfo(T info);
	std::string ToString();
};
Encoder::Encoder() :Pointer(0)
{
	memset(msg, 0, sizeof(msg));
}
template<typename T>
void Encoder::PushInfo(T info)
{
	memcpy(msg, &info, sizeof(T));
	Pointer += sizeof(T);
}
std::string Encoder::ToString()
{
	return std::string(msg, msg + MaxLength);
}

class Decoder
{
private:
	std::string msg;
	int Pointer;
public:
	Decoder(std::string code);
	template<typename T>
	T ReadInfo();
};
Decoder::Decoder(std::string code) :msg(code), Pointer(0) {}
template<typename T>
T Decoder::ReadInfo()
{
	T* ptr = (*T)(msg.c_str() + Pointer);
	Pointer += sizeof(T);
	return *T;
}

/* 人物状态
CantMove 动弹不得（沉迷/前后摇/毕业……）
Default 默认
DoClassroom 去写作业/在写作业
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
#define DoClassroom 0x11
#define OpenGate 0x12
#define OpenChest 0x13
#define Danger 0x14
#define Rousing 0x15
#define Encouraging 0x16
#define Picking 0x17

void AI::play(IStudentAPI& api)
{

	static Utilities<IStudentAPI&> Helper(api);

	// 公共操作
	if (!HasInitMap)
	{
		InitMap(api);
		HasInitMap = true;
	}
	if (this->playerID == 0)
	{
		Helper.DirectOpeningChest(true);

		// 玩家0执行操作
	}
	else if (this->playerID == 1)
	{
		// 玩家1执行操作
	}
	else if (this->playerID == 2)
	{
		// 玩家2执行操作
	}
	else if (this->playerID == 3)
	{
		// 玩家3执行操作
	}
	//当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
	// 公共操作
}

void AI::play(ITrickerAPI& api)
{
	auto self = api.GetSelfInfo();
	api.PrintSelfInfo();
}
