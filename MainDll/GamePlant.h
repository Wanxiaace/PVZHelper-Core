#pragma once
#ifndef __PVZ_GAMEPLANT__
#define __PVZ_GAMEPLANT__
#include "pvzclass.h"
#include <vector>

void InitPlantSystem();

class GamePlant;
//获得所有有效的植物
std::vector<SPT<GamePlant>> GetAllPlantsFixed(PVZ::Board* mBoard);

extern unsigned int g_plant_struct_size;
extern unsigned int g_plant_max_count;

class GamePlant :PVZ::Plant {
public:
	GamePlant(int index);
};

#endif // !__PVZ_GAMEPLANT__
