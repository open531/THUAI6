#include "Pigeon.h"

#include<cstdarg>

Encoder::Encoder() :Pointer(0)
{
	memset(msg, 0, sizeof(msg));
}
template<typename T>
void Encoder::PushInfo(T info)
{
	size_t t = sizeof(T);
	void* ptr = & info;
	for (size_t i = 0; i < t; i++)
	{
		msg[Pointer] = ((*((unsigned char*)ptr + i)) >> 4) + 'a';
		Pointer++;
		msg[Pointer] = ((*((unsigned char*)ptr + i)) & 0x0f) + 'a';
		Pointer++;
	}
}
void Encoder::SetHeader(char header)
{
	PushInfo(header);
}
std::string Encoder::ToString()
{
	return std::string(msg, msg + MaxLength);
}

Decoder::Decoder(std::string code) :msg(code), Pointer(0) {}
template<typename T>
T Decoder::ReadInfo()
{
	T obj;
	void* ptr = &obj;
	size_t t = sizeof(T);
	for (size_t i = 0; i < t; i++)
	{
		*((unsigned char*)ptr + i) = ((((unsigned char)*(msg.c_str() + Pointer) - 'a') << 4)) | (((unsigned char)*(msg.c_str() + Pointer + 1)) - 'a');
		Pointer += 2;
	}
	return obj;
}

Pigeon::Pigeon(IStudentAPI& api) : API(api) {}

void Pigeon::sendInfo(int64_t dest, std::string info)
{
	if (dest != API.GetSelfInfo()->guid) API.SendMessage(dest, info);
}

void Pigeon::sendMapUpdate(int64_t dest, MapUpdateInfo muinfo)
{
	Encoder enc;
	enc.SetHeader(MapUpdate);
	enc.PushInfo(API.GetFrameCount());
	enc.PushInfo(muinfo);
	sendInfo(dest, enc.ToString());
}
void Pigeon::sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val)
{
	MapUpdateInfo muinfo = { type, x, y, val };
	sendMapUpdate(dest, muinfo);
}

std::pair<int, MapUpdateInfo> Pigeon::receiveMapUpdate()
{
	Decoder dec(buf);
	char header = dec.ReadInfo<char>();
	assert(header == MapUpdate);
	int frm = dec.ReadInfo<int>();
	MapUpdateInfo muinfo = dec.ReadInfo<MapUpdateInfo>();
	return std::make_pair<int, MapUpdateInfo>(static_cast<int&&>(frm), static_cast<MapUpdateInfo&&>(muinfo));
}

int Pigeon::receiveMessage()
{
	if (API.HaveMessage())
	{
		buf = API.GetMessage().second; // 是谁发来的好像不太重要，只取信息内容
		Decoder dec(buf);
		return dec.ReadInfo<char>();
	}
	else return NoMessage;
}

void Pigeon::sendTrickerInfo(int64_t dest, TrickerInfo_t tricker)
{
	Encoder enc;
	enc.SetHeader(TrickerInfo);
	enc.PushInfo(API.GetFrameCount());
	enc.PushInfo<std::vector<std::shared_ptr<const THUAI6::Tricker>>>(tricker);
	sendInfo(dest, enc.ToString());

}

std::pair<int, TrickerInfo_t> Pigeon::receiveTrickerInfo()
{
	Decoder dec(buf);
	char header = dec.ReadInfo<char>();
	assert(header == TrickerInfo);
	return std::make_pair<int, TrickerInfo_t>(dec.ReadInfo<int>(), dec.ReadInfo<TrickerInfo_t>());
}
//捣蛋鬼信息的编码和解码函数
std::string sendOneselfMessage(std::shared_ptr<const THUAI6::Student> self)
{
	Encoder enc;
	enc.SetHeader(NeedHelp);
	enc.PushInfo<std::shared_ptr<const THUAI6::Student>>(self);
	std::string info = enc.ToString();
	return info;
}

std::shared_ptr<const THUAI6::Student> receiveOneselfMessage(std::string info)
{
	Decoder dec(info);
	char header = dec.ReadInfo<char>();
	std::shared_ptr<const THUAI6::Student> p1 = dec.ReadInfo<std::shared_ptr<const THUAI6::Student>>();
	return p1;

}
//自己信息的编码和解码函数
std::string sendPropsMessage(std::vector<std::shared_ptr<const THUAI6::Prop>> prop)
{
	Encoder enc;
	enc.SetHeader(WantProp);
	enc.PushInfo<std::vector<std::shared_ptr<const THUAI6::Prop>>>(prop);
	std::string info = enc.ToString();
	return info;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> receivePropsMessage(std::string info)
{
	Decoder dec(info);
	char header = dec.ReadInfo<char>();
	std::vector<std::shared_ptr<const THUAI6::Prop>> p1 = dec.ReadInfo<std::vector<std::shared_ptr<const THUAI6::Prop>>>();
	return p1;
}
//道具信息的编码和解码函数
//void send_Door(IStudentAPI& api1, int64_t playerID)
//{
//	int i, j, n = 0;
//	int a[6], b[6];
//	char c[6];
//	for (i = 0; i < 50; i++)
//	{
//		for (j = 0; j < 50; j++)
//		{
//			if (Access[i][j] = 2u)
//			{
//				a[n] = i;
//				b[n] = j;
//				n++;
//				c[n] = char(api1.IsDoorOpen(i, j));
//			}
//		}
//	}
//	std::string info_Door = sendDoorMessage(a, b, n, c);
//
//	api1.SendMessage(playerID, info_Door);
//
//}
//发送门信息的函数
//void send_Tricker(IStudentAPI& api1, int64_t playerID)
//{
//	std::string info_Tricker = sendTrickerMessage(api1.GetTrickers());
//	api1.SendMessage(playerID, info_Tricker);
//}
//发送捣蛋鬼信息的函数
void send_Oneself(IStudentAPI& api1, int64_t playerID)
{
	std::string info_Oneself = sendOneselfMessage(api1.GetSelfInfo());
	api1.SendMessage(playerID, info_Oneself);
}
//发送自己信息的函数
void send_Prop(IStudentAPI& api1, int64_t playerID)
{
	std::string info_Prop = sendPropsMessage(api1.GetProps());
	api1.SendMessage(playerID, info_Prop);
}
//发送道具信息的函数
//std::pair<char, std::vector<std::pair<std::pair<int, int>, char>>> receive_Door(IStudentAPI& api2)
//{
//	std::string info_Door = api2.GetMessage().second;
//	std::pair<char, std::vector<std::pair<std::pair<int, int>, char>>> p1 = receiveDoorMessage(info_Door);
//	return p1;
//}
//接收门信息的函数
//std::vector<std::shared_ptr<const THUAI6::Tricker>> receive_Tricker(IStudentAPI& api2)
//{
//	std::string info_Tricker = api2.GetMessage().second;
//	std::vector<std::shared_ptr<const THUAI6::Tricker>> p1 = receiveTrickerMessage(info_Tricker);
//	return p1;
//}
//接收捣蛋鬼信息的函数
std::shared_ptr<const THUAI6::Student> receive_Oneself(IStudentAPI& api2)
{
	std::string info_Oneself = api2.GetMessage().second;
	std::shared_ptr<const THUAI6::Student> p1 = receiveOneselfMessage(info_Oneself);
	return p1;
}
//接受发送者自身信息的函数
std::vector<std::shared_ptr<const THUAI6::Prop>> receive_Prop(IStudentAPI& api2)
{
	std::string info_Prop = api2.GetMessage().second;
	std::vector<std::shared_ptr<const THUAI6::Prop>> p1 = receivePropsMessage(info_Prop);
	return p1;
}
//接受道具信息的函数
