# THUAI6_team_SingleFile

> 以该版本为最终提交于<a href="https://eesast.com/contest/211b9ac2-f004-489d-bd71-4bdde335b597/codes">EESAST</a>的版本。

## 0 基本信息

已经整合为单文件`AI.cpp`。

每个`Student/Tricker`都有三个好朋友，地理学家`Alice`、预言家`Bob`和鸽子`Gugu`。他们能力各异，但是都很热心地帮助`S/T`。

在这次合作中，他们组成了一个指挥所，`S/T`作为指挥所`Center`的长官，将不时向三个朋友提出疑问或请求，并且采纳朋友们的建议来做出更好的决策。

### 类信息

类`CommandPost`（指挥所）保存了所有已知信息，并且具有`Geographer`（地理学家）类对象`Alice`、`Pigeon`（鸽子）类对象`Gugu`和`Predictor`（预言家）类对象`Bob`。

简单操作对应的函数在类`CommandPost`下直接定义。涉及寻路等复杂地图操作的函数由`Geographer`实现。涉及通信的操作由`Pigeon`实现。涉及位置记忆和预测的操作由`Predictor`实现。

由`CommandPost`基类派生了`CommandPostStudent`和`CommandPostTricker`，用于增加定义涉及角色控制的函数（如攻击）。

### 关于坐标

程序区别了三种坐标`Cell``Grid``Geop`。

`Cell`是格子，范围是`0~49`的整数；`Grid`是像素，范围是`0~49999`的整数；`Geop`是实坐标。

提供了三种坐标相互转换的方法，例如`Grid Cell::ToGrid();`。

### 新寻路算法

`#define USE_NEW_ASTAR 0`不启用新寻路算法，`#define USE_NEW_ASTAR 1`将启用新寻路算法。

新的寻路算法基于原来寻路算法的路径，给出斜线直达的方案。目前新寻路算法尚未完善，因此启用将会导致编译不通过。

## 1 CommandPost指挥所

- [ ] `CommandPost(IFooAPI &api);`

- [ ] `void Update(MapUpdateInfo upinfo, int t_);`			//更新地图信息，比如门和隐藏校门，需要约定info的格式
- [ ] `std::vector<THUAI6::PropType> GetInventory() { return Inventory; }`	// 查看背包
- [ ] `void OrganizeInventory(std::vector<unsigned char>Priority);`			// 整理背包

- [ ] `bool MoveToAccurate(Cell Dest, bool WithWindows = true);`
- [ ] `bool MoveTo(Cell Dest, bool WithWindows);`		// 往目的地动一动
- [ ] `bool MoveToNearestClassroom(bool WithWindows);`	// 往最近的作业的方向动一动
- [ ] `bool MoveToNearestGate(bool WithWindows);`		// 往最近的关闭的校门旁边动一动
- [ ] `bool MoveToNearestOpenGate(bool WithWindows);`	// 往最近的开启的校门旁边动一动
- [ ] `bool MoveToNearestChest(bool WithWindows);`		// 往最近的箱子的方向动一动

- [ ] `bool NearCell(Cell P, int level = 1);`         // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
- [ ] `bool NearClassroom(bool checkProgress);`							// 已经在作业旁边了吗？
- [ ] `bool NearGate();`								// 已经在关闭的校门旁边了吗？
- [ ] `bool NearOpenGate();`							// 已经在开启的校门旁边了吗？
- [ ] `bool NearChest();`								// 已经在箱子旁边了吗？
- [ ] `bool NearWindow();`								// 已经在窗户旁边了吗？
- [ ] `bool InGrass();`									// 已经在草丛里了吗？

- [ ] `void DirectLearning(bool WithWindows);`			// 前往最近的作业并学习
- [ ] `void DirectOpeningChest(bool WithWindows);`		// 前往最近的箱子并开箱
- [ ] `void DirectOpeningGate(bool WithWindows, bool CanDirectGraduate);`		// 前往最近的关闭的校门并开门
- [ ] `void DirectGraduate(bool WithWindows);`			// 前往最近的开启的校门并毕业
- [ ] `void DirectGrass(bool WithWindows);`				// 前往最近的草丛并躲避
- [ ] `void DirectHide(Cell TrickerLocation, int TrickerViewRange, bool WithWindows);`				// 前往最适合的草丛并躲避
- [ ] `void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);`		// 前往已知价值最高的道具并捡道具
- [ ] `void DirectUseProp(std::vector<unsigned char>Priority);`

- [ ] `int CountFinishedClassroom() const;`
- [ ] `int CountNonemptyChest() const;`
- [ ] `int CountHiddenGate() const;`
- [ ] `int CountClosedGate() const;`
- [ ] `int CountOpenGate() const;`

- [ ] `int GetChestProgress(int cellx, int celly);`
- [ ] `int GetGateProgress(int cellx, int celly);`
- [ ] `int GetClassroomProgress(int cellx, int celly);`
- [ ] `int GetDoorProgress(int cellx, int celly);`
- [ ] `int GetChestProgress(Cell cell) const;`
- [ ] `int GetGateProgress(Cell cell) const;`
- [ ] `int GetClassroomProgress(Cell cell) const;`
- [ ] `int GetDoorProgress(Cell cell) const;`

### CommandPostStudent

- [ ] `CommandPostStudent(IStudentAPI& api) : CommandPost(api) {}`
- [ ] `void AutoUpdate();`

- [ ] `void TeacherPunish();`
- [ ] `double TeacherPunishCD();`

- [ ] `void StraightAStudentWriteAnswers();`
- [ ] `double StraightAStudentWriteAnswersCD();`

- [ ] `void AtheleteCanBeginToCharge();`
- [ ] `double AtheleteCanBeginToChargeCD();`

- [ ] `void SunshineRouse();`
- [ ] `void SunshineEncourage();`
- [ ] `void SunshineInspire();`
- [ ] `double SunshineRouseCD();`
- [ ] `double SunshineEncourageCD();`
- [ ] `double SunshineInspireCD();`

### CommandPostTricker

- [ ] `CommandPostTricker(ITrickerAPI& api) : CommandPost(api) {}`
- [ ] `void AutoUpdate();`

- [ ] `void AssassinDefaultAttack(int stux, int stuy);`	// 刺客普通攻击，传入学生坐标(stux,stuy)
- [ ] `bool AssassinDefaultAttackOver(int rank);`//判断能否稳定命中，传入目前能观察到的学生列表的第几个，从0开始计数
- [ ] `void AssassinBecomeInvisible();`
- [ ] `void AssassinFlyingKnife(int stux, int stuy);`
- [ ] `double AssassinFlyingKnifeCD();`

## 2 Pigeon

要发送信息，只需要调用对应的函数，传入目标角色ID和具体信息。

要接收信息，需要先调用`receiveMessage()`让鸽子尝试代收，如果有信息就会返回对应的信息种类，否则会返回`NoMessage`常量。鸽子会缓存信息字符串，之后再由用户调用读取对应信息的函数就可以返回相应的信息。

```
struct MapUpdateInfo
{
	THUAI6::PlaceType type;
	int x, y, val;
};
typedef std::shared_ptr<const THUAI6::Student> NeedHelpInfo;
typedef std::vector<std::shared_ptr<const THUAI6::Tricker>> TrickerInfo_t;
```

### 发送信息

以下函数中`dest`参数表示发送目标的`PlayerID`。

- [x] `void sendMapUpdate(int64_t dest, MapUpdateInfo muinfo);`
- [x] `void sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val);`
- [ ] `void sendTrickerInfo(int64_t dest, TrickerInfo_t tricker);`
- [ ] `void sendNeedHelp(int64_t dest, NeedHelpInfo self);` // TODO: 由于队友信息透明，所以不用传数据

### 接收信息

以下接收具体信息的函数中，返回值的第一个值是信息发送时的帧数（等价于时间）。

- [x] `int receiveMessage();` // 返回接收到的信息类型
- [x] `std::pair<int, MapUpdateInfo> receiveMapUpdate();`
- [ ] `std::pair<int, TrickerInfo_t> receiveTrickerInfo();`
- [ ] `std::pair<int, int> receiveNeedHelp();`

## 3 Geographer地理学家

`Geographer`主要处理寻路和复杂的地图信息。

- [ ] `Geographer(IFooAPI& api, CommandPost<IFooAPI>& Center_);`
`#if !USE_NEW_ASTAR`
- [ ] `bool IsValidWithoutWindows(int x, int y);`
- [ ] `bool IsValidWithWindows(int x, int y);`
- [ ] `bool IsDestination(int x, int y, Node dest);`
- [ ] `double CalculateH(int x, int y, Node dest);`
- [ ] `std::vector<Node> MakePath(std::array<std::array<Node, 50>, 50> map, Node dest);`
- [ ] `std::vector<Node> AStarWithoutWindows(Node src, Node dest);`
- [ ] `std::vector<Node> AStarWithWindows(Node src, Node dest);`
- [ ] `int EstimateTime(Cell Dest);`					// 去目的地的预估时间
`#else`
- [ ] `std::vector<Geop> FindPath(Geop From_, Geop Dest_);`
`#endif`
- [ ] `bool IsViewable(Cell Src, Cell Dest, int ViewRange);`			// 判断两个位置是否可视

- [ ] `Cell GetNearestGate();`
- [ ] `Cell GetNearestClassroom();` // 仅在没写完的作业中找
- [ ] `Cell GetNearestOpenGate();`

## 4 Predictor预言家

Not implemented.

## 5 建议使用的策略编写方法

定义角色可能处于的状态（已经定义了一些）。

限定每一帧要么维持原状态，要么变换到另一个状态，不能在一帧内多次变化状态（待定）。

具体实现上，使用一个`switch-case`进行不同状态的处理。

## 6 执行顺序

每一帧应当按照以下顺序执行：

1. 调用`Center.AutoUpdate()`进行信息更新；
2. 让`Center.Gugu`接收信息，并对`Gugu`接收的信息进行处理，直到没有新信息，此时可以依据接收的信息进行状态变更的准备；
3. 决定变更状态或维持状态；
4. 执行状态对应的行为。

之后会加入使用道具这一步骤。

## 7 TODO List

## 8 Issues

- [x] ~~开启完校门的角色不会自己毕业，没有开启校门的角色反而能够顺利毕业。~~
- [x] ~~开启校门后没有直接毕业，而是折返，同时会报错。~~
- [ ] 理论上已经判断了不能发给自己，但是没有成功，不过api也会判断所以不是大问题。
- [ ] `AssassinDefaultAttackOver`判断的距离太小了，需要调整。
- [ ] 救人可能存在偶发的解引用报错问题
