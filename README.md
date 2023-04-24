# THUAI6_team

把.h和.hpp添加到CAPI.sln的头文件里，把.cpp添加到源文件里。

## 1 Utilities

`Utilities.hpp`中定义类Utilities，用于实现移动、攻击等操作，以及提供若干实用函数

### 更新

- [x] `void Update(MapUpdateInfo upinfo, int t_);` 传入Pigeon接收到的更新信息`upinfo`和产生信息时的帧数`t_`
- [x] `void AutoUpdate();` 自动更新，检查附近的格子有没有和已知不一致的，如果有就更新并且广播

### 移动

- [x] `void MoveTo(Point Dest, bool WithWindows);`		往目的地动一动
- [x] `void MoveToNearestClassroom(bool WithWindows);`	往最近的作业的方向动一动
- [x] `void MoveToNearestGate(bool WithWindows);`		往最近的校门旁边动一动
- [x] `void MoveToNearestChest(bool WithWindows);`		往最近的箱子的方向动一动

### 邻近信息

- [x] `bool NearPoint(Point P, int level = 1);`         // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
- [x] `bool NearClassroom();`							// 已经在作业旁边了吗？
- [x] `bool NearGate();`								// 已经在关闭的校门旁边了吗？
- [x] `bool NearOpenGate();`							// 已经在开启的校门旁边了吗？
- [x] `bool NearChest();`								// 已经在箱子旁边了吗？

### 执行

- [x] `void DirectLearning(bool WithWindows);`			// 前往最近的作业并学习
- [x] `void DirectOpeningChest(bool WithWindows);`		// 前往最近的箱子并开箱
- [x] `void DirectOpeningGate(bool WithWindows);`		// 前往最近的关闭的校门并开门
- [x] `void DirectGraduate(bool WithWindows);`			// 前往最近的开启的校门并毕业
- [ ] `void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);`		// 前往已知价值最高的道具并捡道具
- [ ] `void DirectUseProp();`
- [ ] `void DirectSkill();`

### 获取最新信息

以下函数用于代替api提供的同名函数，可以结合通信结果给出最新信息。

- [ ] `int GetChestProgress(int cellx, int celly);`
- [ ] `int GetGateProgress(int cellx, int celly);`
- [x] `int GetClassroomProgress(int cellx, int celly);`
- [ ] `int GetDoorProgress(int cellx, int celly);`
- [ ] `int GetChestProgress(Point cell) const;`
- [ ] `int GetGateProgress(Point cell) const;`
- [ ] `int GetClassroomProgress(Point cell) const;`
- [ ] `int GetDoorProgress(Point cell) const;`

### 其他

- [x] `int EstimateTime(Point Dest);`					// 去目的地的预估时间
- [x] `bool IsViewable(Point Src, Point Dest, int ViewRange);`			// 判断两个位置是否可视

### 攻击方法

- [ ] `void AssassinDefaultAttack(int rank);`	// 刺客普通攻击
- [ ] `bool AssassinDefaultAttackOver(int rank);`

`UtilitiesAStar.hpp`中实现Utilities的寻路算法

`UtilitiesAttack.hpp`中实现Utilities的攻击方法

`UtilitiesBasic.hpp`中实现基本移动和有目的的移动，以及其他零散的方法

## 2 Pigeon

`Pigeon.h`通信

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

以下函数中`dest`参数表示发送目标。

- [x] `void sendMapUpdate(int64_t dest, MapUpdateInfo muinfo);`
- [x] `void sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val);`
- [ ] `void sendTrickerInfo(int64_t dest, TrickerInfo_t tricker);`
- [ ] `void sendNeedHelp(std::shared_ptr<const THUAI6::Student> slf);` // TODO: 由于队友信息透明，所以不用传数据

### 接收信息

以下接收具体信息的函数中，返回值的第一个值是信息发送时的帧数（等价于时间）。

- [x] `int receiveMessage();` // 返回接收到的信息类型
- [x] `std::pair<int, MapUpdateInfo> receiveMapUpdate();`
- [ ] `std::pair<int, TrickerInfo_t> receiveTrickerInfo();`
- [ ] `std::pair<int, NeedHelpInfo> receiveNeedHelp();`

`Pigeon.hpp`实现通信

## 3 建议使用的策略编写方法

首先定义角色可能处于的状态（已经定义了一些）。

限定每一帧要么维持原状态，要么变换到另一个状态，不能在一帧内多次变化状态（待定）。

具体实现上，使用一个`switch-case`进行不同状态的处理。

## 4 执行顺序

每一帧应当按照以下顺序执行：

1. 调用`Helper.AutoUpdate()`进行信息更新；
2. 使`Pigeon`接收信息，并对`Pigeon`接收的信息进行处理，直到没有新信息，此时可以依据接收的信息进行状态变更的准备；
3. 决定变更状态或维持状态；
4. 执行状态对应的行为。

## 5 TODO List

- 防止两个角色动作冲突（同地点学习、开箱），需要通信实现
- 防止两个角色相撞
- 建议使用`Utilities`的继承类分别编写不同角色的技能使用方法

## 6 Issues

- [x] ~~开启完校门的角色不会自己毕业，没有开启校门的角色反而能够顺利毕业。~~
- [x] ~~开启校门后没有直接毕业，而是折返，同时会报错。~~
- [ ] 理论上已经判断了不能发给自己，但是没有成功，不过api也会判断所以不是大问题。
