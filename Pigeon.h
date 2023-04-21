#ifndef _PIGEON_H
#define _PIGEON_H

#include "API.h"
#include "AI.h"
#include "Utilities.hpp"
#include<queue>

/* 信息协议
信息头（info[0]）
MapUpdate 地图更新
TrickerInfo 捣蛋鬼信息
NeedHelp 请求支援
	info[1-2]：坐标
WantProp 请求获取道具（可能需要反馈？不然太远了跑去不划算）
*/

#define NoMessage 0x00
#define MapUpdate 0x01
#define TrickerInfo 0x02
#define NeedHelp 0x03
#define WantProp 0x04

struct MapUpdateInfo
{
	THUAI6::PlaceType type;
	int x, y, val;
};
typedef std::shared_ptr<const THUAI6::Student> NeedHelpInfo;
typedef std::vector<std::shared_ptr<const THUAI6::Tricker>> TrickerInfo_t;

class Encoder
{
private:
	static const int MaxLength = 255;
	char msg[MaxLength];
	int Pointer;
public:
	Encoder();
	void SetHeader(char header);
	template<typename T>
	void PushInfo(T info);
	std::string ToString();
};

class Decoder
{
private:
	std::string msg;
	int Pointer;
public:
	Decoder(std::string code);
	template<typename T>
	T ReadInfo();
};

class Pigeon
{
private:
	IStudentAPI& API;
//	Utilities<IStudentAPI&> *bindHelper;
	void sendInfo(int64_t dest, std::string info);
	std::string buf;

public:
	Pigeon(IStudentAPI& api);
	void sendMapUpdate(int64_t dest, MapUpdateInfo muinfo);
	void sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val);
	void sendTrickerInfo(int64_t dest, TrickerInfo_t tricker);
	void sendNeedHelp(std::shared_ptr<const THUAI6::Student> slf);

	int receiveMessage(); // 返回接收到的信息类型
	MapUpdateInfo receiveMapUpdate();
	TrickerInfo_t receiveTrickerInfo();
	NeedHelpInfo receiveNeedHelp();
};

#endif