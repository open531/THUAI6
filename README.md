# THUAI6_team_SingleFile

> 以该版本为最终提交于<a href="https://eesast.com/contest/211b9ac2-f004-489d-bd71-4bdde335b597/codes">EESAST</a>的版本。

## 0 基本信息

已经整合为单文件 `AI.cpp`。

每个 `Student/Tricker` 都有三个好朋友，地理学家 `Alice`、预言家 `Bob` 和鸽子 `Gugu`。他们能力各异，但是都很热心地帮助 `S/T`。

在这次合作中，他们组成了一个指挥所，`S/T` 作为指挥所 `Center` 的长官，将不时向三个朋友提出疑问或请求，并且采纳朋友们的建议来做出更好的决策。

### 类信息

类 `CommandPost`（指挥所）保存了所有已知信息，并且具有 `Geographer`（地理学家）类对象 `Alice`、`Pigeon`（鸽子）类对象 `Gugu` 和 `Predictor`（预言家）类对象 `Bob`。

简单操作对应的函数在类 `CommandPost` 下直接定义。涉及寻路等复杂地图操作的函数由 `Geographer` 实现。涉及通信的操作由 `Pigeon` 实现。涉及位置记忆和预测的操作由 `Predictor` 实现。

由 `CommandPost` 基类派生了 `CommandPostStudent` 和 `CommandPostTricker`，用于增加定义涉及角色控制的函数（如攻击）。

### 关于坐标

程序区别了三种坐标 `Cell` `Grid` `Geop`。

`Cell` 是格子，范围是 `0~49` 的整数；`Grid` 是像素，范围是 `0~49999` 的整数；`Geop` 是实坐标。

提供了三种坐标相互转换的方法，例如 `Grid Cell::ToGrid();`。

### 新寻路算法

`#define USE_NEW_ASTAR 0` 不启用新寻路算法，`#define USE_NEW_ASTAR 1` 将启用新寻路算法。

新的寻路算法基于原来寻路算法的路径，给出斜线直达的方案。目前新寻路算法尚未完善，因此启用将会导致编译不通过。

## 1 CommandPost 指挥所

### 可访问的类对象

- `vector<vector<THUAI6::PlaceType>> Map;`

> 50x50的数组，在CommandPost构造时确定，储存内容与api.GetFullMap()完全一致。

- `unsigned char Access[50][50];`

> 随时更新。储存了所有格子的可访问性。有四个取值：
>
> - `0U` 表示不可进入的格子；
> - `1U` 表示窗；
> - `2U` 表示可进入的格子（非草地）；
> - `3U` 表示草地。 
>
> 可能导致变化产生的更新内容包括：
>
> - 门的开和锁；
> - 隐藏校门出现。（未实现）

- `std::vector<Cell> Classroom;`
- `std::vector<Cell> Gate;`
- `std::vector<Cell> HiddenGate;`
- `std::vector<Cell> Chest;`
- `std::vector<Cell> Grass;`
- `std::vector<Doors> Door;`

> 在CommandPost构造时确定，不会变更。储存了所有对应类型格子所在的坐标。（issue: 事实上Door的DoorStatus原意应当更新，但未实现，且可以被上面的Access替代。）

- `int InfoMem[50][50];`

> 随时更新。储存格子的值。可能储存了以下值：
>
> - 对于`Classroom`，储存了学习进度，取值`0~10000000`；
> - 对于`Chest`，储存了开箱进度，取值`0~10000000`；
> - 对于`Gate`，储存了开门进度，取值`0~18000`；
> - 对于`HiddenGate`，应当储存开门进度，但未实现；
> - 对于其他类型的地块，未定义。

- `int LastUpdateFrame[50][50];`

> 表示 `InfoMem` 中记录的信息的最近更新时间。在进行队内信息传输的更新时需要比较这个数组的值以确保 `InfoMem` 仅在得到最新的信息时才发生变化。如果是自己在当前帧可以直接获取的信息（可以直接看到），那么值应当为当前帧数。

- `Geographer Alice;`
- `Predictor Bob;`
- `Pigeon Gugu;`

> 三位专职。

### 构造函数

- [x] `CommandPost(IFooAPI &api);`

- [ ] `void Update(MapUpdateInfo upinfo, int t_);`			//更新地图信息，比如门和隐藏校门，需要约定info的格式
- [ ] `std::vector<THUAI6::PropType> GetInventory() { return Inventory; }`	// 查看背包
- [ ] `void OrganizeInventory(std::vector<unsigned char>Priority);`			// 整理背包

### 移动

- [x] `bool MoveTo(Cell Dest, bool WithWindows);`

> 往目的地移动一次。`WithWindows` 决定路线是否经过窗。如果已经在目的地旁边就返回`false`，否则返回`true`。

- [x] `bool MoveToNearestClassroom(bool WithWindows);`

> 往最近的作业移动一次。`WithWindows` 决定路线是否经过窗。如果有没写完的作业就返回 `true`，否则返回 `false`。（issue: 需要利用MoveTo的返回值以防不能到达的情况，如果都不能到达也应当返回 `false`）

- [x] `bool MoveToNearestGate(bool WithWindows);`

> 往最近的关闭的校门移动一次。`WithWindows` 决定路线是否经过窗。如果有没开启的校门就返回 `true`，否则返回 `false`。（issue: 需要利用MoveTo的返回值以防不能到达的情况，如果都不能到达也应当返回 `false`）

- [x] `bool MoveToNearestOpenGate(bool WithWindows);`

> 往最近的开启的校门移动一次。`WithWindows` 决定路线是否经过窗。如果有开启的校门就返回 `true`，否则返回 `false`。（issue: 需要利用MoveTo的返回值以防不能到达的情况，如果都不能到达也应当返回 `false`）

- [x] `bool MoveToNearestChest(bool WithWindows);`

> 往最近的箱子移动一次。`WithWindows` 决定路线是否经过窗。如果有未开启的箱子就返回 `true`，否则返回 `false`。（issue: 需要利用MoveTo的返回值以防不能到达的情况，如果都不能到达也应当返回 `false`）

### 附近格子状态

- [ ] `bool NearCell(Cell P, int level = 1);`         // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
- [x] `bool NearClassroom(bool checkProgress);`							// 已经在作业旁边了吗？
- [x] `bool NearGate();`								// 已经在关闭的校门旁边了吗？
- [x] `bool NearOpenGate();`							// 已经在开启的校门旁边了吗？
- [x] `bool NearChest();`								// 已经在箱子旁边了吗？
- [x] `bool NearWindow();`								// 已经在窗户旁边了吗？
- [x] `bool InGrass();`									// 已经在草丛里了吗？

### 有目的地移动

- [ ] `void DirectLearning(bool WithWindows);`			// 前往最近的作业并学习
- [ ] `void DirectOpeningChest(bool WithWindows);`		// 前往最近的箱子并开箱
- [ ] `void DirectOpeningGate(bool WithWindows, bool CanDirectGraduate);`		// 前往最近的关闭的校门并开门
- [ ] `void DirectGraduate(bool WithWindows);`			// 前往最近的开启的校门并毕业
- [ ] `void DirectGrass(bool WithWindows);`				// 前往最近的草丛并躲避
- [ ] `void DirectHide(Cell TrickerLocation, int TrickerViewRange, bool WithWindows);`				// 前往最适合的草丛并躲避
- [ ] `void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);`		// 前往已知价值最高的道具并捡道具
- [ ] `void DirectUseProp(std::vector<unsigned char>Priority);`

### 实时计数及特定格子状态

- [x] `int CountFinishedClassroom() const;`
- [x] `int CountNonemptyChest() const;`
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

### CommandPostStudent: 继承自 `CommandPost<IStudentAPI>`

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

### CommandPostTricker: 继承自 `CommandPost<ITrickerAPI>`

- [ ] `CommandPostTricker(ITrickerAPI& api) : CommandPost(api) {}`
- [ ] `void AutoUpdate();`

- [ ] `void AssassinDefaultAttack(int stux, int stuy);`	// 刺客普通攻击，传入学生坐标(stux,stuy)
- [ ] `bool AssassinDefaultAttackOver(int rank);`//判断能否稳定命中，传入目前能观察到的学生列表的第几个，从0开始计数
- [ ] `void AssassinBecomeInvisible();`
- [ ] `void AssassinFlyingKnife(int stux, int stuy);`
- [ ] `double AssassinFlyingKnifeCD();`

## 2 Pigeon 鸽子

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

## 3 Geographer 地理学家

`Geographer`主要处理寻路和复杂的地图信息。

- [x] `Geographer(IFooAPI& api, CommandPost<IFooAPI>& Center_);`

- [ ] `bool IsValidWithoutWindows(int x, int y);`
- [ ] `bool IsValidWithWindows(int x, int y);`
- [ ] `bool IsDestination(int x, int y, Node dest);`
- [ ] `double CalculateH(int x, int y, Node dest);`
- [ ] `std::vector<Node> MakePath(std::array<std::array<Node, 50>, 50> map, Node dest);`
- [ ] `std::vector<Node> AStarWithoutWindows(Node src, Node dest);`
- [ ] `std::vector<Node> AStarWithWindows(Node src, Node dest);`
- [ ] `int EstimateTime(Cell Dest);`					// 去目的地的预估时间

- [ ] `bool IsViewable(Cell Src, Cell Dest, int ViewRange);`			// 判断两个位置是否可视

- [ ] `Cell GetNearestGate();`
- [ ] `Cell GetNearestClassroom();` // 仅在没写完的作业中找
- [ ] `Cell GetNearestOpenGate();`

## 4 Predictor 预言家

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

- 指挥所更新信息时需要告知三位小伙伴。需要本地广播的地图更新信息目前应该只有一种：玩家的实时位置被队友告知。其他地图更新信息由小伙伴自行调用api得知。这些应该对 `Center.AutoUpdate()` 等相关函数进行修改。
- 制定预言家的基本策略，确定接口。

## 8 Issues

- [x] ~~开启完校门的角色不会自己毕业，没有开启校门的角色反而能够顺利毕业。~~
- [x] ~~开启校门后没有直接毕业，而是折返，同时会报错。~~
- [ ] 理论上已经判断了不能发给自己，但是没有成功，不过api也会判断所以不是大问题。
- [ ] `AssassinDefaultAttackOver`判断的距离太小了，需要调整。
- [ ] 救人可能存在偶发的解引用报错问题
- [ ] MoveToNearest等函数中调用AStarWithWindows(AStarWithoutWindows)次数太多导致卡顿。考虑用Alice.BackwardExpand优化。可以选择在AutoUpdate中调用BackwardExpand得到预处理的EstimateTime。
- [ ] AStarWithWindows(AStarWithoutWindows)出现单次运行卡顿的情况。
- [ ] AStarWithWindows(AStarWithoutWindows)频繁报错。
- [ ] AStarWithWindows(AStarWithoutWindows)可能没有处理找不到路的情况。建议用异常处理，在找不到通路时throw一个异常。
- [ ] MoveTo系列的函数在调用MoveTo时没有利用其返回值。
- [ ] CommandPost::Door的DoorStatus本应更新但未实现，且可以被Access替代。
- [ ] MoveTo系列在判断是否需要走的时候依然使用了诸如`!Classroom.empty()`的方式，然而在目前的实现下，`Classroom`等数组不会变更内容。