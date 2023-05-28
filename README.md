<h1><center>努力少女戏尔危</center></h1>
<h4><center>清华大学第六届人工智能挑战赛深度“学习”——毕业吧少女</center></h4>

<center><a href="https://github.com/Panxuc">潘徐成</a> <a href="https://github.com/always-del">孙宇琦</a> <a href="https://github.com/ytxytx">叶腾逊 [广告位招租]</a> <a href="https://github.com/DGFDIE">杜港枫</a></center>

---

> 该版本为“努力少女戏尔危”最终提交于[EESAST](https://eesast.com/contest/211b9ac2-f004-489d-bd71-4bdde335b597/codes)的版本。

> “努力少女戏尔危”在清华大学第六届人工智能挑战赛深度“学习”——毕业吧少女中荣获**三等奖**。

> 清华大学第六届人工智能挑战赛电子系赛道（原电子系第 24 届队式程序设计大赛 teamstyle24）
>
> GitHub 镜像地址：[THUAI6: GitHub Mirror](https://github.com/eesast/THUAI6)
>
> Gitee 镜像地址：[THUAI6: Gitee Mirror](https://gitee.com/eesast/THUAI6)
>
> GitLink 镜像地址：[THUAI6: GitLink Mirror](https://www.gitlink.org.cn/EESAST/THUAI6)
>
> 项目主页：[THUAI6 Project Home Page](https://eesast.github.io/THUAI6)
>
> 关于本届及历届清华大学人工智能挑战赛与队式程序设计大赛的更多内容参见：[THUAI6 GitHub Wiki](https://github.com/eesast/THUAI6/wiki)

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

- [x] `void Update(MapUpdateInfo upinfo, int t_);`			//更新地图信息，比如门和隐藏校门，需要约定info的格式
- [ ] `std::vector<THUAI6::PropType> GetInventory() { return Inventory; }`	// 查看背包
- [ ] `void OrganizeInventory(std::vector<unsigned char>Priority);`			// 整理背包

- [x] `bool IsAccessible(int x, int y, bool WithWindows);`

> 返回该格子是否为可进入的格子。如果 `WithWindows` 则视窗户为可进入的格子。

### 移动

- [x] `bool MoveTo(Cell Dest, bool WithWindows);`

> 往目的地移动一次。`WithWindows` 决定路线是否经过窗。如果已经在目的地旁边就返回`false`，否则返回`true`。

- [x] `bool MoveToNearestClassroom(bool WithWindows);`

> 往最近的作业移动一次。`WithWindows` 决定路线是否经过窗。如果有没写完的作业就返回 `true`，否则返回 `false`。

- [x] `bool MoveToNearestGate(bool WithWindows);`

> 往最近的关闭的校门移动一次。`WithWindows` 决定路线是否经过窗。如果有没开启的校门就返回 `true`，否则返回 `false`。

- [x] `bool MoveToNearestOpenGate(bool WithWindows);`

> 往最近的开启的校门移动一次。`WithWindows` 决定路线是否经过窗。如果有开启的校门就返回 `true`，否则返回 `false`。

- [x] `bool MoveToNearestChest(bool WithWindows);`

> 往最近的箱子移动一次。`WithWindows` 决定路线是否经过窗。如果有未开启的箱子就返回 `true`，否则返回 `false`。

### 附近格子状态

- [x] `bool NearCell(Cell P, int level = 1);`

> 判断是否在格子P周围。level的不同取值决定范围大小。
>
> `level = 0` 判断当前是否在该格子上；
> `level = 1` 判断是否在格子上或周围4格；
> `level = 2` 判断是否在格子上或周围8格；
> `level = 3` 判断直线距离是否不超过3格；
> `level = 4` 判断直线距离是否不超过4格。

- [x] `bool NearClassroom(bool checkProgress);`

> 判断是否在作业旁边。如果 `checkProgress = true` 则判断是否在没写完的作业旁边。

- [x] `bool NearGate();`

> 已经在关闭的校门旁边了吗？

- [x] `bool NearOpenGate();`

> 已经在开启的校门旁边了吗？

- [x] `bool NearChest();`

> 已经在没开过的箱子旁边了吗？

- [x] `bool NearWindow();`

> 已经在窗户旁边了吗？

- [x] `bool InGrass();`

> 已经在草丛里了吗？

### 有目的地移动

- [x] `void DirectLearning(bool WithWindows);`

> 前往最近的作业并学习。如果 `WithWindows = true` 则允许路线经过窗户。

- [x] `void DirectOpeningChest(bool WithWindows);`

> 前往最近的没开过的箱子并开箱。如果 `WithWindows = true` 则允许路线经过窗户。

- [x] `void DirectOpeningGate(bool WithWindows, bool CanDirectGraduate);`

> 前往最近的关闭的校门并开门。如果 `WithWindows = true` 则允许路线经过窗户。

- [x] `void DirectGraduate(bool WithWindows);`

> 前往最近的开启的校门并毕业。如果 `WithWindows = true` 则允许路线经过窗户。

- [x] `void DirectGrass(bool WithWindows);`

> 前往最近的草丛并躲避。如果 `WithWindows = true` 则允许路线经过窗户。

- [x] `void DirectHide(Cell TrickerLocation, int TrickerViewRange, bool WithWindows);`

> 前往最适合的草丛并躲避。

- [ ] `void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);`

> 前往已知价值最高的道具并捡道具。

- [ ] `void DirectUseProp(std::vector<unsigned char>Priority);`

> 使用道具。

### 实时计数及特定格子状态

- [x] `int CountFinishedClassroom() const;`
- [x] `int CountNonemptyChest() const;`
- [ ] `int CountHiddenGate() const;`
- [x] `int CountClosedGate() const;`
- [x] `int CountOpenGate() const;`

> 对相应状态的格子计数。

- [x] `int GetChestProgress(int cellx, int celly);`
- [x] `int GetGateProgress(int cellx, int celly);`
- [x] `int GetClassroomProgress(int cellx, int celly);`
- [x] `int GetDoorProgress(int cellx, int celly);`
- [x] `int GetChestProgress(Cell cell) const;`
- [x] `int GetGateProgress(Cell cell) const;`
- [x] `int GetClassroomProgress(Cell cell) const;`
- [x] `int GetDoorProgress(Cell cell) const;`

> 返回相应格子的值。

### CommandPostStudent: 继承自 `CommandPost<IStudentAPI>`

- [x] `CommandPostStudent(IStudentAPI& api) : CommandPost(api) {}`
- [x] `void AutoUpdate();`

- [x] `void TeacherPunish();`
- [x] `double TeacherPunishCD();`

- [x] `void StraightAStudentWriteAnswers();`
- [x] `double StraightAStudentWriteAnswersCD();`

- [x] `void AtheleteCanBeginToCharge();`
- [x] `double AtheleteCanBeginToChargeCD();`

- [x] `void SunshineRouse();`
- [x] `void SunshineEncourage();`
- [x] `void SunshineInspire();`
- [x] `double SunshineRouseCD();`
- [x] `double SunshineEncourageCD();`
- [x] `double SunshineInspireCD();`

### CommandPostTricker: 继承自 `CommandPost<ITrickerAPI>`

- [x] `CommandPostTricker(ITrickerAPI& api) : CommandPost(api) {}`
- [x] `void AutoUpdate();`

- [x] `void AssassinDefaultAttack(int stux, int stuy);`	// 刺客普通攻击，传入学生坐标(stux,stuy)
- [x] `bool AssassinDefaultAttackOver(int rank);`//判断能否稳定命中，传入目前能观察到的学生列表的第几个，从0开始计数
- [x] `void AssassinBecomeInvisible();`
- [x] `void AssassinFlyingKnife(int stux, int stuy);`
- [x] `double AssassinFlyingKnifeCD();`

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
- [x] `void sendTrickerInfo(int64_t dest, TrickerInfo_t tricker);`
- [x] `void sendNeedHelp(int64_t dest, NeedHelpInfo self);` // TODO: 由于队友信息透明，所以不用传数据

### 接收信息

以下接收具体信息的函数中，返回值的第一个值是信息发送时的帧数（等价于时间）。

- [x] `int receiveMessage();` // 返回接收到的信息类型
- [x] `std::pair<int, MapUpdateInfo> receiveMapUpdate();`
- [x] `std::pair<int, TrickerInfo_t> receiveTrickerInfo();`
- [x] `std::pair<int, int> receiveNeedHelp();`

## 3 Geographer 地理学家

`Geographer`主要处理寻路和复杂的地图信息。

- [x] `Geographer(IFooAPI& api, CommandPost<IFooAPI>& Center_);`

- [x] `bool IsValidWithoutWindows(int x, int y);`
- [x] `bool IsValidWithWindows(int x, int y);`
- [x] `bool IsDestination(int x, int y, Node dest);`
- [x] `double CalculateH(int x, int y, Node dest);`
- [x] `std::vector<Node> MakePath(std::array<std::array<Node, 50>, 50> map, Node dest);`
- [x] `std::vector<Node> AStar(Node src, Node dest, bool WithWindows);`
- [x] `int EstimateTime(Cell Dest);`					// 去目的地的预估时间

- [x] `bool IsViewable(Cell Src, Cell Dest, int ViewRange);`			// 判断两个位置是否可视

- [x] `Cell GetNearestGate();`
- [x] `Cell GetNearestClassroom();` // 仅在没写完的作业中找
- [x] `Cell GetNearestOpenGate();`

## 4 Predictor 预言家

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

- [ ] ~~`Student`太弱~~
