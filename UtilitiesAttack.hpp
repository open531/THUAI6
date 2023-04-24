#include "Utilities.hpp"

void UtilitiesTricker::AssassinDefaultAttack(int stux, int stuy)//传入要攻击的学生坐标stux与stuy
{
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	API.Attack(atan2(stuy - sy, stux - sx));
}

bool UtilitiesTricker::AssassinDefaultAttackOver(int rank)
{
	int stux = API.GetStudents()[rank]->x;
	int stuy = API.GetStudents()[rank]->y;
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	double Distance = sqrt((stux - sx) * (stux - sx) + (stuy - sy) * (stuy - sy));
	switch (API.GetStudents()[rank]->studentType)
	{
	case THUAI6::StudentType::Athlete://运动员
		if (Distance<double(7400 * 400 / 3150 - 0.297 * 3150))
			return true;
	case THUAI6::StudentType::Teacher://老师
		if (Distance<double(7400 * 400 / 2700 - 0.297 * 2700))
			return true;
	case THUAI6::StudentType::StraightAStudent://学霸
		if (Distance<double(7400 * 400 / 2880 - 0.297 * 2880))
			return true;
	case THUAI6::StudentType::Sunshine://奶妈
		if (Distance<double(7400 * 400 / 3000 - 0.297 * 3000))
			return true;
	}
	return false;
}
