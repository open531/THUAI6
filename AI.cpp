#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include <stack>
#include <queue>
#include <algorithm>

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数

static bool hasInitMap;
static unsigned char Map[50][50];
static unsigned char Access[50][50];

void InitMap(IStudentAPI& api)
{
    int32_t i, j;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            Map[i][j] = (unsigned char)api.GetPlaceType(i, j);
            switch (Map[i][j])
            {
            case 2U://Wall
            case 4U:
            case 11U:
            case 5U:
            case 6U:
                Access[i][j] = 0U;
                break;
            case 3U://Grass
                Access[i][j] = 3U;
                break;
            case 7U://Window
                Access[i][j] = 1U;
                break;
            case 8U://Door3
            case 9U://Door5
            case 10U://Door6
                Access[i][j] = api.IsDoorOpen(i, j) ? 2U : 0U;
                break;
            default:
                Access[i][j] = 2U;
                break;
            }
        }
    }
}

struct Node
{
    int y;
    int x;
    int parentX;
    int parentY;
    float gCost;
    float hCost;
    float fCost;
};

inline bool operator < (const Node& lhs, const Node& rhs)
{
    return lhs.fCost < rhs.fCost;
}

bool IsValidWithoutWindows(int x, int y)
{
    return (bool)(Access[x][y] / 2);
}

bool IsValidWithWindows(int x, int y)
{
    return (bool)Access[x][y];
}

static bool IsDestination(int x, int y, Node dest)
{
    if (x == dest.x && y == dest.y)
    {
        return true;
    }
    else
    {
        return false;
    }
}

double CalculateH(int x, int y, Node dest) 
{
    double H = (sqrt((x - dest.x) * (x - dest.x) + (y - dest.y) * (y - dest.y)));
    return H;
}

std::vector<Node> MakePath(std::array<std::array<Node, 50>, 50> map, Node dest) {
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
    std::cerr << "TEST";
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
    std::cerr << "TEST*";
    int x = src.x;
    int y = src.y;
    std::cerr << x << ' ' << y;
    AStarMap[x][y].fCost = 0.0;
    AStarMap[x][y].gCost = 0.0;
    AStarMap[x][y].hCost = 0.0;
    AStarMap[x][y].parentX = x;
    AStarMap[x][y].parentY = y;
    std::cerr << "TEST**";
    std::vector<Node> OpenList;
    OpenList.emplace_back(AStarMap[x][y]);
    bool FoundDest = false;
    while (!OpenList.empty() && OpenList.size() < 50 * 50) {
		std::cerr << "TEST"<<OpenList.size();
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

Node GetSelfNode(IStudentAPI& api)
{
    Node ret;
    ret.x = api.GetSelfInfo()->x/1000;
    ret.y = api.GetSelfInfo()->y/1000;
    ret.fCost = FLT_MAX;
    ret.gCost = FLT_MAX;
    ret.hCost = FLT_MAX;
    ret.parentX = -1;
    ret.parentY = -1;
    return ret;
}

Node GetSelfNode(ITrickerAPI& api)
{
    Node ret;
    ret.x = api.GetSelfInfo()->x/1000;
    ret.y = api.GetSelfInfo()->y/1000;
    ret.fCost = FLT_MAX;
    ret.gCost = FLT_MAX;
    ret.hCost = FLT_MAX;
    ret.parentX = -1;
    ret.parentY = -1;
    return ret;
}

void MoveDest(IStudentAPI& api,std::vector<Node> UsablePath)
{
    if (UsablePath.size() < 2) return;
    int tx = UsablePath[1].x * 1000 + 500;
    int ty = UsablePath[1].y * 1000 + 500;
    int sx = api.GetSelfInfo()->x;
    int sy = api.GetSelfInfo()->y;
    int dx = tx - sx;
    int dy = ty - sy;
    std::cerr << dx << ' ' << dy << std::endl;
    if(Map[tx/1000][ty/1000]!=7U)
    {
        api.Move(1000*sqrt(dx * dx + dy * dy) / api.GetSelfInfo()->speed, atan2(dy, dx));
    }
    else
    {
        api.SkipWindow();
    }
}

void MoveDest(ITrickerAPI& api, std::vector<Node> UsablePath)
{
    int tx = UsablePath[1].x * 1000 + 500;
    int ty = UsablePath[1].y * 1000 + 500;
    int sx = api.GetSelfInfo()->x;
    int sy = api.GetSelfInfo()->y;
    int dx = tx - sx;
    int dy = ty - sy;
    if (Map[tx][ty] != 7U)
    {
        api.Move((int)(1000*sqrt(dx * dx + dy * dy) / api.GetSelfInfo()->speed), atan2(dy, dx));
    }
    else
    {
        api.SkipWindow();
    }
}

void GoToWithoutWindows(IStudentAPI& api, Node dest)
{
    MoveDest(api, AStarWithoutWindows(GetSelfNode(api), dest));
}

void GoToWithoutWindows(ITrickerAPI& api, Node dest)
{
    MoveDest(api, AStarWithoutWindows(GetSelfNode(api), dest));
}

void GoToWithWindows(IStudentAPI& api, Node dest)
{
    MoveDest(api, AStarWithWindows(GetSelfNode(api), dest));
}

void GoToWithWindows(ITrickerAPI& api, Node dest)
{
    MoveDest(api, AStarWithWindows(GetSelfNode(api), dest));
}

void AI::play(IStudentAPI& api)
{

    // 公共操作

    if (!hasInitMap)
    {
        InitMap(api);
        hasInitMap = true;
    }
   // for (int i = 0; i < 50; i++) {
   //     for (int j = 0; j < 50; j++)
   //     {
   //         if (api.IsDoorOpen(i, j)) std::cerr << i << ' ' << j;
   //         assert(!api.IsDoorOpen(i, j));
			//std::cerr << int(Map[i][j]) << ' ';
   //     }
   //     std::cerr << std::endl;
   // }
    //for (int i = 0; i < 50; i++) {
    //    for (int j = 0; j < 50; j++)
    //        std::cerr << int(Access[i][j]) << ' ';
    //    std::cerr << std::endl;
    //}
    if (this->playerID == 0)
    {
        GoToWithWindows(api, { 48, 48 });
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
