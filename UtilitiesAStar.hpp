#include "Utilities.hpp"

template<typename IFooAPI>
bool Utilities<IFooAPI>::IsValidWithoutWindows(int x, int y) { return (bool)(Access[x][y] / 2); }

template<typename IFooAPI>
bool Utilities<IFooAPI>::IsValidWithWindows(int x, int y) { return (bool)Access[x][y]; }

template<typename IFooAPI>
bool Utilities<IFooAPI>::IsDestination(int x, int y, Node dest)
{
	if (x == dest.x && y == dest.y) return true; else return false;
}

template<typename IFooAPI>
double Utilities<IFooAPI>::CalculateH(int x, int y, Node dest)
{
	double H = (sqrt((x - dest.x) * (x - dest.x) * 100 + (y - dest.y) * (y - dest.y) * 100));
	return H;
}

template<typename IFooAPI>
std::vector<Node> Utilities<IFooAPI>::MakePath(std::array<std::array<Node, 50>, 50> map, Node dest)
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
		while (!Path.empty())
		{
			Node top = Path.top();
			Path.pop();
			UsablePath.emplace_back(top);
		}
		return UsablePath;
	}
	catch (const std::exception& e)
	{
		//		std::cout << e.what() << std::endl;
	}
}

template<typename IFooAPI>
std::vector<Node> Utilities<IFooAPI>::AStarWithoutWindows(Node src, Node dest)
{
	std::vector<Node> empty;
	//if (IsValidWithoutWindows(dest.x, dest.y) == false)
	//{
	//	return empty;
	//}
	if (IsDestination(src.x, src.y, dest))
	{
		return empty;
	}
	bool ClosedList[50][50];
	std::array<std::array < Node, 50>, 50> AStarMap;
	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
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
	while (!OpenList.empty() && OpenList.size() < 50 * 50)
	{
		Node node;
		do
		{
			float temp = FLT_MAX;
			std::vector<Node>::iterator itNode;
			for (std::vector<Node>::iterator it = OpenList.begin(); it != OpenList.end(); it = next(it))
			{
				Node n = *it;
				if (n.fCost < temp)
				{
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
		for (int newX = -1; newX <= 1; newX++)
		{
			for (int newY = -1; newY <= 1; newY++)
			{
				if (newX != 0 && newY != 0) continue;
				double gNew, hNew, fNew;
				if (IsValidWithoutWindows(x + newX, y + newY) ||
					IsDestination(x + newX, y + newY, dest))
				{
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
	if (FoundDest == false)
	{
		return empty;
	}
}

template<typename IFooAPI>
std::vector<Node> Utilities<IFooAPI>::AStarWithWindows(Node src, Node dest)
{
	std::cout << src.x << src.y;
	std::vector<Node> empty;
	//if (IsValidWithWindows(dest.x, dest.y) == false)
	//{
	//	return empty;
	//}
	if (IsDestination(src.x, src.y, dest))
	{
		return empty;
	}
	bool ClosedList[50][50];
	std::array<std::array < Node, 50>, 50> AStarMap;
	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
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
	while (!OpenList.empty() && OpenList.size() < 50 * 50)
	{
		Node node;
		do
		{
			float temp = FLT_MAX;
			std::vector<Node>::iterator itNode;
			for (std::vector<Node>::iterator it = OpenList.begin();
				it != OpenList.end(); it = next(it))
			{
				Node n = *it;
				if (n.fCost < temp)
				{
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
		for (int newX = -1; newX <= 1; newX++)
		{
			for (int newY = -1; newY <= 1; newY++)
			{
				if (newX != 0 && newY != 0) continue;
				double gNew, hNew, fNew;
				if (IsValidWithWindows(x + newX, y + newY) || IsDestination(x + newX, y + newY, dest))
				{
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
	if (FoundDest == false)
	{
		return empty;
	}
}

template<typename IFooAPI>
AStarPlus<IFooAPI>::AStarPlus(IFooAPI api_) : API(api_), SegmentRadius(405), CheckPointRadius(410), PI(acos(-1))
{
	SegmentCompensate = SegmentRadius * tan(PI / 8);
	CheckPointCompensate = CheckPointRadius * tan(PI / 8);
//	std::cerr << SegmentRadius << ' '<< SegmentCompensate << std::endl;
	InitStableMap();
}

template<typename IFooAPI>
GeometryPoint AStarPlus<IFooAPI>::Escape(GeometryPoint P)
{
	GeometryPoint Q(1500, 1500);
	double d = 500;
	for (auto s : StableMap)
	{
		GeometryPoint R(Project(s, P));
		if (Distance(R, P) < d)
		{
			Q = R;
			d = Distance(R, P);
		}
	}
	for (auto s : VariableMap)
	{
		GeometryPoint R(Project(s, P));
		if (Distance(R, P) < d)
		{
			Q = R;
			d = Distance(R, P);
		}
	}
	return Q;
}

template<typename IFooAPI>
bool AStarPlus<IFooAPI>::IsAccessible(THUAI6::PlaceType pt)
{
	return pt == THUAI6::PlaceType::Land
		|| pt == THUAI6::PlaceType::Door3
		|| pt == THUAI6::PlaceType::Door5
		|| pt == THUAI6::PlaceType::Door6
		|| pt == THUAI6::PlaceType::Grass
		|| pt == THUAI6::PlaceType::Window;
}

template<typename IFooAPI>
void AStarPlus<IFooAPI>::BackwardExpand(Point Source, int H[50][50])
{
	for (int i = 0; i < 50; i++)
		for (int j = 0; j < 50; j++)
			H[i][j] = 10000000;
	H[Source.x][Source.y] = 0;
	std::queue<Point> bfs;
	bfs.push(Source);
	while (!bfs.empty())
	{
		Point now(bfs.front());
		bfs.pop();
//		std::cerr << "bfs " << now.x << ' ' << now.y << std::endl;
		for (int i = now.x - 1; i <= now.x + 1; i++)
			for (int j = now.y - 1; j <= now.y + 1; j++)
				if (abs(i - now.x) + abs(j - now.y) == 1 && i >= 0 && i < 50 && j >= 0 && j < 50 && IsAccessible(API.GetPlaceType(i, j)))
				{
					if (H[now.x][now.y] + 10000 < H[i][j])
					{
						H[i][j] = H[now.x][now.y] + 10000;
						bfs.push(Point(i, j));
//						std::cerr << "bfs next" << i << ' ' << j << std::endl;
					}
				}
	}
//	for (int i = 0; i < 50; i++)
//	{
//		for (int j = 0; j < 50; j++)
//			std::cerr << (H[i][j] == 10000000 ? -1 : H[i][j]) << ' ';
//		std::cerr << std::endl;
//	}
}

// static int statistic;

template<typename IFooAPI>
bool AStarPlus<IFooAPI>::DirectReachable(GeometryPoint A, GeometryPoint B, bool IsDest) // if IsDest, allow 1 through
{
	GeometrySegment AB(A, B);
	int icnt = 0;
	for (auto s : StableMap)
	{
		if (Intersect(AB, s)) icnt++;
		if (!IsDest && icnt == 1 || IsDest && icnt == 2) return false;
//		statistic++;
//		if (statistic % 10000000 == 0)
//		{
//			std::cerr << "statistic count " << statistic << std::endl;
//		}
	}
	for (auto s : VariableMap)
	{
		if (Intersect(AB, s)) icnt++;
		if (!IsDest && icnt == 1 || IsDest && icnt == 2) return false;
//		statistic++;
//		if (statistic % 10000000 == 0)
//		{
//			std::cerr << "statistic count " << statistic << std::endl;
//		}
	}
	return true;
}

template<typename IFooAPI>
void AStarPlus<IFooAPI>::InitStableMap()
{
	auto map = API.GetFullMap();
	bool visited[50][50];
	memset(visited, 0, sizeof(visited));

	for (int i = 1; i < 50; i++)
	{
		int leftP = 0;
		while (leftP < 50)
		{
			if (IsAccessible(map[i - 1][leftP]) == IsAccessible(map[i][leftP])) leftP++;
			else
			{
				int rightP = leftP;
				bool upWall = IsAccessible(map[i][leftP]);
				while (IsAccessible(map[i - 1][rightP + 1]) == IsAccessible(map[i - 1][leftP]) && IsAccessible(map[i][rightP + 1]) == IsAccessible(map[i][leftP])) rightP++;
				if (upWall)
				{
					GeometryPoint LP, RP;
					if (IsAccessible(map[i][leftP-1])) LP = GeometryPoint(1000 * i + SegmentRadius, 1000 * leftP - SegmentCompensate);
					else LP = GeometryPoint(1000 * i + SegmentRadius, 1000 * leftP + SegmentRadius);
					if (IsAccessible(map[i][rightP+1])) RP = GeometryPoint(1000 * i + SegmentRadius, 1000 * (rightP + 1) + SegmentCompensate);
					else RP = GeometryPoint(1000 * i + SegmentRadius, 1000 * (rightP + 1) - SegmentRadius);
					StableMap.push_back(GeometrySegment(RP, LP));
//					std::cerr << "AddSegment (" << RP.PointX/1000 << ',' << RP.PointY/1000 << ") -> (" << LP.PointX/1000 << ',' << LP.PointY/1000 << ')' << std::endl;
				}
				else
				{
					GeometryPoint LP, RP;
					if (IsAccessible(map[i-1][leftP-1])) LP = GeometryPoint(1000 * i - SegmentRadius, 1000 * leftP - SegmentCompensate);
					else LP = GeometryPoint(1000 * i - SegmentRadius, 1000 * leftP + SegmentRadius);
					if (IsAccessible(map[i-1][rightP+1])) RP = GeometryPoint(1000 * i - SegmentRadius, 1000 * (rightP + 1) + SegmentCompensate);
					else RP = GeometryPoint(1000 * i - SegmentRadius, 1000 * (rightP + 1) - SegmentRadius);
					StableMap.push_back(GeometrySegment(LP, RP));
//					std::cerr << "AddSegment (" << LP.PointX/1000 << ',' << LP.PointY/1000 << ") -> (" << RP.PointX/1000 << ',' << RP.PointY/1000 << ')' << std::endl;
				}
				leftP = rightP + 1;
			}
		}
	}

	for (int i = 1; i < 50; i++)
	{
		int upP = 0;
		while (upP < 50)
		{
			if (IsAccessible(map[upP][i - 1]) == IsAccessible(map[upP][i])) upP++;
			else
			{
				int downP = upP;
				bool leftWall = IsAccessible(map[upP][i]);
				while (IsAccessible(map[downP+1][i-1]) == IsAccessible(map[upP][i-1]) && IsAccessible(map[downP+1][i]) == IsAccessible(map[upP][i])) downP++;
				if (leftWall)
				{
					GeometryPoint UP, DP;
					if (IsAccessible(map[upP-1][i])) UP = GeometryPoint(1000 * upP - SegmentCompensate, 1000 * i + SegmentRadius);
					else UP = GeometryPoint(1000 * upP + SegmentRadius, 1000 * i + SegmentRadius);
					if (IsAccessible(map[downP+1][i])) DP = GeometryPoint(1000 * (downP + 1) + SegmentCompensate, 1000 * i + SegmentRadius);
					else DP = GeometryPoint(1000 * (downP + 1) - SegmentRadius, 1000 * i + SegmentRadius);
					StableMap.push_back(GeometrySegment(UP, DP));
//					std::cerr << "AddSegment (" << UP.PointX/1000 << ',' << UP.PointY/1000 << ") -> (" << DP.PointX/1000 << ',' << DP.PointY/1000 << ')' << std::endl;
				}
				else
				{
					GeometryPoint UP, DP;
					if (IsAccessible(map[upP-1][i-1])) UP = GeometryPoint(1000 * upP - SegmentCompensate, 1000 * i - SegmentRadius);
					else UP = GeometryPoint(1000 * upP + SegmentRadius, 1000 * i - SegmentRadius);
					if (IsAccessible(map[downP+1][i-1])) DP = GeometryPoint(1000 * (downP + 1) + SegmentCompensate, 1000 * i - SegmentRadius);
					else DP = GeometryPoint(1000 * (downP + 1) - SegmentRadius, 1000 * i - SegmentRadius);
					StableMap.push_back(GeometrySegment(DP, UP));
//					std::cerr << "AddSegment (" << DP.PointX/1000 << ',' << DP.PointY/1000 << ") -> (" << UP.PointX/1000 << ',' << UP.PointY/1000 << ')' << std::endl;
				}
				upP = downP + 1;
			}
		}
	}

	for (int i = 1; i < 50; i++)
		for (int j = 1; j < 50; j++)
		{
			int cntWall = (int)!IsAccessible(map[i - 1][j - 1]) + (int)!IsAccessible(map[i - 1][j]) + (int)!IsAccessible(map[i][j - 1]) + (int)!IsAccessible(map[i][j]);
			if (cntWall == 1)
			{
				GeometryPoint AP, BP, CAP, CBP;
				if (!IsAccessible(map[i - 1][j - 1]))
				{
					AP = GeometryPoint(1000 * i + SegmentCompensate, 1000 * j + SegmentRadius);
					BP = GeometryPoint(1000 * i + SegmentRadius, 1000 * j + SegmentCompensate);
				}
				else if (!IsAccessible(map[i - 1][j]))
				{
					AP = GeometryPoint(1000 * i + SegmentRadius, 1000 * j - SegmentCompensate);
					BP = GeometryPoint(1000 * i + SegmentCompensate, 1000 * j - SegmentRadius);
				}
				else if (!IsAccessible(map[i][j-1]))
				{
					AP = GeometryPoint(1000 * i - SegmentRadius, 1000 * j + SegmentCompensate);
					BP = GeometryPoint(1000 * i - SegmentCompensate, 1000 * j + SegmentRadius);
				}
				else if (!IsAccessible(map[i][j]))
				{
					AP = GeometryPoint(1000 * i - SegmentCompensate, 1000 * j - SegmentRadius);
					BP = GeometryPoint(1000 * i - SegmentRadius, 1000 * j - SegmentCompensate);
				}

				if (!IsAccessible(map[i - 1][j - 1]))
				{
					CAP = GeometryPoint(1000 * i + CheckPointCompensate, 1000 * j + CheckPointRadius);
					CBP = GeometryPoint(1000 * i + CheckPointRadius, 1000 * j + CheckPointCompensate);
				}
				else if (!IsAccessible(map[i - 1][j]))
				{
					CAP = GeometryPoint(1000 * i + CheckPointRadius, 1000 * j - CheckPointCompensate);
					CBP = GeometryPoint(1000 * i + CheckPointCompensate, 1000 * j - CheckPointRadius);
				}
				else if (!IsAccessible(map[i][j-1]))
				{
					CAP = GeometryPoint(1000 * i - CheckPointRadius, 1000 * j + CheckPointCompensate);
					CBP = GeometryPoint(1000 * i - CheckPointCompensate, 1000 * j + CheckPointRadius);
				}
				else if (!IsAccessible(map[i][j]))
				{
					CAP = GeometryPoint(1000 * i - CheckPointCompensate, 1000 * j - CheckPointRadius);
					CBP = GeometryPoint(1000 * i - CheckPointRadius, 1000 * j - CheckPointCompensate);
				}
				StableMap.push_back(GeometrySegment(AP, BP));
//				std::cerr << "AddSegment (" << AP.PointX/1000 << ',' << AP.PointY/1000 << ") -> (" << BP.PointX/1000 << ',' << BP.PointY/1000 << ')' << std::endl;
				StableCheckPoint.push_back(CAP);
				StableCheckPoint.push_back(CBP);
			}
		}
	std::cerr << StableCheckPoint.size() << std::endl;
}

template<typename IFooAPI>
bool AStarPlus<IFooAPI>::InsideObstacle(GeometryPoint P)
{
	double rot = 0;
	for (auto s : StableMap) rot += s.GetTheta(P);
	for (auto s : VariableMap) rot += s.GetTheta(P);
	std::cerr << "Rot = " << rot / (2 * PI) << " (cycle)" << std::endl;
	if (fabs(rot - 2 * PI) < PI*0.8) return false;
	else return true;
}

void AStarPlus<ITrickerAPI&>::AddPlayer()
{
	std::cerr << "Add Player" << std::endl;
	auto stuinfo = API.GetStudents();
	double rads = (400 + SegmentRadius) / cos(PI / 8);
	for (auto s : stuinfo)
	{
		GeometryPoint P[8], CP[8];
		for (int i = 0; i < 8; i++)
		{
			double theta = i * PI / 4 + PI / 8;
			P[i] = GeometryPoint(s->x + rads * cos(theta), s->y + rads * sin(theta));
			CP[i] = GeometryPoint(s->x + rads * cos(theta), s->y + rads * sin(theta));
		}
		for (int i = 1; i < 8; i++)
		{
			VariableMap.push_back(GeometrySegment(P[i], P[i-1]));
//			std::cerr << "AddSegment (" << P[i].PointX/1000 << ',' << P[i].PointY/1000 << ") -> (" << P[i-1].PointX/1000 << ',' << P[i-1].PointY/1000 << ')' << std::endl;
			VariableCheckPoint.push_back(CP[i]);
		}
		VariableMap.push_back(GeometrySegment(P[0], P[7]));
		VariableCheckPoint.push_back(CP[0]);
	}
}

template<typename IFooAPI>
void AStarPlus<IFooAPI>::AddPlayer()
{
//	std::cerr << "Add Player" << std::endl;
	auto stuinfo = API.GetStudents();
	double rads = (400 + SegmentRadius) / cos(PI / 8);
	for (auto s : stuinfo)
	{
		if (s->playerID == API.GetSelfInfo()->playerID) continue;
		GeometryPoint P[8], CP[8];
		for (int i = 0; i < 8; i++)
		{
			double theta = i * PI / 4 + PI / 8;
			P[i] = GeometryPoint(s->x + rads * cos(theta), s->y + rads * sin(theta));
			CP[i] = GeometryPoint(s->x + rads * cos(theta), s->y + rads * sin(theta));
		}
		for (int i = 1; i < 8; i++)
		{
			VariableMap.push_back(GeometrySegment(P[i], P[i-1]));
//			std::cerr << "AddSegment (" << P[i].PointX/1000 << ',' << P[i].PointY/1000 << ") -> (" << P[i-1].PointX/1000 << ',' << P[i-1].PointY/1000 << ')' << std::endl;
			VariableCheckPoint.push_back(CP[i]);
		}
		VariableMap.push_back(GeometrySegment(P[0], P[7]));
		VariableCheckPoint.push_back(CP[0]);
	}

	auto trkinfo = API.GetTrickers();
	for (auto s : trkinfo)
	{
		if (s->playerID == API.GetSelfInfo()->playerID) continue;
		GeometryPoint P[8], CP[8];
		for (int i = 0; i < 8; i++)
		{
			double theta = i * PI / 4 + PI / 8;
			P[i] = GeometryPoint(s->x + rads * cos(theta), s->y + rads * sin(theta));
			CP[i] = GeometryPoint(s->x + rads * cos(theta), s->y + rads * sin(theta));
		}
		for (int i = 1; i < 8; i++)
		{
			VariableMap.push_back(GeometrySegment(P[i], P[i-1]));
//			std::cerr << "AddSegment (" << P[i].PointX/1000 << ',' << P[i].PointY/1000 << ") -> (" << P[i-1].PointX/1000 << ',' << P[i-1].PointY/1000 << ')' << std::endl;
			VariableCheckPoint.push_back(CP[i]);
		}
		VariableMap.push_back(GeometrySegment(P[0], P[7]));
		VariableCheckPoint.push_back(CP[0]);
	}
}

class AStarPlusNode
{
public:
	double valueQ, valueH;
	bool parenttype,type;
	int parentid, id;
	double value() const { return valueQ + valueH; }
	AStarPlusNode(double valueQ_, double valueH_, bool type_, int id_, bool parenttype_, int parentid_)
		: valueQ(valueQ_), valueH(valueH_), type(type_), id(id_), parenttype(parenttype_), parentid(parentid_) {}

	bool operator< (const AStarPlusNode& n) const { return value() > n.value(); }
};

template<typename IFooAPI>
std::vector<GeometryPoint> AStarPlus<IFooAPI>::FindPath(GeometryPoint From_, GeometryPoint Dest_)
{
	std::cerr << "Destination is (" << Dest_.PointX << ' ' << Dest_.PointY << ')' << std::endl;
	AddPlayer();
	std::vector<GeometryPoint> Path;
	bool x = InsideObstacle(From_);
	if (x)
	{
		Path.push_back(Escape(From_));
		VariableCheckPoint.clear();
		VariableMap.clear();
		return Path;
	}
	int preH[50][50];
	BackwardExpand(Point(Dest_.PointX / 1000, Dest_.PointY / 1000), preH);
	std::vector<double> StableF(StableCheckPoint.size(), 1e10);
	VariableCheckPoint.push_back(From_);
	VariableCheckPoint.push_back(Dest_);
	std::vector<double> VariableF(VariableCheckPoint.size(), 1e10);

	std::vector<std::pair<double, int> > StableParent(StableCheckPoint.size());
	std::vector<std::pair<double, int> > VariableParent(VariableCheckPoint.size());
	VariableParent[VariableCheckPoint.size() - 1].second = -1;
	std::priority_queue<AStarPlusNode> Q;
	Q.push(AStarPlusNode(0, preH[(int)From_.PointX/1000][(int)From_.PointY/1000], 1, VariableCheckPoint.size() - 2, 1, VariableCheckPoint.size() - 2));
	int cnt1 = 0, cnt2 = 0;
	while (true)
	{
		AStarPlusNode A(Q.top());
		Q.pop();
		GeometryPoint APoint = (A.type ? VariableCheckPoint : StableCheckPoint)[A.id];
		if (A.value() > (A.type ? VariableF : StableF)[A.id]) continue;
//		if (A.type || true)
//		{
//			std::cerr << "AStaring Point: " << APoint.PointX << ' ' << APoint.PointY << ' ' << A.value() << std::endl;
//			std::cerr << "Last Point: " << A.parenttype << ' ' << A.parentid << std::endl;
//		}
		(A.type ? VariableF : StableF)[A.id] = A.value();
		(A.type ? VariableParent : StableParent)[A.id] = std::make_pair(A.parenttype, A.parentid);
//		std::cerr << "Fine." << std::endl;
		if (A.type && A.id == VariableCheckPoint.size() - 1)
		{
			break;
		}
		for (int i = 0; i < StableCheckPoint.size(); i++)
		{
			cnt1++;
			GeometryPoint p = StableCheckPoint[i];
			if (DirectReachable(APoint, p))
			{
				cnt2++;
//				std::cerr << "AStaring Next Point: " << p.PointX << ' ' << p.PointY << std::endl;
				Q.push(AStarPlusNode(A.valueQ + Distance(APoint, p) + 1, preH[(int)p.PointX/1000][(int)p.PointY/1000], 0, i, A.type, A.id));
//				Sleep(1000);
			}
		}
//		std::cerr << "Fine.2" << std::endl;
//		Sleep(1000);
		for (int i = 0; i < VariableCheckPoint.size(); i++)
		{
			cnt1++;
			GeometryPoint p = VariableCheckPoint[i];
			if (DirectReachable(APoint, p))
			{
				cnt2++;
//				std::cerr << "AStaring Next Point: " << p.PointX << ' ' << p.PointY << std::endl;
				Q.push(AStarPlusNode(A.valueQ + Distance(APoint, p) + 1, preH[(int)p.PointX/1000][(int)p.PointY/1000], 1, i, A.type, A.id));
//				Sleep(1000);
			}
		}
//		std::cerr << "Fine.6" << std::endl;
//		Sleep(1000);
	}
	if (VariableParent[VariableCheckPoint.size() - 1].second != -1)
	{
		Path.push_back(Dest_);
		bool tp = true;
		int id = VariableCheckPoint.size() - 1;
		while (!(tp && id == VariableCheckPoint.size() - 2))
		{
			auto pr = (tp ? VariableParent : StableParent)[id];
			assert(!(tp == pr.first && id == pr.second));
			tp = pr.first;
			id = pr.second;
			GeometryPoint last = (tp ? VariableCheckPoint : StableCheckPoint)[id];
//			std::cerr << "AStaring Traceback Point: " << last.PointX << ' ' << last.PointY << std::endl;
			Path.push_back(last);
		}
	}
	std::cerr << "cnt1 = " << cnt1 << ';' << "cnt2 = " << cnt2 << std::endl;
	std::reverse(Path.begin(), Path.end());
	VariableCheckPoint.clear();
	VariableMap.clear();
	return Path;
}
