#ifndef _PIGEON_H
#define _PIGEON_H

#include "API.h"
#include "AI.h"

/* 信息协议
信息头（info[0]）
MapUpdate 地图更新
TrickerInfo 捣蛋鬼信息
NeedHelp 请求支援
	info[1-2]：坐标
WantTool 请求获取道具（可能需要反馈？不然太远了跑去不划算）
*/

#define MapUpdate 0x01
#define TrickerInfo 0x02
#define NeedHelp 0x03
#define WantTool 0x04

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
public:
	Pigeon(IStudentAPI& api);
};

#endif