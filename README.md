# THUAI6_team

`Utilities.hpp`中定义类Utilities，用于实现移动、攻击等操作，以及提供若干实用函数

- [ ] `void UpdateClassroom();`
- [ ] `void UpdateGate();`
- [ ] `void UpdateChest();`
- [ ] `void UpdateDoor();`
- [ ] `void AutoUpdate();` // TODO: 自动更新，检查附近的格子有没有和已知不一致的，如果有就更新并且广播

- [x] `void MoveTo(Point Dest, bool WithWindows);`		// 往目的地动一动
- [x] `void MoveToNearestClassroom(bool WithWindows);`	// 往最近的作业的方向动一动
- [ ] `void MoveToNearestGate(bool WithWindows);`		// 往最近的校门旁边动一动
- [ ] `void MoveToNearestChest(bool WithWindows);`		// 往最近的箱子的方向动一动
- [ ] `bool NearPoint(Point P, int level = 1);`         // level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
- [ ] `bool NearClassroom();`							// 已经在作业旁边了吗？
- [ ] `bool NearGate();`								// 已经在校门旁边了吗？
- [ ] `bool NearChest();`								// 已经在箱子旁边了吗？
- [ ] `void DirectLearning(bool WithWindows);`			// 前往最近的作业并学习
- [ ] `void DirectOpeningChest(bool WithWindows);`		// 前往最近的箱子并开箱
- [ ] `void DirectProp(std::vector<unsigned char>Priority, int DistanceInfluence, int PropInfluence, bool WithWindows);`		// 捡与使用道具

- [ ] `int EstimateTime(Point Dest);`					// 去目的地的预估时间
- [ ] `bool IsViewable(Point Src, Point Dest, int ViewRange);`			// 判断两个位置是否可视
- [ ] `int CountFinishedClassroom() const;`
- [ ] `int CountNonemptyChest() const;` // TODO: 暂未实现`

- [ ] `void AssassinDefaultAttack(int rank);`	// 刺客普通攻击
- [ ] `bool AssassinDefaultAttackOver(int rank);`

`UtilitiesAStar.hpp`中实现Utilities的寻路算法

`UtilitiesAttack.hpp`中实现Utilities的攻击方法

`UtilitiesBasic.hpp`中实现基本移动和有目的的移动，以及其他零散的方法

`Pigeon.h`通信

`Pigeon.cpp`实现通信
