#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "API.h"
#include "AI.h"
#include "Pigeon.h"
#include<stack>

class Point
{
public:
	Point(int x_ = 0, int y_ = 0) :x(x_), y(y_) {}
	Point ToAcc() { return Point(x * 1000 + 500, y * 1000 + 500); }
	Point ToNormal() { return Point(x / 1000, y / 1000); }
	int x;
	int y;
};
class Doors :public Point
{
public:
	Doors(int x_ = 0, int y_ = 0, bool ds_ = true, THUAI6::PlaceType dt_ = THUAI6::PlaceType::Door3)
		:Point(x_, y_), DoorStatus(ds_), DoorType(dt_) {};
	Doors(Point p_, bool ds_ = true, THUAI6::PlaceType dt_ = THUAI6::PlaceType::Door3)
		:Point(p_), DoorStatus(ds_), DoorType(dt_) {};
	bool DoorStatus;
	THUAI6::PlaceType DoorType;
};
class Node :public Point
{
public:
	Node(int x_ = 0, int y_ = 0, int px_ = -1, int py_ = -1,
		float fc_ = FLT_MAX, float gc_ = FLT_MAX, float hc_ = FLT_MAX)
		:Point(x_, y_), parentX(px_), parentY(py_), fCost(fc_), gCost(gc_), hCost(hc_) {};
	Node(Point p_, int px_ = -1, int py_ = -1,
		float fc_ = FLT_MAX, float gc_ = FLT_MAX, float hc_ = FLT_MAX)
		:Point(p_), parentX(px_), parentY(py_), fCost(fc_), gCost(gc_), hCost(hc_) {};
	int parentX;
	int parentY;
	float fCost;
	float gCost;
	float hCost;
};

// accurate position
class GeometryPoint
{
public:
	double PointX, PointY;
	GeometryPoint() : PointX(0), PointY(0) {}
	GeometryPoint(double PX, double PY) : PointX(PX), PointY(PY) {}
	GeometryPoint(const GeometryPoint& P_) : PointX(P_.PointX), PointY(P_.PointY) {}
};

double Distance(GeometryPoint A, GeometryPoint B)
{
	return sqrt((A.PointX - B.PointX) * (A.PointX - B.PointX) + (A.PointY - B.PointY) * (A.PointY - B.PointY));
}

// with direction S->T(can be treated as a vector), right side of the segment represents inside
class GeometrySegment
{
public:
	GeometryPoint S, T;
	GeometrySegment(const GeometryPoint& PS, const GeometryPoint& PT) : S(PS), T(PT) {}
	double GetTheta(GeometryPoint P);
};

GeometryPoint Project(GeometrySegment S, GeometryPoint P)
{
	double lambda = ((P.PointX - S.S.PointX) * (S.T.PointX - S.S.PointX) + (P.PointY - S.S.PointY) * (S.T.PointY - S.S.PointY) ) / pow(Distance(S.S, S.T), 2);
//	std::cerr << lambda << std::endl;
	if (lambda < 0 || lambda > 1) return S.S; // simple
	else return GeometryPoint(S.S.PointX + lambda * (S.T.PointX - S.S.PointX), S.S.PointY + lambda * (S.T.PointY - S.S.PointY));
}

double GeometrySegment::GetTheta(GeometryPoint P)
{
	double CrossDot = (S.PointX - P.PointX) * (T.PointY - P.PointY) - (S.PointY - P.PointY) * (T.PointX - P.PointX);
	double InnerDot = (S.PointX - P.PointX) * (T.PointX - P.PointX) + (S.PointY - P.PointY) * (T.PointY - P.PointY);
	double theta = acos(InnerDot / Distance(S, P) / Distance(T, P));
	if (fabs(CrossDot) < 1e-4)
	{
		std::cerr << "[Common Line Warning!]" << std::endl;
		return 0;
	}
	return CrossDot > 0 ? theta : -theta;
}

bool Intersect(GeometrySegment A, GeometrySegment B)
{
//	std::cerr << "CheckIntersect" << std::endl;
//	std::cerr << '[' << A.S.PointX / 1000 << ' ' << A.S.PointY / 1000 << ']' << std::endl;
//	std::cerr << '[' << A.T.PointX / 1000 << ' ' << A.T.PointY / 1000 << ']' << std::endl;
//	std::cerr << '[' << B.S.PointX / 1000 << ' ' << B.S.PointY / 1000 << ']' << std::endl;
//	std::cerr << '[' << B.T.PointX / 1000 << ' ' << B.T.PointY / 1000 << ']' << std::endl;
	double CDAS = (B.S.PointX - A.S.PointX) * (B.T.PointY - A.S.PointY) - (B.S.PointY - A.S.PointY) * (B.T.PointX - A.S.PointX);
	double CDAT = (B.S.PointX - A.T.PointX) * (B.T.PointY - A.T.PointY) - (B.S.PointY - A.T.PointY) * (B.T.PointX - A.T.PointX);
	double CDBS = (A.S.PointX - B.S.PointX) * (A.T.PointY - B.S.PointY) - (A.S.PointY - B.S.PointY) * (A.T.PointX - B.S.PointX);
	double CDBT = (A.S.PointX - B.T.PointX) * (A.T.PointY - B.T.PointY) - (A.S.PointY - B.T.PointY) * (A.T.PointX - B.T.PointX);
	if (fabs(CDAS) < 1e-4 || fabs(CDAT) < 1e-4 || fabs(CDBS) < 1e-4 || fabs(CDBT) < 1e-4) return false;
	if (CDAS * CDAT < 0 && CDBS * CDBT < 0) return true;
	return false;
}

template<typename IFooAPI>
class AStarPlus
{
private:
	std::vector<GeometrySegment> StableMap;
	std::vector<GeometryPoint> StableCheckPoint;
	std::vector<GeometrySegment> VariableMap;
	std::vector<GeometryPoint> VariableCheckPoint;
	IFooAPI API;

	bool IsAccessible(THUAI6::PlaceType pt);
	bool DirectReachable(GeometryPoint A, GeometryPoint B);
	void BackwardExpand(Point Source, int H[50][50]);
	GeometryPoint Escape(GeometryPoint P);

	void InitStableMap();
	const double Radius;
	const double PI;
	const double Compensate;

public:
	AStarPlus(IFooAPI api_);
	std::vector<GeometryPoint> FindPath(GeometryPoint From_, GeometryPoint Dest_);
	bool InsideObstacle(GeometryPoint P);
	void ResetVariableMap();
	void AddPlayer();
	void AddWindow();
	void AddLockedDoor();
};

template<typename IFooAPI>
class Utilities
{
protected:
	int ProgressMem[50][50];
	int LastUpdateFrame[50][50];
	int LastAutoUpdateFrame;
	AStarPlus<IFooAPI> AStarHelper;

public:
	unsigned char Map[50][50];
	unsigned char Access[50][50];
	std::vector<Point> Classroom;
	std::vector<Point> Gate;
	std::vector<Point> HiddenGate;
	std::vector<Point> Chest;
	std::vector<Doors> Door;

	bool IsValidWithoutWindows(int x, int y);
	bool IsValidWithWindows(int x, int y);
	bool IsDestination(int x, int y, Node dest);
	double CalculateH(int x, int y, Node dest);
	std::vector<Node> MakePath(std::array<std::array<Node, 50>, 50> map, Node dest);
	std::vector<Node> AStarWithoutWindows(Node src, Node dest);
	std::vector<Node> AStarWithWindows(Node src, Node dest);

	const IFooAPI& API;
	Point TEMP;
	std::vector<THUAI6::PropType> Inventory;
	static std::vector<unsigned char> PickPropPriority;
	static std::vector<unsigned char> UsePropPriority;
	const int UpdateInterval = 1;

	void InitMap(IFooAPI& api);

public:
	Utilities(IFooAPI api);

	void Update(MapUpdateInfo upinfo, int t_);			//更新地图信息，比如门和隐藏校门，需要约定info的格式
	//void UpdateClassroom();
	//void UpdateGate();
	//void UpdateChest();
	//void UpdateDoor();
	std::vector<THUAI6::PropType> GetInventory() { return Inventory; }	// 查看背包
	void OrganizeInventory(std::vector<unsigned char>Priority);			// 整理背包

	bool MoveToAccurate(Point Dest, bool WithWindows = true);
	bool MoveTo(Point Dest, bool WithWindows);		// 往目的地动一动
	bool MoveToNearestClassroom(bool WithWindows);	// 往最近的作业的方向动一动
	bool MoveToNearestGate(bool WithWindows);		// 往最近的关闭的校门旁边动一动
	bool MoveToNearestOpenGate(bool WithWindows);	// 往最近的开启的校门旁边动一动
	bool MoveToNearestChest(bool WithWindows);		// 往最近的箱子的方向动一动
	bool NearPoint(Point P, int level = 1);         // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
	bool NearClassroom(bool checkProgress);							// 已经在作业旁边了吗？
	bool NearGate();								// 已经在关闭的校门旁边了吗？
	bool NearOpenGate();							// 已经在开启的校门旁边了吗？
	bool NearChest();								// 已经在箱子旁边了吗？
	void DirectLearning(bool WithWindows);			// 前往最近的作业并学习
	void DirectOpeningChest(bool WithWindows);		// 前往最近的箱子并开箱
	void DirectOpeningGate(bool WithWindows, bool CanDirectGraduate);		// 前往最近的关闭的校门并开门
	void DirectGraduate(bool WithWindows);			// 前往最近的开启的校门并毕业
	void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);		// 前往已知价值最高的道具并捡道具
	void DirectUseProp(std::vector<unsigned char>Priority);



	int EstimateTime(Point Dest);					// 去目的地的预估时间
	bool IsViewable(Point Src, Point Dest, int ViewRange);			// 判断两个位置是否可视
	int CountFinishedClassroom() const;
	int CountNonemptyChest() const; // TODO: 暂未实现
	int CountHiddenGate() const;
	int CountClosedGate() const;
	int CountOpenGate() const;

	int GetChestProgress(int cellx, int celly);
	int GetGateProgress(int cellx, int celly);
	int GetClassroomProgress(int cellx, int celly);
	int GetDoorProgress(int cellx, int celly);
	int GetChestProgress(Point cell) const;
	int GetGateProgress(Point cell) const;
	int GetClassroomProgress(Point cell) const;
	int GetDoorProgress(Point cell) const;
};

class UtilitiesStudent : public Utilities<IStudentAPI&>
{
private:
	Pigeon& gugu;
public:
	UtilitiesStudent(IStudentAPI& api, Pigeon& gugu_);
	void AtheleteCanBeginToCharge();
	void TeacherPunish();
	void StraightAStudentWriteAnswers();
	void SunshineRouse();
	void SunshineEncourage();
	void SunshineInspire();
	double AtheleteCanBeginToChargeCD();
	double TeacherPunishCD();
	double StraightAStudentWriteAnswersCD();
	double SunshineRouseCD();
	double SunshineEncourageCD();
	double SunshineInspireCD();
	void AutoUpdate();
};

class UtilitiesTricker : public Utilities<ITrickerAPI&>
{
public:
	UtilitiesTricker(ITrickerAPI& api);
	void AutoUpdate();
	void AssassinDefaultAttack(int stux, int stuy);	// 刺客普通攻击，传入学生坐标(stux,stuy)
	bool AssassinDefaultAttackOver(int rank);//判断能否稳定命中，传入目前能观察到的学生列表的第几个，从0开始计数
	void AssassinBecomeInvisible();
	void AssassinFlyingKnife(int stux, int stuy);
	double AssassinFlyingKnifeCD();
};

UtilitiesStudent::UtilitiesStudent(IStudentAPI& api, Pigeon& gugu_) : Utilities<IStudentAPI&>(api), gugu(gugu_) { }
UtilitiesTricker::UtilitiesTricker(ITrickerAPI& api) : Utilities<ITrickerAPI&>(api) { }

#include "UtilitiesBasic.hpp"
#include "UtilitiesAttack.hpp"
#include "UtilitiesAStar.hpp"

#endif
