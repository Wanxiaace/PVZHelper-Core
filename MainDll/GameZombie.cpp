#include "GameZombie.h"


unsigned int g_zombie_struct_size;
unsigned int g_zombie_max_count;

GameZombie::GameZombie(int index):Zombie(1025)
{
	if (index > g_zombie_max_count)
		BaseAddress = index;
	else
		BaseAddress = Memory::ReadMemory<int>(PVZBASEADDRESS + 0x90) + index * g_zombie_struct_size;
}

bool GameZombie::IsOnBoard()
{
	bool re = false;
	unsigned int Func = 0x52AE40;
	int b_ads = BaseAddress;
	__asm {
		mov eax, b_ads
		call Func
		mov re,al
	}
	return re;
}

void InitZombieSystem()
{
	g_zombie_struct_size = PVZ::Memory::ReadMemory<unsigned int>(0x41DDBD);
	g_zombie_max_count = PVZ::Memory::ReadMemory<unsigned int>(0x407CAA) / g_zombie_struct_size;
}

ScoreInteger GetDeltaScore(GameZombie* a_zombie)
{
	int Hp, HpMax = 0;
	a_zombie->GetBodyHp(&Hp, &HpMax);
	
	auto a1 = a_zombie->GetAccessoriesType1();
	auto a2 = a_zombie->GetAccessoriesType2();

	int deltaSc = (a_zombie->Speed + (a_zombie->X / 800.0f) + 1)*(HpMax + a1.MaxHp + a2.MaxHp) * 5;
	return deltaSc;
}

std::vector<SPT<GameZombie>> GetAllZombiesFixed(PVZ::Board* mBoard)
{
	
	std::vector<SPT<GameZombie>> zombies;
	int maxnum = Memory::ReadMemory<int>(mBoard->BaseAddress + 0x94);
	for (int i = 0; i < maxnum; i++)
	{
		if (!Memory::ReadPointer(mBoard->BaseAddress + 0x90, 0xEC + g_zombie_struct_size * i))
			zombies.push_back(MKS<GameZombie>(i));
	}
	return zombies;
}
