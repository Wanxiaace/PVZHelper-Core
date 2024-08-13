#include "Board.h"
#include "Common.h"
#include "Hacker.h"

lpCrater Board::SpawnCrater(int grid_x, int grid_y)
{//->mGridItemCounter = 180000
	lpFunc func = 0x408F80;
	lpBoard bd = TryGetBoard();

	lpCrater result = 0;
	if (bd) {
		__asm {
			mov edi, grid_y
			push grid_x
			mov eax,bd
			call func
			mov dword ptr [eax+0x18],180000
			mov result,eax
		}
	}
	else
		return 0x0;
	return result;
}

void Board::SpawnTomb(int grid_x, int grid_y)
{
	lpFunc func = 0x426620;
	lpBoard bd = TryGetBoard();

	auto challenge = PVZ::Memory::ReadMemory<unsigned int>(bd + 0x160);
	__asm {
		mov edi, grid_y
		mov ebx, grid_x
		push challenge
		call func
	}
}

void Board::SpawnTombInRect(int grid_origin_x, int grid_origin_y, int range_width, int range_height, int index)
{
	int rX = rand() % range_width + grid_origin_x;
	int rY = rand() % range_height + grid_origin_y;
	SpawnTomb(rX, rY);
}

Lawn::_Zombie* Board::AddZombieInRow(int row, ZombieType::ZombieType type)
{
	lpFunc func = 0x40DDC0;
	lpBoard board = TryGetBoard();
	if (!board)
		return nullptr;
	Lawn::_Zombie* result = nullptr;
	int type_integer = type;
	__asm {
		mov ebx,0
		mov eax,board
		push row
		push type_integer
		call func
		mov result,eax
	}
	return result;
}

lpPlant Board::SpawnPlant(int grid_x, int grid_y, PlantType::PlantType type)
{
	lpFunc func = 0x40D120;
	lpBoard bd = TryGetBoard();
	int type_integer = type;
	lpPlant result = 0;
	if (bd) {
		__asm {
			push -1
			push type_integer
			mov eax,grid_y
			push grid_x
			push bd
			call func
			mov result,eax
		}
	}

	return result;
}

void Board::SpawnCardAt(int card_x, int card_y, int seedtype)
{
	lpBoard bd = TryGetBoard();
	if (!bd)
		return;
	lpFunc func = 0x40CB10;
	__asm {
		push 0x2
		push 0x10
		push card_y
		push card_x
		mov ecx,bd
		call func
		mov ebx, seedtype
		mov [eax+0x68], ebx
	}
}

bool Board::CanTombPlaceAt(int grid_x, int grid_y)
{
	bool re = false;
	lpFunc Func = 0x408ED0;
	//lpBoard bd = g_Board->BaseAddress;
	__asm {
		mov edi, grid_x
		mov ebx, grid_y
		mov edx,0x6a9ec0
		mov edx,[edx]
		mov edx,[edx+0x768]
		call Func
		mov re,al
	}
	return re;
}

bool Board::CanPlantAt(int grid_x, int grid_y,PlantType::PlantType type)
{
	lpFunc func = 0x40E020;
	lpBoard bd = TryGetBoard();
	int plant_type_integer = (int)type;
	if (bd) {
		int result = 0;
		__asm {
			push plant_type_integer
			push grid_y
			mov eax,grid_x
			push bd
			call func
			mov result,eax
		}
		return result ? false : true;
	}else
		return false;
}

void Board::StopFire()
{
	lpFunc haddr = HookAtWithoutBehaviour(0x466E00,8);
	char returnbuf[] = {
		RETN(16),
	};
	PVZ::Memory::WriteArray(haddr, returnbuf, sizeof(returnbuf));
}

lpBoard Board::TryGetBoard()
{
	lpBoard result = 0x0;
	__asm {
		mov eax,0x6a9ec0
		mov eax,[eax]
		mov eax,[eax+0x768]
		mov result,eax
	}
	return result;
}

int Board::GridXtoPixelX(int grid_x,int grid_y)
{
	lpFunc func = 0x41C680;
	int result = -1;
	__asm {
		mov esi, grid_y
		mov eax, grid_x
		mov ecx, 0x6a9ec0
		mov ecx, [ecx]
		mov ecx, [ecx + 0x768]
		call func
		mov result, eax
	}
	return result;
}

int Board::GridYtoPixelY(int grid_x, int grid_y)
{
	lpFunc func = 0x41C740;
	int result = -1;
	__asm {
		mov eax, grid_y
		
		mov ecx, 0x6a9ec0
		mov ecx, [ecx]
		mov ecx, [ecx + 0x768]

		mov ebx,ecx

		mov ecx, grid_x

		call func
		mov result, eax
	}
	return result;
}
