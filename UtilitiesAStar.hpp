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
	double H = (sqrt((x - dest.x) * (x - dest.x) + (y - dest.y) * (y - dest.y)));
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

// accurate position
class GeometryPoint
{
private:
	double PointX, PointY;

public:
	GeometryPoint(double PX, double PY) : PointX(PX), PointY(PY) {}
	GeometryPoint(const GeometryPoint& P_) : PointX(P_.PointX), PointY(P_.PointY) {}
};

// with direction S->T(can be treated as a vector), right side of the segment represents inside
class GeometrySegment
{
private:
	GeometryPoint S, T;
	GeometrySegment(GeometryPoint PS, GeometryPoint PT) : S(PS), T(PT) {}
};

template<typename IFooAPI>
class AStarPlus
{
private:
	std::vector<GeometrySegment> StableMap;
	std::vector<GeometryPoint> StableCheckPoint;
	std::vector<GeometrySegment> VariableMap;
	std::vector<GeometryPoint> VariableCheckPoint;

	void InitStableMap(IFooAPI& API);
	bool InsideMap(std::vector<GeometrySegment> StableMap)

public:
	bool InsideMap();
	void ResetVariableMap();
	void AddPlayerObstacle();
};
