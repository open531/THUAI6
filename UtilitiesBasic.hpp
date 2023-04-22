#include "Utilities.hpp"

template<typename IFooAPI>
void Utilities<IFooAPI>::InitMap(IStudentAPI& api)
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
				Door.emplace_back(Point(i, j));
				break;
			case 4U:	// ClassRoom
				Access[i][j] = 0U;
				Classroom.emplace_back(Point(i, j));
				break;
			case 5U:	// Gate
				Access[i][j] = 0U;
				Gate.emplace_back(Point(i, j));
				break;
			case 6U:	// HiddenGate
				Access[i][j] = 0U;
				HiddenGate.emplace_back(Point(i, j));
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

template<typename IFooAPI>
Utilities<IFooAPI>::Utilities(IFooAPI api) : API(api), cntFinishedClassroom(0), LastUpdateTime(0)
{
	memset(ClassroomState, 0, sizeof(ClassroomState));
	memset(ChestState, 0, sizeof(ChestState));
	InitMap(api);
}

template<typename IFooAPI>
void Utilities<IFooAPI>::AutoUpdate()
{
	auto msec = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
	if (msec - LastUpdateTime < UpdateInterval) return;
	std::shared_ptr<const THUAI6::Student> selfinfo = API.GetSelfInfo();
	LastUpdateTime = msec;
	for (auto it : Door)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			bool newDoor = false, checkopen = API.IsDoorOpen(it.x, it.y);
			if (checkopen && Access[it.x][it.y] == 0U)
			{
				newDoor = true;
				Access[it.x][it.y] = 2U;
			}
			else if (!checkopen && Access[it.x][it.y] == 2U)
			{
				newDoor = true;
				Access[it.x][it.y] = 0U;
			}
			if (newDoor)
			{
				// TODO: 广播门的最新信息
			}
		}
	}
	for (auto it : Classroom)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetClassroomProgress(it.x, it.y) >= 10000000 && !ClassroomState[it.x][it.y])
			{
				ClassroomState[it.x][it.y] = true;
				// TODO: 广播作业写完的信息
			}
		}
	}
	for (auto it : Chest)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetChestProgress(it.x, it.y) >= 10000000 && !ChestState[it.x][it.y])
			{
				ChestState[it.x][it.y] = true;
				// TODO: 广播箱子开启的信息
			}
		}
	}
	cntFinishedClassroom = 0;
	for (auto it : Classroom) cntFinishedClassroom += ClassroomState[it.x][it.y];
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateClassroom()
{
	int Size = Classroom.size();
	if (Size)
	{
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
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateGate()
{
	//int HiddenGateSize = HiddenGate.size();
	//if (HiddenGateSize)
	//{
	//	for (int i = 0; i < HiddenGateSize; i++)
	//	{
	//		if (API.GetHiddenGateState(HiddenGate[i].x, HiddenGate[i].y) == THUAI6::HiddenGateState::Opened)
	//		{
	//			Map[HiddenGate[i].x][HiddenGate[i].y] = 5U;
	//			OpenGate.emplace_back(Point(HiddenGate[i].x, HiddenGate[i].y));
	//			HiddenGate.erase(HiddenGate.begin() + i);
	//			i--; HiddenGateSize--;
	//		}
	//	}
	//}
	int GateSize = Gate.size();
	if (GateSize)
	{
		for (int i = 0; i < GateSize; i++)
		{
			if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
			{
				//Map[Gate[i].x][Gate[i].y] = 12U;
				//OpenGate.emplace_back(Point(Gate[i].x, Gate[i].y));
				//Gate.erase(Gate.begin() + i);
				//i--; GateSize--;
			}
		}
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateChest()
{
	int Size = Chest.size();
	if (Size)
	{
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
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::UpdateDoor()
{
	int Size = Door.size();
	if (Size)
	{
		for (int i = 0; i < Size; i++)
		{
			if (API.IsDoorOpen(Door[i].x, Door[i].y) && IsViewable(Point(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000), Point(Door[i].x, Door[i].y), API.GetSelfInfo()->viewRange))
			{
				Access[Door[i].x][Door[i].y] = 2U;
			}
			if (!API.IsDoorOpen(Door[i].x, Door[i].y) && IsViewable(Point(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000), Point(Door[i].x, Door[i].y), API.GetSelfInfo()->viewRange))
			{
				Access[Door[i].x][Door[i].y] = 0U;
			}
		}
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::MoveTo(Point Dest, bool WithWindows)
{
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	bool IsStuck = (sx == TEMP.x && sy == TEMP.y);
	std::vector<unsigned char> AccessTempS;
	std::vector<unsigned char> AccessTempT;
	for (int i = 0; i < API.GetStudents().size(); i++)
	{
		if ((API.GetStudents()[i]->x / 1000 != sx / 1000) && (API.GetStudents()[i]->y / 1000 != sy / 1000))
		{
			AccessTempS.emplace_back(Access[(API.GetStudents()[i]->x) / 1000][(API.GetStudents()[i]->y) / 1000]);
			Access[(API.GetStudents()[i]->x) / 1000][(API.GetStudents()[i]->y) / 1000] = 0U;
		}
	}
	for (int i = 0; i < API.GetTrickers().size(); i++)
	{
		if ((API.GetTrickers()[i]->x / 1000 != sx / 1000) && (API.GetTrickers()[i]->y / 1000 != sy / 1000))
		{
			AccessTempT.emplace_back(Access[(API.GetTrickers()[i]->x) / 1000][(API.GetTrickers()[i]->y) / 1000]);
			Access[(API.GetTrickers()[i]->x) / 1000][(API.GetTrickers()[i]->y) / 1000] = 0U;
		}
	}
	std::vector<Node> UsablePath;
	if (WithWindows) UsablePath = AStarWithWindows(Node(sx / 1000, sy / 1000), Dest);
	else UsablePath = AStarWithoutWindows(Node(sx / 1000, sy / 1000), Dest);
	if (UsablePath.size() < 2) return false;
	int tx, ty;
	if (UsablePath.size() >= 3
		&& IsValidWithoutWindows(sx / 1000, sy / 1000)
		&& IsValidWithoutWindows(UsablePath[1].x, UsablePath[1].y)
		&& IsValidWithoutWindows(UsablePath[2].x, UsablePath[2].y)
		&& IsValidWithoutWindows(sx / 1000, UsablePath[2].y)
		&& IsValidWithoutWindows(UsablePath[2].x, sy / 1000))
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
			srand((unsigned)time(&t) + sx + sy + API.GetSelfInfo()->speed);
			API.Move(150 * sqrt(dx * dx + dy * dy) / API.GetSelfInfo()->speed, atan2(dy, dx) + rand());
		}
	}
	else
	{
		API.SkipWindow();
	}
	TEMP.x = sx; TEMP.y = sy;
	for (int i = 0, j = 0; i < API.GetStudents().size(); i++)
	{
		if ((API.GetStudents()[i]->x / 1000 != sx / 1000) && (API.GetStudents()[i]->y / 1000 != sy / 1000))
		{
			Access[(API.GetStudents()[i]->x) / 1000][(API.GetStudents()[i]->y) / 1000] = AccessTempS[j];
			j++;
		}
	}
	for (int i = 0, j = 0; i < API.GetTrickers().size(); i++)
	{
		if ((API.GetTrickers()[i]->x / 1000 != sx / 1000) && (API.GetTrickers()[i]->y / 1000 != sy / 1000))
		{
			Access[(API.GetTrickers()[i]->x) / 1000][(API.GetTrickers()[i]->y) / 1000] = AccessTempT[i];
			j++;
		}
	}
	return true;
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
bool Utilities<typename IFooAPI>::MoveToNearestClassroom(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Classroom.empty())
	{
		for (int i = 0; i < Classroom.size(); i++)
		{
			if (API.GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)
			{
				Distance = WithWindows ? AStarWithWindows(Self, Classroom[i]).size() : AStarWithoutWindows(Self, Classroom[i]).size();
				if (Distance < minDistance && Distance != 0)
				{
					minDistance = Distance;
					minNum = i;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Classroom[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearClassroom()
{
	for (int i = 0; i < Classroom.size(); i++)
	{
		if (NearPoint(Classroom[i], 2)&& API.GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::MoveToNearestGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Gate.empty())
	{
		for (int i = 0; i < Gate.size(); i++)
		{
			if (API.GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
			{
				Distance = WithWindows ? AStarWithWindows(Self, Gate[i]).size() : AStarWithoutWindows(Self, Gate[i]).size();
				if (Distance < minDistance && Distance != 0)
				{
					minDistance = Distance;
					minNum = i;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Gate[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::MoveToNearestOpenGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Gate.empty())
	{
		for (int i = 0; i < Gate.size(); i++)
		{
			if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
			{
				Distance = WithWindows ? AStarWithWindows(Self, OpenGate[i]).size() : AStarWithoutWindows(Self, OpenGate[i]).size();
				if (Distance < minDistance && Distance != 0)
				{
					minDistance = Distance;
					minNum = i;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Gate[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearPoint(Gate[i], 2)&& API.GetGateProgress(Gate[i].x, Gate[i].y) < 18000) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearOpenGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearPoint(Gate[i], 2)&& API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::MoveToNearestChest(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Chest.empty())
	{
		for (int i = 0; i < Chest.size(); i++)
		{
			if (API.GetChestProgress(Chest[i].x, Chest[i].y) < 10000000)
			{
				Distance = WithWindows ? AStarWithWindows(Self, Chest[i]).size() : AStarWithoutWindows(Self, Chest[i]).size();
				if (Distance < minDistance && Distance != 0)
				{
					minDistance = Distance;
					minNum = i;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Chest[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::NearChest()
{
	for (int i = 0; i < Chest.size(); i++)
	{
		if (NearPoint(Chest[i], 2)&& API.GetChestProgress(Chest[i].x, Chest[i].y) < 10000000) return true;
	}
	return false;
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::EstimateTime(Point Dest)
{
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	int Distance = AStarWithWindows(Self, Dest).size();
	int Speed = API.GetSelfInfo()->speed;
	int Time = Distance * 1000 / Speed;
	return Time;
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectLearning(bool WithWindows)
{
	//UpdateClassroom();
	if (!NearClassroom())
	{
		MoveToNearestClassroom(WithWindows);
	}
	else
	{
		API.StartLearning();
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectOpeningChest(bool WithWindows)
{
	//UpdateChest();
	if (!NearChest())
	{
		MoveToNearestChest(WithWindows);
	}
	else
	{
		API.StartOpenChest();
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectOpeningGate(bool WithWindows)
{
	//UpdateGate();
	if (!NearGate() && !NearOpenGate())
	{
		MoveToNearestGate(WithWindows);
	}
	else
	{
		API.StartOpenGate();
	}
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectGraduate(bool WithWindows)
{
	//UpdateGate();
	if (!NearOpenGate())
	{
		MoveToNearestOpenGate(WithWindows);
	}
	else
	{
		API.Graduate();
	}
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::CountFinishedClassroom() const
{
	return cntFinishedClassroom;
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::CountNonemptyChest() const
{
	return Chest.size();
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::CountHiddenGate() const
{
	return HiddenGate.size();
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::CountClosedGate() const
{
	return Gate.size();
}

template<typename IFooAPI>
int Utilities<typename IFooAPI>::CountOpenGate() const
{
	return OpenGate.size();
}

template<typename IFooAPI>
void Utilities<typename IFooAPI>::DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows)
{
	if (Inventory.size() < 3)
	{
		int MaxValue = 0, MaxNum = 0;
		std::vector<std::shared_ptr<const THUAI6::Prop>> ViewableProps = API.GetProps();
		int PropValue = 0;
		for (int i = 0; i < ViewableProps.size(); i++)
		{
			int dx = (API.GetProps()[i]->x - API.GetSelfInfo()->x) / 1000;
			int dy = (API.GetProps()[i]->y - API.GetSelfInfo()->y) / 1000;
			int Distance = sqrt(dx * dx + dy * dy);
			PropValue = (Distance * DistanceInfluence + Priority[(int)(API.GetProps()[i]->type)] * PropInfluence);
			if (PropValue >= MaxValue)
			{
				MaxValue = PropValue;
				MaxNum = i;
			}
		}
		if (!NearPoint(Point(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), 0))
		{
			MoveTo(Point(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), WithWindows);
		}
		else
		{
			API.PickProp(ViewableProps[MaxNum]->type);
			Inventory.emplace_back(ViewableProps[MaxNum]->type);
		}
	}
}

template<typename IFooAPI>
bool Utilities<typename IFooAPI>::IsViewable(Point Src, Point Dest, int ViewRange)
{
	int deltaX = (Dest.x - Src.x) * 1000;
	int deltaY = (Dest.y - Src.y) * 1000;
	int Distance = deltaX * deltaX + deltaY * deltaY;
	unsigned char SrcType = Map[Src.x][Src.y];
	unsigned char DestType = Map[Dest.x][Dest.y];
	if (DestType == 3U && SrcType != 3U)  // 草丛外必不可能看到草丛内
		return false;
	if (Distance < ViewRange * ViewRange)
	{
		int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
		if (divide == 0)
			return true;
		double dx = deltaX / divide;
		double dy = deltaY / divide;
		double myX = double(Src.x * 1000);
		double myY = double(Src.y * 1000);
		if (DestType == 3U && SrcType == 3U)  // 都在草丛内，要另作判断
			for (int i = 0; i < divide; i++)
			{
				myX += dx;
				myY += dy;
				if (Map[(int)myX / 1000][(int)myY / 1000] != 3U)
					return false;
			}
		else  // 不在草丛内，只需要没有墙即可
			for (int i = 0; i < divide; i++)
			{
				myX += dx;
				myY += dy;
				if (Map[(int)myX / 1000][(int)myY / 1000] == 2U)
					return false;
			}
		return true;
	}
	else
		return false;
}
