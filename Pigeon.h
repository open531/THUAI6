#ifndef _PIGEON_H
#define _PIGEON_H

#include "API.h"
#include "AI.h"
#include<queue>

/* ��ϢЭ��
��Ϣͷ��info[0]��
MapUpdate ��ͼ����
TrickerInfo ��������Ϣ
NeedHelp ����֧Ԯ
	info[1-2]������
WantProp �����ȡ���ߣ�������Ҫ��������Ȼ̫Զ����ȥ�����㣩
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
	void sendNeedHelp(int64_t dest,NeedHelpInfo self);

	int receiveMessage(); // ���ؽ��յ�����Ϣ����
	std::pair<int, MapUpdateInfo> receiveMapUpdate();
	std::pair<int, TrickerInfo_t> receiveTrickerInfo();
	std::pair<int, int> receiveNeedHelp();
};

#include "Pigeon.hpp"

#endif