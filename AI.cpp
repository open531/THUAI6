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
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Athlete,
	THUAI6::StudentType::Sunshine };

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
#define sinspiring 0x18

#define sFindPlayer 0x20
#define sAttackPlayer 0x21
#define sChasePlayer 0x22


void AI::play(IStudentAPI& api)
{
	api.PrintSelfInfo();
	static std::vector<unsigned char> Priority = { 0,1,2,3,4,5,6,7,8 };
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



	// 公共操作
	if (this->playerID == 0)
	{
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();
		static bool visitClassroom[10];
		static bool visitClassroomUpdated[10];
		static int countVisitedClassroom = 0;
		bool haveTricker = false;
		if (!triinfo.empty()) haveTricker = true;
		static bool ChaseIt = false;
		static Point ChaseDest;

		switch (CurrentState)
		{
		case sDefault:
			if (haveTricker && Helper.TeacherPunishCD() < 10) CurrentState = sAttackPlayer;
			else if (ChaseIt) CurrentState = sChasePlayer;
			else CurrentState = sFindPlayer;
			break;
		case sFindPlayer:
			if (haveTricker) CurrentState = sAttackPlayer;
			break;
		case sAttackPlayer:
			ChaseIt = true;
			if (haveTricker && !Helper.NearPoint(ChaseDest.ToNormal(), 3)) ChaseDest = Point(triinfo[0]->x, triinfo[0]->y);
			if (haveTricker && Helper.NearPoint(ChaseDest.ToNormal(), 4))
			{
				ChaseIt = false;
				CurrentState = sDefault;
			}
			if (!haveTricker) CurrentState = sChasePlayer;
			break;
		case sChasePlayer:
			if (haveTricker && !Helper.NearPoint(ChaseDest.ToNormal(), 4)) CurrentState = sAttackPlayer;
			else
			{
				ChaseIt = false;
				CurrentState = sDefault;
			}
			break;
		}

		switch (CurrentState)
		{
		case sDefault:
			std::cerr << "CurrentState: sDefault" << std::endl;
			break;
		case sFindPlayer:
			std::cerr << "CurrentState: sFindPlayer" << std::endl;
			if (Helper.NearClassroom(false))
			{
				for (int i = 0; i < 10; i++)
					if (Helper.NearPoint(Helper.Classroom[i], 3))
					{
						visitClassroom[i] = true;
						//countVisitedClassroom++;
					}
			}
			for (int i = 0; i < 10; i++)
			{
				if (visitClassroom[i] && !visitClassroomUpdated[i])
				{
					countVisitedClassroom++;
					visitClassroomUpdated[i] = true;
				}
			}
			if (countVisitedClassroom == 10)
			{
				for (int i = 0; i < 10; i++)
				{
					visitClassroom[i] = false;
				}
				countVisitedClassroom = 0;
			}
			for (int i = 0; i < 10; i++)
				if (!visitClassroom[i])
				{
					Helper.MoveTo(Helper.Classroom[i], 1);
					break;
				}
			break;
		case sAttackPlayer:
			if (!triinfo.empty())
				if (Helper.IsViewable(triinfo[0]->x, triinfo[0]->y, api.GetSelfInfo()->viewRange)
					&& triinfo[0]->playerState == THUAI6::PlayerState::Climbing
					|| triinfo[0]->playerState == THUAI6::PlayerState::Locking
					|| triinfo[0]->playerState == THUAI6::PlayerState::Attacking
					|| triinfo[0]->playerState == THUAI6::PlayerState::Swinging)
				{
					if (!Helper.TeacherPunishCD()) Helper.TeacherPunish();
					else
					{
						for (int i = 0; i < 10; i++)
							if (!visitClassroom[i])
							{
								Helper.MoveTo(Helper.Classroom[i], 1);
								break;
							}
					}
					//			api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
				}
				else
				{
					//api.EndAllAction();
					Helper.MoveTo(Point(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
				}
			break;
		case sChasePlayer:
			std::cerr << "CurrentState: sChasePlayer" << std::endl;
			Helper.MoveTo(ChaseDest.ToNormal(), true);
			if (Helper.NearPoint(ChaseDest.ToNormal(), 3))
			{
				ChaseIt = false;
				CurrentState = sDefault;
			}
			break;
			// 玩家0执行操作
		}
	}
	else if (this->playerID == 1)
	{
		if (Helper.CountFinishedClassroom() > 7)
		{
			if (!Helper.CountOpenGate())
			{
				//api.EndAllAction();
				Helper.DirectOpeningGate(true, true);
			}
			else
			{
				//api.EndAllAction();
				Helper.DirectGraduate(true);
			}
		}
		else
		{
			//api.EndAllAction();
			Helper.DirectLearning(true);
		}
		// 玩家1执行操作
	}
	else if (this->playerID == 2)
	{
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();

		static bool visitClassroom[10];
		static bool visitClassroomUpdated[10];
		static int countVisitedClassroom = 0;
		bool haveTricker = false;
		if (!triinfo.empty()) haveTricker = true;
		static bool ChaseIt = false;
		static Point ChaseDest;

		switch (CurrentState)
		{
		case sDefault:
			if (haveTricker && !Helper.AtheleteCanBeginToChargeCD()) CurrentState = sAttackPlayer;
			else if (ChaseIt) CurrentState = sChasePlayer;
			else CurrentState = sFindPlayer;
			break;
		case sFindPlayer:
			if (haveTricker) CurrentState = sAttackPlayer;
			break;
		case sAttackPlayer:
			ChaseIt = true;
			if (!triinfo.empty()) ChaseDest = Point(triinfo[0]->x, triinfo[0]->y);
			if (!haveTricker && !Helper.AtheleteCanBeginToChargeCD()) CurrentState = sChasePlayer;
			if (Helper.AtheleteCanBeginToChargeCD())
			{
				ChaseIt = false;
				CurrentState = sDefault;
			}
			break;
		case sChasePlayer:
			if (haveTricker) CurrentState = sAttackPlayer;
			else if (Helper.NearPoint(ChaseDest.ToNormal(), 2))
			{
				ChaseIt = false;
				CurrentState = sDefault;
			}
			break;
		}

		switch (CurrentState)
		{
		case sDefault:
			std::cerr << "CurrentState: sDefault" << std::endl;
			break;
		case sFindPlayer:
			std::cerr << "CurrentState: sFindPlayer" << std::endl;
			if (Helper.NearClassroom(false))
			{
				for (int i = 0; i < 10; i++)
					if (Helper.NearPoint(Helper.Classroom[i], 3))
					{
						visitClassroom[i] = true;
						//countVisitedClassroom++;
					}
			}
			for (int i = 0; i < 10; i++)
			{
				if (visitClassroom[i] && !visitClassroomUpdated[i])
				{
					countVisitedClassroom++;
					visitClassroomUpdated[i] = true;
				}
			}
			if (countVisitedClassroom == 10)
			{
				for (int i = 0; i < 10; i++)
				{
					visitClassroom[i] = false;
				}
				countVisitedClassroom = 0;
			}
			for (int i = 0; i < 10; i++)
				if (!visitClassroom[i])
				{
					Helper.MoveTo(Helper.Classroom[i], 1);
					break;
				}
			break;
		case sAttackPlayer:
			std::cerr << "CurrentState: sAttackPlayer" << std::endl;

			if (!triinfo.empty())
			{
				auto tritype = triinfo[0]->trickerType;
				auto trirange = sqrt((triinfo[0]->x - api.GetSelfInfo()->x) * (triinfo[0]->x - api.GetSelfInfo()->x) + (triinfo[0]->y - api.GetSelfInfo()->y) * (triinfo[0]->y - api.GetSelfInfo()->y));
				if (!Helper.AtheleteCanBeginToChargeCD() && (static_cast<int>(tritype) == 1 && trirange < 1954.5 || static_cast<int>(tritype) == 2 && trirange < 2050 || static_cast<int>(tritype) == 3 && trirange < 1981.31 || static_cast<int>(tritype) == 4 && trirange < 2050))
					//攻击（满足有技能，在攻击范围内）
				{
					Helper.AtheleteCanBeginToCharge();
					Helper.MoveTo(Point(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
				}
				if (api.GetSelfInfo()->speed > 6400 && static_cast<int>(triinfo[0]->playerState) != 8)
				{
					Helper.MoveTo(Point(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
				}
				else if ((api.GetSelfInfo()->x - stuinfo[1]->x) * (api.GetSelfInfo()->x - stuinfo[1]->x) + (api.GetSelfInfo()->y - stuinfo[1]->y) * (api.GetSelfInfo()->y - stuinfo[1]->y) < 25000000)
				{
					Helper.MoveTo(Point(stuinfo[1]->x / 1000, stuinfo[1]->y / 1000), true);
				}
					//逃跑
				else
				{
					for (int i = 0; i < 10; i++)
						if (!visitClassroom[i])
						{
							Helper.MoveTo(Helper.Classroom[i], 1);
							break;
						}
				}
				//			api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
			}
			//else
			//{
				//api.EndAllAction();
			//	Helper.MoveTo(Point(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
			//}
			break;
		case sChasePlayer:
			std::cerr << "CurrentState: sChasePlayer" << std::endl;
			Helper.MoveTo(ChaseDest.ToNormal(), true);
			break;
		}
		// 玩家2执行操作
	}
	else if (this->playerID == 3)
	{
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();
		static bool visitClassroom[10];
		static bool visitClassroomUpdated[10];
		static int countVisitedClassroom = 0;
		bool haveTricker = false;
		if (!triinfo.empty()) haveTricker = true;
		bool haveAddictedStudent = false;
		int AddictedId = -1;
		static bool ChaseIt = false;
		static Point ChaseDest;
		for (int i = 0; i < stuinfo.size(); i++)
		{
			if (stuinfo[i]->playerState == THUAI6::PlayerState::Addicted)
			{
				haveAddictedStudent = true;
				AddictedId = i;
			}
		}
		bool Needhelp = false;
		int NeedhelpID = -1;
		for (int i = 0; i < stuinfo.size(); i++)
		{
			if (stuinfo[i]->determination < 750000)
			{
				Needhelp = true;
				NeedhelpID = i;
			}
		}

		switch (CurrentState)
		{
		case sDefault:
			if (haveAddictedStudent) CurrentState = sRousing;
			else if (Needhelp) CurrentState = sEncouraging;
			else if (haveTricker) CurrentState = sinspiring;
			else CurrentState = sDoClassroom;
			break;
		case sRousing:
			if (!haveAddictedStudent) CurrentState = sDefault;
			break;
		case sEncouraging:
			if (haveAddictedStudent) CurrentState = sRousing;
			else if (!Needhelp && !haveAddictedStudent) CurrentState = sDefault;
			break;
		case sinspiring:
			if (haveAddictedStudent) CurrentState = sRousing;
			else if (Needhelp) CurrentState = sEncouraging;
			break;
		case sDoClassroom:
			if (haveAddictedStudent) CurrentState = sRousing;
			else if (Needhelp) CurrentState = sEncouraging;
			else if (haveTricker) CurrentState = sinspiring;
			break;
		}
		switch (CurrentState)
		{
		case sDefault:
			std::cerr << "CurrentState: sDefault" << std::endl;
			break;
		case sRousing:
			std::cerr << "CurrentState: sRousing" << std::endl;
			ChaseIt = true;
			ChaseDest = Point(stuinfo[AddictedId]->x, stuinfo[AddictedId]->y);
			if ((api.GetSelfInfo()->x - stuinfo[AddictedId]->x)* (api.GetSelfInfo()->x - stuinfo[AddictedId]->x) + (api.GetSelfInfo()->y - stuinfo[AddictedId]->y)* (api.GetSelfInfo()->y - stuinfo[AddictedId]->y) < 25000000)
			{
				if (!Helper.SunshineRouseCD()) Helper.SunshineRouse();
			}
			else
			{
				Helper.MoveTo(Point(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), true);
			}
			break;
		case sEncouraging:
			std::cerr << "CurrentState: sEncouraging" << std::endl;
			ChaseIt = true;
			ChaseDest = Point(stuinfo[NeedhelpID]->x, stuinfo[NeedhelpID]->y);
			if ((api.GetSelfInfo()->x - stuinfo[NeedhelpID]->x) * (api.GetSelfInfo()->x - stuinfo[NeedhelpID]->x) + (api.GetSelfInfo()->y - stuinfo[NeedhelpID]->y) * (api.GetSelfInfo()->y - stuinfo[NeedhelpID]->y) < 25000000)
			{
				if (!Helper.SunshineEncourageCD()) Helper.SunshineEncourage();
			}
			else
			{
				Helper.MoveTo(Point(stuinfo[NeedhelpID]->x / 1000, stuinfo[NeedhelpID]->y / 1000), true);
			}
			break;
		case sinspiring:
			std::cerr << "CurrentState: sinspiring" << std::endl;
			if (!Helper.SunshineInspireCD()) Helper.SunshineInspire();
			if ((api.GetSelfInfo()->x - stuinfo[1]->x) * (api.GetSelfInfo()->x - stuinfo[1]->x) + (api.GetSelfInfo()->y - stuinfo[1]->y) * (api.GetSelfInfo()->y - stuinfo[1]->y) < 25000000)
			{
				Helper.MoveTo(Point(stuinfo[1]->x / 1000, stuinfo[1]->y / 1000), true);
			}
			else if ((api.GetSelfInfo()->x - stuinfo[2]->x) * (api.GetSelfInfo()->x - stuinfo[2]->x) + (api.GetSelfInfo()->y - stuinfo[2]->y) * (api.GetSelfInfo()->y - stuinfo[2]->y) < 25000000)
			{
				Helper.MoveTo(Point(stuinfo[2]->x / 1000, stuinfo[2]->y / 1000), true);
			}
			else
			{
				for (int i = 0; i < 10; i++)
					if (!visitClassroom[i])
					{
						Helper.MoveTo(Helper.Classroom[i], 1);
						break;
					}
			}
			break;
		case sDoClassroom:
			std::cerr << "CurrentState: sDoClassroom" << std::endl;
			if (Helper.CountFinishedClassroom() > 7)
			{
				if (!Helper.CountOpenGate())
				{
					//api.EndAllAction();
					Helper.DirectOpeningGate(true, true);
				}
				else
				{
					//api.EndAllAction();
					Helper.DirectGraduate(true);
				}
			}
			else
			{
				//api.EndAllAction();
				Helper.DirectLearning(true);
			}
			break;
			// 玩家3执行操作
		}
		
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

	auto stuinfo = api.GetStudents();

	static bool visitClassroom[10];
	static bool visitClassroomUpdated[10];
	static int countVisitedClassroom = 0;
	bool haveNonAddictedStudent = false;
	int nonAddictedId = -1;
	for (int i = 0; i < stuinfo.size(); i++)
	{
		if (stuinfo[i]->playerState != THUAI6::PlayerState::Addicted)
		{
			haveNonAddictedStudent = true;
			nonAddictedId = i;
		}
	}
	static bool ChaseIt = false;
	static Point ChaseDest;

	switch (CurrentState)
	{
	case sDefault:
		if (haveNonAddictedStudent) CurrentState = sAttackPlayer;
		else if (ChaseIt) CurrentState = sChasePlayer;
		else CurrentState = sFindPlayer;
		break;
	case sFindPlayer:
		if (haveNonAddictedStudent) CurrentState = sAttackPlayer;
		break;
	case sAttackPlayer:
		if (!haveNonAddictedStudent) CurrentState = sChasePlayer;
		break;
	case sChasePlayer:
		if (haveNonAddictedStudent) CurrentState = sAttackPlayer;
		else if (Helper.NearPoint(ChaseDest.ToNormal(), 2))
		{
			ChaseIt = false;
			CurrentState = sDefault;
		}
		break;
	}

	switch (CurrentState)
	{
	case sDefault:
		std::cerr << "CurrentState: sDefault" << std::endl;
		break;
	case sFindPlayer:
		std::cerr << "CurrentState: sFindPlayer" << std::endl;
		if (Helper.NearClassroom(false))
		{
			for (int i = 0; i < 10; i++)
				if (Helper.NearPoint(Helper.Classroom[i], 3))
				{
					visitClassroom[i] = true;
					//countVisitedClassroom++;
				}
		}
		for (int i = 0; i < 10; i++)
		{
			if (visitClassroom[i] && !visitClassroomUpdated[i])
			{
				countVisitedClassroom++;
				visitClassroomUpdated[i] = true;
			}
		}
		if (countVisitedClassroom == 10)
		{
			for (int i = 0; i < 10; i++)
			{
				visitClassroom[i] = false;
			}
			countVisitedClassroom = 0;
		}
		for (int i = 0; i < 10; i++)
			if (!visitClassroom[i])
			{
				Helper.MoveTo(Helper.Classroom[i], 1);
				break;
			}
		break;
	case sAttackPlayer:
		std::cerr << "CurrentState: sAttackPlayer" << std::endl;
		std::cerr << "See student " << stuinfo.size() << std::endl;
		std::cerr << "Decide to attack " << stuinfo[nonAddictedId]->playerID << std::endl;
		ChaseIt = true;
		ChaseDest = Point(stuinfo[nonAddictedId]->x, stuinfo[nonAddictedId]->y);
		if (abs(api.GetSelfInfo()->x - stuinfo[nonAddictedId]->x) + abs(api.GetSelfInfo()->y - stuinfo[nonAddictedId]->y) < 2000)
		{
			Helper.AssassinDefaultAttack(stuinfo[nonAddictedId]->x, stuinfo[nonAddictedId]->y);
			//			api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
		}
		else
		{
			//api.EndAllAction();
			Helper.MoveTo(Point(stuinfo[nonAddictedId]->x / 1000, stuinfo[nonAddictedId]->y / 1000), true);
		}
		break;
	case sChasePlayer:
		std::cerr << "CurrentState: sChasePlayer" << std::endl;
		Helper.MoveTo(ChaseDest.ToNormal(), true);
		break;
	}
}
