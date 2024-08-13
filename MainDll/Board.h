#pragma once
#include "pvzclass.h"
#include "LawnStruct.h"
#include "Common.h"



namespace Board {
	lpCrater SpawnCrater(int grid_x,int grid_y);
	void SpawnTomb(int grid_x, int grid_y);
	void SpawnTombInRect(int grid_origin_x, int grid_origin_y, int range_width,int range_height, int index = 0);
	lpPlant SpawnPlant(int grid_x, int grid_y, PlantType::PlantType type);
	void SpawnCardAt(int card_x,int card_y,int seedtype);

	Lawn::_Zombie* AddZombieInRow(int row, ZombieType::ZombieType type);
	bool CanTombPlaceAt(int grid_x, int grid_y);
	bool CanPlantAt(int grid_x, int grid_y, PlantType::PlantType);
	void StopFire();
	lpBoard TryGetBoard();

	int GridXtoPixelX(int grid_x, int grid_y);
	int GridYtoPixelY(int grid_x, int grid_y);
}