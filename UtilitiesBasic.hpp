#include "Utilities.hpp"

template<typename IFooAPI>
void Utilities<IFooAPI>::InitMap(IFooAPI& api)
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
				Grass.emplace_back(Point(i, j));
				break;
			case 7U:	// Window
				Access[i][j] = 1U;
				break;
			case 8U:	// Door3
			case 9U:	// Door5
			case 10U:	// Door6
				Access[i][j] = 2U;
				Door.emplace_back(Point(i, j), true, api.GetPlaceType(i, j));
				break;
			case 4U:	// Classroom
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
Utilities<IFooAPI>::Utilities(IFooAPI api) : API(api), LastAutoUpdateFrame(0), AStarHelper(api)
{
	srand(time(NULL));
	InitMap(api);
}

void UtilitiesStudent::AutoUpdate()
{
	int cntframe = API.GetFrameCount();
	if (cntframe - LastAutoUpdateFrame < UpdateInterval) return;
	std::shared_ptr<const THUAI6::Student> selfinfo = API.GetSelfInfo();
	LastAutoUpdateFrame = cntframe;
	for (auto it : Door)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			bool newDoor = false, checkopen = API.IsDoorOpen(it.x, it.y);
			if (checkopen && Access[it.x][it.y] == 0U)
			{
				newDoor = true;
				Access[it.x][it.y] = 2U;
				it.DoorStatus = true;
			}
			else if (!checkopen && Access[it.x][it.y] == 2U)
			{
				newDoor = true;
				Access[it.x][it.y] = 0U;
				it.DoorStatus = false;
			}
			if (newDoor)
			{
				gugu.sendMapUpdate(0, it.DoorType, it.x, it.y, checkopen ? 2U : 0U); // 这里没有区分Door3, Door5, Door6的区别，可能要改
				gugu.sendMapUpdate(1, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
				gugu.sendMapUpdate(2, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
				gugu.sendMapUpdate(3, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
			}
		}
	}
	for (auto it : Classroom)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetClassroomProgress(it.x, it.y) >= 10000000 && ProgressMem[it.x][it.y] < 10000000)
			{
				ProgressMem[it.x][it.y] = 10000000;
				gugu.sendMapUpdate(0, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				gugu.sendMapUpdate(1, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				gugu.sendMapUpdate(2, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				gugu.sendMapUpdate(3, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
			}
		}
	}
	for (auto it : Chest)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetChestProgress(it.x, it.y) >= 10000000 && ProgressMem[it.x][it.y] < 10000000)
			{
				ProgressMem[it.x][it.y] = 10000000;
				gugu.sendMapUpdate(0, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				gugu.sendMapUpdate(1, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				gugu.sendMapUpdate(2, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				gugu.sendMapUpdate(3, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
			}
		}
	}
	for (auto it : Gate)
	{
		if (IsViewable(Point(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetGateProgress(it.x, it.y) >= 18000 && ProgressMem[it.x][it.y] < 18000)
			{
				ProgressMem[it.x][it.y] = 18000;
				gugu.sendMapUpdate(0, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				gugu.sendMapUpdate(1, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				gugu.sendMapUpdate(2, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				gugu.sendMapUpdate(3, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
			}
		}
	}
}

#define USE_NEW_ASTAR 0

#if !USE_NEW_ASTAR
template<typename IFooAPI>
bool Utilities<IFooAPI>::MoveTo(Point Dest, bool WithWindows)
{
	std::cerr << "Destination is (" << Dest.x << ',' << Dest.y << ')' << std::endl;
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	bool IsStuck = (sx == TEMP.x && sy == TEMP.y);
	std::vector<std::shared_ptr<const THUAI6::Student>> TempS = API.GetStudents();
	std::vector<std::shared_ptr<const THUAI6::Tricker>> TempT = API.GetTrickers();
	std::vector<unsigned char> AccessTempS;
	std::vector<unsigned char> AccessTempT;
	for (int i = 0; i < TempS.size(); i++)
	{
		if ((TempS[i]->x / 1000 != sx / 1000) && (TempS[i]->y / 1000 != sy / 1000))
		{
			AccessTempS.emplace_back(Access[(TempS[i]->x) / 1000][(TempS[i]->y) / 1000]);
			Access[(TempS[i]->x) / 1000][(TempS[i]->y) / 1000] = 0U;
		}
	}
	for (int i = 0; i < API.GetTrickers().size(); i++)
	{
		if ((TempT[i]->x / 1000 != sx / 1000) && (TempT[i]->y / 1000 != sy / 1000))
		{
			AccessTempT.emplace_back(Access[(TempT[i]->x) / 1000][(TempT[i]->y) / 1000]);
			Access[(TempT[i]->x) / 1000][(TempT[i]->y) / 1000] = 0U;
		}
	}
	std::vector<Node> UsablePath;
	if (WithWindows) UsablePath = AStarWithWindows(Node(sx / 1000, sy / 1000), Dest);
	else UsablePath = AStarWithoutWindows(Node(sx / 1000, sy / 1000), Dest);
	for (auto i : UsablePath)
	{
		std::cerr << '(' << i.x << ',' << i.y << ')' << ';';
	}
	if (UsablePath.size() < 2)
	{
		for (int i = 0, j = 0; i < TempS.size(); i++)
		{
			if ((TempS[i]->x / 1000 != sx / 1000) && (TempS[i]->y / 1000 != sy / 1000))
			{
				Access[(TempS[i]->x) / 1000][(TempS[i]->y) / 1000] = AccessTempS[j];
				j++;
			}
		}
		for (int i = 0, j = 0; i < TempT.size(); i++)
		{
			if ((TempT[i]->x / 1000 != sx / 1000) && (TempT[i]->y / 1000 != sy / 1000))
			{
				Access[(TempT[i]->x) / 1000][(TempT[i]->y) / 1000] = AccessTempT[i];
				j++;
			}
		}
		return false;
	}
	else
	{
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
				API.Move(200 * sqrt(dx * dx + dy * dy) / API.GetSelfInfo()->speed, atan2(dy, dx) + rand());
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
}

#else
template<typename IFooAPI>
bool Utilities<IFooAPI>::MoveTo(Point Dest, bool WithWindows)
{
	auto self = API.GetSelfInfo();
	GeometryPoint GFrom(API.GetSelfInfo()->x, API.GetSelfInfo()->y), GDest(Dest.x*1000+500, Dest.y*1000+500);
	auto Path = AStarHelper.FindPath(GFrom, GDest);
	for (auto p : Path)
	{
		std::cerr << '(' << p.PointX << ',' << p.PointY << ')' << "->";
	}
	int ptr = 0;
	if (Path.size() != 1)
	{
		while (ptr < Path.size() && Distance(GFrom, Path[ptr]) < 10) ptr++;
	}
	if (ptr == Path.size()) return true;
	API.Move((int)std::max<double>((std::min<double>(150, Distance(Path[ptr], GFrom) / API.GetSelfInfo()->speed * 1000)), 10), atan2(Path[ptr].PointY - GFrom.PointY, Path[ptr].PointX - GFrom.PointX));
	if (WithWindows)
	{
		if (self->x == TEMP.x && self->y == TEMP.y && NearWindow())
			API.SkipWindow();
		TEMP = Point(self->x, self->y);
	}
	std::cerr << "move angle = " << atan2(Path[ptr].PointY - GFrom.PointY, Path[ptr].PointX - GFrom.PointX) / acos(-1) * 180;
}

#endif

template<typename IFooAPI>
bool Utilities<IFooAPI>::NearPoint(Point P, int level)
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
	case 3:	// Hide Tricker
		return ((P.x - Self.x) * (P.x - Self.x) + (P.y - Self.y) * (P.y - Self.y) <= 9) ? true : false;
		break;
	case 4:	// Hide Tricker
		return ((P.x - Self.x) * (P.x - Self.x) + (P.y - Self.y) * (P.y - Self.y) <= 25) ? true : false;
		break;
	}
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::MoveToNearestClassroom(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Classroom.empty())
	{
		for (int i = 0; i < Classroom.size(); i++)
		{
			//			if (API.GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)
			if (GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)
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
bool Utilities<IFooAPI>::NearClassroom(bool checkProgress)
{
	for (int i = 0; i < Classroom.size(); i++)
	{
		if (NearPoint(Classroom[i], 2) && (!checkProgress || (checkProgress && GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000))) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::MoveToNearestGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Gate.empty())
	{
		for (int i = 0; i < Gate.size(); i++)
		{
			//			if (API.GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
			if (GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
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
bool Utilities<IFooAPI>::MoveToNearestOpenGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Gate.empty())
	{
		for (int i = 0; i < Gate.size(); i++)
		{
			//			if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
			if (GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
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
bool Utilities<IFooAPI>::NearGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearPoint(Gate[i], 2) && GetGateProgress(Gate[i].x, Gate[i].y) < 18000) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::NearOpenGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearPoint(Gate[i], 2) && GetGateProgress(Gate[i].x, Gate[i].y) >= 18000) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::MoveToNearestChest(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Chest.empty())
	{
		for (int i = 0; i < Chest.size(); i++)
		{
			//			if (API.GetChestProgress(Chest[i].x, Chest[i].y) < 10000000)
			if (GetChestProgress(Chest[i].x, Chest[i].y) < 10000000)
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
bool Utilities<IFooAPI>::NearChest()
{
	for (int i = 0; i < Chest.size(); i++)
	{
		if (NearPoint(Chest[i], 2) && API.GetChestProgress(Chest[i].x, Chest[i].y) < 10000000) return true;
	}
	return false;
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::NearWindow()
{
	int X = API.GetSelfInfo()->x / 1000, Y = API.GetSelfInfo()->y / 1000;
	for (int i = X - 1; i <= X + 1; i++)
		for (int j = Y - 1; j <= Y + 1; j++)
			if (abs(i - X) + abs(j - Y) == 1 && API.GetPlaceType(i, j) == THUAI6::PlaceType::Window)
				return true;
	return false;
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::InGrass()
{
	if (Map[API.GetSelfInfo()->x / 1000][API.GetSelfInfo()->x / 1000] == 3U)
	{
		return true;
	}
	else return false;
}

template<typename IFooAPI>
int Utilities<IFooAPI>::EstimateTime(Point Dest)
{
	Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	int Distance = AStarWithWindows(Self, Dest).size();
	int Speed = API.GetSelfInfo()->speed;
	int Time = Distance * 1000 / Speed;
	return Time;
}

template<typename IFooAPI>
void Utilities<IFooAPI>::DirectLearning(bool WithWindows)
{
	if (!NearClassroom(true))
	{
		MoveToNearestClassroom(WithWindows);
	}
	else
	{
		API.StartLearning();
	}
}

template<typename IFooAPI>
void Utilities<IFooAPI>::DirectOpeningChest(bool WithWindows)
{
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
void Utilities<IFooAPI>::DirectOpeningGate(bool WithWindows, bool CanDirectGraduate)
{
	if (!NearGate())
	{
		MoveToNearestGate(WithWindows);
	}
	else
	{
		if (!NearOpenGate())
		{
			API.StartOpenGate();
		}
		else
		{
			if (CanDirectGraduate)
			{
				DirectGraduate(WithWindows);
			}
			else
			{
				MoveToNearestGate(WithWindows);
			}
		}
	}
}

template<typename IFooAPI>
void Utilities<IFooAPI>::DirectGraduate(bool WithWindows)
{
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
void Utilities<IFooAPI>::DirectGrass(bool WithWindows)
{
	if (!InGrass())
	{
		int minDistance = INT_MAX;
		int minNum = -1;
		int Distance = INT_MAX;
		Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
		if (!Grass.empty())
		{
			for (int i = 0; i < Grass.size(); i++)
			{
				Distance = WithWindows ? AStarWithWindows(Self, Grass[i]).size() : AStarWithoutWindows(Self, Grass[i]).size();
				if (Distance < minDistance && Distance != 0)
				{
					minDistance = Distance;
					minNum = i;
				}
			}
		}
		if (minNum >= 0)
		{
			MoveTo(Grass[minNum], WithWindows);
		}
	}
}

template<typename IFooAPI>
void Utilities<IFooAPI>::DirectHide(Point TrickerLocation, int TrickerViewRange, bool WithWindows)
{
	if (!(InGrass() && IsViewable(TrickerLocation, Point(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000), TrickerViewRange)))
	{
		int minDistance = INT_MAX;
		int minNum = -1;
		int Distance = INT_MAX;
		Point Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
		if (!Grass.empty())
		{
			for (int i = 0; i < Grass.size(); i++)
			{
				if ((TrickerLocation.x - Grass[i].x) * (TrickerLocation.x - Grass[i].x) + (TrickerLocation.y - Grass[i].y) * (TrickerLocation.y - Grass[i].y) > 25
					&& !IsViewable(TrickerLocation, Point(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000), TrickerViewRange))
				{
					Distance = WithWindows ? AStarWithWindows(Self, Grass[i]).size() : AStarWithoutWindows(Self, Grass[i]).size();
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
			MoveTo(Grass[minNum], WithWindows);
		}
	}
}

template<typename IFooAPI>
int Utilities<IFooAPI>::CountFinishedClassroom() const
{
	int cnt = 0;
	for (auto i : Classroom)
	{
		if (ProgressMem[i.x][i.y] >= 10000000) cnt++;
	}
	return cnt;
}

template<typename IFooAPI>
int Utilities<IFooAPI>::CountNonemptyChest() const
{
	int cnt = 0;
	for (auto i : Chest)
	{
		if (ProgressMem[i.x][i.y] < 10000000) cnt++;
	}
	return cnt;
}

template<typename IFooAPI>
int Utilities<IFooAPI>::CountHiddenGate() const
{
	return HiddenGate.size();
}

template<typename IFooAPI>
int Utilities<IFooAPI>::CountClosedGate() const
{
	int ret = 0;
	for (int i = 0; i < Gate.size(); i++)
	{
		if (API.GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
		{
			ret++;
		}
	}
	return ret;
}

template<typename IFooAPI>
int Utilities<IFooAPI>::CountOpenGate() const
{
	int ret = 0;
	for (int i = 0; i < Gate.size(); i++)
	{
		if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
		{
			ret++;
		}
	}
	return ret;
}

template<typename IFooAPI>
void Utilities<IFooAPI>::OrganizeInventory(std::vector<unsigned char>Priority)
{
	if (Inventory.size() > 1)
	{
		for (int i = 0; i < Inventory.size(); i++)
		{
			for (int j = i + 1; j < Inventory.size(); j++)
			{
				if (Priority[(int)Inventory[i]] < Priority[(int)Inventory[j]])
				{
					THUAI6::PropType temp = Inventory[i];
					Inventory[i] = Inventory[j];
					Inventory[j] = temp;
				}
			}
		}
	}
}

template<typename IFooAPI>
void Utilities<IFooAPI>::DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows)
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
			OrganizeInventory(Priority);
		}
	}
	else
	{
		int MaxValue = Priority[(int)Inventory[2]], MaxNum = 0;
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
		if (MaxValue > Priority[(int)Inventory[2]])
		{
			if (!NearPoint(Point(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), 0))
			{
				MoveTo(Point(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), WithWindows);
			}
			else
			{
				API.ThrowProp(Inventory[2]);
				API.PickProp(ViewableProps[MaxNum]->type);
				Inventory.emplace_back(ViewableProps[MaxNum]->type);
				OrganizeInventory(Priority);
			}
		}
	}
}

template<typename IFooAPI>
void Utilities<IFooAPI>::DirectUseProp(std::vector<unsigned char>Priority)
{
	if (!Inventory.empty())
	{
		OrganizeInventory(Priority);
		API.UseProp(Inventory[0]);
	}
}

template<typename IFooAPI>
bool Utilities<IFooAPI>::IsViewable(Point Src, Point Dest, int ViewRange)
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
		double divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100.;
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

template<typename IFooAPI>
int Utilities<IFooAPI>::GetChestProgress(int cellx, int celly)
{
	if (IsViewable(cellx, celly, API.GetSelfInfo()->viewRange)) return API.GetChestProgress(cellx, celly);
	else return ProgressMem[cellx][celly];
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetGateProgress(int cellx, int celly)
{
	if (IsViewable(cellx, celly, API.GetSelfInfo()->viewRange)) return API.GetGateProgress(cellx, celly);
	else return ProgressMem[cellx][celly];
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetClassroomProgress(int cellx, int celly)
{
	if (IsViewable(cellx, celly, API.GetSelfInfo()->viewRange)) return API.GetClassroomProgress(cellx, celly);
	else return ProgressMem[cellx][celly];
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetDoorProgress(int cellx, int celly)
{
	if (IsViewable(cellx, celly, API.GetSelfInfo()->viewRange)) return API.GetDoorProgress(cellx, celly);
	else return ProgressMem[cellx][celly];
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetChestProgress(Point cell) const
{
	return GetChestProgress(cell.x, cell.y);
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetGateProgress(Point cell) const
{
	return GetGateProgress(cell.x, cell.y);
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetClassroomProgress(Point cell) const
{
	return GetClassroomProgress(cell.x, cell.y);
}

template<typename IFooAPI>
int Utilities<IFooAPI>::GetDoorProgress(Point cell) const
{
	return GetDoorProgress(cell.x, cell.y);
}

template<typename IFooAPI>
void Utilities<IFooAPI>::Update(MapUpdateInfo upinfo, int t_)
{
	if (t_ < LastUpdateFrame[upinfo.x][upinfo.y]) return;
	LastUpdateFrame[upinfo.x][upinfo.y] = t_;
	if (upinfo.type == THUAI6::PlaceType::Chest
		|| upinfo.type == THUAI6::PlaceType::ClassRoom
		|| upinfo.type == THUAI6::PlaceType::Gate
		|| upinfo.type == THUAI6::PlaceType::HiddenGate)
	{
		ProgressMem[upinfo.x][upinfo.y] = upinfo.val;
	}
	else if (upinfo.type == THUAI6::PlaceType::Door3
		|| upinfo.type == THUAI6::PlaceType::Door5
		|| upinfo.type == THUAI6::PlaceType::Door6)
	{
		Access[upinfo.x][upinfo.y] = upinfo.val;
	}
}


void UtilitiesStudent::AtheleteCanBeginToCharge()
{
	API.UseSkill(0);
}

void UtilitiesStudent::TeacherPunish()
{
	API.UseSkill(0);
}

void UtilitiesStudent::StraightAStudentWriteAnswers()
{
	API.UseSkill(0);
}

void UtilitiesStudent::SunshineRouse()
{
	API.UseSkill(0);
}

void UtilitiesStudent::SunshineEncourage()
{
	API.UseSkill(1);
}

void UtilitiesStudent::SunshineInspire()
{
	API.UseSkill(2);
}

double UtilitiesStudent::AtheleteCanBeginToChargeCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double UtilitiesStudent::TeacherPunishCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double UtilitiesStudent::StraightAStudentWriteAnswersCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double UtilitiesStudent::SunshineRouseCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double UtilitiesStudent::SunshineEncourageCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[1];
}

double UtilitiesStudent::SunshineInspireCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[2];
}