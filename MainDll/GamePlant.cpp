#include "GamePlant.h"

unsigned int g_plant_struct_size = 0;
unsigned int g_plant_max_count = 0;

GamePlant::GamePlant(int index)	:Plant(1025)
{
	if (index > g_plant_max_count)
		BaseAddress = index;
	else
		BaseAddress = Memory::ReadMemory<int>(PVZBASEADDRESS + 0xAC) + index * g_plant_struct_size;
}

void InitPlantSystem()
{
	g_plant_struct_size = PVZ::Memory::ReadMemory<int>(0x4109F6);
	if(g_plant_struct_size == 0x14C)
		g_plant_struct_size = PVZ::Memory::ReadMemory<int>(0x41BAED);
	if (g_plant_struct_size == 0x14C)
		g_plant_struct_size = PVZ::Memory::ReadMemory<int>(0x41BAFF);

	g_plant_max_count = PVZ::Memory::ReadMemory<int>(0x407CC2) / g_plant_struct_size;
	//再三确认
}

std::vector<SPT<GamePlant>> GetAllPlantsFixed(PVZ::Board* mBoard)
{
	std::vector<SPT<GamePlant>> plants;
	int maxnum = Memory::ReadMemory<int>(mBoard->BaseAddress + 0xB0);
	for (int i = 0; i < maxnum; i++)
	{
		if (!Memory::ReadMemory<byte>(Memory::ReadMemory<int>(mBoard->BaseAddress + 0xAC) + 0x141 + g_plant_struct_size * i))//判断植物是否存在
			plants.push_back(MKS<GamePlant>(i));
	}
	return plants;
}
