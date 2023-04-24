#include "Utilities.hpp"

void UtilitiesTricker::AssassinDefaultAttack(int rank)//rank��Ҫvector��Ҫ��������ţ���vector�н���һ��ʱ���Զ�ѡ��
{
	int stux = API.GetStudents()[rank]->x;
	int stuy = API.GetStudents()[rank]->y;
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
	case THUAI6::StudentType::Athlete://�˶�Ա
		if (Distance<double(7400 * 400 / 3150 - 0.297 * 3150))
			return true;
	case THUAI6::StudentType::Teacher://��ʦ
		if (Distance<double(7400 * 400 / 2700 - 0.297 * 2700))
			return true;
	case THUAI6::StudentType::StraightAStudent://ѧ��
		if (Distance<double(7400 * 400 / 2880 - 0.297 * 2880))
			return true;
	case THUAI6::StudentType::Sunshine://����
		if (Distance<double(7400 * 400 / 3000 - 0.297 * 3000))
			return true;
	}
	return false;
}
