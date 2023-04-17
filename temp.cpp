// static bool hasInitMap = false;
// static bool IHaveArrived = false;
// static status BotStatus = status::initial;
// static status LastStatus = status::reset;
static THUAI6::PlaceType map[50][50];
static int vis[50][50];
// static int desire[50][50];
// static int danger[50][50];
// static int wantmovethere[50][50];
// static int CPUMap[50][50];
// static int lastX = 0, lastY = 0;
static int steps;
// static int lastFrameCount = 0;
struct MapNode
{
	int x, y;
};
static MapNode* path[2500];
// static int sorroundbeento = 0;
// static int hasBeenTo = 1;
static MapNode* dest;
static MapNode conplace[50][50];
// static MapNode* Sorroundpath[100] = { &conplace[19][20],&conplace[21][31],&conplace[31][29],&conplace[28][17], &conplace[19][19],&conplace[19][30],&conplace[28][26],&conplace[27][18] };
static bool BFSed = false;
// static MapNode* pastDest;

bool BFS(int x1, int y1, int x2, int y2)
{
	if (BFSed) return true;
	std::queue<MapNode*> bfsq;
	bool exist = false;
	std::memset(vis, 0, sizeof(vis));
	MapNode* current_node = &conplace[x1][y1];
	MapNode* dest_node = &conplace[x2][y2];
	bfsq.push(current_node);
	while (!exist)
	{
		int i, j;
		MapNode* t = bfsq.front();
		//std::printf("dealing %d,%d\n", t->x, t->y);
		bfsq.pop();
		for (i = t->x - 1; i < t->x + 2; i++)
		{
			for (j = t->y - 1; j < t->y + 2; j++)
			{
				if (i < 0 || i>49 || j < 0 || j>49 || map[i][j] == THUAI6::PlaceType::Wall
					|| map[i - 1][j] == THUAI6::PlaceType::Wall || map[i + 1][j] == THUAI6::PlaceType::Wall ||
					map[i][j - 1] == THUAI6::PlaceType::Wall || map[i][j + 1] == THUAI6::PlaceType::Wall) continue;
				if (vis[i][j] == 0)
				{
					vis[i][j] = vis[t->x][t->y] + 1;
					bfsq.push(&conplace[i][j]);
				}
				if (t->x == x2 && t->y == y2)
				{
					exist = true;
					//std::printf("=====find!======\n");
					goto existdeal;
				}
			}
		}
	}
existdeal:
	if (exist)
	{
		int i, j;
		auto tv = vis[dest_node->x][dest_node->y];
		path[tv] = dest_node;
		steps = tv;
		dest = path[tv];
		while (tv > 0)
		{
			MapNode* t = path[tv];
			tv--;
			for (i = t->x - 1; i < t->x + 2; i++)
			{
				bool find = false;
				for (j = t->y - 1; j < t->y + 2; j++)
				{
					if (i < 0 || i>49 || j < 0 || j>49 || map[i][j] == THUAI6::PlaceType::Wall) continue;
					if (vis[i][j] == tv)
					{
						path[tv] = &conplace[i][j];
						find = true;
						break;
					}
				}
				if (find) break;
			}
		}
		//for (int i = 1; path[i] != dest;i++) std::printf("->%d,%d", path[i]->x, path[i]->y);
		return true;
	}
	return false;
}
void Goto(ITrickerAPI& api, int destX, int destY)
{

	//std::printf("goto %d,%d\n", destX, destY);
	//if (map[destX][destY] == THUAI5::PlaceType::Wall) return;
	auto self = api.GetSelfInfo();
	int sx = self->x;
	int sy = self->y;
	//std::printf("-------------%d,%d------------\n", sx, sy);
	auto delta_x = (double)(destX * 1000 - sx);
	auto delta_y = (double)(destY * 1000 - sy);
	//std::printf("-------------%lf,%lf------------\n", delta_x, delta_y);
	double ang = 0;
	//直接走
	ang = atan2(delta_y, delta_x);
	api.Move(300, ang + (std::rand() % 10 - 10) / 10);
}
void Goto(IStudentAPI& api, int destX, int destY)
{

	//std::printf("goto %d,%d\n", destX, destY);
	//if (map[destX][destY] == THUAI5::PlaceType::Wall) return;
	auto self = api.GetSelfInfo();
	int sx = self->x;
	int sy = self->y;
	//std::printf("-------------%d,%d------------\n", sx, sy);
	auto delta_x = (double)(destX * 1000 - sx);
	auto delta_y = (double)(destY * 1000 - sy);
	//std::printf("-------------%lf,%lf------------\n", delta_x, delta_y);
	double ang = 0;
	//直接走
	ang = atan2(delta_y, delta_x);
	api.Move(300, ang + (std::rand() % 10 - 10) / 10);
}
bool arriveAt(int x1, int y1, int x2, int y2)
{
	if (std::abs(x1 - 1000 * x2) + std::abs(y1 - 1000 * y2) < 100) return true;
	return false;
}
bool RangeAt(int x1, int y1, int x2, int y2)
{
	if (std::abs(x1 - 1000 * x2 - 500) + std::abs(y1 - 1000 * y2 - 500) < 10000) return true;
	return false;
}
