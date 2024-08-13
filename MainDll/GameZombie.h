#pragma once
#ifndef __PVZ_GAMEZOMBIE__
#define __PVZ_GAMEZOMBIE__
#include "pvzclass.h"
#include "Common.h"
#include <vector>

void InitZombieSystem();
//尽量使用Lawn::Zombie而不是GameZombie


class GameZombie;
//获得所有有效的僵尸
std::vector<SPT<GameZombie>> GetAllZombiesFixed(PVZ::Board* mBoard);

ScoreInteger GetDeltaScore(GameZombie* a_zombie);

extern unsigned int g_zombie_struct_size;
extern unsigned int g_zombie_max_count;

class GameZombie :public PVZ::Zombie {
public:
	GameZombie(int index);
	bool IsOnBoard();
};

#endif // !__PVZ_GAMEPLANT__
