static bool hasInitMap = false;
static bool IHaveArrived = false;
// static status BotStatus = status::initial;
// static status LastStatus = status::reset;
static THUAI6::PlaceType map[50][50];
static int vis[50][50];
static int desire[50][50];
// static int danger[50][50];
// static int wantmovethere[50][50];
// static int CPUMap[50][50];
static int lastX = 0, lastY = 0;
static int steps;
// static int lastFrameCount = 0;
struct MapNode
{
	int x, y;
};
static MapNode* path[2500];
// static int sorroundbeento = 0;
static int hasBeenTo = 1;
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
//int CalculateDesire(int i, int j)
//{
//	int ii, jj, res = 0;
//	for (ii = i - 1; ii < i + 2; ii++)
//		for (jj = j - 1; jj < j + 2; jj++)
//		{
//			if (ii < 0 || ii>49 || jj < 0 || jj>49) continue;
//			switch (map[ii][jj])
//			{
//			case THUAI6::PlaceType::Wall:
//				break;
//			case THUAI6::PlaceType::Grass:
//				res += 15;
//				break;
//			default:
//				res += 10;
//			}
//		}
//	return res;
//}
//MapNode* generateDesire(IStudentAPI& api)
//{
//	//auto self = api.GetSelfInfo();
//	//MapNode* t = &conplace[0][0];
//	//memset(desire, 0, sizeof(desire));
//	//auto props = api.GetProps();
//	////std::cout << "props are" << props.size()<<std::endl;
//	//for (int i = 0; i < props.size(); i++)
//	//{
//	//	switch (props[i]->type) {
//	//		case(THUAI6::PropType::CPU):
//	//			for (int xi = props[i]->x / 1000 - 1; xi < props[i]->x / 1000 + 3; xi++)
//	//			{
//	//				for (int yi = props[i]->y / 1000 - 1; yi < props[i]->y / 1000 + 3; yi++)
//	//				{
//	//					if (xi >= 0 && yi >= 0 && xi < 50 && yi < 50) desire[xi][yi] += 200 * props[i]->size;
//	//				}
//	//			}
//	//			break;
//	//		}
//	//	}
//	//	//std::cout << "getpropsok" << std::endl;
//	//	//auto bots = api.GetRobots();
//	//	//for (int i = 0; i < bots.size(); i++)
//	//	//{
//	//	//	if (bots[i]->teamID == self->teamID)
//	//	//	{
//	//	//		desire[bots[i]->x / 1000][bots[i]->y / 1000] += 100;
//	//	//	}
//	//	//}
//	//	//std::cout << "getbotsok" << std::endl;
//	//	if (hasInitMap)
//	//	{
//	//		for (int i = 0; i < 50; i++)
//	//			for (int j = 0; j < 50; j++)
//	//			{
//	//				desire[i][j] += CalculateDesire(i, j);
//	//				desire[i][j] -= std::abs(self->x / 1000 - i) + std::abs(self->y / 1000 - j);
//	//				if (desire[i][j] > desire[t->x][t->y])
//	//				{
//	//					t = &conplace[i][j];
//	//				}
//	//			}
//	//	}
//	//	return t;
//	//}
//}
//void ContinueMyPath(std::shared_ptr<const THUAI6::Student> self, IStudentAPI& api)
//{
//	//MapNode* maxdesire = generateDesire(api);
//	//BFSed = BFS((int)self->x / 1000, (int)self->y / 1000, maxdesire->x, maxdesire->y);
//	BFSed = BFS((int)self->x / 1000, (int)self->y / 1000, 48, 48);
//	//optim(hasBeenTo, dest);
//	hasBeenTo = 1;
//	//for (MapNode* p = hasBeenTo; p < dest; p++) std::printf("->%d,%d", p->x, p->y);
//	if (arriveAt(self->x, self->y, path[hasBeenTo]->x, path[hasBeenTo]->y))
//	{
//		//std::printf("arriveat %d,%d\n", hasBeenTo->x, hasBeenTo->y);
//		hasBeenTo++;
//	}
//	Goto(api, path[hasBeenTo]->x, path[hasBeenTo]->y);
//}
bool setDest(IStudentAPI& api, int dest_X, int dest_Y)
{
	//if (BFSed) return true;
	auto self = api.GetSelfInfo();
	bool success = BFS((int)(self->x / 1000), (int)(self->y / 1000), dest_X, dest_Y);
	if (!success) return false;
	hasBeenTo = 1;
	//BFSed = true;
	return true;
}
void GoToDest(IStudentAPI& api)
{
	auto self = api.GetSelfInfo();
	if (std::abs(lastX - (int)api.GetSelfInfo()->x) + std::abs(lastY - (int)api.GetSelfInfo()->y) < 100)
	{
		//lastX = api.GetSelfInfo()->x;
		//lastY = api.GetSelfInfo()->y;
		api.Move(200, std::rand());
		return;
	}
	//lastX = api.GetSelfInfo()->x;
	//lastY = api.GetSelfInfo()->y;
	if (arriveAt(self->x, self->y, path[hasBeenTo]->x, path[hasBeenTo]->y))
	{
		if (path[hasBeenTo] != dest) hasBeenTo++;
		else
		{
			IHaveArrived = true;
		}
	}
	if (!IHaveArrived) Goto(api, path[hasBeenTo]->x, path[hasBeenTo]->y);
}
