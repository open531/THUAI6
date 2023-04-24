#include <array>
#include <cmath>
#include <queue>
#include <stack>
#include <chrono>
#include <format>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <algorithm>
#include <functional>
#include "AI.h"
#include "API.h"
#include "constants.h"
#include "Utilities.hpp"
#include "Pigeon.h"

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数

/* 人物状态
sCantMove 动弹不得（沉迷/前后摇/毕业……）
sDefault 默认
sDoClassroom 去写作业/在写作业
sOpenGate 去开校门/在开校门
sOpenChest 去开箱子/在开箱子
sDanger 应对危险
sRousing 唤醒某人
sEncouraging 勉励某人
sPicking 去捡道具
以上是所有角色普遍拥有的状态，其他状态需要自行定义，建议从0x80开始
比如运动员可以定义一个状态叫正面硬刚，老师可以定义一个状态叫巡逻（试图在视野内跟着捣蛋鬼），等等
基本逻辑就是每次AI::Play都会根据信息来决定维持当前状态还是跳到另一个状态（类似于图灵机模拟思维）
需要一些空间储存必要的信息，可能会用到通信
或许可以给各种状态分个类或者优先级，方便写代码
*/
#define sCantMove 0x00
#define sDefault 0x10

#define sDoClassroom 0x11
#define sOpenGate 0x12
#define sOpenChest 0x13
#define sDanger 0x14
#define sRousing 0x15
#define sEncouraging 0x16
#define sPicking 0x17

#define sFindStudent 0x20
#define sAttackStudent 0x21


void AI::play(IStudentAPI& api)
{
	api.PrintSelfInfo();
	static std::vector<unsigned char> Priority = { 0,1,1,1,5,2,9,7,6 };
	static Pigeon gugu(api);
	static UtilitiesStudent Helper(api, gugu);
	static int CurrentState = sDefault;

	Helper.AutoUpdate();
	int MessageType;
	while ((MessageType = gugu.receiveMessage()) != NoMessage)
	{
		std::cerr << "MessageType = " << MessageType << std::endl;
		if (MessageType == MapUpdate)
		{
			auto ms = gugu.receiveMapUpdate();
			std::cerr << "[custom]" << ms.second.x << ' ' << ms.second.y << std::endl;
			api.Print(std::to_string(ms.second.x) + " " + std::to_string(ms.second.y));
			Helper.Update(ms.second, ms.first);
		}
	}
	std::cerr << "[FinishedClassroom]" << Helper.CountFinishedClassroom() << std::endl;
	std::cerr << "[OpenGate]" << Helper.CountOpenGate() << std::endl;
	for (int i = 0; i < Helper.Gate.size(); i++)
	{
		std::cerr << "[Gate" << i << "]" << api.GetGateProgress(Helper.Gate[i].x, Helper.Gate[i].y) << std::endl;
	}
	if (Helper.CountFinishedClassroom() >= 7)
	{
		if (!Helper.CountOpenGate())
		{
<<<<<<< HEAD
=======
//			api.EndAllAction();
>>>>>>> ceb677d709a11f31e97f4df4b2195d189f4df231
			Helper.DirectOpeningGate(true, true);
		}
		else
		{
<<<<<<< HEAD
=======
//			api.EndAllAction();
>>>>>>> ceb677d709a11f31e97f4df4b2195d189f4df231
			Helper.DirectGraduate(true);
		}
	}
	else
	{
<<<<<<< HEAD
=======
//			api.EndAllAction();
>>>>>>> ceb677d709a11f31e97f4df4b2195d189f4df231
		Helper.DirectLearning(true);
	}


	// 公共操作
	if (this->playerID == 0)
	{
		//Helper.AutoUpdate();
		//if (Helper.CountFinishedClassroom() > 7)
		//{
		//	if (Helper.CountOpenGate() < 1) Helper.DirectOpeningGate(true);
		//	else Helper.DirectGraduate(true);
		//}
		//else Helper.DirectLearning(true);

		// 玩家0执行操作
	}
	else if (this->playerID == 1)
	{
		//Helper.DirectLearning(1);
		//return;
		//Helper.AutoUpdate();
		//while (gugu.receiveMessage()); // 收信息

		//std::shared_ptr<const THUAI6::Student> selfinfo = api.GetSelfInfo();
		//std::vector<std::shared_ptr<const THUAI6::Tricker>> tinfo = api.GetTrickers();
		//if (CurrentState != sDanger)
		//{
		//	if (!tinfo.empty())
		//	{
		//		api.EndAllAction();
		//		gugu.sendTrickerInfo(0, tinfo);
		//		gugu.sendTrickerInfo(2, tinfo);
		//		gugu.sendTrickerInfo(3, tinfo);
		//		CurrentState = sDanger;
		//	}
		//}
		//if (CurrentState == sDefault)
		//{
		//	if (Helper.CountFinishedClassroom() <= 7) CurrentState = sDoClassroom;
		//	else CurrentState = sOpenGate;
		//}
		//if (CurrentState == sDoClassroom)
		//{
		//	Helper.DirectLearning(1);
		//	if (Helper.CountFinishedClassroom() > 7) CurrentState = sDefault;
		//	// 学完了就走
		//}
		//else if (CurrentState == sOpenGate)
		//{
		//	if (!Helper.NearGate()) Helper.MoveToNearestGate(1);
		//	else api.StartOpenGate();
		//}
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

	static std::vector<unsigned char> Priority = { 0,1,2,3,4,5,6,7,8 };
	static UtilitiesTricker Helper(api);
	static int CurrentState = sDefault;

	//	Helper.AutoUpdate();

	if (CurrentState == sDefault)
	{
		CurrentState = sFindStudent;
	}
	auto stuinfo = api.GetStudents();

	switch (CurrentState)
	{
<<<<<<< HEAD
	case sDefault:
		break;
	case sFindStudentAndAttack:
		auto stuinfo = api.GetStudents();
		std::cerr << "See student " << stuinfo.size();
		if (stuinfo.size() != 0)
		{
			if (abs(self->x - stuinfo[0]->x) + abs(self->y - stuinfo[0]->y) < 1000) api.Attack(atan2(self->y - stuinfo[0]->y, self->x - stuinfo[0]->x));
			else
			{
				std::cerr << "[MoveTo]" << stuinfo[0]->x / 1000 << ' ' << stuinfo[0]->y / 1000;
				Helper.MoveTo(stuinfo[0]->x / 1000, stuinfo[0]->y / 1000);
			}
		}
		else Helper.MoveToNearestClassroom(true);
		break;
=======
		case sDefault:
			if (stuinfo.size() > 0) CurrentState = sAttackStudent;
			else CurrentState = sFindStudent;
			break;
		case sFindStudent:
			if (stuinfo.size() > 0) CurrentState = sAttackStudent;
			break;
		case sAttackStudent:
			if (stuinfo.size() == 0) CurrentState = sFindStudent;
			break;
	}


	switch (CurrentState)
	{
		case sDefault:
			std::cerr << "CurrentState: sDefault" << std::endl;
			break;
		case sFindStudent:
			std::cerr << "CurrentState: sFindStudent" << std::endl;
			Helper.MoveToNearestClassroom(true);
			break;
		case sAttackStudent:
			std::cerr << "CurrentState: sAttackStudent" << std::endl;
			std::cerr << "See student " << stuinfo.size();
			if (abs(self->x - stuinfo[0]->x) + abs(self->y - stuinfo[0]->y) < 1000) api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
			else
			{
				Helper.MoveTo(Point(stuinfo[0]->x / 1000, stuinfo[0]->y / 1000), true);
			}
			break;
>>>>>>> ceb677d709a11f31e97f4df4b2195d189f4df231
	}
}
