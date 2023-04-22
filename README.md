# THUAI6_team

把.h和.hpp添加到CAPI.sln的头文件里，把.cpp添加到源文件里。

## Utilities

`Utilities.hpp`中定义类Utilities，用于实现移动、攻击等操作，以及提供若干实用函数

- [ ] `void UpdateClassroom();`
- [ ] `void UpdateGate();`
- [ ] `void UpdateChest();`
- [ ] `void UpdateDoor();`
- [ ] `void AutoUpdate();` // TODO: 自动更新，检查附近的格子有没有和已知不一致的，如果有就更新并且广播

- [x] `void MoveTo(Point Dest, bool WithWindows);`		// 往目的地动一动
- [x] `void MoveToNearestClassroom(bool WithWindows);`	// 往最近的作业的方向动一动
- [x] `void MoveToNearestGate(bool WithWindows);`		// 往最近的校门旁边动一动
- [x] `void MoveToNearestChest(bool WithWindows);`		// 往最近的箱子的方向动一动
- [x] `bool NearPoint(Point P, int level = 1);`         // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
- [x] `bool NearClassroom();`							// 已经在作业旁边了吗？
- [x] `bool NearGate();`								// 已经在校门旁边了吗？
- [x] `bool NearChest();`								// 已经在箱子旁边了吗？
- [x] `void DirectLearning(bool WithWindows);`			// 前往最近的作业并学习
- [x] `void DirectOpeningChest(bool WithWindows);`		// 前往最近的箱子并开箱
- [ ] `void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);`		// 捡与使用道具

- [x] `int EstimateTime(Point Dest);`					// 去目的地的预估时间
- [x] `bool IsViewable(Point Src, Point Dest, int ViewRange);`			// 判断两个位置是否可视
- [ ] `int CountFinishedClassroom() const;`
- [ ] `int CountNonemptyChest() const;` // TODO: 暂未实现`

- [ ] `void AssassinDefaultAttack(int rank);`	// 刺客普通攻击
- [ ] `bool AssassinDefaultAttackOver(int rank);`

`UtilitiesAStar.hpp`中实现Utilities的寻路算法

`UtilitiesAttack.hpp`中实现Utilities的攻击方法

`UtilitiesBasic.hpp`中实现基本移动和有目的的移动，以及其他零散的方法

## Pigeon

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

- [x] `void sendMapUpdate(int64_t dest, MapUpdateInfo muinfo);`
- [x] `void sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val);`
- [x] `void sendTrickerInfo(int64_t dest, TrickerInfo_t tricker);`
- [ ] `void sendNeedHelp(std::shared_ptr<const THUAI6::Student> slf);`

- [x] `int receiveMessage();` // 返回接收到的信息类型
- [x] `MapUpdateInfo receiveMapUpdate();`
- [x] `TrickerInfo_t receiveTrickerInfo();`
- [ ] `NeedHelpInfo receiveNeedHelp();`


`Pigeon.cpp`实现通信

## TODO List

- 让 Utilities 绑定一只 Pigeon，完善 AutoUpdate
- 实现传入 MapUpdateInfo 更新地图
- 防止两个角色动作冲突（同地点学习、开箱），需要通信实现
- 防止两个角色相撞

## Issues

- 开启完校门的角色不会自己毕业，没有开启校门的角色反而能够顺利毕业。
- 开启校门后没有直接毕业，而是折返，同时会报错。
